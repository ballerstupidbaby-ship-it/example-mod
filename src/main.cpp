#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify(AutoDecoEditorUI, EditorUI) {
    struct TemplateObject {
        int objectID = 0;

        CCPoint offset = CCPoint(0, 0);

        float scale = 1.0f;
        float rotation = 0.0f;

        GLubyte opacity = 255;
    };

    struct Fields {
        std::vector<TemplateObject> templateObjects;

        bool hasTemplate = false;

        CCMenu* menu = nullptr;

        // 0 = middle-left
        // 1 = middle-right
        // 2 = top-left
        // 3 = top-right
        int menuPosition = 1;
    };

    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer))
            return false;

        log::info("Auto Deco loaded");

        auto fields = m_fields.self();

        auto menu = CCMenu::create();

        if (!menu)
            return false;

        menu->setPosition(0, 0);

        fields->menu = menu;

        // -------------------------
        // SAVE
        // -------------------------

        auto saveSprite = ButtonSprite::create(
            "SAVE",
            50,
            true,
            "goldFont.fnt",
            "GJ_button_01.png",
            25,
            0.6f
        );

        auto saveButton = CCMenuItemSpriteExtra::create(
            saveSprite,
            this,
            menu_selector(AutoDecoEditorUI::onSaveTemplate)
        );

        saveButton->setPosition(55, 40);
        menu->addChild(saveButton);

        // -------------------------
        // DECO
        // -------------------------

        auto decoSprite = ButtonSprite::create(
            "DECO",
            50,
            true,
            "goldFont.fnt",
            "GJ_button_02.png",
            25,
            0.6f
        );

        auto decoButton = CCMenuItemSpriteExtra::create(
            decoSprite,
            this,
            menu_selector(AutoDecoEditorUI::onAutoDeco)
        );

        decoButton->setPosition(135, 40);
        menu->addChild(decoButton);

        // -------------------------
        // MOVE
        // -------------------------

        auto moveSprite = ButtonSprite::create(
            "MOVE",
            50,
            true,
            "goldFont.fnt",
            "GJ_button_03.png",
            25,
            0.6f
        );

        auto moveButton = CCMenuItemSpriteExtra::create(
            moveSprite,
            this,
            menu_selector(AutoDecoEditorUI::onMoveMenu)
        );

        moveButton->setPosition(215, 40);
        menu->addChild(moveButton);

        this->addChild(menu);

        updateMenuPosition();

        return true;
    }

    void updateMenuPosition() {
        auto fields = m_fields.self();

        if (!fields->menu)
            return;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        CCPoint position;

        switch (fields->menuPosition) {
            // Middle-left
            case 0:
                position = CCPoint(
                    20,
                    (winSize.height / 2.0f) - 40.0f
                );
                break;

            // Middle-right
            case 1:
                position = CCPoint(
                    winSize.width - 255.0f,
                    (winSize.height / 2.0f) - 40.0f
                );
                break;

            // Top-left
            case 2:
                position = CCPoint(
                    20,
                    winSize.height - 130.0f
                );
                break;

            // Top-right
            case 3:
                position = CCPoint(
                    winSize.width - 255.0f,
                    winSize.height - 130.0f
                );
                break;

            default:
                position = CCPoint(
                    winSize.width - 255.0f,
                    (winSize.height / 2.0f) - 40.0f
                );
                break;
        }

        fields->menu->setPosition(position);
    }

    void onMoveMenu(CCObject*) {
        auto fields = m_fields.self();

        fields->menuPosition++;

        if (fields->menuPosition > 3)
            fields->menuPosition = 0;

        updateMenuPosition();
    }

    void onSaveTemplate(CCObject*) {
        auto selected = this->m_selectedObjects;

        if (!selected || selected->count() == 0) {
            FLAlertLayer::create(
                "Auto Deco",
                "Select your decoration objects first!",
                "OK"
            )->show();

            return;
        }

        auto fields = m_fields.self();

        fields->templateObjects.clear();

        auto anchor =
            static_cast<GameObject*>(selected->objectAtIndex(0));

        if (!anchor) {
            FLAlertLayer::create(
                "Auto Deco",
                "Couldn't find the template anchor.",
                "OK"
            )->show();

            return;
        }

        CCPoint anchorPosition = anchor->getPosition();

        for (unsigned int i = 0; i < selected->count(); i++) {
            auto object =
                static_cast<GameObject*>(selected->objectAtIndex(i));

            if (!object)
                continue;

            TemplateObject data;

            data.objectID = object->m_objectID;

            CCPoint objectPosition = object->getPosition();

            data.offset = CCPoint(
                objectPosition.x - anchorPosition.x,
                objectPosition.y - anchorPosition.y
            );

            data.scale = object->getScale();
            data.rotation = object->getRotation();
            data.opacity = object->getOpacity();

            fields->templateObjects.push_back(data);
        }

        if (fields->templateObjects.empty()) {
            FLAlertLayer::create(
                "Auto Deco",
                "No valid objects were found.",
                "OK"
            )->show();

            return;
        }

        fields->hasTemplate = true;

        auto message = fmt::format(
            "Template saved!\n{} objects",
            fields->templateObjects.size()
        );

        FLAlertLayer::create(
            "Auto Deco",
            message.c_str(),
            "OK"
        )->show();

        log::info(
            "Auto Deco saved {} objects",
            fields->templateObjects.size()
        );
    }

    void onAutoDeco(CCObject*) {
        auto fields = m_fields.self();

        if (!fields->hasTemplate ||
            fields->templateObjects.empty()) {

            FLAlertLayer::create(
                "Auto Deco",
                "Save a template first!",
                "OK"
            )->show();

            return;
        }

        auto selected = this->m_selectedObjects;

        if (!selected || selected->count() == 0) {
            FLAlertLayer::create(
                "Auto Deco",
                "Select the block you want to decorate!",
                "OK"
            )->show();

            return;
        }

        auto anchor =
            static_cast<GameObject*>(selected->objectAtIndex(0));

        if (!anchor) {
            FLAlertLayer::create(
                "Auto Deco",
                "Invalid selected block.",
                "OK"
            )->show();

            return;
        }

        CCPoint anchorPosition = anchor->getPosition();

        int created = 0;

        for (auto const& data : fields->templateObjects) {
            CCPoint position(
                anchorPosition.x + data.offset.x,
                anchorPosition.y + data.offset.y
            );

            auto object = this->m_editorLayer->createObject(
                data.objectID,
                position,
                false
            );

            if (!object)
                continue;

            object->setScale(data.scale);
            object->setRotation(data.rotation);
            object->setOpacity(data.opacity);

            created++;
        }

        auto message = fmt::format(
            "Created {} objects!",
