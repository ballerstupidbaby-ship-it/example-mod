#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify(AutoDecoEditorUI, EditorUI) {

    // =========================
    // COLORS
    // =========================

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


    // =========================
    // CREATE DECORATION OBJECT
    // =========================

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

        obj->updateMainColor(mainColor);
        obj->updateSecondaryColor(detailColor);

        obj->setObjectColor(mainColor);
        obj->setGlowColor(mainColor);
        obj->setChildColor(detailColor);

        return obj;
    }


    // =========================
    // EDITOR BUTTON
    // =========================

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


    // =========================
    // MAKE THE 3D DECORATION
    // =========================

    void decorate(GameObject* source) {

        if (!source)
            return;

        auto p = source->getPosition();

        float s = source->getScale();

        if (s <= 0.0f)
            s = 1.0f;


        // Put the decoration to the right
        float x = p.x + (55.0f * s);
        float y = p.y;


        // -------------------------
        // BACK LAYER
        // -------------------------

        make(
            207,
            CCPoint(x + 10.0f * s, y - 10.0f * s),
            s * 0.30f,
            dark(),
            darkPurple()
        );

        make(
            207,
            CCPoint(x + 20.0f * s, y - 20.0f * s),
            s * 0.30f,
            dark(),
            darkPurple()
        );


        // -------------------------
        // 3D CORNERS
        // -------------------------

        make(
            511,
            CCPoint(x + 18.0f * s, y + 18.0f * s),
            s * 0.25f,
            purple(),
            darkPurple()
        );

        make(
            511,
            CCPoint(x + 18.0f * s, y - 18.0f * s),
            s * 0.25f,
            purple(),
            darkPurple(),
            90
        );

        make(
            510,
            CCPoint(x - 18.0f * s, y - 18.0f * s),
            s * 0.25f,
            neon(),
            darkPurple(),
            180
        );


        // -------------------------
        // OUTER FRAME
        // -------------------------

        make(
            506,
            CCPoint(x - 18.0f * s, y + 18.0f * s),
            s * 0.25f,
            neon(),
            purple()
        );

        make(
            507,
            CCPoint(x, y + 18.0f * s),
            s * 0.25f,
            neon(),
            purple()
        );

        make(
            509,
            CCPoint(x + 18.0f * s, y + 18.0f * s),
            s * 0.25f,
            pink(),
            purple()
        );

        make(
            506,
            CCPoint(x - 18.0f * s, y),
            s * 0.25f,
            neon(),
            purple(),
            270
        );

        make(
            509,
            CCPoint(x + 18.0f * s, y),
            s * 0.25f,
            pink(),
            purple(),
            90
        );

        make(
            506,
            CCPoint(x - 18.0f * s, y - 18.0f * s),
            s * 0.25f,
            purple(),
            darkPurple(),
            180
        );

        make(
            507,
            CCPoint(x, y - 18.0f * s),
            s * 0.25f,
            purple(),
            darkPurple(),
            180
        );


        // -------------------------
        // DARK CENTER
        // -------------------------

        make(
            207,
            CCPoint(x, y),
            s * 0.30f,
            dark(),
            darkPurple()
        );


        // -------------------------
        // INNER FRAME
        // -------------------------

        make(
            524,
            CCPoint(x - 9.0f * s, y + 9.0f * s),
            s * 0.15f,
            purple(),
            darkPurple()
        );

        make(
            525,
            CCPoint(x, y + 9.0f * s),
            s * 0.15f,
            neon(),
            purple()
        );

        make(
            527,
            CCPoint(x + 9.0f * s, y + 9.0f * s),
            s * 0.15f,
            pink(),
            purple()
        );


        // -------------------------
        // CENTER PANEL
        // -------------------------

        make(
            210,
            CCPoint(x, y),
            s * 0.13f,
            purple(),
            neon()
        );

        make(
            220,
            CCPoint(x, y),
            s * 0.065f,
            cyan(),
            white()
        );


        // -------------------------
        // SMALL TECH NODES
        // -------------------------

        make(
            220,
            CCPoint(x - 20.0f * s, y + 20.0f * s),
            s * 0.045f,
            cyan(),
            white()
        );

        make(
            220,
            CCPoint(x + 20.0f * s, y + 20.0f * s),
            s * 0.045f,
            pink(),
            white()
        );

        make(
            220,
            CCPoint(x - 20.0f * s, y - 20.0f * s),
            s * 0.045f,
            purple(),
            cyan()
        );

        make(
            220,
            CCPoint(x + 20.0f * s, y - 20.0f * s),
            s * 0.045f,
            cyan(),
            purple()
        );


        // -------------------------
        // CORNER HIGHLIGHTS
        // -------------------------

        make(
            228,
            CCPoint(x + 21.0f * s, y - 21.0f * s),
            s * 0.12f,
            neon(),
            purple()
        );

        make(
            242,
            CCPoint(x - 21.0f * s, y + 21.0f * s),
            s * 0.10f,
            white(),
            neon(),
            45
        );
    }


    // =========================
    // AUTO DECO BUTTON
    // =========================

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


        if (selected->count() > 20) {

            FLAlertLayer::create(
                "AUTO DECO",
                "Select 20 or fewer blocks at once.",
                "OK"
            )->show();

            return;
        }


        // Decorate every selected block
        for (int i = 0; i < selected->count(); i++) {

            auto block = static_cast<GameObject*>(
                selected->objectAtIndex(i)
            );

            if (block)
                decorate(block);
        }


        // Confirmation
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
