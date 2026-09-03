#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify(AutoDecoEditorUI, EditorUI) {

    // =========================
    // CREATE DECORATION OBJECT (STABLE MULTI-LAYER COMPILER FIX)
    // =========================
    GameObject* make(
        int id,
        CCPoint pos,
        float scale,
        int baseChannel,    // Color channel ID (e.g. 1)
        int detailChannel,  // Color channel ID (e.g. 2)
        int customZOrder,   // Stacking level override
        float rotation = 0.0f
    ) {
        auto obj = this->m_editorLayer->createObject(id, pos, false);

        if (!obj)
            return nullptr;

        obj->setScale(scale);
        obj->setRotation(rotation);

        // Geode 2.2 safe channel structure properties
        if (obj->m_baseColor) {
            obj->m_baseColor->m_colorID = baseChannel;
        }
        if (obj->m_detailColor) {
            obj->m_detailColor->m_colorID = detailChannel;
        }

        // Force native engine rendering layer stack properties
        obj->m_zOrder = customZOrder;

        return obj;
    }

    // =========================
    // EDITOR BUTTON
    // =========================
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer))
            return false;

        auto menu = CCMenu::create();
        menu->setPosition(0, 0);

        auto buttonSprite = ButtonSprite::create(
            "AUTO DECO",
            60,
            true,
            "goldFont.fnt",
            "GJ_button_01.png",
            25,
            0.6f
        );

        auto button = CCMenuItemSpriteExtra::create(
            buttonSprite,
            this,
            menu_selector(AutoDecoEditorUI::onAutoDeco)
        );

        button->setPosition(100, 100);

        menu->addChild(button);
        this->addChild(menu);

        return true;
    }

    // =========================
    // CYBERPUNK DECORATION ENGINE
    // =========================
    void decorate(GameObject* source) {
        if (!source)
            return;

        auto p = source->getPosition();
        float s = source->getScale();
        if (s <= 0.0f) s = 1.0f;

        float x = p.x;
        float y = p.y;

        // Custom Layer stack numbers: 
        // Background Fill = 1 (lowest) -> Grids/Depth = 2 -> Frame = 3 -> Glow/Cross = 4 (highest)

        // 1. Background Fill Layer (ID 210 - Solid Square behind everything)
        make(210, CCPoint(x, y), s * 1.0f, 1, 1, 1);

        // 2. Tech Grid Texture Overlay (ID 1006)
        make(1006, CCPoint(x, y), s * 0.95f, 2, 1, 2);

        // 3. 3D Volumetric Depth Frame (ID 239 - Spatial Shadowing Offset)
        make(239, CCPoint(x + 6.0f * s, y - 6.0f * s), s * 1.0f, 1, 1, 2);

        // 4. Main Front Outline Frame (ID 239)
        make(239, CCPoint(x, y), s * 1.0f, 2, 2, 3);

        // 5. Heavy Outer Edge Glow Elements (ID 211)
        make(211, CCPoint(x - 15.0f * s, y), s * 1.0f, 2, 2, 4, 90);  // Left
        make(211, CCPoint(x + 15.0f * s, y), s * 1.0f, 2, 2, 4, 270); // Right
        make(211, CCPoint(x, y + 15.0f * s), s * 1.0f, 2, 2, 4, 180); // Top
        make(211, CCPoint(x, y - 15.0f * s), s * 1.0f, 2, 2, 4, 0);   // Bottom

        // 6. Cyber Core Center Vertex (ID 1825 - Focal Crosshair node)
        make(1825, CCPoint(x, y), s * 0.40f, 2, 2, 4);
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

        for (int i = 0; i < selected->count(); i++) {
            auto block = static_cast<GameObject*>(selected->objectAtIndex(i));
            this->decorate(block);
        }
    }
};
