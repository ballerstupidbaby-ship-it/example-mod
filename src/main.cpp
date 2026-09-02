#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify(AutoDecoEditorUI, EditorUI) {
    static constexpr int DEPTH = 7;
    static constexpr float DX = 3.0f;
    static constexpr float DY = -2.0f;

    static constexpr int MAIN = 207;
    static constexpr int MAIN_TOP = 208;
    static constexpr int CORNER = 209;
    static constexpr int INNER = 210;
    static constexpr int HIGHLIGHT = 227;

    ccColor3B bright() {
        return {210, 90, 255};
    }

    ccColor3B purple() {
        return {155, 45, 235};
    }

    ccColor3B dark() {
        return {65, 15, 100};
    }

    ccColor3B shadow() {
        return {30, 5, 50};
    }

    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer))
            return false;

        auto menu = CCMenu::create();
        menu->setPosition(0, 0);

        auto sprite = ButtonSprite::create(
            "AUTO DECO",
            60,
            true,
            "goldFont.fnt",
            "GJ_button_01.png",
            25,
            0.6f
        );

        auto button = CCMenuItemSpriteExtra::create(
            sprite,
            this,
            menu_selector(AutoDecoEditorUI::onAutoDeco)
        );

        button->setPosition(100, 100);

        menu->addChild(button);
        this->addChild(menu);

        return true;
    }

    GameObject* makeObject(
        int id,
        CCPoint position,
        float scale,
        ccColor3B color,
        GLubyte opacity = 255
    ) {
        auto object = this->m_editorLayer->createObject(
            id,
            position,
            false
        );

        if (!object)
            return nullptr;

        object->setScale(scale);
        object->setChildColor(color);
        object->setOpacity(opacity);

        return object;
    }

    void createDepth(GameObject* source, int layer) {
        auto pos = source->getPosition();

        float x = pos.x + DX * layer;
        float y = pos.y + DY * layer;
        float scale = source->getScale() * 0.92f;

        ccColor3B color;

        if (layer <= 2)
            color = purple();
        else if (layer <= 5)
            color = dark();
        else
            color = shadow();

        makeObject(
            MAIN,
            CCPoint(x, y),
            scale,
            color,
            static_cast<GLubyte>(255 - layer * 10)
        );
    }

    void createFront(GameObject* source) {
        auto pos = source->getPosition();

        auto front = makeObject(
            MAIN,
            CCPoint(
                pos.x + 2.0f,
                pos.y + 1.0f
            ),
            source->getScale(),
            bright()
        );

        if (front)
            front->setRotation(0.0f);
    }

    void createTopEdge(GameObject* source) {
        auto pos = source->getPosition();

        auto top = makeObject(
            MAIN_TOP,
            CCPoint(
                pos.x + 1.5f,
                pos.y + 7.0f
            ),
            source->getScale() * 0.88f,
            {235, 150, 255},
            245
        );

        if (top)
            top->setRotation(-1.0f);
    }

    void createBottomShadow(GameObject* source) {
        auto pos = source->getPosition();

        auto bottom = makeObject(
            INNER,
            CCPoint(
                pos.x + 5.0f,
                pos.y - 7.0f
            ),
            source->getScale() * 0.90f,
            shadow(),
            210
        );

        if (bottom)
            bottom->setRotation(1.0f);
    }

    void createEdgeDetails(GameObject* source) {
        auto pos = source->getPosition();
        float scale = source->getScale() * 0.72f;

        auto right = makeObject(
            CORNER,
            CCPoint(
                pos.x + 11.0f,
                pos.y - 2.0f
            ),
            scale,
            dark(),
            235
        );

        if (right)
            right->setRotation(90.0f);

        auto bottom = makeObject(
            CORNER,
            CCPoint(
                pos.x + 7.0f,
                pos.y - 10.0f
            ),
            scale,
            shadow(),
            220
        );

        if (bottom)
            bottom->setRotation(180.0f);
    }

    void createHighlights(GameObject* source) {
        auto pos = source->getPosition();
        float scale = source->getScale() * 0.55f;

        auto shine = makeObject(
            HIGHLIGHT,
            CCPoint(
                pos.x - 5.0f,
                pos.y + 7.0f
            ),
            scale,
            {255, 225, 255},
            210
        );

        if (shine)
            shine->setRotation(-10.0f);

        auto shine2 = makeObject(
            HIGHLIGHT,
            CCPoint(
                pos.x + 4.0f,
                pos.y + 3.0f
            ),
            scale * 0.65f,
            bright(),
            170
        );

        if (shine2)
            shine2->setRotation(25.0f);
    }

    void createCornerDetail(GameObject* source) {
        auto pos = source->getPosition();

        auto corner = makeObject(
            CORNER,
            CCPoint(
                pos.x - 6.0f,
                pos.y - 6.0f
            ),
            source->getScale() * 0.48f,
            dark(),
            230
        );

        if (corner)
            corner->setRotation(45.0f);
    }

    void decorate(GameObject* source) {
        if (!source)
            return;

        for (int i = DEPTH; i >= 1; i--)
            createDepth(source, i);

        createFront(source);
        createTopEdge(source);
        createBottomShadow(source);
        createEdgeDetails(source);
        createCornerDetail(source);
        createHighlights(source);
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

        if (count > 50) {
            FLAlertLayer::create(
                "Auto Deco",
                "Select 50 or fewer objects at once.",
                "OK"
            )->show();
            return;
        }

        for (int i = 0; i < count; i++) {
            auto object = static_cast<GameObject*>(
                selected->objectAtIndex(i)
            );

            if (object)
                decorate(object);
        }

        auto message = fmt::format(
            "Applied detailed 3D deco to {} objects!",
            count
        );

        FLAlertLayer::create(
            "Auto Deco",
            message.c_str(),
            "OK"
        )->show();
    }
};
