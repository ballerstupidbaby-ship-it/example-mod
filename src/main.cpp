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
        int count = selected ? selected->count() : 0;

        if (count == 0) {
            FLAlertLayer::create(
                "Auto Deco",
                "Select some blocks first!",
                "OK"
            )->show();
            return;
        }

        int created = 0;

        for (unsigned int i = 0; i < selected->count(); i++) {
            auto object = static_cast<GameObject*>(
                selected->objectAtIndex(i)
            );

            if (!object)
                continue;

            auto deco = GameObject::createWithKey(1);

            if (!deco)
                continue;

            auto pos = object->getPosition();

            deco->setPosition(
                pos.x + object->getScaledContentSize().width + 15.f,
                pos.y
            );

            deco->setScale(object->getScale());
            deco->setRotation(object->getRotation());

            this->m_editorLayer->addObject(deco);
            created++;
        }

        FLAlertLayer::create(
            "Auto Deco",
            fmt::format(
                "Decorated {} of {} selected objects.",
                created,
                count
            ).c_str(),
            "OK"
        )->show();
    }
};
