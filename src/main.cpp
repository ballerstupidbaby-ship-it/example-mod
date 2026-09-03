#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify(AutoDecoEditorUI, EditorUI) {

    // =========================================================
    // COLORS
    // =========================================================

    static ccColor3B dark() {
        return {8, 4, 14};
    }

    static ccColor3B darkPurple() {
        return {45, 5, 70};
    }

    static ccColor3B purple() {
        return {150, 0, 255};
    }

    static ccColor3B neon() {
        return {235, 25, 255};
    }

    static ccColor3B pink() {
        return {255, 45, 190};
    }

    static ccColor3B cyan() {
        return {0, 220, 255};
    }

    static ccColor3B white() {
        return {235, 220, 255};
    }

    // =========================================================
    // COLOR + OBJECT SETUP
    // =========================================================

    GameObject* make(
        int id,
        CCPoint pos,
        float scale,
        ccColor3B mainColor,
        ccColor3B detailColor,
        float rotation = 0.0f,
        GLubyte opacity = 255
    ) {
        auto obj = this->m_editorLayer->createObject(
            id,
            pos,
            false
        );

        if (!obj)
            return nullptr;

        obj->setScale(scale);
        obj->setRotation(rotation);
        obj->setOpacity(opacity);

        // Geometry Dash's actual GameObject color system.
        obj->updateMainColor(mainColor);
        obj->updateSecondaryColor(detailColor);

        // Also update the rendered object immediately.
        obj->setObjectColor(mainColor);
        obj->setGlowColor(mainColor);
        obj->setChildColor(detailColor);

        return obj;
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
    // ONE CLEAN FUTURISTIC PIECE
    // =========================================================

    void decorate(GameObject* source) {

        if (!source)
            return;

        auto p = source->getPosition();

        float s = source->getScale();

        if (s <= 0.0f)
            s = 1.0f;

        // Move the whole design to the RIGHT.
        float x = p.x + 42.0f;
        float y = p.y;

        // =====================================================
        // BACK / DARK 3D EXTRUSION
        // =====================================================

        make(
            207,
            CCPoint(x + 12, y - 10),
            s * 0.95f,
            dark(),
            darkPurple(),
            0,
            255
        );

        make(
            207,
            CCPoint(x + 8, y - 7),
            s * 0.95f,
            darkPurple(),
            purple(),
            0,
            255
        );

        // =====================================================
        // OUTER L-SHAPED FRAME
        // =====================================================

        // Top-left
        make(
            506,
            CCPoint(x - 15, y + 15),
            s * 0.72f,
            neon(),
            purple()
        );

        // Top-middle
        make(
            507,
            CCPoint(x, y + 15),
            s * 0.72f,
            neon(),
            purple()
        );

        // Top-right
        make(
            509,
            CCPoint(x + 15, y + 15),
            s * 0.72f,
            pink(),
            purple()
        );

        // Right vertical
        make(
            511,
            CCPoint(x + 15, y),
            s * 0.72f,
            purple(),
            darkPurple(),
            90
        );

        // Bottom-right
        make(
            510,
            CCPoint(x + 15, y - 15),
            s * 0.72f,
            purple(),
            darkPurple(),
            90
        );

        // Left vertical
        make(
            506,
            CCPoint(x - 15, y),
            s * 0.72f,
            neon(),
            purple(),
            270
        );

        // =====================================================
        // INNER DARK PANEL
        // =====================================================

        make(
            207,
            CCPoint(x, y),
            s * 0.72f,
            dark(),
            darkPurple()
        );

        // =====================================================
        // INNER NESTED FRAME
        // =====================================================

        make(
            524,
            CCPoint(x - 8, y + 8),
            s * 0.42f,
            purple(),
            darkPurple()
        );

        make(
            525,
            CCPoint(x, y + 8),
            s * 0.42f,
            neon(),
            purple()
        );

        make(
            527,
            CCPoint(x + 8, y + 8),
            s * 0.42f,
            pink(),
            purple()
        );

        // =====================================================
        // CENTER TECH BLOCK
        // =====================================================

        make(
            210,
            CCPoint(x, y),
            s * 0.38f,
            purple(),
            neon()
        );

        make(
            220,
            CCPoint(x, y),
            s * 0.16f,
            cyan(),
            white()
        );

        make(
            220,
            CCPoint(x, y),
            s * 0.075f,
            white(),
            cyan()
        );

        // =====================================================
        // 3D CORNER
        // =====================================================

        make(
            228,
            CCPoint(x + 17, y - 17),
            s * 0.32f,
            neon(),
            purple(),
            0
        );

        make(
            242,
            CCPoint(x - 17, y + 17),
            s * 0.28f,
            white(),
            neon(),
            45
        );

        // =====================================================
        // LITTLE TECH NODES
        // =====================================================

        make(
            220,
            CCPoint(x - 18, y + 19),
            s * 0.10f,
            cyan(),
            white()
        );

        make(
            220,
            CCPoint(x + 18, y + 19),
            s * 0.10f,
            pink(),
            white()
        );

        make(
            220,
            CCPoint(x - 18, y - 19),
            s * 0.10f,
            purple(),
            cyan()
        );

        make(
            220,
            CCPoint(x + 18, y - 19),
            s * 0.10f,
            cyan(),
            purple()
        );
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

        // Prevent accidentally creating thousands of objects.
        if (selected->count() > 20) {

            FLAlertLayer::create(
                "AUTO DECO",
                "Select 20 or fewer blocks at once.",
                "OK"
            )->show();

            return;
        }

        for (int i = 0; i < selected->count(); i++) {

            auto block = static_cast<GameObject*>(
                selected->objectAtIndex(i)
            );

            if (block)
                decorate(block);
        }

        auto text = fmt::format(
            "Added futuristic 3D deco to {} block{}!",
            selected->count(),
            selected->count() == 1 ? "" : "s"
        );

        FLAlertLayer::create(
            "AUTO DECO",
            text.c_str(),
            "OK"
        )->show();
    }
};
