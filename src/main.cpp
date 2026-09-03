#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify(AutoDecoEditorUI, EditorUI) {

    // =========================
    // CREATE DECORATION OBJECT (FIXED Z-ORDER & RENDER)
    // =========================
    GameObject* make(
        int id,
        CCPoint pos,
        float scale,
        int baseChannel,    // Color channel ID (e.g. 1)
        int detailChannel,  // Color channel ID (e.g. 2)
        int zOrder,         // Force exact layer stacking order
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

        // Force standard sprite engine initialization so colors render transparent accents
        obj->setupCustomSprites();

        // Put the object into the correct layering queue
        this->m_editorLayer->m_objectsLayer->reorderChild(obj, zOrder);

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

        // FIXED: Changed offset to 0.0f so it spawns directly over the source block!
        float x = p.x + (0.0f * s);
        float y = p.y;

        // Layer Ordering Map: 
        // Background = Lower Z (-10) -> Details = Medium Z (0) -> Frames = High Z (10)

        // 1. Background Fill Layer (ID 210 - Solid Square behind everything)
        make(210, CCPoint(x, y), s * 1.0f, 1, 1, -10);

        // 2. Tech Grid Texture (ID 1006)
        make(1006, CCPoint(x, y), s * 0.95f, 2, 1, -5);

        // 3. 3D Volumetric Depth Frame (ID 239 - Shifted Depth)
        make(239, CCPoint(x + 6.0f * s, y - 6.0f * s), s * 1.0f, 1, 1, -2);

        // 4. Main Front Outline Frame (ID 239 - Front Frame)
        make(239, CCPoint(x, y), s * 1.0f, 2, 2, 5);

        // 5. Heavy Outer Edge Glow (ID 211)
        make(211, CCPoint(x - 15.0f * s, y), s * 1.0f, 2, 2, 8, 90);  // Left Edge
        make(211, CCPoint(x + 15.0f * s, y), s * 1.0f, 2, 2, 8, 270); // Right Edge
        make(211, CCPoint(x, y + 15.0f * s), s * 1.0f, 2, 2, 8, 180); // Top Edge
        make(211, CCPoint(x, y - 15.0f * s), s * 1.0f, 2, 2, 8, 0);   // Bottom Edge

        // 6. Cyber Core Center (ID 1825 - Crosshair node on top)
        make(1825, CCPoint(x, y), s * 0.40f, 2, 2, 10);
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
