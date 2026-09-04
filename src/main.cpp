#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <vector>

using namespace geode::prelude;

class $modify(AutoDecoEditorUI, EditorUI) {

    // =========================
    // CREATE DECORATION OBJECT (STABLE MULTI-LAYER RECONSTRUCTION)
    // =========================
    GameObject* make(
        int id,
        CCPoint pos,
        float scale,
        int baseChannel,
        int detailChannel,
        ZLayer layerGroup,
        int zOrderOffset,
        float rotation = 0.0f
    ) {
        auto obj = this->m_editorLayer->createObject(id, pos, false);
        if (!obj) return nullptr;

        obj->setScale(scale);
        obj->setRotation(rotation);

        // Safe Geode color channel assignment parameters
        if (obj->m_baseColor) {
            obj->m_baseColor->m_colorID = baseChannel;
        }
        if (obj->m_detailColor) {
            obj->m_detailColor->m_colorID = detailChannel;
        }

        obj->m_zLayer = layerGroup;
        obj->m_zOrder = zOrderOffset;

        return obj;
    }

    // =========================
    // EDITOR BUTTON INITIALIZATION
    // =========================
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) return false;

        auto menu = CCMenu::create();
        menu->setPosition(0, 0);

        auto buttonSprite = ButtonSprite::create(
            "AUTO DECO", 60, true, "goldFont.fnt", "GJ_button_01.png", 25, 0.6f
        );

        auto button = CCMenuItemSpriteExtra::create(
            buttonSprite, this, menu_selector(AutoDecoEditorUI::onAutoDeco)
        );
        button->setPosition(100, 100);

        menu->addChild(button);
        this->addChild(menu);
        return true;
    }

    // =========================
    // ADVANCED STRUCTURE PROCESSING
    // =========================
    void decorateStructure(CCArray* selectedObjects) {
        std::vector<GameObject*> blocks;
        for (int i = 0; i < selectedObjects->count(); i++) {
            auto obj = static_cast<GameObject*>(selectedObjects->objectAtIndex(i));
            if (obj && obj->m_objectID <= 500) {
                blocks.push_back(obj);
            }
        }

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

            // Neighbor grid scanning (30 unit boundary)
            for (auto target : blocks) {
                if (target == source) continue;
                auto tp = target->getPosition();
                
                if (fabs(tp.y - y) < 5.0f) {
                    if (tp.x < x && tp.x >= x - 32.0f) hasLeft = true;
                    if (tp.x > x && tp.x <= x + 32.0f) hasRight = true;
                }
                if (fabs(tp.x - x) < 5.0f) {
                    if (tp.y > y && tp.y <= y + 32.0f) hasTop = true;
                    if (tp.y < y && tp.y >= y - 32.0f) hasBottom = true;
                }
            }

            // --------------------------------------------------
            // CYBER TECH HOLLOW WIREFRAME COMPOSITION
            // --------------------------------------------------
            
            // 1. Core Background Base Fill (A thin grid layout plate instead of a massive solid brick)
            make(1006, CCPoint(x, y), s * 0.95f, 1, 1, ZLayer::B2, 1);

            // 2. Interior Tech Core Matrix (Only spawns inside full solid segments)
            if (hasLeft && hasRight && hasTop && hasBottom) {
                make(1006, CCPoint(x, y), s * 0.90f, 2, 1, ZLayer::B1, 1);
                make(1825, CCPoint(x, y), s * 0.40f, 2, 2, ZLayer::T1, 15); // Center crosshair
            } else {
                make(1006, CCPoint(x, y), s * 0.60f, 2, 1, ZLayer::B1, 1);
            }

            // 3. Main Outline Frame Rules (Swapped solid blocks for fine line pipe frames)
            if (!hasTop && !hasLeft) {
                // Top-Left External Corner Line Frame (ID 240)
                make(240, CCPoint(x, y), s * 1.0f, 2, 2, ZLayer::T1, 5, 0.0f);
                // Tech Spine Spikes
                make(398, CCPoint(x - 8.0f * s, y + 8.0f * s), s * 0.5f, 1, 1, ZLayer::B1, -1, 45.0f);
            }
            else if (!hasTop && !hasRight) {
                // Top-Right External Corner Line Frame (ID 240 rotated)
                make(240, CCPoint(x, y), s * 1.0f, 2, 2, ZLayer::T1, 5, 90.0f);
                make(398, CCPoint(x + 8.0f * s, y + 8.0f * s), s * 0.5f, 1, 1, ZLayer::B1, -1, 135.0f);
            }
            else if (!hasBottom && !hasLeft) {
                // Bottom-Left External Corner Line Frame
                make(240, CCPoint(x, y), s * 1.0f, 2, 2, ZLayer::T1, 5, 270.0f);
            }
            else if (!hasBottom && !hasRight) {
                // Bottom-Right External Corner Line Frame
                make(240, CCPoint(x, y), s * 1.0f, 2, 2, ZLayer::T1, 5, 180.0f);
            }
            else if (!hasTop || !hasBottom || !hasLeft || !hasRight) {
                // Exposed Straight Edges (Use straight thin frame line ID 239)
                float edgeRot = 0.0f;
                if (!hasLeft || !hasRight) edgeRot = 90.0f; // Align vertical vs horizontal
                make(239, CCPoint(x, y), s * 1.0f, 2, 2, ZLayer::T1, 5, edgeRot);
            }

            // 4. Ambient Laser Glow Lining (Only fires outwards into open air space)
            if (!hasLeft)   make(211, CCPoint(x - 15.0f * s, y), s * 1.0f, 2, 2, ZLayer::T1, 10, 90);
            if (!hasRight)  make(211, CCPoint(x + 15.0f * s, y), s * 1.0f, 2, 2, ZLayer::T1, 10, 270);
            if (!hasTop)    make(211, CCPoint(x, y + 15.0f * s), s * 1.0f, 2, 2, ZLayer::T1, 10, 180);
            if (!hasBottom) make(211, CCPoint(x, y - 15.0f * s), s * 1.0f, 2, 2, ZLayer::T1, 10, 0);

            // 5. Dimensional Volumetric Projections (Slightly offset background laser copy)
            if (!hasTop || !hasBottom || !hasLeft || !hasRight) {
                make(239, CCPoint(x + 4.0f * s, y - 4.0f * s), s * 1.0f, 1, 1, ZLayer::B1, 2, 0.0f);
            }
        }
    }

    // =========================
    // AUTO DECO BUTTON TRIGGER
    // =========================
    void onAutoDeco(CCObject*) {
        auto selected = this->m_selectedObjects;

        if (!selected || selected->count() == 0) {
            FLAlertLayer::create("AUTO DECO", "Select some blocks first!", "OK")->show();
            return;
        }

        this->decorateStructure(selected);
    }
};
