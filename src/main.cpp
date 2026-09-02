#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify(AutoDecoEditorUI, EditorUI) {
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

    void onAutoDeco(CCObject*) {
        auto selected = this->m_selectedObjects;

        if (!selected || selected->count() == 0) {
            FLAlertLayer::create(
                "Auto Deco",
                "Select some blocks first!",
                "OK"
            )->show();
            return;
        }

        for (int i = 0; i < selected->count(); i++) {
            auto block = static_cast<GameObject*>(
                selected->objectAtIndex(i)
            );

            if (!block)
                continue;

            auto pos = block->getPosition();

            // Main 3D side piece
            this->m_editorLayer->createObject(
                207,
                CCPoint(pos.x + 18.0f, pos.y - 10.0f),
                false
            );

            // Lower 3D piece
            this->m_editorLayer->createObject(
                208,
                CCPoint(pos.x + 9.0f, pos.y - 18.0f),
                false
            );
        }

        FLAlertLayer::create(
            "Auto Deco",
            "3D decoration added!",
            "OK"
        )->show();
    }
};
