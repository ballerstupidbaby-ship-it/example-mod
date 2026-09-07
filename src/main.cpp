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

    enum class Mode {
        Main,
        Picker,
        Layers
    };

    static constexpr int TAG_SAVE = 1;
    static constexpr int TAG_LOAD = 2;
    static constexpr int TAG_PLACE_LAYER = 4;
    static constexpr int TAG_PREV = 5;
    static constexpr int TAG_NEXT = 6;
    static constexpr int TAG_PLACE_ALL = 7;
    static constexpr int TAG_X_MINUS = 8;
    static constexpr int TAG_X_PLUS = 9;
    static constexpr int TAG_Y_MINUS = 10;
    static constexpr int TAG_Y_PLUS = 11;
    static constexpr int TAG_CLOSE = 12;
    static constexpr int TAG_NEW = 13;
    static constexpr int TAG_BACK = 14;
    static constexpr int TAG_PICKER_BASE = 100;
    static constexpr int MAX_PICKER_ENTRIES = 6;

    float const m_panelWidth = 560.f;
    float const m_panelHeight = 460.f;

    EditorUI* m_editor = nullptr;

    Blueprint m_blueprint;

    bool m_hasBlueprint = false;

    std::vector<int> m_layers;

    int m_currentLayer = 0;

    CCNode* m_ghostLayer = nullptr;

    CCLabelBMFont* m_layerLabel = nullptr;

    CCLabelBMFont* m_statusLabel = nullptr;

    float m_blueprintX = 0.f;
    float m_blueprintY = 0.f;

    CCMenu* m_mainMenu = nullptr;
    CCMenu* m_pickerMenu = nullptr;
    CCMenu* m_layerMenu = nullptr;
    CCMenu* m_closeMenu = nullptr;

    std::vector<std::filesystem::path> m_blueprintFiles;

    Mode m_mode = Mode::Main;

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

        auto panel =
            CCLayerColor::create(
                {30, 25, 42, 255},
                m_panelWidth,
                m_panelHeight
            );

        panel->setPosition(
            (win.width - m_panelWidth) / 2.f,
            (win.height - m_panelHeight) / 2.f
        );

        this->addChild(panel);

        auto title =
            CCLabelBMFont::create(
                "BLUEPRINT BUILDER",
                "goldFont.fnt"
            );

        title->setScale(0.7f);

        title->setPosition(
            m_panelWidth / 2.f - 40.f,
            m_panelHeight - 35.f
        );

        panel->addChild(title);

        m_layerLabel =
            CCLabelBMFont::create(
                "NO BLUEPRINT",
                "bigFont.fnt"
            );

        m_layerLabel->setScale(0.45f);

        m_layerLabel->setPosition(
            m_panelWidth / 2.f,
            m_panelHeight - 85.f
        );

        panel->addChild(m_layerLabel);

        m_statusLabel =
            CCLabelBMFont::create(
                "Select objects, then press SAVE",
                "goldFont.fnt"
            );

        m_statusLabel->setScale(0.38f);

        m_statusLabel->setPosition(
            m_panelWidth / 2.f,
            m_panelHeight - 115.f
        );

        panel->addChild(m_statusLabel);

        m_closeMenu = CCMenu::create();
        m_closeMenu->setPosition(0, 0);
        panel->addChild(m_closeMenu);

        addButton(
            m_closeMenu,
            "CLOSE",
            m_panelWidth - 55.f,
            m_panelHeight - 25.f,
            TAG_CLOSE
        );

        m_mainMenu = CCMenu::create();
        m_mainMenu->setPosition(0, 0);
        panel->addChild(m_mainMenu);

        addButton(
            m_mainMenu,
            "SAVE",
            m_panelWidth / 2.f - 110.f,
            m_panelHeight / 2.f,
            TAG_SAVE
        );

        addButton(
            m_mainMenu,
            "LOAD",
            m_panelWidth / 2.f + 110.f,
            m_panelHeight / 2.f,
            TAG_LOAD
        );

        m_pickerMenu = CCMenu::create();
        m_pickerMenu->setPosition(0, 0);
        panel->addChild(m_pickerMenu);

        m_layerMenu = CCMenu::create();
        m_layerMenu->setPosition(0, 0);
        panel->addChild(m_layerMenu);

        addButton(
            m_layerMenu,
            "PREV",
            110.f,
            230.f,
            TAG_PREV
        );

        addButton(
            m_layerMenu,
            "NEXT",
            210.f,
            230.f,
            TAG_NEXT
        );

        addButton(
            m_layerMenu,
            "PLACE LAYER",
            370.f,
            230.f,
            TAG_PLACE_LAYER
        );

        addButton(
            m_layerMenu,
            "PLACE ALL",
            490.f,
            230.f,
            TAG_PLACE_ALL
        );

        addButton(
            m_layerMenu,
            "X -",
            110.f,
            160.f,
            TAG_X_MINUS
        );

        addButton(
            m_layerMenu,
            "X +",
            210.f,
            160.f,
            TAG_X_PLUS
        );

        addButton(
            m_layerMenu,
            "Y -",
            370.f,
            160.f,
            TAG_Y_MINUS
        );

        addButton(
            m_layerMenu,
            "Y +",
            490.f,
            160.f,
            TAG_Y_PLUS
        );

        addButton(
            m_layerMenu,
            "NEW BLUEPRINT",
            m_panelWidth / 2.f,
            90.f,
            TAG_NEW
        );

        setMode(Mode::Main);

        return true;
    }

    void addButton(
        CCMenu* menu,
        std::string const& text,
        float x,
        float y,
        int tag
    ) {

        auto button =
            CCMenuItemSpriteExtra::create(
                ButtonSprite::create(
                    text.c_str(),
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

    void setMode(Mode mode) {

        m_mode = mode;

        m_mainMenu->setVisible(mode == Mode::Main);
        m_pickerMenu->setVisible(mode == Mode::Picker);
        m_layerMenu->setVisible(mode == Mode::Layers);

        if (mode == Mode::Main) {

            if (!m_hasBlueprint) {

                m_layerLabel->setString(
                    "NO BLUEPRINT"
                );

                m_statusLabel->setString(
                    "Select objects, then press SAVE"
                );
            }

        } else if (mode == Mode::Picker) {

            m_layerLabel->setString(
                "CHOOSE A BLUEPRINT"
            );

            m_statusLabel->setString(
                "Select where it should start first"
            );
        }
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

        Blueprint toSave;

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

            toSave.objects.push_back(data);
        }

        toSave.name =
            "Blueprint_" +
            std::to_string(
                getBlueprints().size() + 1
            );

        auto path =
            getBlueprintFolder() /
            (
                toSave.name +
                ".blueprint"
            );

        if (!saveBlueprint(
            toSave,
            path
        )) {

            showMessage(
                "Blueprint",
                "Failed to save blueprint."
            );

            return false;
        }

        showMessage(
            "Blueprint",
            fmt::format(
                "Saved {} objects as {}. Use LOAD to place it.",
                toSave.objects.size(),
                toSave.name
            ).c_str()
        );

        setMode(Mode::Main);

        return true;
    }

    void openPicker() {

        m_blueprintFiles = getBlueprints();

        if (m_blueprintFiles.empty()) {

            showMessage(
                "Blueprint",
                "No saved blueprints yet. Select objects and press SAVE first."
            );

            return;
        }

        rebuildPickerMenu();

        setMode(Mode::Picker);
    }

    void rebuildPickerMenu() {

        m_pickerMenu->removeAllChildrenWithCleanup(true);

        addButton(
            m_pickerMenu,
            "BACK",
            m_panelWidth / 2.f,
            55.f,
            TAG_BACK
        );

        float y = m_panelHeight - 160.f;

        int shown =
            std::min(
                static_cast<int>(m_blueprintFiles.size()),
                MAX_PICKER_ENTRIES
            );

        for (int i = 0; i < shown; ++i) {

            auto name =
                m_blueprintFiles[i]
                    .stem()
                    .string();

            addButton(
                m_pickerMenu,
                name,
                m_panelWidth / 2.f,
                y,
                TAG_PICKER_BASE + i
            );

            y -= 40.f;
        }
    }

    void loadPicked(int index) {

        if (
            index < 0 ||
            index >= static_cast<int>(m_blueprintFiles.size())
        )
            return;

        auto selected = getSelected();

        if (selected.empty()) {

            showMessage(
                "Blueprint",
                "Select the object where the blueprint should start, then press LOAD again."
            );

            setMode(Mode::Main);

            return;
        }

        Blueprint loaded;

        if (!loadBlueprint(
            m_blueprintFiles[index],
            loaded
        )) {

            showMessage(
                "Blueprint",
                "Failed to load blueprint."
            );

            setMode(Mode::Main);

            return;
        }

        m_blueprint = loaded;

        m_hasBlueprint = true;

        rebuildLayers();

        m_currentLayer = 0;

        auto anchorPos =
            selected.front()->getPosition();

        m_blueprintX = anchorPos.x;
        m_blueprintY = anchorPos.y;

        updateUI();

        showGhost();

        setMode(Mode::Layers);
    }

    void newBlueprint() {

        clearGhost();

        m_hasBlueprint = false;

        m_blueprint = Blueprint{};

        setMode(Mode::Main);
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
                "Select objects, then press SAVE"
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

        int tag = button->getTag();

        if (tag >= TAG_PICKER_BASE) {

            loadPicked(tag - TAG_PICKER_BASE);

            return;
        }

        switch (tag) {

            case TAG_SAVE:
                createFromSelection();
                break;

            case TAG_LOAD:
                openPicker();
                break;

            case TAG_BACK:
                setMode(Mode::Main);
                break;

            case TAG_PLACE_LAYER:
                placeLayer();
                break;

            case TAG_PREV:
                previousLayer();
                break;

            case TAG_NEXT:
                nextLayer();
                break;

            case TAG_PLACE_ALL:
                placeAll();
                break;

            case TAG_X_MINUS:
                moveX(-10.f);
                break;

            case TAG_X_PLUS:
                moveX(10.f);
                break;

            case TAG_Y_MINUS:
                moveY(-10.f);
                break;

            case TAG_Y_PLUS:
                moveY(10.f);
                break;

            case TAG_NEW:
                newBlueprint();
                break;

            case TAG_CLOSE:
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
