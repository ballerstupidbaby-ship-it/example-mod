#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify(AutoDecoEditorUI, EditorUI) {

    // ============================================================
    // AUTO DECO CONFIG
    // ============================================================

    static constexpr int DEPTH = 7;

    // 3D direction
    static constexpr float DX = 3.0f;
    static constexpr float DY = -2.0f;

    // Main block pieces
    static constexpr int MAIN = 207;
    static constexpr int MAIN_TOP = 208;

    // Corner / inner pieces
    static constexpr int CORNER = 209;
    static constexpr int INNER = 210;

    // Highlight
    static constexpr int HIGHLIGHT = 227;

    // ============================================================
    // PURPLE COLOR PALETTE
    // ============================================================

    ccColor3B purpleBright() {
        return ccColor3B{210, 90, 255};
    }

    ccColor3B purple() {
        return ccColor3B{155, 45, 235};
    }

    ccColor3B purpleDark() {
        return ccColor3B{65, 15, 100};
    }

    ccColor3B purpleShadow() {
        return ccColor3B{30, 5, 50};
    }

    // ============================================================
    // INIT
    // ============================================================

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

    // ============================================================
    // CREATE OBJECT HELPER
    // ============================================================

    GameObject* makeObject(
        int id,
        CCPoint position,
        float scale,
        ccColor3B color,
        GLubyte opacity = 255
    ) {

        auto object =
            this->m_editorLayer->createObject(
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

    // ============================================================
    // CREATE 3D BACK DEPTH
    // ============================================================

    void createDepth(
        GameObject* source,
        int layer
    ) {

        auto pos = source->getPosition();

        float scale =
            source->getScale() *
            0.92f;

        float x =
            pos.x +
            (DX * layer);

        float y =
            pos.y +
            (DY * layer);

        float t =
            static_cast<float>(layer) /
            static_cast<float>(DEPTH);

        ccColor3B color;

        if (t < 0.35f) {

            color = purple();

        } else if (t < 0.7f) {

            color = purpleDark();

        } else {

            color = purpleShadow();
        }

        auto depth = makeObject(
            MAIN,
            CCPoint(x, y),
            scale,
            color,
            static_cast<GLubyte>(
                255.0f - (layer * 10.0f)
            )
        );

        if (!depth)
            return;

        // Slight perspective rotation
        depth->setRotation(
            -0.5f * layer
        );
    }

    // ============================================================
    // TOP / FRONT FACE
    // ============================================================

    void createFront(GameObject* source) {

        auto pos = source->getPosition();

        float scale =
            source->getScale();

        auto front = makeObject(
            MAIN,
            CCPoint(
                pos.x + 2.0f,
                pos.y + 1.0f
            ),
            scale,
            purpleBright()
        );

        if (!front)
            return;

        front->setRotation(0);
    }

    // ============================================================
    // TOP EDGE
    // ============================================================

    void createTopEdge(GameObject* source) {

        auto pos = source->getPosition();

        float scale =
            source->getScale() *
            0.88f;

        auto top = makeObject(
            MAIN_TOP,
            CCPoint(
                pos.x + 1.5f,
                pos.y + 7.0f
            ),
            scale,
            ccColor3B{
                235,
