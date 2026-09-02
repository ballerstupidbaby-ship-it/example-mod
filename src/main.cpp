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
            auto block = static_cast<GameObject*>(selected->objectAtIndex(i));

            if (!block)
                continue;

            auto pos = block->getPosition();

            // 3D side piece
            auto side = GameObject::createWithKey(207);

            if (side) {
                side->setPosition(
                    CCPoint(
                        pos.x + 18.0f,
                        pos.y - 10.0f
                    )
                );

                side->setScale(0.85f);
                side->setRotation(0.0f);

                this->m_editorLayer->addObject(side);
            }

            // Darker-looking lower piece
            auto lower = GameObject::createWithKey(208);

            if (lower) {
                lower->setPosition(
                    CCPoint(
                        pos.x + 9.0f,
                        pos.y - 18.0f
                    )
                );

                lower->setScale(0.85f);
                lower->setRotation(0.0f);

                this->m_editorLayer->addObject(lower);
            }
        }

        FLAlertLayer::create(
            "Auto Deco",
            "3D decoration added!",
            "OK"
        )->show();
    }
};
