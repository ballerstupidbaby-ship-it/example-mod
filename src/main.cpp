#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

class $modify(AutoDecoEditorUI, EditorUI) {

    // ==============================
    // AUTO DECO SETTINGS
    // ==============================

    static constexpr int DEPTH_LAYERS = 8;

    // How far each 3D layer moves
    static constexpr float DEPTH_X = 3.0f;
    static constexpr float DEPTH_Y = -2.0f;

    // Main decoration
    static constexpr int MAIN_OBJECT = 207;

    // Secondary/depth decoration
    static constexpr int DEPTH_OBJECT = 208;

    // Highlight decoration
    static constexpr int HIGHLIGHT_OBJECT = 227;

    // Overall size
    static constexpr float SCALE_MULTIPLIER = 0.85f;

    // ==============================
    // INIT
    // ==============================

    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer))
            return false;

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

    // ==============================
    // COLOR HELPERS
    // ==============================

    ccColor3B getDepthColor(int layer) {

        // Purple main color
        ccColor3B brightPurple = {
            190,
            80,
            255
        };

        // Dark purple shadow
        ccColor3B darkPurple = {
            45,
            10,
            75
        };

        float t =
            static_cast<float>(layer) /
            static_cast<float>(DEPTH_LAYERS);

        auto lerp = [](GLubyte a, GLubyte b, float t) {
            return static_cast<GLubyte>(
                a + (b - a) * t
            );
        };

        return {
            lerp(
                brightPurple.r,
                darkPurple.r,
                t
            ),

            lerp(
                brightPurple.g,
                darkPurple.g,
                t
            ),

            lerp(
                brightPurple.b,
                darkPurple.b,
                t
            )
        };
    }

    // ==============================
    // CREATE DEPTH LAYER
    // ==============================

    void createDepthLayer(
        GameObject* source,
        int objectID,
        int layer
    ) {

        if (!source)
            return;

        auto position = source->getPosition();

        float x =
            position.x +
            (DEPTH_X * layer);

        float y =
            position.y +
            (DEPTH_Y * layer);

        auto object =
            this->m_editorLayer->createObject(
                objectID,
                CCPoint(x, y),
                false
            );

        if (!object)
            return;

        // Match the original block size
        object->setScale(
            source->getScale() *
            SCALE_MULTIPLIER
        );

        // Gradually darken the depth
        object->setChildColor(
            getDepthColor(layer)
        );

        // Make deeper layers slightly darker
        GLubyte opacity =
            static_cast<GLubyte>(
                255.0f -
                (layer * 15.0f)
            );

        object->setOpacity(opacity);
    }

    // ==============================
    // HIGHLIGHT
    // ==============================

    void createHighlight(GameObject* source) {

        if (!source)
            return;

        auto position =
            source->getPosition();

        auto highlight =
            this->m_editorLayer->createObject(
                HIGHLIGHT_OBJECT,
                CCPoint(
                    position.x + 3.0f,
                    position.y + 5.0f
                ),
                false
            );

        if (!highlight)
            return;

        highlight->setScale(
            source->getScale() *
            0.72f
        );

        // Bright highlight
        highlight->setChildColor(
            ccColor3B{
                245,
                200,
                255
            }
        );

        highlight->setOpacity(210);
    }

    // ==============================
    // SHADOW
    // ==============================

    void createShadow(GameObject* source) {

        if (!source)
            return;

        auto position =
            source->getPosition();

        auto shadow =
            this->m_editorLayer->createObject(
                DEPTH_OBJECT,
                CCPoint(
                    position.x + 6.0f,
                    position.y - 7.0f
                ),
                false
            );

        if (!shadow)
            return;

        shadow->setScale(
            source->getScale() *
            0.88f
        );

        shadow->setChildColor(
            ccColor3B{
                25,
                5,
                40
            }
        );

        shadow->setOpacity(180);
    }

    // ==============================
    // MAIN AUTO DECO
    // ==============================

    void decorateObject(GameObject* source) {

        if (!source)
            return;

        // --------------------------------
        // 1. Deep 3D extrusion
        // --------------------------------

        for (
            int layer = DEPTH_LAYERS;
            layer >= 1;
            layer--
        ) {

            createDepthLayer(
                source,
                DEPTH_OBJECT,
                layer
            );
        }

        // --------------------------------
        // 2. Main 3D side
        // --------------------------------

        auto position =
            source->getPosition();

        auto main =
            this->m_editorLayer->createObject(
                MAIN_OBJECT,
                CCPoint(
                    position.x + 4.0f,
                    position.y - 3.0f
                ),
                false
            );

        if (main) {

            main->setScale(
                source->getScale() *
                SCALE_MULTIPLIER
            );

            main->setChildColor(
                ccColor3B{
                    165,
                    55,
                    240
                }
            );

            main->setOpacity(255);
        }

        // --------------------------------
        // 3. Dark shadow
        // --------------------------------

        createShadow(source);

        // --------------------------------
        // 4. Bright highlight
        // --------------------------------

        createHighlight(source);
    }

    // ==============================
    // BUTTON
    // ==============================

    void onAutoDeco(CCObject*) {

        auto selected =
            this->m_selectedObjects;

        int count =
            selected ?
            selected->count() :
            0;

        if (count == 0) {

            FLAlertLayer::create(
                "Auto Deco",
                "Select some blocks first!",
                "OK"
            )->show();

            return;
        }

        // Safety limit
        if (count > 100) {

            FLAlertLayer::create(
                "Auto Deco",
                "Too many objects selected!\n"
                "Select 100 or fewer blocks.",
                "OK"
            )->show();

            return;
        }

        // --------------------------------
        // Decorate every selected object
        // --------------------------------

        for (int i = 0; i < count; i++) {

            auto object =
                static_cast<GameObject*>(
                    selected->objectAtIndex(i)
                );

            if (!object)
                continue;

            decorateObject(object);
        }

        // --------------------------------
        // Finished
        // --------------------------------

        auto message =
            fmt::format(
                "Applied advanced 3D deco to {} objects!",
                count
            );

        FLAlertLayer::create(
            "Auto Deco",
            message.c_str(),
            "OK"
        )->show();
    }
};
