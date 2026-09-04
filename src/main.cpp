#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <vector>
#include <cmath>
#include <queue>
#include <set>
#include <unordered_map>

using namespace geode::prelude;

// Corner pieces use the triangle/slope block (ID 8), rotated to sit
// diagonally at the corner spots, giving a pointed accent instead of
// a flat patch.
static constexpr int CORNER_OUTER_ID = 8; // slope triangle, rotated diagonally outward
static constexpr int CORNER_INNER_ID = 8; // slope triangle, rotated diagonally to patch the notch

class $modify(AutoDecoEditorUI, EditorUI) {
    struct Fields {
        CCArray* m_savedObjects = nullptr;
        ccColor3B m_activeThemeColor = {240, 30, 255};
    };

    // Simple flags for the 8 neighbor directions around a block.
    struct Neighbors {
        bool N = false, S = false, E = false, W = false;
        bool NE = false, NW = false, SE = false, SW = false;
    };

    static int64_t gridKey(int gx, int gy) {
        return (static_cast<int64_t>(gx) << 32) ^ static_cast<uint32_t>(gy);
    }

    GameObject* make(int id, CCPoint pos, float scale, ccColor3B mainColor, ccColor3B detailColor, ZLayer layerGroup, int zOrderOffset, float rotation = 0.0f) {
        auto obj = this->m_editorLayer->createObject(id, pos, false);
        if (!obj) return nullptr;
        obj->setScale(scale);
        obj->setRotation(rotation);
        if (obj->m_baseColor) {
            obj->m_baseColor->m_customColor = mainColor;
            obj->m_baseColor->m_usesCustomBlend = true;
        }
        if (obj->m_detailColor) {
            obj->m_detailColor->m_customColor = detailColor;
            obj->m_detailColor->m_usesCustomBlend = true;
        }
        obj->m_zLayer = layerGroup;
        obj->m_zOrder = zOrderOffset;
        return obj;
    }

    void onSelectPurple(CCObject*) { m_fields->m_activeThemeColor = {240, 30, 255}; }
    void onSelectCyan(CCObject*)   { m_fields->m_activeThemeColor = {0, 240, 255}; }
    void onSelectPink(CCObject*)   { m_fields->m_activeThemeColor = {255, 40, 180}; }
    void onSelectGreen(CCObject*)  { m_fields->m_activeThemeColor = {30, 255, 100}; }

    // Prints the object ID of whatever is currently selected to the Geode
    // console/log. Select a block in the editor, tap "LOG ID", then check
    // Geode's console for a line like "Selected object ID: 211".
    void onLogSelectedIDs(CCObject*) {
        auto selected = this->m_selectedObjects;
        if (!selected || selected->count() == 0) {
            FLAlertLayer::create("LOG ID", "Select something first!", "OK")->show();
            return;
        }
        for (int i = 0; i < selected->count(); i++) {
            auto obj = static_cast<GameObject*>(selected->objectAtIndex(i));
            if (obj) geode::log::info("Selected object ID: {}", obj->m_objectID);
        }
    }

    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) return false;
        auto menu = CCMenu::create();
        menu->setPosition(0, 0);
        auto buttonSprite = ButtonSprite::create("AUTO DECO", 60, true, "goldFont.fnt", "GJ_button_01.png", 25, 0.6f);
        auto button = CCMenuItemSpriteExtra::create(buttonSprite, this, menu_selector(AutoDecoEditorUI::onAutoDecoClicked));
        button->setPosition(100, 110);
        menu->addChild(button);
        auto logButtonSprite = ButtonSprite::create("LOG ID", 60, true, "goldFont.fnt", "GJ_button_01.png", 25, 0.6f);
        auto logButton = CCMenuItemSpriteExtra::create(logButtonSprite, this, menu_selector(AutoDecoEditorUI::onLogSelectedIDs));
        logButton->setPosition(100, 140);
        menu->addChild(logButton);
        float dotX = 65.0f; float dotY = 75.0f;
        auto pDot = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
        pDot->setColor({240, 30, 255}); pDot->setScale(0.5f);
        auto btnP = CCMenuItemSpriteExtra::create(pDot, this, menu_selector(AutoDecoEditorUI::onSelectPurple));
        btnP->setPosition(dotX, dotY); menu->addChild(btnP);
        auto cDot = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
        cDot->setColor({0, 240, 255}); cDot->setScale(0.5f);
        auto btnC = CCMenuItemSpriteExtra::create(cDot, this, menu_selector(AutoDecoEditorUI::onSelectCyan));
        btnC->setPosition(dotX + 22.0f, dotY); menu->addChild(btnC);
        auto pinkDot = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
        pinkDot->setColor({255, 40, 180}); pinkDot->setScale(0.5f);
        auto btnPink = CCMenuItemSpriteExtra::create(pinkDot, this, menu_selector(AutoDecoEditorUI::onSelectPink));
        btnPink->setPosition(dotX + 44.0f, dotY); menu->addChild(btnPink);
        auto gDot = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
        gDot->setColor({30, 255, 100}); gDot->setScale(0.5f);
        auto btnG = CCMenuItemSpriteExtra::create(gDot, this, menu_selector(AutoDecoEditorUI::onSelectGreen));
        btnG->setPosition(dotX + 66.0f, dotY); menu->addChild(btnG);
        this->addChild(menu);
        return true;
    }

    void decorateStructure(CCArray* selectedObjects) {
        std::vector<GameObject*> blocks;
        if (!selectedObjects) return;
        for (int i = 0; i < selectedObjects->count(); i++) {
            auto obj = static_cast<GameObject*>(selectedObjects->objectAtIndex(i));
            if (obj && obj->m_objectID <= 500) blocks.push_back(obj);
        }

        ccColor3B whiteColor = m_fields->m_activeThemeColor;
        ccColor3B blackColor = {10, 10, 15};
        ccColor3B darkGreyDetails = {50, 50, 60};

        // Grid size assumption matches your original 32u spacing check.
        const float cell = 32.0f;
        std::unordered_map<int64_t, GameObject*> grid;
        auto keyFor = [&](float x, float y) -> int64_t {
            int gx = static_cast<int>(std::round(x / cell));
            int gy = static_cast<int>(std::round(y / cell));
            return gridKey(gx, gy);
        };
        for (auto b : blocks) {
            auto pos = b->getPosition();
            grid[keyFor(pos.x, pos.y)] = b;
        }

        for (auto source : blocks) {
            auto p = source->getPosition();
            float s = source->getScale();
            if (s <= 0.0f) s = 1.0f;
            float x = p.x; float y = p.y;

            Neighbors n;
            n.W  = grid.count(keyFor(x - cell, y)) > 0;
            n.E  = grid.count(keyFor(x + cell, y)) > 0;
            n.N  = grid.count(keyFor(x, y + cell)) > 0;
            n.S  = grid.count(keyFor(x, y - cell)) > 0;
            n.NW = grid.count(keyFor(x - cell, y + cell)) > 0;
            n.NE = grid.count(keyFor(x + cell, y + cell)) > 0;
            n.SW = grid.count(keyFor(x - cell, y - cell)) > 0;
            n.SE = grid.count(keyFor(x + cell, y - cell)) > 0;

            bool anyEdgeExposed = !n.N || !n.S || !n.E || !n.W;

            // Base fill block, always present.
            make(210, CCPoint(x, y), s * 1.0f, whiteColor, whiteColor, ZLayer::B2, 1);

            // Shadow/highlight accent kept from your original, on any exposed side.
            if (anyEdgeExposed) {
                make(239, CCPoint(x + 4.0f * s, y - 4.0f * s), s * 1.0f, whiteColor, whiteColor, ZLayer::B1, 2);
                make(210, CCPoint(x + 2.0f * s, y - 2.0f * s), s * 1.0f, blackColor, blackColor, ZLayer::B2, -2);
            }

            // Straight edges — placed independently per side now, instead of
            // one edge piece per block guessing a single rotation.
            if (!n.N) make(211, CCPoint(x, y + 14.0f * s), s * 1.0f, whiteColor, whiteColor, ZLayer::T1, 8, 180.0f);
            if (!n.S) make(211, CCPoint(x, y - 14.0f * s), s * 1.0f, blackColor, blackColor, ZLayer::B1, 3, 0.0f);
            if (!n.W) make(239, CCPoint(x - 14.0f * s, y), s * 1.0f, blackColor, blackColor, ZLayer::B1, 5, 90.0f);
            if (!n.E) make(239, CCPoint(x + 14.0f * s, y), s * 1.0f, blackColor, blackColor, ZLayer::B1, 5, 90.0f);

            // Outer (convex) corners: two adjacent flat sides both missing.
            // Rotated so the spike points away from the structure.
            if (!n.N && !n.E) make(CORNER_OUTER_ID, CCPoint(x + 10.0f * s, y + 10.0f * s), s * 1.0f, whiteColor, blackColor, ZLayer::T1, 9, 45.0f);
            if (!n.N && !n.W) make(CORNER_OUTER_ID, CCPoint(x - 10.0f * s, y + 10.0f * s), s * 1.0f, whiteColor, blackColor, ZLayer::T1, 9, 315.0f);
            if (!n.S && !n.W) make(CORNER_OUTER_ID, CCPoint(x - 10.0f * s, y - 10.0f * s), s * 1.0f, whiteColor, blackColor, ZLayer::T1, 9, 225.0f);
            if (!n.S && !n.E) make(CORNER_OUTER_ID, CCPoint(x + 10.0f * s, y - 10.0f * s), s * 1.0f, whiteColor, blackColor, ZLayer::T1, 9, 135.0f);

            // Inner (concave) corners: both adjacent sides filled, but the
            // diagonal between them is empty. Without this, the outline has
            // a visible gap at every inward turn of the structure.
            if (n.N && n.E && !n.NE) make(CORNER_INNER_ID, CCPoint(x + 14.0f * s, y + 14.0f * s), s * 1.0f, blackColor, blackColor, ZLayer::B1, 6, 45.0f);
            if (n.N && n.W && !n.NW) make(CORNER_INNER_ID, CCPoint(x - 14.0f * s, y + 14.0f * s), s * 1.0f, blackColor, blackColor, ZLayer::B1, 6, 315.0f);
            if (n.S && n.W && !n.SW) make(CORNER_INNER_ID, CCPoint(x - 14.0f * s, y - 14.0f * s), s * 1.0f, blackColor, blackColor, ZLayer::B1, 6, 225.0f);
            if (n.S && n.E && !n.SE) make(CORNER_INNER_ID, CCPoint(x + 14.0f * s, y - 14.0f * s), s * 1.0f, blackColor, blackColor, ZLayer::B1, 6, 135.0f);

            // Center detail: only blocks with all 8 neighbors present are
            // "fully interior" and get the denser nested-square look.
            bool fullyInterior = n.N && n.S && n.E && n.W && n.NE && n.NW && n.SE && n.SW;
            if (fullyInterior) {
                make(1006, CCPoint(x, y), s * 0.90f, darkGreyDetails, blackColor, ZLayer::B1, 4);
                make(1324, CCPoint(x, y), s * 0.55f, whiteColor, blackColor, ZLayer::T1, 10);
                make(1324, CCPoint(x, y), s * 0.30f, blackColor, blackColor, ZLayer::T1, 11);
            } else {
                make(1006, CCPoint(x, y), s * 0.50f, darkGreyDetails, blackColor, ZLayer::B1, 4);
            }

            this->m_editorLayer->removeObject(source, false);
        }

        if (m_fields->m_savedObjects) { m_fields->m_savedObjects->release(); m_fields->m_savedObjects = nullptr; }
        this->m_selectedObjects->removeAllObjects();
        this->updateButtons();
    }

    void onAutoDecoClicked(CCObject*) {
        auto selected = this->m_selectedObjects;
        if (!selected || selected->count() == 0) {
            FLAlertLayer::create("AUTO DECO", "Select at least ONE layout block first!", "OK")->show();
            return;
        }
        auto fullStructure = CCArray::create();
        auto allObjectsArr = this->m_editorLayer->getAllObjects();
        if (!allObjectsArr) return;
        std::queue<GameObject*> openSet; std::set<GameObject*> visitedSet;
        for (int i = 0; i < selected->count(); i++) {
            auto obj = static_cast<GameObject*>(selected->objectAtIndex(i));
            if (obj && obj->m_objectID <= 500) { openSet.push(obj); visitedSet.insert(obj); fullStructure->addObject(obj); }
        }
        while (!openSet.empty()) {
            auto current = openSet.front(); openSet.pop();
            CCPoint currentPos = current->getPosition();
            for (int k = 0; k < allObjectsArr->count(); k++) {
                auto potentialNeighbor = static_cast<GameObject*>(allObjectsArr->objectAtIndex(k));
                if (!potentialNeighbor || potentialNeighbor->m_objectID > 500) continue;
                if (visitedSet.find(potentialNeighbor) != visitedSet.end()) continue;
                CCPoint neighborPos = potentialNeighbor->getPosition();
                float deltaX = std::fabs(currentPos.x - neighborPos.x); float deltaY = std::fabs(currentPos.y - neighborPos.y);
                if ((deltaX <= 32.0f && deltaY < 5.0f) || (deltaY <= 32.0f && deltaX < 5.0f)) {
                    visitedSet.insert(potentialNeighbor); openSet.push(potentialNeighbor); fullStructure->addObject(potentialNeighbor);
                }
            }
        }
        if (m_fields->m_savedObjects) m_fields->m_savedObjects->release();
        m_fields->m_savedObjects = fullStructure; m_fields->m_savedObjects->retain();
        this->m_selectedObjects->removeAllObjects();
        for (int i = 0; i < fullStructure->count(); i++) { this->m_selectedObjects->addObject(fullStructure->objectAtIndex(i)); }
        this->updateButtons();
        this->decorateStructure(m_fields->m_savedObjects);
    }
};
