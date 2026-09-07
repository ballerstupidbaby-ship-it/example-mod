#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <vector>

using namespace geode::prelude;

struct BlueprintObject {
    int id = 0;
    float x = 0.f;
    float y = 0.f;
    float scale = 1.f;
    float rotation = 0.f;
    int opacity = 255;
    int z = 0;
};

struct Blueprint {
    std::string name;
    std::vector<BlueprintObject> objects;
};

static std::filesystem::path getBlueprintFolder() {
    auto folder = Mod::get()->getSaveDir() / "blueprints";

    std::error_code ec;
    std::filesystem::create_directories(folder, ec);

    return folder;
}

static bool saveBlueprint(
    Blueprint const& blueprint,
    std::filesystem::path const& path
) {
    std::ofstream file(path);

    if (!file)
        return false;

    file << "BLUEPRINT1\n";
    file << blueprint.name << "\n";
    file << blueprint.objects.size() << "\n";

    for (auto const& obj : blueprint.objects) {
        file
            << obj.id << " "
            << obj.x << " "
            << obj.y << " "
            << obj.scale << " "
            << obj.rotation << " "
            << obj.opacity << " "
            << obj.z << "\n";
    }

    return true;
}

static bool loadBlueprint(
    std::filesystem::path const& path,
    Blueprint& blueprint
) {
    std::ifstream file(path);

    if (!file)
        return false;

    std::string header;

    std::getline(file, header);

    if (header != "BLUEPRINT1")
        return false;

    std::getline(file, blueprint.name);

    size_t count = 0;

    if (!(file >> count))
        return false;

    blueprint.objects.clear();
    blueprint.objects.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        BlueprintObject obj;

        if (!(file
            >> obj.id
            >> obj.x
            >> obj.y
            >> obj.scale
            >> obj.rotation
            >> obj.opacity
            >> obj.z
        )) {
            return false;
        }

        blueprint.objects.push_back(obj);
    }

    return true;
}

static std::vector<std::filesystem::path> getBlueprints() {
    std::vector<std::filesystem::path> result;

    auto folder = getBlueprintFolder();

    std::error_code ec;

    for (auto const& entry :
        std::filesystem::directory_iterator(folder, ec)) {

        if (ec)
            break;

        if (
            entry.is_regular_file() &&
            entry.path().extension() == ".blueprint"
        ) {
            result.push_back(entry.path());
        }
    }

    std::sort(result.begin(), result.end());

    return result;
}

class BlueprintMenu : public CCLayer {

protected:

    EditorUI* m_editor = nullptr;

    Blueprint m_blueprint;

    bool m_hasBlueprint = false;

    std::filesystem::path m_loadedPath;

    std::vector<int> m_layers;

    int m_currentLayer = 0;

    CCNode* m_ghostLayer = nullptr;

    CCLabelBMFont* m_layerLabel = nullptr;

    CCLabelBMFont* m_statusLabel = nullptr;

    float m_blueprintX = 0.f;
    float m_blueprintY = 0.f;

    bool init(EditorUI* editor) {

        if (!CCLayer::init())
            return false;

        m_editor = editor;

        auto win =
            CCDirector::sharedDirector()->getWinSize();

        this->setContentSize(win);

        auto background =
            CCLayerColor::create(
                {0, 0, 0, 160},
                win.width,
                win.height
            );

        this->addChild(background);

        float width = 560.f;
        float height = 430.f;

        auto panel =
            CCLayerColor::create(
                {30, 25, 42, 255},
                width,
                height
            );

        panel->setPosition(
            (win.width - width) / 2.f,
            (win.height - height) / 2.f
        );

        this->addChild(panel);

        auto title =
            CCLabelBMFont::create(
                "BLUEPRINT BUILDER",
                "goldFont.fnt"
            );

        title->setScale(0.7f);

        title->setPosition(
            width / 2.f,
            height - 35.f
        );

        panel->addChild(title);

        m_layerLabel =
            CCLabelBMFont::create(
                "NO BLUEPRINT",
                "bigFont.fnt"
            );

        m_layerLabel->setScale(0.45f);

        m_layerLabel->setPosition(
            width / 2.f,
            height - 85.f
        );

        panel->addChild(m_layerLabel);

        m_statusLabel =
            CCLabelBMFont::create(
                "Select objects and save a blueprint",
                "goldFont.fnt"
            );

        m_statusLabel->setScale(0.38f);

        m_statusLabel->setPosition(
            width / 2.f,
            height - 115.f
        );

        panel->addChild(m_statusLabel);

        auto menu = CCMenu::create();

        menu->setPosition(0, 0);

        panel->addChild(menu);

        addButton(
            menu,
            "SAVE",
            85.f,
            280.f,
            1
        );

        addButton(
            menu,
            "LOAD",
            195.f,
            280.f,
            2
        );

        addButton(
            menu,
            "GHOST",
            305.f,
            280.f,
            3
        );

        addButton(
            menu,
            "PLACE LAYER",
            445.f,
            280.f,
            4
        );

        addButton(
            menu,
            "PREV",
            100.f,
            205.f,
            5
        );

        addButton(
            menu,
            "NEXT",
            200.f,
            205.f,
            6
        );

        addButton(
            menu,
            "PLACE ALL",
            350.f,
            205.f,
            7
        );

        addButton(
            menu,
            "X -",
            100.f,
            130.f,
            8
        );

        addButton(
            menu,
            "X +",
            200.f,
            130.f,
            9
        );

        addButton(
            menu,
            "Y -",
            300.f,
            130.f,
            10
        );

        addButton(
            menu,
            "Y +",
            400.f,
            130.f,
            11
        );

        addButton(
            menu,
            "CLOSE",
            width / 2.f,
            55.f,
            12
        );

        return true;
    }

    void addButton(
        CCMenu* menu,
        char const* text,
        float x,
        float y,
        int tag
    ) {

        auto button =
            CCMenuItemSpriteExtra::create(
                ButtonSprite::create(
                    text,
                    95,
                    true,
                    "goldFont.fnt",
                    "GJ_button_01.png",
                    25,
                    0.5f
                ),
                this,
                menu_selector(
                    BlueprintMenu::onButton
                )
            );

        button->setTag(tag);

        button->setPosition(x, y);

        menu->addChild(button);
    }

    std::vector<GameObject*> getSelected() {

        std::vector<GameObject*> result;

        if (!m_editor ||
            !m_editor->m_selectedObjects)
            return result;

        auto count =
            m_editor->m_selectedObjects->count();

        for (unsigned i = 0; i < count; ++i) {

            auto obj =
                static_cast<GameObject*>(
                    m_editor->m_selectedObjects
                        ->objectAtIndex(i)
                );

            if (obj)
                result.push_back(obj);
        }

        return result;
    }

    bool createFromSelection() {

        auto selected = getSelected();

        if (selected.empty()) {

            showMessage(
                "Blueprint",
                "Select the decoration objects first."
            );

            return false;
        }

        auto anchor = selected.front();

        auto anchorPos =
            anchor->getPosition();

        m_blueprint.objects.clear();

        for (auto object : selected) {

            BlueprintObject data;

            data.id =
                object->m_objectID;

            data.x =
                object->getPositionX()
                - anchorPos.x;

            data.y =
                object->getPositionY()
                - anchorPos.y;

            data.scale =
                object->getScale();

            data.rotation =
                object->getRotation();

            data.opacity =
                object->getOpacity();

            data.z =
                object->getZOrder();

            m_blueprint.objects.push_back(data);
        }

        m_blueprint.name =
            "Blueprint_" +
            std::to_string(
                getBlueprints().size() + 1
            );

        auto path =
            getBlueprintFolder() /
            (
                m_blueprint.name +
                ".blueprint"
            );

        if (!saveBlueprint(
            m_blueprint,
            path
        )) {

            showMessage(
                "Blueprint",
                "Failed to save blueprint."
            );

            return false;
        }

        m_loadedPath = path;

        m_hasBlueprint = true;

        rebuildLayers();

        m_currentLayer = 0;

        m_blueprintX = anchorPos.x;
        m_blueprintY = anchorPos.y;

        updateUI();

        showGhost();

        showMessage(
            "Blueprint",
            fmt::format(
                "Saved {} objects.",
                m_blueprint.objects.size()
            ).c_str()
        );

        return true;
    }

    void rebuildLayers() {

        m_layers.clear();

        for (auto const& object :
            m_blueprint.objects) {

            if (
                std::find(
                    m_layers.begin(),
                    m_layers.end(),
                    object.z
                ) == m_layers.end()
            ) {
                m_layers.push_back(object.z);
            }
        }

        std::sort(
            m_layers.begin(),
            m_layers.end()
        );
    }

    void updateUI() {

        if (!m_hasBlueprint) {

            m_layerLabel->setString(
                "NO BLUEPRINT"
            );

            m_statusLabel->setString(
                "Select objects and save a blueprint"
            );

            return;
        }

        if (m_layers.empty())
            return;

        m_layerLabel->setString(
            fmt::format(
                "LAYER {} / {}",
                m_currentLayer + 1,
                m_layers.size()
            ).c_str()
        );

        m_statusLabel->setString(
            fmt::format(
                "X {:.0f}   Y {:.0f}",
                m_blueprintX,
                m_blueprintY
            ).c_str()
        );
    }

    void clearGhost() {

        if (m_ghostLayer) {

            m_ghostLayer
                ->removeFromParentAndCleanup(true);

            m_ghostLayer = nullptr;
        }
    }

    void showGhost() {

        clearGhost();

        if (!m_hasBlueprint)
            return;

        if (!m_editor ||
            !m_editor->m_editorLayer)
            return;

        m_ghostLayer = CCLayer::create();

        m_editor->m_editorLayer
            ->addChild(
                m_ghostLayer,
                9998
            );

        if (
            m_currentLayer < 0 ||
            m_currentLayer >=
                static_cast<int>(
                    m_layers.size()
                )
        )
            return;

        int wantedZ =
            m_layers[m_currentLayer];

        for (auto const& data :
            m_blueprint.objects) {

            if (data.z != wantedZ)
                continue;

            auto sprite =
                CCSprite::createWithSpriteFrameName(
                    "GJ_square01.png"
                );

            if (!sprite)
                continue;

            sprite->setOpacity(90);

            sprite->setScale(
                std::max(
                    0.15f,
                    data.scale
                )
            );

            sprite->setRotation(
                data.rotation
            );

            sprite->setPosition(
                ccp(
                    m_blueprintX + data.x,
                    m_blueprintY + data.y
                )
            );

            m_ghostLayer->addChild(
                sprite
            );
        }
    }

    void loadFirstBlueprint() {

        auto files = getBlueprints();

        if (files.empty()) {

            showMessage(
                "Blueprint",
                "No saved blueprints yet."
            );

            return;
        }

        Blueprint loaded;

        if (!loadBlueprint(
            files.front(),
            loaded
        )) {

            showMessage(
                "Blueprint",
                "Failed to load blueprint."
            );

            return;
        }

        m_blueprint = loaded;

        m_loadedPath = files.front();

        m_hasBlueprint = true;

        rebuildLayers();

        m_currentLayer = 0;

        auto selected = getSelected();

        if (!selected.empty()) {

            auto position =
                selected.front()->getPosition();

            m_blueprintX =
                position.x;

            m_blueprintY =
                position.y;
        }

        updateUI();

        showGhost();
    }

    void placeLayer() {

        if (!m_hasBlueprint)
            return;

        if (!m_editor ||
            !m_editor->m_editorLayer)
            return;

        if (
            m_currentLayer < 0 ||
            m_currentLayer >=
                static_cast<int>(
                    m_layers.size()
                )
        )
            return;

        int wantedZ =
            m_layers[m_currentLayer];

        int created = 0;

        for (auto const& data :
            m_blueprint.objects) {

            if (data.z != wantedZ)
                continue;

            auto object =
                m_editor->m_editorLayer->createObject(
                    data.id,
                    {
                        m_blueprintX + data.x,
                        m_blueprintY + data.y
                    },
                    false
                );

            if (!object)
                continue;

            object->setScale(
                data.scale
            );

            object->setRotation(
                data.rotation
            );

            object->setOpacity(
                static_cast<GLubyte>(
                    std::clamp(
                        data.opacity,
                        0,
                        255
                    )
                )
            );

            object->setZOrder(
                data.z
            );

            created++;
        }

        showMessage(
            "Blueprint",
            fmt::format(
                "Placed {} objects on layer {}.",
                created,
                m_currentLayer + 1
            ).c_str()
        );
    }

    void placeAll() {

        if (!m_hasBlueprint)
            return;

        int oldLayer = m_currentLayer;

        for (
            int i = 0;
            i < static_cast<int>(
                m_layers.size()
            );
            ++i
        ) {

            m_currentLayer = i;

            placeLayer();
        }

        m_currentLayer = oldLayer;

        showGhost();
        updateUI();
    }

    void nextLayer() {

        if (!m_hasBlueprint)
            return;

        if (
            m_currentLayer + 1 <
            static_cast<int>(
                m_layers.size()
            )
        ) {

            m_currentLayer++;

            updateUI();
            showGhost();
        }
    }

    void previousLayer() {

        if (!m_hasBlueprint)
            return;

        if (m_currentLayer > 0) {

            m_currentLayer--;

            updateUI();
            showGhost();
        }
    }

    void moveX(float amount) {

        if (!m_hasBlueprint)
            return;

        m_blueprintX += amount;

        updateUI();
        showGhost();
    }

    void moveY(float amount) {

        if (!m_hasBlueprint)
            return;

        m_blueprintY += amount;

        updateUI();
        showGhost();
    }

    void showMessage(
        char const* title,
        char const* message
    ) {

        FLAlertLayer::create(
            title,
            message,
            "OK"
        )->show();
    }

    void onButton(CCObject* sender) {

        auto button =
            static_cast<CCMenuItemSpriteExtra*>(
                sender
            );

        switch (button->getTag()) {

            case 1:
                createFromSelection();
                break;

            case 2:
                loadFirstBlueprint();
                break;

            case 3:
                showGhost();
                break;

            case 4:
                placeLayer();
                break;

            case 5:
                previousLayer();
                break;

            case 6:
                nextLayer();
                break;

            case 7:
                placeAll();
                break;

            case 8:
                moveX(-10.f);
                break;

            case 9:
                moveX(10.f);
                break;

            case 10:
                moveY(-10.f);
                break;

            case 11:
                moveY(10.f);
                break;

            case 12:
                clearGhost();
                removeFromParentAndCleanup(true);
                break;

            default:
                break;
        }
    }

public:

    static BlueprintMenu* create(
        EditorUI* editor
    ) {

        auto ret =
            new BlueprintMenu();

        if (
            ret &&
            ret->init(editor)
        ) {

            ret->autorelease();

            return ret;
        }

        delete ret;

        return nullptr;
    }

    ~BlueprintMenu() {
        clearGhost();
    }
};

class BlueprintOpenButton : public CCLayer {

protected:

    EditorUI* m_editor = nullptr;

    bool init(EditorUI* editor) {

        if (!CCLayer::init())
            return false;

        m_editor = editor;

        auto menu = CCMenu::create();

        menu->setPosition(0, 0);

        this->addChild(menu);

        auto button =
            CCMenuItemSpriteExtra::create(
                ButtonSprite::create(
                    "BLUEPRINT",
                    90,
                    true,
                    "goldFont.fnt",
                    "GJ_button_01.png",
                    25,
                    0.55f
                ),
                this,
                menu_selector(
                    BlueprintOpenButton::onOpen
                )
            );

        button->setPosition(
            70.f,
            70.f
        );

        menu->addChild(button);

        this->setID(
            "blueprint.open"_spr
        );

        return true;
    }

    void onOpen(CCObject*) {

        auto menu =
            BlueprintMenu::create(
                m_editor
            );

        if (!menu)
            return;

        m_editor->addChild(
            menu,
            10000
        );
    }

public:

    static BlueprintOpenButton* create(
        EditorUI* editor
    ) {

        auto ret =
            new BlueprintOpenButton();

        if (
            ret &&
            ret->init(editor)
        ) {

            ret->autorelease();

            return ret;
        }

        delete ret;

        return nullptr;
    }
};

class $modify(
    BlueprintEditorUI,
    EditorUI
) {

    bool init(
        LevelEditorLayer* editorLayer
    ) {

        if (!EditorUI::init(
            editorLayer
        ))
            return false;

        auto button =
            BlueprintOpenButton::create(
                this
            );

        if (button) {

            this->addChild(
                button,
                9999
            );
        }

        log::info(
            "Blueprint Builder loaded"
        );

        return true;
    }
};
