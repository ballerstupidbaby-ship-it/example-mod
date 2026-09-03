#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify(AutoDecoEditorUI, EditorUI) {

    // =========================
    // CREATE DECORATION OBJECT (STABLE 2.2081 FIX)
    // =========================
    GameObject* make(
        int id,
        CCPoint pos,
        float scale,
        int baseChannel,    // Color channel ID (e.g. 1)
        int detailChannel,  // Color channel ID (e.g. 2)
        float rotation = 0.0f
    ) {
        auto obj = this->m_editorLayer->createObject(id, pos, false);

        if (!obj)
            return nullptr;

        obj->setScale(scale);
        obj->setRotation(rotation);

        // Uses 2.2081 stable pointers to map color choices safely
        if (obj->m_baseColorCode) {
            obj->m_baseColorCode->m_colorID = baseChannel;
        }
        if (obj->m_detailColorCode) {
            obj->m_detailColorCode->m_colorID = detailChannel;
        }

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

        // Offset the generated decoration block to the right side
        float x = p.x + (60.0f * s);
        float y = p.y;

        // Channel 1 = Main Color (Set this to Dark Purple in your level options!)
        // Channel 2 = Detail Color (Set this to Light Neon Purple in your level options!)

        // 1. Background Fill Layer (ID 210 - Solid Square)
        make(210, CCPoint(x, y), s * 1.0f, 1, 1);

        // 2. Tech Grid Texture (ID 1006)
        make(1006, CCPoint(x, y), s * 0.95f, 2, 1);

        // 3. 3D Volumetric Depth Frame (ID 239 - Solid Frame)
        make(239, CCPoint(x + 6.0f * s, y - 6.0f * s), s * 1.0f, 1, 1);

        // 4. Main Front Outline Frame (ID 239)
        make(239, CCPoint(x, y), s * 1.0f, 2, 2);

        // 5. Heavy Outer Edge Glow (ID 211)
        make(211, CCPoint(x - 15.0f * s, y), s * 1.0f, 2, 2, 90);  // Left Edge
        make(211, CCPoint(x + 15.0f * s, y), s * 1.0f, 2, 2, 270); // Right Edge
        make(211, CCPoint(x, y + 15.0f * s), s * 1.0f, 2, 2, 180); // Top Edge
        make(211, CCPoint(x, y - 15.0f * s), s * 1.0f, 2, 2, 0);   // Bottom Edge

        // 6. Cyber Core Center (ID 1825 - Crosshair node)
        make(1825, CCPoint(x, y), s * 0.40f, 2, 2);
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
