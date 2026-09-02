#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify(AutoDecoEditorUI, EditorUI) {
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer))
            return false;

        log::info("Auto Deco loaded");

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
        int count = selected ? selected->count() : 0;

        if (count == 0) {
            FLAlertLayer::create(
                "Auto Deco",
                "Select some blocks first!",
                "OK"
            )->show();
            return;
        }

        auto message = fmt::format(
            "Selected {} objects.",
            count
        );

        FLAlertLayer::create(
            "Auto Deco",
            message.c_str(),
            "OK"
        )->show();
    }
};
