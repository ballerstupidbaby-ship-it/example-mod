#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify(AutoDecoEditorUI, EditorUI) {

    // =========================
    // CYBERPUNK AUTO DECO
    // =========================

    static constexpr int MAIN = 207;
    static constexpr int TOP = 208;
    static constexpr int CORNER = 209;
    static constexpr int INNER = 210;
    static constexpr int SPARK = 227;

    // ---------- COLORS ----------

    ccColor3B cyan() {
        return {0, 240, 255};
    }

    ccColor3B cyanBright() {
        return {150, 255, 255};
    }

    ccColor3B cyanDark() {
        return {0, 100, 125};
    }

    ccColor3B cyanDeep() {
        return {0, 35, 50};
    }

    ccColor3B magenta() {
        return {255, 0, 170};
    }

    ccColor3B purple() {
        return {170, 0, 255};
    }

    ccColor3B blackMetal() {
        return {8, 15, 22};
    }

    // ---------- OBJECT CREATOR ----------

    GameObject* make(
        int id,
        CCPoint pos,
        float scale,
        ccColor3B color,
        GLubyte opacity = 255,
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
        obj->setChildColor(color);
        obj->setOpacity(opacity);
        obj->setRotation(rotation);

        return obj;
    }

    // ---------- BUTTON ----------

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
    // 1. DEEP 3D EXTRUSION
    // =========================================================

    void depth(GameObject* src) {

        auto p = src->getPosition();
        float s = src->getScale();

        for (int i = 8; i >= 1; i--) {

            float x = p.x + (i * 3.0f);
            float y = p.y - (i * 2.0f);

            ccColor3B c;

            if (i >= 7)
                c = {5, 20, 28};
            else if (i >= 5)
                c = cyanDeep();
            else if (i >= 3)
                c = cyanDark();
            else
                c = cyan();

            make(
                MAIN,
                CCPoint(x, y),
                s * (0.94f - i * 0.008f),
                c,
                static_cast<GLubyte>(150 + i * 10)
            );
        }
    }

    // =========================================================
    // 2. MAIN HOLOGRAPHIC FACE
    // =========================================================

    void front(GameObject* src) {

        auto p = src->getPosition();
        float s = src->getScale();

        make(
            MAIN,
            CCPoint(p.x + 2, p.y + 2),
            s,
            blackMetal(),
            255
        );

        make(
            TOP,
            CCPoint(p.x + 2, p.y + 7),
            s * 0.90f,
            cyan(),
            255
        );

        make(
            INNER,
            CCPoint(p.x + 4, p.y - 5),
            s * 0.84f,
            cyanDeep(),
            230
        );
    }

    // =========================================================
    // 3. CYAN ENERGY EDGES
    // =========================================================

    void energyEdges(GameObject* src) {

        auto p = src->getPosition();
        float s = src->getScale();

        // top beam
        make(
            TOP,
            CCPoint(p.x, p.y + 11),
            s * 0.65f,
            cyanBright(),
            245,
            0
        );

        // bottom beam
        make(
            TOP,
            CCPoint(p.x + 3, p.y - 11),
            s * 0.60f,
            cyanDark(),
            220,
            180
        );

        // right edge
        make(
            CORNER,
            CCPoint(p.x + 12, p.y),
            s * 0.60f,
            cyan(),
            245,
            90
        );

        // left edge
        make(
            CORNER,
            CCPoint(p.x - 9, p.y + 1),
            s * 0.55f,
            cyanDark(),
            220,
            270
        );
    }

    // =========================================================
    // 4. CIRCUIT GRID
    // =========================================================

    void circuits(GameObject* src) {

        auto p = src->getPosition();
        float s = src->getScale();

        for (int i = -2; i <= 2; i++) {

            make(
                INNER,
                CCPoint(
                    p.x + i * 5.0f,
                    p.y + 3.0f
                ),
                s * 0.25f,
                cyanDark(),
                190,
                0
            );
        }

        for (int i = -2; i <= 2; i++) {

            make(
                INNER,
                CCPoint(
                    p.x + 2.0f,
                    p.y + i * 5.0f
                ),
                s * 0.22f,
                cyanDark(),
                175,
                90
            );
        }

        // center circuit node
        make(
            CORNER,
            CCPoint(p.x + 4, p.y + 4),
            s * 0.30f,
            cyanBright(),
            230,
            45
        );
    }

    // =========================================================
    // 5. HEX / DIAMOND FRAME
    // =========================================================

    void hexFrame(GameObject* src) {

        auto p = src->getPosition();
        float s = src->getScale();

        make(
            CORNER,
            CCPoint(p.x - 7, p.y + 7),
            s * 0.42f,
            cyan(),
            225,
            45
        );

        make(
            CORNER,
            CCPoint(p.x + 8, p.y + 7),
            s * 0.38f,
            cyanDark(),
            215,
            135
        );

        make(
            CORNER,
            CCPoint(p.x + 9, p.y - 7),
            s * 0.35f,
            cyan(),
            210,
            45
        );

        make(
            CORNER,
            CCPoint(p.x - 7, p.y - 7),
            s * 0.32f,
            cyanDark(),
            200,
            135
        );
    }

    // =========================================================
    // 6. CROSS BRACES
    // =========================================================

    void braces(GameObject* src) {

        auto p = src->getPosition();
        float s = src->getScale();

        make(
            CORNER,
            CCPoint(p.x + 10, p.y + 8),
            s * 0.50f,
            cyan(),
            215,
            45
        );

        make(
            CORNER,
            CCPoint(p.x + 10, p.y - 8),
            s * 0.50f,
            cyanDark(),
            205,
            135
        );

        make(
            CORNER,
            CCPoint(p.x - 8, p.y + 8),
            s * 0.42f,
            cyanBright(),
            190,
            135
        );
    }

    // =========================================================
    // 7. VERTICAL ENERGY TOWERS
    // =========================================================

    void towers(GameObject* src) {

        auto p = src->getPosition();
        float s = src->getScale();

        for (int i = 1; i <= 4; i++) {

            make(
                TOP,
                CCPoint(
                    p.x + 15.0f + i * 4.0f,
                    p.y + i * 7.0f
                ),
                s * 0.35f,
                cyan(),
                190,
                0
            );
        }

        for (int i = 1; i <= 3; i++) {

            make(
                TOP,
                CCPoint(
                    p.x - 15.0f,
                    p.y - i * 7.0f
                ),
                s * 0.30f,
                cyanDark(),
                180,
                0
            );
        }
    }

    // =========================================================
    // 8. FLOATING TECH PANELS
    // =========================================================

    void panels(GameObject* src) {

        auto p = src->getPosition();
        float s = src->getScale();

        make(
            MAIN,
            CCPoint(p.x + 20, p.y + 13),
            s * 0.45f,
            blackMetal(),
            230
        );

        make(
            TOP,
            CCPoint(p.x + 20, p.y + 17),
            s * 0.38f,
            cyan(),
            220
        );

        make(
            MAIN,
            CCPoint(p.x - 18, p.y - 14),
            s * 0.40f,
            blackMetal(),
            220
        );

        make(
            TOP,
            CCPoint(p.x - 18, p.y - 10),
            s * 0.34f,
            magenta(),
            210
        );
    }

    // =========================================================
    // 9. MAGENTA SIDE ACCENTS
    // =========================================================

    void magentaAccents(GameObject* src) {

        auto p = src->getPosition();
        float s = src->getScale();

        make(
            TOP,
            CCPoint(p.x - 23, p.y + 5),
            s * 0.48f,
            magenta(),
            200,
            90
        );

        make(
            TOP,
            CCPoint(p.x + 23, p.y - 5),
            s * 0.48f,
            magenta(),
            200,
            90
        );

        make(
            CORNER,
            CCPoint(p.x - 20, p.y + 13),
            s * 0.32f,
            purple(),
            190,
            45
        );

        make(
            CORNER,
            CCPoint(p.x + 20, p.y - 13),
            s * 0.32f,
            purple(),
            190,
            135
        );
    }

    // =========================================================
    // 10. SMALL FLOATING PARTICLES
    // =========================================================

    void particles(GameObject* src) {

        auto p = src->getPosition();
        float s = src->getScale();

        make(
            SPARK,
            CCPoint(p.x + 27, p.y + 18),
            s * 0.20f,
            cyanBright(),
            210
        );

        make(
            SPARK,
            CCPoint(p.x - 25, p.y + 20),
            s * 0.17f,
            cyan(),
            190
        );

        make(
            SPARK,
            CCPoint(p.x + 29, p.y - 18),
            s * 0.15f,
            magenta(),
            190
        );

        make(
            SPARK,
            CCPoint(p.x - 28, p.y - 16),
            s * 0.18f,
            cyan(),
            180
        );

        make(
            SPARK,
            CCPoint(p.x + 14, p.y + 22),
            s * 0.12f,
            cyanBright(),
            170
        );
    }

    // =========================================================
    // 11. MECHANICAL CORNERS
    // =========================================================

    void mechanical(GameObject* src) {

        auto p = src->getPosition();
        float s = src->getScale();

        make(
            CORNER,
            CCPoint(p.x + 13, p.y + 12),
            s * 0.30f,
            cyanBright(),
            220,
            45
        );

        make(
            CORNER,
            CCPoint(p.x + 13, p.y - 12),
            s * 0.30f,
            cyanDark(),
            220,
            135
        );

        make(
            CORNER,
            CCPoint(p.x - 13, p.y + 12),
            s * 0.28f,
            cyan(),
            210,
            135
        );
    }

    // =========================================================
    // 12. FULL DECORATION
    // =========================================================

    void decorate(GameObject* src) {

        if (!src)
            return;

        depth(src);
        front(src);
        energyEdges(src);
        circuits(src);
        hexFrame(src);
        braces(src);
        towers(src);
        panels(src);
        magentaAccents(src);
        particles(src);
        mechanical(src);
    }

    // =========================================================
    // AUTO DECO BUTTON
    // =========================================================

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

        // Prevent accidentally generating thousands of objects.
        if (count > 30) {

            FLAlertLayer::create(
                "Auto Deco",
                "Select 30 or fewer objects at once.",
                "OK"
            )->show();

            return;
        }

        for (int i = 0; i < count; i++) {

            auto object = static_cast<GameObject*>(
                selected->objectAtIndex(i)
            );

            if (!object)
                continue;

            decorate(object);
        }

        auto message = fmt::format(
            "Cyberpunk deco applied to {} blocks!",
            count
        );

        FLAlertLayer::create(
            "AUTO DECO",
            message.c_str(),
            "OK"
        )->show();
    }
};
