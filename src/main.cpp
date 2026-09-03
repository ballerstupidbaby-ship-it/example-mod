#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify(AutoDecoEditorUI, EditorUI) {

    // =========================
    // MODERN NEON PURPLE PALETTE
    // =========================

    static ccColor3B dark() {
        return {12, 6, 24}; // Very deep cyber void background
    }

    static ccColor3B darkPurple() {
        return {50, 10, 85}; // Structural backing shading
    }

    static ccColor3B purple() {
        return {160, 0, 255}; // Dominant theme color
    }

    static ccColor3B neon() {
        return {240, 30, 255}; // Radiant edge color
    }

    static ccColor3B pink() {
        return {255, 50, 200}; // Core hot highlights
    }

    static ccColor3B cyan() {
        return {0, 240, 255}; // Contrast cyber nodes
    }

    static ccColor3B white() {
        return {245, 230, 255}; // Peak glow reflections
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

        log::info("Auto Deco loaded successfully!");

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
    // CYBERPUNK 3D TECH BLOCK GENERATION
    // =========================

    void decorate(GameObject* source) {
        if (!source)
            return;

        auto p = source->getPosition();
        float s = source->getScale();
        if (s <= 0.0f) s = 1.0f;

        // Offset the generated decoration block to the right side
        float x = p.x + (60.0f * s);
        float y = p.y;


        // -------------------------
        // 1. BACKGROUND FILL LAYER
        // -------------------------
        // Uses ID 210 (Solid Square Fill) to mask out behind the texturing
        make(210, CCPoint(x, y), s * 1.0f, dark(), dark());


        // -------------------------
        // 2. TECH INNER GRID TEXTURE
        // -------------------------
        // Uses ID 1006 (Tech Grid Layout) inside the block to simulate the cyber details
        make(1006, CCPoint(x, y), s * 0.95f, darkPurple(), purple(), 0.0f, 180);


        // -------------------------
        // 3. 3D SHADOW & DEPTH
        // -------------------------
        // Shifted downward and diagonally to produce a clean volumetric projection
        make(239, CCPoint(x + 6.0f * s, y - 6.0f * s), s * 1.0f, darkPurple(), dark());


        // -------------------------
        // 4. MAIN STRUCTURAL OUTLINE FRAME
        // -------------------------
        // Replaced broken 500-lines with ID 239 (Solid 2.1 Block Outlines)
        make(239, CCPoint(x, y), s * 1.0f, purple(), neon());


        // -------------------------
        // 5. INTENSE NEON EDGE GLOW
        // -------------------------
        // Uses ID 211 (Thick Glow Strip Edge) to emit light outwards
        make(211, CCPoint(x - 15.0f * s, y), s * 1.0f, neon(), neon(), 90, 200);  // Left Edge
        make(211, CCPoint(x + 15.0f * s, y), s * 1.0f, neon(), neon(), 270, 200); // Right Edge
        make(211, CCPoint(x, y + 15.0f * s), s * 1.0f, neon(), neon(), 180, 200); // Top Edge
        make(211, CCPoint(x, y - 15.0f * s), s * 1.0f, neon(), neon(), 0, 200);   // Bottom Edge


        // -------------------------
        // 6. INTERNAL CYBER STRUCTURES
        // -------------------------
        // Layering ID 1324 (Tech Inserts) inside the four quadrants
        make(1324, CCPoint(x - 8.0f * s, y + 8.0f * s), s * 0.4f, darkPurple(), pink());
        make(1324, CCPoint(x + 8.0f * s, y + 8.0f * s), s * 0.4f, darkPurple(), pink());
        make(1324, CCPoint(x - 8.0f * s, y - 8.0f * s), s * 0.4f, darkPurple(), pink());
        make(1324, CCPoint(x + 8.0f * s, y - 8.0f * s), s * 0.4f, darkPurple(), pink());


        // -------------------------
        // 7. BRIGHT CENTERPULSE CORE
        // -------------------------
        // The pulsing tech block right in the center of the structure
        make(210, CCPoint(x, y), s * 0.35f, pink(), white());
        make(1825, CCPoint(x, y), s * 0.30f, cyan(), white()); // Cyber Crosshair Detail


        // -------------------------
        // 8. CORNER TECH HIGHLIGHTS
        // -------------------------
        // Small nodes mapping out the vertices of the blocks
        make(220, CCPoint(x - 15.0f * s, y + 15.0f * s), s * 0.2f, cyan(), white());
        make(220, CCPoint(x + 15.0f * s, y + 15.0f * s), s * 0.2f, cyan(), white());
        make(220, CCPoint(x - 15.0f * s, y - 15.0f * s), s * 0.2f, cyan(), white());
        make(220, CCPoint(x + 15.0f * s, y - 15.0f * s), s * 0.2f, cyan(), white());
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
            auto block = static_cast<GameObject*>(selected->objectAtIndex(i));
            this->decorate(block);
        }
    }
};
