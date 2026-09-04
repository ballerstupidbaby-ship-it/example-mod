#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <vector>
#include <cmath>
#include <queue>
#include <set>

using namespace geode::prelude;

class $modify(AutoDecoEditorUI, EditorUI) {

    // Persistent fields to hold layout selections safely in mod session memory
    struct Fields {
        CCArray* m_savedObjects = nullptr;
    };

    // ========================================================
    // CREATE DECORATION OBJECT (STABLE CUSTOM RGB OVERRIDES)
    // ========================================================
    GameObject* make(
        int id,
        CCPoint pos,
        float scale,
        ccColor3B mainColor,
        ccColor3B detailColor,
        ZLayer layerGroup,
        int zOrderOffset,
        float rotation = 0.0f
    ) {
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

    // ========================================================
    // EDITOR BUTTON COMPONENT INITIALIZATION
    // ========================================================
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) return false;

        auto menu = CCMenu::create();
        menu->setPosition(0, 0);

        auto buttonSprite = ButtonSprite::create(
            "AUTO DECO", 60, true, "goldFont.fnt", "GJ_button_01.png", 25, 0.6f
        );

        auto button = CCMenuItemSpriteExtra::create(
            buttonSprite, this, menu_selector(AutoDecoEditorUI::onAutoDecoClicked)
        );
        button->setPosition(100, 100);

        menu->addChild(button);
        this->addChild(menu);
        return true;
    }

    // ========================================================
    // TUTORIAL-BASED MODERN TECH GENERATION ENGINE
    // ========================================================
    void decorateStructure(CCArray* selectedObjects, ccColor3B userColor) {
        std::vector<GameObject*> blocks;
        if (!selectedObjects) return;

        for (int i = 0; i < selectedObjects->count(); i++) {
            auto obj = static_cast<GameObject*>(selectedObjects->objectAtIndex(i));
            if (obj && obj->m_objectID <= 500) {
                blocks.push_back(obj);
            }
        }

        ccColor3B whiteColor = userColor; 
        ccColor3B blackColor = {10, 10, 15}; 
        ccColor3B darkGreyDetails = {50, 50, 60}; 

        for (auto source : blocks) {
            auto p = source->getPosition();
            float s = source->getScale();
            if (s <= 0.0f) s = 1.0f;

            float x = p.x;
            float y = p.y;

            bool hasLeft = false;
            bool hasRight = false;
            bool hasTop = false;
            bool hasBottom = false;

            for (auto target : blocks) {
                if (target == source) continue;
                auto tp = target->getPosition();
                
                if (std::fabs(tp.y - y) < 5.0f) {
                    if (tp.x < x && tp.x >= x - 32.0f) hasLeft = true;
                    if (tp.x > x && tp.x <= x + 32.0f) hasRight = true;
                }
                if (std::fabs(tp.x - x) < 5.0f) {
                    if (tp.y > y && tp.y <= y + 32.0f) hasTop = true;
                    if (tp.y < y && tp.y >= y - 32.0f) hasBottom = true;
                }
            }

            // WHITE FILL IN THE BLOCK & ADD BLACK OUTLINE
            make(210, CCPoint(x, y), s * 1.0f, whiteColor, whiteColor, ZLayer::B2, 1);

            if (!hasTop || !hasBottom || !hasLeft || !hasRight) {
                float rot = 0.0f;
                if (!hasLeft || !hasRight) rot = 90.0f;
                make(239, CCPoint(x, y), s * 1.0f, blackColor, blackColor, ZLayer::B1, 5, rot);
            }

            // ADD WHITE 3D OBJ & BLACK 3D FILL
            if (!hasTop || !hasBottom || !hasLeft || !hasRight) {
                make(239, CCPoint(x + 4.0f * s, y - 4.0f * s), s * 1.0f, whiteColor, whiteColor, ZLayer::B1, 2);
                make(210, CCPoint(x + 2.0f * s, y - 2.0f * s), s * 1.0f, blackColor, blackColor, ZLayer::B2, -2);
            }

            // LIGHT AND SHADOW GLOW MATRIX
            if (!hasTop)    make(211, CCPoint(x, y + 14.0f * s), s * 1.0f, whiteColor, whiteColor, ZLayer::T1, 8, 180);
            if (!hasBottom) make(211, CCPoint(x, y - 14.0f * s), s * 1.0f, blackColor, blackColor, ZLayer::B1, 3, 0);

            // INTERNAL TECH DECO OBJECTS
            if (hasLeft && hasRight && hasTop && hasBottom) {
                make(1006, CCPoint(x, y), s * 0.90f, darkGreyDetails, blackColor, ZLayer::B1, 4);
                make(1324, CCPoint(x, y), s * 0.40f, whiteColor, blackColor, ZLayer::T1, 10);
            } else {
                make(1006, CCPoint(x, y), s * 0.50f, darkGreyDetails, blackColor, ZLayer::B1, 4);
            }

            this->m_editorLayer->removeObject(source, false);
        }

        if (m_fields->m_savedObjects) {
            m_fields->m_savedObjects->release();
            m_fields->m_savedObjects = nullptr;
        }
        this->m_selectedObjects->removeAllObjects();
        this->updateButtons();
    }

    // ========================================================
    // AUTO DECO CLICK HOOK INTERCEPTOR (CRASH PROOF GEODE UI)
    // ========================================================
    void onAutoDecoClicked(CCObject*) {
        auto selected = this->m_selectedObjects;

        if (!selected || selected->count() == 0) {
            FLAlertLayer::create("AUTO DECO", "Select at least ONE layout block first!", "OK")->show();
            return;
        }

        auto fullStructure = CCArray::create();
        auto allObjectsArr = this->m_editorLayer->getAllObjects();
        
        if (!allObjectsArr) return;

        std::queue<GameObject*> openSet;
        std::set<GameObject*> visitedSet;

        for (int i = 0; i < selected->count(); i++) {
            auto obj = static_cast<GameObject*>(selected->objectAtIndex(i));
            if (obj && obj->m_objectID <= 500) {
                openSet.push(obj);
                visitedSet.insert(obj);
                fullStructure->addObject(obj);
            }
        }

        while (!openSet.empty()) {
            auto current = openSet.front();
            openSet.pop();

            CCPoint currentPos = current->getPosition();

            for (int k = 0; k < allObjectsArr->count(); k++) {
                auto potentialNeighbor = static_cast<GameObject*>(allObjectsArr->objectAtIndex(k));
                
                if (!potentialNeighbor || potentialNeighbor->m_objectID > 500) continue;
                if (visitedSet.find(potentialNeighbor) != visitedSet.end()) continue;

                CCPoint neighborPos = potentialNeighbor->getPosition();
                
                float deltaX = std::fabs(currentPos.x - neighborPos.x);
                float deltaY = std::fabs(currentPos.y - neighborPos.y);

                if ((deltaX <= 32.0f && deltaY < 5.0f) || (deltaY <= 32.0f && deltaX < 5.0f)) {
                    visitedSet.insert(potentialNeighbor);
                    openSet.push(potentialNeighbor);
                    fullStructure->addObject(potentialNeighbor);
                }
            }
        }

        if (m_fields->m_savedObjects) m_fields->m_savedObjects->release();
        m_fields->m_savedObjects = fullStructure;
        m_fields->m_savedObjects->retain();

        this->m_selectedObjects->removeAllObjects();
        for (int i = 0; i < fullStructure->count(); i++) {
            this->m_selectedObjects->addObject(fullStructure->objectAtIndex(i));
        }
        this->updateButtons();

        // FIXED: Using Geode's safe built-in visual color picker component popup overlay
        // It provides a visual color wheel completely independent of the game's level memory state.
        auto pickerPopup =ColorPickPopup::create(
            {255, 255, 255}, // Default base start color
            [this](ccColor3B pickedColor) {
                // Instantly run generation logic on chosen block structures when color is picked!
                this->decorateStructure(m_fields->m_savedObjects, pickedColor);
            }
        );

        if (pickerPopup) {
            pickerPopup->show();
        }
    }
};
