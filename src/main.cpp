#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify(AutoDecoEditorUI, EditorUI) {
    struct Fields {
        int style = 0;
    };

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
    // CREATE OFFSET OBJECT
    // =========================

    void piece(
        GameObject* source,
        int id,
        float x,
        float y,
        float scale,
        float rotation = 0.0f,
        ccColor3B main = purple(),
        ccColor3B detail = neon()
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
                p.x + x * s,
                p.y + y * s
            ),
            scale * s,
            main,
            detail,
            rotation
        );
    }

    // =========================
    // EDITOR BUTTONS
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
        auto fields = m_fields;

        fields->style++;

        if (fields->style > 3)
            fields->style = 0;

        const char* names[] = {
            "MECHANICAL",
            "ANGULAR",
            "TECH FRAME",
            "RANDOM"
        };

        auto text = fmt::format(
            "Style: {}",
            names[fields->style]
        );

        FLAlertLayer::create(
            "AUTO DECO",
            text.c_str(),
            "OK"
        )->show();
    }

    // =========================
    // STYLE 1
    // MECHANICAL
    // =========================

    void mechanical(GameObject* source) {
        if (!source)
            return;

        // Outer mechanical frame
        piece(source, 237, -28, 20, 0.12f, 0, neon(), purple());
        piece(source, 237, 0, 20, 0.12f, 0, purple(), neon());
        piece(source, 237, 28, 20, 0.12f, 0, pink(), purple());

        piece(source, 237, -28, -20, 0.12f, 0, purple(), neon());
        piece(source, 237, 0, -20, 0.12f, 0, neon(), purple());
        piece(source, 237, 28, -20, 0.12f, 0, pink(), purple());

        // Side pieces
        piece(source, 238, -28, 0, 0.12f, 90, neon(), purple());
        piece(source, 238, 28, 0, 0.12f, 90, pink(), purple());

        // 3D depth
        piece(source, 506, -20, 25, 0.10f, 0, purple(), darkPurple());
        piece(source, 507, 5, 25, 0.10f, 0, neon(), purple());
        piece(source, 509, 28, 25, 0.10f, 0, pink(), purple());

        // Inner corners
        piece(source, 210, -14, 14, 0.08f, 0, purple(), neon());
        piece(source, 210, 14, 14, 0.08f, 0, pink(), neon());
        piece(source, 210, -14, -14, 0.08f, 0, neon(), purple());
        piece(source, 210, 14, -14, 0.08f, 0, purple(), pink());

        // Center node
        auto p = source->getPosition();
        float s = source->getScale();

        make(
            220,
            p,
            0.055f * s,
            cyan(),
            white()
        );
    }

    // =========================
    // STYLE 2
    // ANGULAR
    // =========================

    void angular(GameObject* source) {
        if
