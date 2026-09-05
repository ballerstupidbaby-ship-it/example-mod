#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class DraggablePanel : public CCLayer {
protected:
    CCMenu* m_menu = nullptr;
    bool m_dragging = false;
    CCPoint m_touchOffset = CCPoint(0, 0);

public:
    static DraggablePanel* create(CCMenu* menu) {
        auto ret = new DraggablePanel();

        if (ret && ret->init()) {
            ret->m_menu = menu;
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }

    bool init() {
        if (!CCLayer::init())
            return false;

        this->setTouchEnabled(true);

        return true;
    }

    void registerWithTouchDispatcher() {
        CCDirector::sharedDirector()
            ->getTouchDispatcher()
            ->addTargetedDelegate(this, -500, true);
    }

    bool ccTouchBegan(CCTouch* touch, CCEvent*) {
        if (!m_menu)
            return false;

        auto location = this->convertTouchToNodeSpace(touch);

        // The draggable area is the top part of the panel.
        if (location.y < 20.0f || location.y > 75.0f)
            return false;

        auto panelPosition = m_menu->getPosition();

        m_touchOffset = CCPoint(
            location.x - panelPosition.x,
            location.y - panelPosition.y
        );

        m_dragging = true;

        return true;
    }

    void ccTouchMoved(CCTouch* touch, CCEvent*) {
        if (!m_dragging || !m_menu)
            return;

        auto location = this->convertTouchToNodeSpace(touch);

        CCPoint newPosition(
            location.x - m_touchOffset.x,
            location.y - m_touchOffset.y
        );

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        // Keep the panel on screen.
        float minX = 20.0f;
        float minY = 20.0f;

        float maxX = winSize.width - 250.0f;
        float maxY = winSize.height - 20.0f;

        newPosition.x = std::max(minX, newPosition.x);
        newPosition.y = std::max(minY, newPosition.y);

        newPosition.x = std::min(maxX, newPosition.x);
        newPosition.y = std::min(maxY, newPosition.y);

        m_menu->setPosition(newPosition);
    }

    void ccTouchEnded(CCTouch*, CCEvent*) {
        m_dragging = false;
    }

    void ccTouchCancelled(CCTouch*, CCEvent*) {
        m_dragging = false;
    }
};


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
        DraggablePanel* dragPanel = nullptr;
    };

    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer))
            return false;

        log::info("Auto Deco loaded");

        auto fields = m_fields.self();

        auto menu = CCMenu::create();

        if (!menu)
            return false;

        menu->setPosition(
            CCDirector::sharedDirector()->getWinSize().width - 270.0f,
            CCDirector::sharedDirector()->getWinSize().height / 2.0f
        );

        fields->menu = menu;

        // SAVE BUTTON
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

        saveButton->setPosition(45, 40);
        menu->addChild(saveButton);

        // DECO BUTTON
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

        decoButton->setPosition(125, 40);
        menu->addChild(decoButton);

        // DRAG AREA
        auto dragSprite = ButtonSprite::create(
            "DRAG",
            50,
            true,
            "goldFont.fnt",
            "GJ_button_03.png",
            25,
            0.6f
        );

        auto dragButton = CCMenuItemSpriteExtra::create(
            dragSprite,
            this,
            menu_selector(AutoDecoEditorUI::onDragButton)
        );

        dragButton->setPosition(205, 40);
        menu->addChild(dragButton);

        this->addChild(menu);

        // Touch layer for dragging.
        auto dragPanel = DraggablePanel::create(menu);

        if (dragPanel) {
            dragPanel->setContentSize(
                CCDirector::sharedDirector()->getWinSize()
            );

            dragPanel->setPosition(0, 0);

            fields->dragPanel = dragPanel;

            this->addChild(dragPanel);
        }

        return true;
    }

    void onDragButton(CCObject*) {
        FLAlertLayer::create(
            "Auto Deco",
            "Drag the DRAG button to move the panel.",
            "OK"
        )->show();
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

        auto anchor = static_cast<GameObject*>(
            selected->objectAtIndex(0)
        );

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
            auto object = static_cast<GameObject*>(
                selected->objectAtIndex(i)
            );

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

        FLAlertLayer::create(
            "Auto Deco",
            "Template saved successfully!",
            "OK"
        )->show();

        log::info(
            "Saved {} template objects",
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

        auto anchor = static_cast<GameObject*>(
            selected->objectAtIndex(0)
        );

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

        log::info(
            "Auto Deco created {} objects",
            created
        );

        FLAlertLayer::create(
            "Auto Deco",
            "Decoration created successfully!",
            "OK"
        )->show();
    }
};
