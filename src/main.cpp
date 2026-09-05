#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/utils/file.hpp>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

using namespace geode::prelude;

struct SavedPiece {
    int id = 0;
    float x = 0.f;
    float y = 0.f;
    float scale = 1.f;
    float rotation = 0.f;
    int opacity = 255;
    int z = 0;
};

struct SavedDecoration {
    std::string name;
    std::vector<SavedPiece> pieces;
};

static std::filesystem::path decoFolder() {
    auto dir = Mod::get()->getSaveDir() / "decorations";
    std::error_code ec;
    std::filesystem::create_directories(dir, ec);
    return dir;
}

static std::string safeFileName(std::string name) {
    if (name.empty())
        name = "Decoration";

    for (auto& c : name) {
        if (!(std::isalnum(static_cast<unsigned char>(c)) ||
              c == '_' || c == '-')) {
            c = '_';
        }
    }

    return name;
}

static bool saveDecorationFile(
    SavedDecoration const& deco,
    std::filesystem::path const& path
) {
    std::ofstream out(path);

    if (!out)
        return false;

    out << "AUTODECO1\n";
    out << deco.name << "\n";
    out << deco.pieces.size() << "\n";

    for (auto const& p : deco.pieces) {
        out << p.id << ' '
            << p.x << ' '
            << p.y << ' '
            << p.scale << ' '
            << p.rotation << ' '
            << p.opacity << ' '
            << p.z << "\n";
    }

    return true;
}

static bool loadDecorationFile(
    std::filesystem::path const& path,
    SavedDecoration& outDeco
) {
    std::ifstream in(path);

    if (!in)
        return false;

    std::string header;
    std::getline(in, header);

    if (header != "AUTODECO1")
        return false;

    std::getline(in, outDeco.name);

    size_t count = 0;

    if (!(in >> count))
        return false;

    outDeco.pieces.clear();
    outDeco.pieces.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        SavedPiece p;

        if (!(in >>
            p.id >>
            p.x >>
            p.y >>
            p.scale >>
            p.rotation >>
            p.opacity >>
            p.z)) {
            return false;
        }

        outDeco.pieces.push_back(p);
    }

    return true;
}

static std::vector<std::filesystem::path> getDecorationFiles() {
    std::vector<std::filesystem::path> files;

    auto dir = decoFolder();

    std::error_code ec;

    for (auto const& entry :
         std::filesystem::directory_iterator(dir, ec)) {

        if (ec)
            break;

        if (entry.is_regular_file() &&
            entry.path().extension() == ".deco") {

            files.push_back(entry.path());
        }
    }

    std::sort(files.begin(), files.end());

    return files;
}

static void showInfo(
    std::string const& title,
    std::string const& text
) {
    FLAlertLayer::create(
        title.c_str(),
        text.c_str(),
        "OK"
    )->show();
}

class AutoDecoMenu : public CCLayer {

protected:

    EditorUI* m_editor = nullptr;

    CCLayerColor* m_dim = nullptr;
    CCLayerColor* m_window = nullptr;
    CCLayerColor* m_side = nullptr;

    CCLabelBMFont* m_pageTitle = nullptr;

    ScrollLayer* m_scroll = nullptr;

    CCMenu* m_tabMenu = nullptr;
    CCMenu* m_contentMenu = nullptr;

    std::vector<CCMenuItemSpriteExtra*> m_tabs;

    int m_tab = 0;

    // 0 = selected block
    // 1 = all matching blocks
    int m_applyMode = 0;

    std::string m_selectedFile;

    bool init(EditorUI* editor) {

        if (!CCLayer::init())
            return false;

        m_editor = editor;

        this->setTouchEnabled(true);

        auto win =
            CCDirector::sharedDirector()->getWinSize();

        this->setContentSize(win);

        m_dim = CCLayerColor::create(
            {0, 0, 0, 150},
            win.width,
            win.height
        );

        m_dim->setPosition(0, 0);

        this->addChild(m_dim);

        float margin = 18.f;

        float width =
            std::max(560.f, win.width - margin * 2.f);

        float height =
            std::max(320.f, win.height - margin * 2.f);

        float left =
            (win.width - width) / 2.f;

        float bottom =
            (win.height - height) / 2.f;

        m_window = CCLayerColor::create(
            {25, 21, 36, 255},
            width,
            height
        );

        m_window->setPosition(left, bottom);

        this->addChild(m_window, 2);

        auto header = CCLayerColor::create(
            {37, 30, 52, 255},
            width,
            54.f
        );

        header->setPosition(
            0,
            height - 54.f
        );

        m_window->addChild(header);

        auto title = CCLabelBMFont::create(
            "AUTO DECO",
            "goldFont.fnt"
        );

        title->setScale(0.75f);

        title->setAnchorPoint({0.f, 0.5f});

        title->setPosition(
            20.f,
            27.f
        );

        header->addChild(title);

        auto closeMenu = CCMenu::create();

        closeMenu->setPosition(
            width - 28.f,
            27.f
        );

        header->addChild(closeMenu);

        auto close =
            CCMenuItemSpriteExtra::create(
                ButtonSprite::create(
                    "X",
                    38,
                    true,
                    "goldFont.fnt",
                    "GJ_button_01.png",
                    22,
                    0.7f
                ),
                this,
                menu_selector(
                    AutoDecoMenu::onClose
                )
            );

        closeMenu->addChild(close);

        float sideW = 145.f;

        m_side = CCLayerColor::create(
            {31, 26, 44, 255},
            sideW,
            height - 54.f
        );

        m_side->setPosition(0, 0);

        m_window->addChild(m_side);

        m_tabMenu = CCMenu::create();

        m_tabMenu->setPosition(0, 0);

        m_tabMenu->setContentSize({
            sideW,
            height - 54.f
        });

        m_side->addChild(m_tabMenu);

        addTab("BLOCKS", 0, 0);
        addTab("SAVE DECO", 1, 1);
        addTab("DECORATIONS", 2, 2);
        addTab("SETTINGS", 3, 3);

        float contentX =
            sideW + 10.f;

        float contentY = 10.f;

        float contentW =
            width - sideW - 20.f;

        float contentH =
            height - 74.f;

        m_scroll = ScrollLayer::create(
            {contentW, contentH},
            true,
            true
        );

        m_scroll->setPosition(
            contentX,
            contentY
        );

        m_scroll->m_contentLayer->setContentSize({
            contentW,
            900.f
        });

        m_window->addChild(
            m_scroll,
            2
        );

        m_pageTitle =
            CCLabelBMFont::create(
                "BLOCKS",
                "bigFont.fnt"
            );

        m_pageTitle->setScale(0.55f);

        m_pageTitle->setAnchorPoint({
            0.f,
            1.f
        });

        m_pageTitle->setPosition(
            18.f,
            865.f
        );

        m_scroll->m_contentLayer
            ->addChild(m_pageTitle);

        showTab(0);

        return true;
    }

    void addTab(
        char const* text,
        int tag,
        int index
    ) {

        auto button =
            CCMenuItemSpriteExtra::create(
                ButtonSprite::create(
                    text,
                    120,
                    true,
                    "goldFont.fnt",
                    "GJ_button_01.png",
                    25,
                    0.55f
                ),
                this,
                menu_selector(
                    AutoDecoMenu::onTab
                )
            );

        button->setTag(tag);

        button->setPosition(
            72.5f,
            235.f - index * 58.f
        );

        m_tabMenu->addChild(button);

        m_tabs.push_back(button);
    }

    CCMenuItemSpriteExtra* addContentButton(
        char const* text,
        float x,
        float y,
        int tag = 0,
        float scale = 0.7f
    ) {

        auto b =
            CCMenuItemSpriteExtra::create(
                ButtonSprite::create(
                    text,
                    120,
                    true,
                    "goldFont.fnt",
                    "GJ_button_01.png",
                    25,
                    scale
                ),
                this,
                menu_selector(
                    AutoDecoMenu::onContentButton
                )
            );

        b->setTag(tag);

        b->setPosition(x, y);

        m_contentMenu->addChild(b);

        return b;
    }

    void clearContent() {

        if (m_contentMenu)
            m_contentMenu->removeFromParent();

        m_contentMenu = CCMenu::create();

        m_contentMenu->setPosition(0, 0);

        m_contentMenu->setContentSize(
            m_scroll->m_contentLayer
                ->getContentSize()
        );

        m_scroll->m_contentLayer
            ->addChild(m_contentMenu);

        m_scroll->setContentOffset(
            {0.f, 0.f},
            false
        );
    }

    void label(
        char const* text,
        float x,
        float y,
        float scale = 0.42f
    ) {

        auto l =
            CCLabelBMFont::create(
                text,
                "goldFont.fnt"
            );

        l->setScale(scale);

        l->setAnchorPoint({
            0.f,
            0.5f
        });

        l->setPosition(x, y);

        m_contentMenu->addChild(l);
    }

    void showTab(int tab) {

        m_tab = tab;

        const char* titles[] = {
            "BLOCKS",
            "SAVE DECORATION",
            "DECORATIONS",
            "SETTINGS"
        };

        m_pageTitle->setString(
            titles[tab]
        );

        clearContent();

        if (tab == 0)
            buildBlocks();

        else if (tab == 1)
            buildSave();

        else if (tab == 2)
            buildDecorations();

        else
            buildSettings();
    }

    void buildBlocks() {

        label(
            "Choose what Auto Deco should decorate.",
            18.f,
            820.f
        );

        label(
            "Select a block in the editor first.",
            18.f,
            790.f
        );

        auto one =
            addContentButton(
                "ONE SELECTED",
                100.f,
                735.f,
                10,
                0.55f
            );

        auto all =
            addContentButton(
                "ALL MATCHING",
                250.f,
                735.f,
                11,
                0.55f
            );

        if (m_applyMode == 0)
            one->setColor({
                120,
                210,
                255
            });

        else
            all->setColor({
                120,
                210,
                255
            });

        addContentButton(
            "DECORATE",
            175.f,
            650.f,
            12,
            0.75f
        );

        label(
            "ONE SELECTED = only the block you picked.",
            18.f,
            585.f,
            0.38f
        );

        label(
            "ALL MATCHING = every block with the same ID.",
            18.f,
            555.f,
            0.38f
        );

        std::string current =
            "No decoration selected";

        if (!m_selectedFile.empty())
            current =
                "Loaded: " + m_selectedFile;

        label(
            current.c_str(),
            18.f,
            500.f,
            0.42f
        );

        if (!m_selectedFile.empty()) {

            addContentButton(
                "CLEAR",
                175.f,
                435.f,
                13,
                0.55f
            );
        }
    }

    void buildSave() {

        label(
            "Select the decoration pieces in the editor,",
            18.f,
            820.f
        );

        label(
            "then press SAVE DECORATION.",
            18.f,
            790.f
        );

        label(
            "The first selected object becomes the anchor.",
            18.f,
            745.f,
            0.38f
        );

        addContentButton(
            "SAVE DECORATION",
            175.f,
            665.f,
            20,
            0.65f
        );

        addContentButton(
            "OPEN FOLDER",
            175.f,
            600.f,
            21,
            0.55f
        );

        label(
            "Saved files are stored in the mod's",
            18.f,
            525.f,
            0.38f
        );

        label(
            "decorations folder.",
            18.f,
            495.f,
            0.38f
        );
    }

    void buildDecorations() {

        auto files =
            getDecorationFiles();

        if (files.empty()) {

            label(
                "No saved decorations yet.",
                18.f,
                800.f
            );

            label(
                "Use SAVE DECO after selecting your pieces.",
                18.f,
                760.f,
                0.38f
            );

            return;
        }

        float y = 805.f;

        int tag = 100;

        for (auto const& path : files) {

            SavedDecoration d;

            if (!loadDecorationFile(
                path,
                d
            ))
                continue;

            auto row =
                CCMenuItemSpriteExtra::create(
                    ButtonSprite::create(
                        d.name.c_str(),
                        120,
                        true,
                        "goldFont.fnt",
                        "GJ_button_01.png",
                        25,
                        0.55f
                    ),
                    this,
                    menu_selector(
                        AutoDecoMenu::onContentButton
                    )
                );

            row->setTag(tag++);

            row->setUserObject(
                CCString::create(
                    path.string()
                )
            );

            row->setPosition(
                95.f,
                y
            );

            m_contentMenu->addChild(row);

            auto load =
                addContentButton(
                    "LOAD",
                    225.f,
                    y,
                    row->getTag() + 1000,
                    0.48f
                );

            load->setUserObject(
                CCString::create(
                    path.string()
                )
            );

            auto del =
                addContentButton(
                    "DELETE",
                    325.f,
                    y,
                    row->getTag() + 2000,
                    0.48f
                );

            del->setUserObject(
                CCString::create(
                    path.string()
                )
            );

            y -= 58.f;

            if (y < 80.f)
                break;
        }
    }

    void buildSettings() {

        label(
            "Auto Deco",
            18.f,
            820.f,
            0.5f
        );

        label(
            "Saved decorations use the exact selected pieces:",
            18.f,
            770.f,
            0.38f
        );

        label(
            "object ID, position, scale, rotation, opacity and Z order.",
            18.f,
            740.f,
            0.38f
        );

        label(
            "No separate drag button is used.",
            18.f,
            680.f,
            0.38f
        );

        label(
            "The editor menu uses smooth scrolling for long lists.",
            18.f,
            650.f,
            0.38f
        );
    }

    std::vector<GameObject*> selectedObjects() const {

        std::vector<GameObject*> result;

        if (!m_editor ||
            !m_editor->m_selectedObjects)
            return result;

        auto count =
            m_editor->m_selectedObjects->count();

        for (unsigned i = 0;
             i < count;
             ++i) {

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

    bool saveCurrentSelection() {

        auto selected =
            selectedObjects();

        if (selected.empty()) {

            showInfo(
                "Auto Deco",
                "Select your decoration pieces first."
            );

            return false;
        }

        auto anchor =
            selected.front();

        SavedDecoration d;

        d.name = fmt::format(
            "Decoration_{}",
            getDecorationFiles().size() + 1
        );

        auto anchorPos =
            anchor->getPosition();

        for (auto obj : selected) {

            SavedPiece p;

            p.id =
                obj->m_objectID;

            p.x =
                obj->getPositionX()
                - anchorPos.x;

            p.y =
                obj->getPositionY()
                - anchorPos.y;

            p.scale =
                obj->getScale();

            p.rotation =
                obj->getRotation();

            p.opacity =
                obj->getOpacity();

            p.z =
                obj->getZOrder();

            d.pieces.push_back(p);
        }

        auto path =
            decoFolder() /
            (safeFileName(d.name) + ".deco");

        if (!saveDecorationFile(
            d,
            path
        )) {

            showInfo(
                "Auto Deco",
                "Couldn't save the decoration."
            );

            return false;
        }

        m_selectedFile =
            path.string();

        showInfo(
            "Auto Deco",
            fmt::format(
                "Saved {} pieces as {}.",
                d.pieces.size(),
                d.name
            ).c_str()
        );

        showTab(2);

        return true;
    }

    bool applyDecoration(
        std::filesystem::path const& path
    ) {

        SavedDecoration d;

        if (!loadDecorationFile(
            path,
            d
        )) {

            showInfo(
                "Auto Deco",
                "That decoration file could not be loaded."
            );

            return false;
        }

        auto selected =
            selectedObjects();

        if (selected.empty()) {

            showInfo(
                "Auto Deco",
                "Select a block first in the editor."
            );

            return false;
        }

        std::vector<GameObject*> targets;

        if (m_applyMode == 0) {

            targets.push_back(
                selected.front()
            );

        } else {

            if (!m_editor->m_editorLayer ||
                !m_editor->m_editorLayer->m_objects)
                return false;

            int wantedID =
                selected.front()->m_objectID;

            auto objects =
                m_editor->m_editorLayer->m_objects;

            for (unsigned i = 0;
                 i < objects->count();
                 ++i) {

                auto obj =
                    static_cast<GameObject*>(
                        objects->objectAtIndex(i)
                    );

                if (obj &&
                    obj->m_objectID == wantedID) {

                    targets.push_back(obj);
                }
            }
        }

        if (!m_editor->m_editorLayer)
            return false;

        int created = 0;

        for (auto target : targets) {

            auto base =
                target->getPosition();

            for (auto const& p : d.pieces) {

                auto obj =
                    m_editor->m_editorLayer->createObject(
                        p.id,
                        {
                            base.x + p.x,
                            base.y + p.y
                        },
                        false
                    );

                if (!obj)
                    continue;

                obj->setScale(
                    p.scale
                );

                obj->setRotation(
                    p.rotation
                );

                obj->setOpacity(
                    static_cast<GLubyte>(
                        std::clamp(
                            p.opacity,
                            0,
                            255
                        )
                    )
                );

                obj->setZOrder(
                    p.z
                );

                created++;
            }
        }

        showInfo(
            "Auto Deco",
            fmt::format(
                "Placed {} decoration pieces.",
                created
            ).c_str()
        );

        return true;
    }

    void deleteDecoration(
        std::filesystem::path const& path
    ) {

        std::error_code ec;

        std::filesystem::remove(
            path,
            ec
        );

        if (ec) {

            showInfo(
                "Auto Deco",
                "Couldn't delete that decoration."
            );

        } else {

            if (m_selectedFile ==
                path.string()) {

                m_selectedFile.clear();
            }

            showTab(2);
        }
    }

    void onTab(CCObject* sender) {

        auto item =
            static_cast<CCMenuItemSpriteExtra*>(
                sender
            );

        showTab(
            item->getTag()
        );
    }

    void onContentButton(
        CCObject* sender
    ) {

        auto item =
            static_cast<CCMenuItemSpriteExtra*>(
                sender
            );

        int tag =
            item->getTag();

        if (tag == 10) {

            m_applyMode = 0;

            showTab(0);

            return;
        }

        if (tag == 11) {

            m_applyMode = 1;

            showTab(0);

            return;
        }

        if (tag == 12) {

            if (m_selectedFile.empty()) {

                showInfo(
                    "Auto Deco",
                    "Go to Decorations and LOAD one first."
                );

                return;
            }

            applyDecoration(
                std::filesystem::path(
                    m_selectedFile
                )
            );

            return;
        }

        if (tag == 13) {

            m_selectedFile.clear();

            showTab(0);

            return;
        }

        if (tag == 20) {

            saveCurrentSelection();

            return;
        }

        if (tag == 21) {

            utils::file::openFolder(
                decoFolder()
            );

            return;
        }

        if (tag >= 1000 &&
            tag < 3000) {

            auto pathObj =
                item->getUserObject();

            auto str =
                typeinfo_cast<CCString*>(
                    pathObj
                );

            if (!str)
                return;

            auto path =
                std::filesystem::path(
                    str->getCString()
                );

            if (tag >= 2000) {

                deleteDecoration(path);

                return;
            }

            if (tag >= 1000) {

                m_selectedFile =
                    path.string();

                showTab(0);

                return;
            }
        }
    }

    void onClose(CCObject*) {

        this->removeFromParentAndCleanup(
            true
        );

        if (m_editor) {

            auto panel =
                m_editor->getChildByID(
                    "autodeco.open"
                );

            if (panel)
                panel->setVisible(true);
        }
    }

public:

    static AutoDecoMenu* create(
        EditorUI* editor
    ) {

        auto ret =
            new AutoDecoMenu();

        if (ret &&
            ret->init(editor)) {

            ret->autorelease();

            return ret;
        }

        delete ret;

        return nullptr;
    }
};

class AutoDecoOpenButton : public CCLayer {

protected:

    EditorUI* m_editor = nullptr;

    bool init(EditorUI* editor) {

        if (!CCLayer::init())
            return false;

        m_editor = editor;

        auto menu =
            CCMenu::create();

        menu->setPosition(0, 0);

        this->addChild(menu);

        auto button =
            CCMenuItemSpriteExtra::create(
                ButtonSprite::create(
                    "AUTO DECO",
                    80,
                    true,
                    "goldFont.fnt",
                    "GJ_button_01.png",
                    25,
                    0.55f
                ),
                this,
                menu_selector(
                    AutoDecoOpenButton::onOpen
                )
            );

        button->setPosition(
            75.f,
            75.f
        );

        menu->addChild(button);

        this->setID(
            "autodeco.open"
        );

        return true;
    }

    void onOpen(CCObject*) {

        auto menu =
            AutoDecoMenu::create(
                m_editor
            );

        if (!menu)
            return;

        m_editor->addChild(
            menu,
            10000
        );

        this->setVisible(false);
    }

public:

    static AutoDecoOpenButton* create(
        EditorUI* editor
    ) {

        auto ret =
            new AutoDecoOpenButton();

        if (ret &&
            ret->init(editor)) {

            ret->autorelease();

            return ret;
        }

        delete ret;

        return nullptr;
    }
};

class $modify(
    AutoDecoEditorUI,
    EditorUI
) {

    bool init(
        LevelEditorLayer* editorLayer
    ) {

        if (!EditorUI::init(
            editorLayer
        ))
            return false;

        auto open =
            AutoDecoOpenButton::create(
                this
            );

        if (open) {

            this->addChild(
                open,
                9999
            );
        }

        log::info(
            "Auto Deco loaded"
        );

        return true;
    }
};
