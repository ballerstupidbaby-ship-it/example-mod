#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify(AutoDecoEditorUI, EditorUI) {

    // =========================================================
    // COLORS
    // =========================================================

    ccColor3B dark() {
        return {8, 5, 15};
    }

    ccColor3B darkPurple() {
        return {35, 5, 55};
    }

    ccColor3B purple() {
        return {150, 0, 255};
    }

    ccColor3B neonPurple() {
        return {235, 30, 255};
    }

    ccColor3B pink() {
        return {255, 40, 190};
    }

    ccColor3B cyan() {
        return {0, 220, 255};
    }

    ccColor3B bright() {
        return {210, 255, 255};
    }

    // =========================================================
    // CREATE OBJECT
    // =========================================================

    GameObject* make(
        int id,
        CCPoint position,
        float scale,
        ccColor3B color,
        float rotation = 0.0f,
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

        // IMPORTANT:
        // Set BOTH colors.
        object->setColor(color);
        object->setChildColor(color);

        object->setRotation(rotation);
        object->setOpacity(opacity);

        return object;
    }

    // =========================================================
    // BUTTON
    // =========================================================

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

    // =========================================================
    // 3D SHADOW
    // =========================================================

    void addDepth(CCPoint p, float s) {

        // Offset layers create the fake extrusion.

        make(
            207,
            CCPoint(p.x + 18, p.y - 12),
            s * 1.20f,
            dark(),
            0,
            180
        );

        make(
            207,
            CCPoint(p.x + 14, p.y - 9),
            s * 1.18f,
            darkPurple(),
            0,
            180
        );

        make(
            207,
            CCPoint(p.x + 10, p.y - 6),
            s * 1.16f,
            purple(),
            0,
            200
        );
    }

    // =========================================================
    // BIG OUTER FRAME
    // =========================================================

    void addOuterFrame(CCPoint p, float s) {

        // top-left corner
        make(
            506,
            CCPoint(p.x + 25, p.y + 25),
            s * 0.90f,
            neonPurple(),
            0
        );

        // top
        make(
            507,
            CCPoint(p.x + 45, p.y + 25),
            s * 0.90f,
            neonPurple(),
            0
        );

        // top-right
        make(
            509,
            CCPoint(p.x + 65, p.y + 25),
            s * 0.90f,
            pink(),
            0
        );

        // right side
        make(
            511,
            CCPoint(p.x + 65, p.y + 5),
            s * 0.90f,
            purple(),
            90
        );

        // lower-right
        make(
            510,
            CCPoint(p.x + 65, p.y - 15),
            s * 0.90f,
            purple(),
            90
        );

        // bottom
        make(
            508,
            CCPoint(p.x + 45, p.y - 15),
            s * 0.90f,
            darkPurple(),
            180
        );

        // left side
        make(
            506,
            CCPoint(p.x + 25, p.y + 5),
            s * 0.90f,
            neonPurple(),
            270
        );
    }

    // =========================================================
    // NESTED INNER FRAME
    // =========================================================

    void addInnerFrame(CCPoint p, float s) {

        // dark inner panel
        make(
            207,
            CCPoint(p.x + 45, p.y + 5),
            s * 0.95f,
            dark(),
            0
        );

        // inner 3D frame
        make(
            524,
            CCPoint(p.x + 32, p.y + 18),
            s * 0.65f,
            purple()
        );

        make(
            525,
            CCPoint(p.x + 45, p.y + 18),
            s * 0.65f,
            neonPurple()
        );

        make(
            527,
            CCPoint(p.x + 58, p.y + 18),
            s * 0.65f,
            pink()
        );

        make(
            529,
            CCPoint(p.x + 58, p.y + 3),
            s * 0.65f,
            purple(),
            90
        );

        make(
            528,
            CCPoint(p.x + 32, p.y + 3),
            s * 0.65f,
            cyan(),
            270
        );
    }

    // =========================================================
    // MECHANICAL CENTER
    // =========================================================

    void addCenter(CCPoint p, float s) {

        // Large mechanical center
        make(
            263,
            CCPoint(p.x + 45, p.y + 5),
            s * 0.72f,
            darkPurple()
        );

        // inner colored square
        make(
            266,
            CCPoint(p.x + 45, p.y + 5),
            s * 0.52f,
            purple()
        );

        // small center
        make(
            220,
            CCPoint(p.x + 45, p.y + 5),
            s * 0.23f,
            cyan()
        );

        // bright center highlight
        make(
            220,
            CCPoint(p.x + 45, p.y + 5),
            s * 0.11f,
            bright()
        );
    }

    // =========================================================
    // SIDE TECH PANELS
    // =========================================================

    void addSidePanels(CCPoint p, float s) {

        // upper-left panel
        make(
            209,
            CCPoint(p.x + 20, p.y + 35),
            s * 0.42f,
            purple()
        );

        make(
            210,
            CCPoint(p.x + 20, p.y + 35),
            s * 0.27f,
            cyan()
        );

        // upper-right panel
        make(
            209,
            CCPoint(p.x + 70, p.y + 35),
            s * 0.42f,
            pink()
        );

        make(
            210,
            CCPoint(p.x + 70, p.y + 35),
            s * 0.27f,
            purple()
        );

        // lower-left panel
        make(
            209,
            CCPoint(p.x + 20, p.y - 25),
            s * 0.42f,
            darkPurple()
        );

        make(
            210,
            CCPoint(p.x + 20, p.y - 25),
            s * 0.27f,
            purple()
        );

        // lower-right panel
        make(
            209,
            CCPoint(p.x + 70, p.y - 25),
            s * 0.42f,
            purple()
        );

        make(
            210,
            CCPoint(p.x + 70, p.y - 25),
            s * 0.27f,
            cyan()
        );
    }

    // =========================================================
    // HEXAGON DETAILS
    // =========================================================

    void addHexagons(CCPoint p, float s) {

        make(
            229,
            CCPoint(p.x + 15, p.y + 5),
            s * 0.42f,
            cyan(),
            0,
            190
        );

        make(
            231,
            CCPoint(p.x + 76, p.y + 5),
            s * 0.35f,
            pink(),
            0,
            90
        );

        make(
            230,
            CCPoint(p.x + 20, p.y - 22),
            s * 0.32f,
            purple(),
            45
        );

        make(
            232,
            CCPoint(p.x + 72, p.y + 31),
            s * 0.28f,
            cyan(),
            45
        );
    }

    // =========================================================
    // LITTLE TECH NODES
    // =========================================================

    void addNodes(CCPoint p, float s) {

        make(
            220,
            CCPoint(p.x + 30, p.y + 29),
            s * 0.16f,
            bright()
        );

        make(
            220,
            CCPoint(p.x + 60, p.y + 29),
            s * 0.16f,
            cyan()
        );

        make(
            220,
            CCPoint(p.x + 30, p.y - 19),
            s * 0.16f,
            pink()
        );

        make(
            220,
            CCPoint(p.x + 60, p.y - 19),
            s * 0.16f,
            purple()
        );
    }

    // =========================================================
    // ANGLED HIGHLIGHTS
    // =========================================================

    void addHighlights(CCPoint p, float s) {

        make(
            227,
            CCPoint(p.x + 28, p.y + 28),
            s * 0.28f,
            bright(),
            45,
            230
        );

        make(
            227,
            CCPoint(p.x + 62, p.y + 28),
            s * 0.28f,
            cyan(),
            45,
            230
        );

        make(
            228,
            CCPoint(p.x + 62, p.y - 18),
            s * 0.24f,
            pink(),
            45,
            230
        );
    }

    // =========================================================
    // SMALL PIPE CONNECTIONS
    // =========================================================

    void addPipes(CCPoint p, float s) {

        make(
            237,
            CCPoint(p.x + 12, p.y + 20),
            s * 0.30f,
            cyan(),
            0,
            90
        );

        make(
            237,
            CCPoint(p.x + 78, p.y + 20),
            s * 0.30f,
            pink(),
            0,
            90
        );

        make(
            238,
            CCPoint(p.x + 13, p.y - 15),
            s * 0.30f,
            purple()
        );

        make(
            238,
            CCPoint(p.x + 77, p.y - 15),
            s * 0.30f,
            cyan()
        );
    }

    // =========================================================
    // FULL DECORATION
    // =========================================================

    void decorate(GameObject* source) {

        if (!source)
            return;

        auto p = source->getPosition();
        float s = source->getScale();

        if (s <= 0.0f)
            s = 1.0f;

        // Everything is shifted RIGHT of the selected block.

        CCPoint base(
            p.x + 30.0f,
            p.y
        );

        addDepth(base, s);
        addOuterFrame(base, s);
        addInnerFrame(base, s);
        addCenter(base, s);
        addSidePanels(base, s);
        addHexagons(base, s);
        addNodes(base, s);
        addHighlights(base, s);
        addPipes(base, s);
    }

    // =========================================================
    // AUTO DECO
    // =========================================================

    void onAutoDeco(CCObject*) {

        auto selected = this->m_selectedObjects;

        if (!selected || selected->count() == 0) {

            FLAlertLayer::create(
                "AUTO DECO",
                "Select some blocks first!",
                "OK"
            )->show();

            return;
        }

        if (selected->count() > 15) {

            FLAlertLayer::create(
                "AUTO DECO",
                "Select 15 or fewer blocks at once.",
                "OK"
            )->show();

            return;
        }

        for (int i = 0; i < selected->count(); i++) {

            auto block = static_cast<GameObject*>(
                selected->objectAtIndex(i)
            );

            decorate(block);
        }

        auto message = fmt::format(
            "Applied futuristic 3D deco to {} block{}!",
            selected->count(),
            selected->count() == 1 ? "" : "s"
        );

        FLAlertLayer::create(
            "AUTO DECO",
            message.c_str(),
            "OK"
        )->show();
    }
};
