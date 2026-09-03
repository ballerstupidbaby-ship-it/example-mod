#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify(AutoDecoEditorUI, EditorUI) {

    int m_style = 0;

    // =========================
    // COLORS
    // =========================

    static ccColor3B dark() {
        return {8, 4, 14};
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

    static ccColor3B white() {
        return {235, 220, 255};
    }


    // =========================
    // CREATE OBJECT
    // =========================

    GameObject* make(
        int id,
        CCPoint pos,
        float scale,
        ccColor3B mainColor,
        ccColor3B detailColor,
        float rotation = 0.0f
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

        obj->updateMainColor(mainColor);
        obj->updateSecondaryColor(detailColor);

        obj->setObjectColor(mainColor);
        obj->setGlowColor(mainColor);
        obj->setChildColor(detailColor);

        return obj;
    }


    // =========================
    // EDITOR UI
    // =========================

    bool init(LevelEditorLayer* editorLayer) {

        if (!EditorUI::init(editorLayer))
            return false;

        log::info("Auto Deco loaded");

        auto menu = CCMenu::create();
        menu->setPosition(0, 0);


        // STYLE BUTTON

        auto styleSprite = ButtonSprite::create(
            "STYLE",
            60,
            true,
            "goldFont.fnt",
            "GJ_button_01.png",
            25,
            0.6f
        );

        auto styleButton = CCMenuItemSpriteExtra::create(
            styleSprite,
            this,
            menu_selector(AutoDecoEditorUI::onStyle)
        );

        styleButton->setPosition(100, 150);

        menu->addChild(styleButton);


        // AUTO DECO BUTTON

        auto autoSprite = ButtonSprite::create(
            "AUTO DECO",
            60,
            true,
            "goldFont.fnt",
            "GJ_button_01.png",
            25,
            0.6f
        );

        auto autoButton = CCMenuItemSpriteExtra::create(
            autoSprite,
            this,
            menu_selector(AutoDecoEditorUI::onAutoDeco)
        );

        autoButton->setPosition(100, 100);

        menu->addChild(autoButton);


        this->addChild(menu);

        return true;
    }


    // =========================
    // STYLE SELECTOR
    // =========================

    void onStyle(CCObject*) {

        m_style++;

        if (m_style > 3)
            m_style = 0;

        const char* names[] = {
            "MECHANICAL",
            "ANGULAR",
            "TECH FRAME",
            "RANDOM"
        };

        auto text = fmt::format(
            "Style: {}",
            names[m_style]
        );

        FLAlertLayer::create(
            "AUTO DECO",
            text.c_str(),
            "OK"
        )->show();
    }


    // =========================
    // SMALL FRAME PIECE
    // =========================

    void frame(
        GameObject* source,
        float ox,
        float oy,
        float scale,
        int id,
        float rotation = 0.0f
    ) {
        if (!source)
            return;

        auto p = source->getPosition();
        float s = source->getScale();

        if (s <= 0.0f)
            s = 1.0f;

        make(
            id,
            CCPoint(
                p.x + ox * s,
                p.y + oy * s
            ),
            scale * s,
            purple(),
            neon(),
            rotation
        );
    }


    // =========================
    // STYLE 1
    // MECHANICAL
    // =========================

    void mechanical(GameObject* source) {

        if (!source)
            return;

        auto p = source->getPosition();
        float s = source->getScale();

        if (s <= 0.0f)
            s = 1.0f;


        // BACK DEPTH

        frame(source, -24, 24, 0.18f, 237);
        frame(source, 0, 24, 0.18f, 237);
        frame(source, 24, 24, 0.18f, 237);

        frame(source, -24, -24, 0.18f, 238, 90);
        frame(source, 24, -24, 0.18f, 238, 90);


        // MAIN OUTER STRUCTURE

        frame(source, -24, 0, 0.18f, 237);
        frame(source, 24, 0, 0.18f, 237);

        frame(source, -24, -24, 0.18f, 238);
        frame(source, 24, 24, 0.18f, 238);


        // INNER STRUCTURE

        frame(source, -12, 12, 0.12f, 207);
        frame(source, 12, 12, 0.12f, 207);
        frame(source, -12, -12, 0.12f, 207);
        frame(source, 12, -12, 0.12f, 207);


        // CENTER

        make(
            220,
            CCPoint(p.x, p.y),
            0.07f * s,
            white(),
            neon()
        );
    }


    // =========================
    // STYLE 2
    // ANGULAR
    // =========================

    void angular(GameObject* source) {

        if (!source)
            return;

        auto p = source->getPosition();
        float s = source->getScale();

        if (s <= 0.0f)
            s = 1.0f;


        // DIAGONAL OUTER PIECES

        frame(source, -22, 22, 0.16f, 227, 0);
        frame(source, 22, 22, 0.16f, 227, 90);

        frame(source, -22, -22, 0.16f, 227, 270);
        frame(source, 22, -22, 0.16f, 227, 180);


        // INNER ANGLES

        frame(source, -13, 13, 0.13f, 228);
        frame(source, 13, 13, 0.13f, 228, 90);

        frame(source, -13, -13, 0.13f, 228, 270);
        frame(source, 13, -13, 0.13f, 228, 180);


        // CORNER DEPTH

        frame(source, -27, 0, 0.10f, 242, 270);
        frame(source, 27, 0, 0.10f, 242, 90);

        frame(source, 0, 27, 0.10f, 242);
        frame(source, 0, -27, 0.10f, 242, 180);


        // CENTER

        make(
            220,
            CCPoint(p.x, p.y),
            0.065f * s,
            pink(),
            white()
        );
    }


    // =========================
    // STYLE 3
    // TECH FRAME
    // =========================

    void techFrame(GameObject* source) {

        if (!source)
            return;

        auto p = source->getPosition();
        float s = source->getScale();

        if (s <= 0.0f)
            s = 1.0f;


        // OUTER FRAME

        frame(source, -20, 20, 0.17f, 237);
        frame(source, 0, 20, 0.17f, 237);
        frame(source, 20, 20, 0.17f, 237);

        frame(source, -20, -20, 0.17f, 237);
        frame(source, 0, -20, 0.17f, 237);
        frame(source, 20, -20, 0.17f, 237);


        // SIDE STRUCTURE

        frame(source, -20, 0, 0.17f, 238, 90);
        frame(source, 20, 0, 0.17f, 238, 90);


        // INNER PANELS

        frame(source, -10, 10, 0.10f, 207);
        frame(source, 10, 10, 0.10f, 207);
        frame(source, -10, -10, 0.10f, 207);
        frame(source, 10, -10, 0.10f, 207);


        // TECH NODES

        make(
            220,
            CCPoint(p.x - 24 * s, p.y + 24 * s),
            0.045f * s,
            cyan(),
            white()
        );

        make(
            220,
            CCPoint(p.x + 24 * s, p.y + 24 * s),
            0.045f * s,
            pink(),
            white()
        );

        make(
            220,
            CCPoint(p.x - 24 * s, p.y - 24 * s),
            0.045f * s,
            purple(),
            white()
        );

        make(
            220,
            CCPoint(p.x + 24 * s, p.y - 24 * s),
            0.045f * s,
            neon(),
            white()
        );
    }


    // =========================
    // RANDOM
    // =========================

    void randomStyle(GameObject* source) {

        if (!source)
            return;

        int choice = rand() % 3;

        if (choice == 0)
            mechanical(source);

        else if (choice == 1)
            angular(source);

        else
            techFrame(source);
    }


    // =========================
    // DECORATE
    // =========================

    void decorate(GameObject* source) {

        if (!source)
            return;

        if (m_style == 0)
            mechanical(source);

        else if (m_style == 1)
            angular(source);

        else if (m_style == 2)
            techFrame(source);

        else
            randomStyle(source);
    }


    // =========================
    // AUTO DECO
    // =========================

    void onAutoDeco(CCObject*) {

        auto selected = this->m_selectedObjects;

        if (!selected || selected->count() == 0) {

            FLAlertLayer::create(
                "AUTO DECO",
                "Select some blocks first!",
                "OK"
            )->show
