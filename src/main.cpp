#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify(AutoDecoEditorUI, EditorUI) {

    // ============================================================
    // CYBERPUNK / FUTURISTIC TEMPLATE
    // ============================================================

    // Colored square pieces
    static constexpr int BLOCK = 207;
    static constexpr int TOP = 208;
    static constexpr int OUTER = 209;
    static constexpr int INNER = 210;
    static constexpr int PILLAR_TOP = 212;
    static constexpr int PILLAR = 213;

    // Hexagon pieces
    static constexpr int HEX = 229;
    static constexpr int HEX_CORNER = 230;
    static constexpr int SMALL_HEX = 231;
    static constexpr int SMALL_HEX_CORNER = 232;

    // Pipes
    static constexpr int PIPE = 237;

    // 3DL pieces
    static constexpr int THREE_DL_TOP_LEFT = 506;
    static constexpr int THREE_DL_TOP_MIDDLE = 507;
    static constexpr int THREE_DL_HALF_TOP = 508;
    static constexpr int THREE_DL_TOP_RIGHT = 509;
    static constexpr int THREE_DL_INNER = 510;
    static constexpr int THREE_DL_OUTER = 511;

    // Grid 3DL
    static constexpr int GRID_3DL_LEFT = 515;
    static constexpr int GRID_3DL_MIDDLE = 516;
    static constexpr int GRID_3DL_RIGHT = 518;

    // Beveled 3DL
    static constexpr int BEVEL_LEFT = 524;
    static constexpr int BEVEL_MIDDLE = 525;
    static constexpr int BEVEL_RIGHT = 527;

    // Neon outline
    static constexpr int NEON_TOP = 1191;
    static constexpr int NEON_OUTER = 1192;
    static constexpr int NEON_PILLAR_TOP = 1193;
    static constexpr int NEON_INNER = 1194;
    static constexpr int NEON_CENTER = 1195;
    static constexpr int NEON_SQUARE = 1196;
    static constexpr int NEON_PILLAR = 1197;

    // Thick outline
    static constexpr int THICK_TOP = 1202;
    static constexpr int THICK_OUTER = 1203;
    static constexpr int THICK_INNER = 1205;
    static constexpr int THICK_SMALL = 1208;
    static constexpr int THICK_PILLAR = 1209;
    static constexpr int THICK_SQUARE = 1210;

    // ============================================================
    // COLORS
    // ============================================================

    ccColor3B cyan() {
        return {0, 240, 255};
    }

    ccColor3B cyanBright() {
        return {170, 255, 255};
    }

    ccColor3B cyanDark() {
        return {0, 110, 140};
    }

    ccColor3B darkMetal() {
        return {10, 15, 25};
    }

    ccColor3B deepPurple() {
        return {65, 0, 100};
    }

    ccColor3B purple() {
        return {175, 0, 255};
    }

    ccColor3B magenta() {
        return {255, 0, 190};
    }

    // ============================================================
    // OBJECT CREATOR
    // ============================================================

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

    // ============================================================
    // BUTTON
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
    // 3D BACK DEPTH
    // ============================================================

    void createDepth(GameObject* source) {

        auto p = source->getPosition();
        float s = source->getScale();

        for (int i = 8; i >= 1; i--) {

            float x = p.x + i * 3.0f;
            float y = p.y - i * 2.0f;

            ccColor3B c;

            if (i >= 7)
                c = deepPurple();
            else if (i >= 5)
                c = darkMetal();
            else if (i >= 3)
                c = cyanDark();
            else
                c = cyan();

            make(
                THREE_DL_TOP_MIDDLE,
                CCPoint(x, y),
                s * 0.85f,
                c,
                static_cast<GLubyte>(110 + i * 15)
            );
        }
    }

    // ============================================================
    // MAIN TECH FRAME
    // ============================================================

    void createMainFrame(GameObject* source) {

        auto p = source->getPosition();
        float s = source->getScale();

        // dark body
        make(
            BLOCK,
            CCPoint(p.x + 1, p.y + 1),
            s * 1.15f,
            darkMetal()
        );

        // top neon rail
        make(
            NEON_TOP,
            CCPoint(p.x, p.y + 10),
            s * 0.90f,
            cyanBright(),
            255
        );

        // bottom rail
        make(
            NEON_TOP,
            CCPoint(p.x + 2, p.y - 10),
            s * 0.82f,
            cyanDark(),
            230,
            180
        );

        // left frame
        make(
            NEON_PILLAR,
            CCPoint(p.x - 10, p.y),
            s * 0.90f,
            cyan(),
            245,
            90
        );

        // right frame
        make(
            NEON_PILLAR,
            CCPoint(p.x + 11, p.y),
            s * 0.90f,
            cyan(),
            245,
            90
        );
    }

    // ============================================================
    // L-SHAPED ARCHITECTURE
    // ============================================================

    void createLShapes(GameObject* source) {

        auto p = source->getPosition();
        float s = source->getScale();

        // upper left
        make(
            NEON_OUTER,
            CCPoint(p.x - 13, p.y + 13),
            s * 0.75f,
            cyan(),
            245,
            0
        );

        // lower left
        make(
            NEON_INNER,
            CCPoint(p.x - 13, p.y - 13),
            s * 0.75f,
            purple(),
            235,
            0
        );

        // upper right
        make(
            NEON_OUTER,
            CCPoint(p.x + 13, p.y + 13),
            s * 0.75f,
            cyanBright(),
            240,
            90
        );

        // lower right
        make(
            NEON_INNER,
            CCPoint(p.x + 13, p.y - 13),
            s * 0.75f,
            magenta(),
            235,
            90
        );
    }

    // ============================================================
    // INNER NESTED FRAMES
    // ============================================================

    void createNestedFrames(GameObject* source) {

        auto p = source->getPosition();
        float s = source->getScale();

        make(
            THICK_SQUARE,
            CCPoint(p.x - 4, p.y + 4),
            s * 0.72f,
            cyanDark(),
            230
        );

        make(
            THICK_INNER,
            CCPoint(p.x + 5, p.y + 5),
            s * 0.58f,
            cyan(),
            230
        );

        make(
            NEON_CENTER,
            CCPoint(p.x + 7, p.y + 6),
            s * 0.32f,
            magenta(),
            245
        );

        make(
            THICK_SMALL,
            CCPoint(p.x + 7, p.y + 6),
            s * 0.17f,
            cyanBright(),
            255
        );
    }

    // ============================================================
    // HEXAGON TECH DETAILS
    // ============================================================

    void createHexDetails(GameObject* source) {

        auto p = source->getPosition();
        float s = source->getScale();

        make(
            HEX,
            CCPoint(p.x - 17, p.y + 7),
            s * 0.55f,
            cyan(),
            210
        );

        make(
            SMALL_HEX,
            CCPoint(p.x + 18, p.y + 8),
            s * 0.40f,
            magenta(),
            220
        );

        make(
            HEX_CORNER,
            CCPoint(p.x + 18, p.y - 8),
            s * 0.42f,
            purple(),
            210,
            90
        );

        make(
            SMALL_HEX_CORNER,
            CCPoint(p.x - 17, p.y - 9),
            s * 0.36f,
            cyanBright(),
            200,
            45
        );
    }

    // ============================================================
    // 3DL CORNERS / DEPTH EDGES
    // ============================================================

    void create3DFrame(GameObject* source) {

        auto p = source->getPosition();
        float s = source->getScale();

        make(
            THREE_DL_TOP_LEFT,
            CCPoint(p.x - 12, p.y + 12),
            s * 0.75f,
            cyanBright(),
            235
        );

        make(
            THREE_DL_TOP_MIDDLE,
            CCPoint(p.x, p.y + 14),
            s * 0.78f,
            cyan(),
            230
        );

        make(
            THREE_DL_TOP_RIGHT,
            CCPoint(p.x + 12, p.y + 12),
            s * 0.75f,
            cyan(),
            235
        );

        make(
            THREE_DL_OUTER,
            CCPoint(p.x + 15, p.y - 7),
            s * 0.70f,
            purple(),
            210
        );

        make(
            THREE_DL_INNER,
            CCPoint(p.x - 14, p.y - 7),
            s * 0.70f,
            cyanDark(),
            210
        );
    }

    // ============================================================
    // GRID / CIRCUIT DETAILS
    // ============================================================

    void createGrid(GameObject* source) {

        auto p = source->getPosition();
        float s = source->getScale();

        for (int i = -2; i <= 2; i++) {

            make(
                GRID_3DL_MIDDLE,
                CCPoint(
                    p.x + i * 5.0f,
                    p.y + 18
                ),
                s * 0.25f,
                cyanDark(),
                175
            );
        }

        for (int i = -2; i <= 2; i++) {

            make(
                GRID_3DL_MIDDLE,
                CCPoint(
                    p.x + 18,
                    p.y + i * 5.0f
                ),
                s * 0.23f,
                purple(),
                160,
                90
            );
        }
    }

    // ============================================================
    // MECHANICAL CORNERS
    // ============================================================

    void createMechanical(GameObject* source) {

        auto p = source->getPosition();
        float s = source->getScale();

        make(
            PILLAR_TOP,
            CCPoint(p.x - 20, p.y + 15),
            s * 0.35f,
            cyan(),
            220
        );

        make(
            PILLAR,
            CCPoint(p.x + 20, p.y + 15),
            s * 0.35f,
            magenta(),
            220
        );

        make(
            PILLAR,
            CCPoint(p.x - 20, p.y - 15),
            s * 0.30f,
            purple(),
            210
        );

        make(
            PILLAR,
            CCPoint(p.x + 20, p.y - 15),
            s * 0.30f,
            cyan(),
            210
        );
    }

    // ============================================================
    // SIDE PANELS
    // ============================================================

    void createSidePanels(GameObject* source) {

        auto p = source->getPosition();
        float s = source->getScale();

        // left panel
        make(
            BLOCK,
            CCPoint(p.x - 27, p.y + 2),
            s * 0.55f,
            darkMetal(),
            235
        );

        make(
            THICK_TOP,
            CCPoint(p.x - 27, p.y + 7),
            s * 0.42f,
            magenta(),
            225
        );

        // right panel
        make(
            BLOCK,
            CCPoint(p.x + 27, p.y - 2),
            s * 0.55f,
            darkMetal(),
            235
        );

        make(
            THICK_TOP,
            CCPoint(p.x + 27, p.y + 3),
            s * 0.42f,
            cyan(),
            225
        );
    }

    // ============================================================
    // FLOATING NODES
    // ============================================================

    void createNodes(GameObject* source) {

        auto p = source->getPosition();
        float s = source->getScale();

        make(
            NEON_CENTER,
            CCPoint(p.x - 30, p.y + 22),
            s * 0.25f,
            cyanBright(),
            230
        );

        make(
            NEON_CENTER,
            CCPoint(p.x + 30, p.y + 22),
            s * 0.25f,
            magenta(),
            230
        );

        make(
            SMALL_HEX,
            CCPoint(p.x - 29, p.y - 21),
            s * 0.22f,
            purple(),
            210
        );

        make(
            SMALL_HEX,
            CCPoint(p.x + 30, p.y - 21),
            s * 0.22f,
            cyan(),
            210
        );
    }

    // ============================================================
    // FULL TEMPLATE
    // ============================================================

    void decorate(GameObject* source) {

        if (!source)
            return;

        createDepth(source);
        createMainFrame(source);
        createLShapes(source);
        createNestedFrames(source);
        createHexDetails(source);
        create3DFrame(source);
        createGrid(source);
        createMechanical(source);
        createSidePanels(source);
        createNodes(source);
    }

    // ============================================================
    // BUTTON ACTION
    // ============================================================

    void onAutoDeco(CCObject*) {

        auto selected = this->m_selectedObjects;

        int count = selected ? selected->count() : 0;

        if (count == 0) {

            FLAlertLayer::create(
                "AUTO DECO",
                "Select some blocks first!",
                "OK"
            )->show();

            return;
        }

        // Each block creates many objects.
        // Keep this reasonable for mobile.
        if (count > 20) {

            FLAlertLayer::create(
                "AUTO DECO",
                "Select 20 or fewer blocks at once.",
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
            "Cyberpunk template applied to {} block{}!",
            count,
            count == 1 ? "" : "s"
        );

        FLAlertLayer::create(
            "AUTO DECO",
            message.c_str(),
            "OK"
        )->show();
    }
};
