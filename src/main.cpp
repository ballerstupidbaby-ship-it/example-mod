#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/utils/file.hpp>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <vector>

using namespace geode::prelude;

struct DecoPiece {
    int id;
    float x;
    float y;
    float scale;
    float rotation;
    int z;
};

static std::vector<DecoPiece> getStyle(int style, float w, float h) {
    float sx = std::max(20.f, w * 0.5f);
    float sy = std::max(20.f, h * 0.5f);

    if (style == 1) {
        return {
            {207, -sx, sy, 0.65f, 0.f, 1},
            {207, sx, sy, 0.65f, 0.f, 1},
            {207, -sx, -sy, 0.65f, 0.f, 1},
            {207, sx, -sy, 0.65f, 0.f, 1},
            {227, -sx, 0.f, 0.55f, 0.f, 2},
            {227, sx, 0.f, 0.55f, 180.f, 2},
            {503, 0.f, sy + 10.f, 0.65f, 0.f, 2},
            {503, 0.f, -sy - 10.f, 0.65f, 180.f, 2}
        };
    }

    if (style == 2) {
        return {
            {228, -sx, sy, 0.55f, 0.f, 2},
            {228, sx, sy, 0.55f, 90.f, 2},
            {228, -sx, -sy, 0.55f, 270.f, 2},
            {228, sx, -sy, 0.55f, 180.f, 2},
            {237, -sx - 8.f, 0.f, 0.55f, 90.f, 1},
            {237, sx + 8.f, 0.f, 0.55f, 90.f, 1},
            {503, 0.f, 0.f, 0.45f, 0.f, 2}
        };
    }

    return {
        {1202, -sx, 0.f, 0.55f, 90.f, 2},
        {1202, sx, 0.f, 0.55f, 90.f, 2},
        {1202, 0.f, sy, 0.55f, 0.f, 2},
        {1202, 0.f, -sy, 0.55f, 180.f, 2},
        {227, -sx, sy, 0.45f, 0.f, 3},
        {227, sx, sy, 0.45f, 90.f, 3},
        {227, -sx, -sy, 0.45f, 270.f, 3},
        {227, sx, -sy, 0.45f, 180.f, 3}
    };
}

class AutoDecoPanel : public CCLayer {
public:
    EditorUI* m_editor = nullptr;

    CCLabelBMFont* m_title = nullptr;
    CCLabelBMFont* m_status = nullptr;

    CCMenuItemSpriteExtra* m_deco = nullptr;
    CCMenuItemSpriteExtra* m_style = nullptr;
    CCMenuItemSpriteExtra* m_save = nullptr;
    CCMenuItemSpriteExtra* m_copy = nullptr;

    bool m_dragging = false;
    bool m_moved = false;

    CCPoint m_downPos = CCPointZero;
    CCPoint m_startPanelPos = CCPointZero;

    CCMenuItemSpriteExtra* m_pressed = nullptr;

    int m_styleIndex = 1;
    int m_savedID = 0;
    int m_nextDesign = 1;

    static AutoDecoPanel* create(EditorUI* editor) {
        auto ret = new AutoDecoPanel();

        if (ret && ret->init(editor)) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }

    bool init(EditorUI* editor) {
        if (!CCLayer::init())
            return false;

        m_editor = editor;

        this->setContentSize(CCSize(360.f, 118.f));
        this->setTouchEnabled(true);

        auto bg = CCScale9Sprite::create("square02b_001.png");

        if (bg) {
            bg->setContentSize(CCSize(360.f, 118.f));
            bg->setOpacity(235);
            bg->setPosition(CCPoint(180.f, 59.f));
            this->addChild(bg, -2);
        }

        auto accent = CCScale9Sprite::create("square02b_001.png");

        if (accent) {
            accent->setContentSize(CCSize(330.f, 3.f));
            accent->setOpacity(220);
            accent->setPosition(CCPoint(180.f, 91.f));
            this->addChild(accent, -1);
        }

        m_title = CCLabelBMFont::create(
            "AUTO DECO",
            "bigFont.fnt"
        );

        m_title->setScale(0.50f);
        m_title->setPosition(CCPoint(180.f, 102.f));
        this->addChild(m_title);

        m_status = CCLabelBMFont::create(
            "NO TARGET",
            "goldFont.fnt"
        );

        m_status->setScale(0.34f);
        m_status->setPosition(CCPoint(180.f, 76.f));
        this->addChild(m_status);

        auto menu = CCMenu::create();
        menu->setPosition(CCPointZero);
        this->addChild(menu, 5);

        m_save = makeButton(
            "SAVE",
            55.f,
            40.f,
            0.48f
        );

        m_deco = makeButton(
            "DECO",
            130.f,
            40.f,
            0.48f
        );

        m_style = makeButton(
            "STYLE 1",
            215.f,
            40.f,
            0.42f
        );

        m_copy = makeButton(
            "COPY",
            305.f,
            40.f,
            0.48f
        );

        menu->addChild(m_save);
        menu->addChild(m_deco);
        menu->addChild(m_style);
        menu->addChild(m_copy);

        return true;
    }

    CCMenuItemSpriteExtra* makeButton(
        const char* text,
        float x,
        float y,
        float scale
    ) {
        auto sprite = ButtonSprite::create(
            text,
            45,
            true,
            "goldFont.fnt",
            "GJ_button_01.png",
            22,
            scale
        );

        auto item = CCMenuItemSpriteExtra::create(
            sprite,
            this,
            nullptr
        );

        item->setPosition(CCPoint(x, y));

        return item;
    }

    bool insideButton(
        CCMenuItemSpriteExtra* item,
        CCPoint p
    ) {
        if (!item)
            return false;

        return item->boundingBox().containsPoint(p);
    }

    CCMenuItemSpriteExtra* buttonAt(CCPoint p) {
        if (insideButton(m_save, p))
            return m_save;

        if (insideButton(m_deco, p))
            return m_deco;

        if (insideButton(m_style, p))
            return m_style;

        if (insideButton(m_copy, p))
            return m_copy;

        return nullptr;
    }

    void registerWithTouchDispatcher() {
        CCDirector::sharedDirector()
            ->getTouchDispatcher()
            ->addTargetedDelegate(
                this,
                -500,
                true
            );
    }

    bool ccTouchBegan(
        CCTouch* touch,
        CCEvent*
    ) {
        auto p = this->convertTouchToNodeSpace(touch);

        if (
            p.x < 0.f ||
            p.y < 0.f ||
            p.x > 360.f ||
            p.y > 118.f
        ) {
            return false;
        }

        m_pressed = buttonAt(p);

        m_dragging = false;
        m_moved = false;

        m_downPos = p;
        m_startPanelPos = this->getPosition();

        return true;
    }

    void ccTouchMoved(
        CCTouch* touch,
        CCEvent*
    ) {
        auto p = this->convertTouchToNodeSpace(touch);

        float dx = p.x - m_downPos.x;
        float dy = p.y - m_downPos.y;

        if (
            !m_dragging &&
            (
                std::fabs(dx) > 10.f ||
                std::fabs(dy) > 10.f
            )
        ) {
            m_dragging = true;
            m_moved = true;
            m_pressed = nullptr;
        }

        if (!m_dragging)
            return;

        auto parent = this->getParent();

        if (!parent)
            return;

        auto newPos = CCPoint(
            m_startPanelPos.x + dx,
            m_startPanelPos.y + dy
        );

        auto size = parent->getContentSize();

        newPos.x = std::clamp(
            newPos.x,
            180.f,
            std::max(
                180.f,
                size.width - 180.f
            )
        );

        newPos.y = std::clamp(
            newPos.y,
            59.f,
            std::max(
                59.f,
                size.height - 59.f
            )
        );

        this->setPosition(newPos);
    }

    void ccTouchEnded(
        CCTouch*,
        CCEvent*
    ) {
        if (
            !m_dragging &&
            !m_moved &&
            m_pressed
        ) {
            if (m_pressed == m_deco)
                onDeco();

            else if (m_pressed == m_style)
                onStyle();

            else if (m_pressed == m_save)
                onSave();

            else if (m_pressed == m_copy)
                onCopy();
        }

        m_dragging = false;
        m_moved = false;
        m_pressed = nullptr;
    }

    void ccTouchCancelled(
        CCTouch*,
        CCEvent*
    ) {
        m_dragging = false;
        m_moved = false;
        m_pressed = nullptr;
    }

    void updateStatus() {
        if (m_savedID == 0) {
            m_status->setString("NO TARGET");
        }
        else {
            m_status->setString(
                fmt::format(
                    "TARGET {}  •  STYLE {}",
                    m_savedID,
                    m_styleIndex
                ).c_str()
            );
        }
    }

    void onStyle() {
        m_styleIndex++;

        if (m_styleIndex > 3)
            m_styleIndex = 1;

        m_style->setNormalImage(
            ButtonSprite::create(
                fmt::format(
                    "STYLE {}",
                    m_styleIndex
                ).c_str(),
                45,
                true,
                "goldFont.fnt",
                "GJ_button_01.png",
                22,
                0.42f
            )
        );

        updateStatus();
    }

    std::filesystem::path decorationsDir() {
        auto dir =
            Mod::get()->getSaveDir() /
            "decorations";

        utils::file::createDirectoryAll(dir);

        return dir;
    }

    std::filesystem::path designPath(int number) {
        return decorationsDir() /
            fmt::format(
                "Design_{}.json",
                number
            );
    }

    bool saveDesign(int number) {
        auto path = designPath(number);

        auto pieces = getStyle(
            m_styleIndex,
            80.f,
            80.f
        );

        std::ofstream file(
            path.string(),
            std::ios::out |
            std::ios::trunc
        );

        if (!file.is_open())
            return false;

        file << "{\n";

        file << "  \"name\": "
             << "\"Design_" << number << "\",\n";

        file << "  \"target_id\": "
             << m_savedID << ",\n";

        file << "  \"style\": "
             << m_styleIndex << ",\n";

        file << "  \"pieces\": [\n";

        for (size_t i = 0; i < pieces.size(); i++) {
            auto const& piece = pieces[i];

            file << "    {"
                 << "\"id\":" << piece.id
                 << ",\"x\":" << piece.x
                 << ",\"y\":" << piece.y
                 << ",\"scale\":" << piece.scale
                 << ",\"rotation\":" << piece.rotation
                 << ",\"z\":" << piece.z
                 << "}";

            if (i + 1 < pieces.size())
                file << ",";

            file << "\n";
        }

        file << "  ]\n";
        file << "}\n";

        return true;
    }

    int findNextDesign() {
        int n = 1;

        while (
            utils::file::fileExists(
                designPath(n)
            )
        ) {
            n++;
        }

        return n;
    }

    void onSave() {
        auto selected =
            m_editor ?
            m_editor->m_selectedObjects :
            nullptr;

        if (
            !selected ||
            selected->count() != 1
        ) {
            FLAlertLayer::create(
                "Auto Deco",
                "Pick one object first.",
                "OK"
            )->show();

            return;
        }

        auto object =
            static_cast<GameObject*>(
                selected->objectAtIndex(0)
            );

        if (!object)
            return;

        m_savedID = object->m_objectID;
        m_nextDesign = findNextDesign();

        if (!saveDesign(m_nextDesign)) {
            FLAlertLayer::create(
                "Auto Deco",
                "Couldn't save the decoration.",
                "OK"
            )->show();

            return;
        }

        auto message = fmt::format(
            "Saved Design_{}.json\n\n"
            "Decorations folder created automatically.",
            m_nextDesign
        );

        m_nextDesign++;

        updateStatus();

        FLAlertLayer::create(
            "Auto Deco",
            message.c_str(),
            "OK"
        )->show();
    }

    void onCopy() {
        int source = findNextDesign() - 1;

        if (
            source < 1 ||
            !utils::file::fileExists(
                designPath(source)
            )
        ) {
            FLAlertLayer::create(
                "Auto Deco",
                "Save a design first.",
                "OK"
            )->show();

            return;
        }

        int copyNumber = findNextDesign();

        auto sourcePath = designPath(source);
        auto destPath = designPath(copyNumber);

        auto result =
            utils::file::readString(sourcePath);

        if (!result) {
            FLAlertLayer::create(
                "Auto Deco",
                "Couldn't read the saved design.",
                "OK"
            )->show();

            return;
        }

        auto text = *result;

        auto oldName =
            fmt::format(
                "\"Design_{}\"",
                source
            );

        auto newName =
            fmt::format(
                "\"Design_{}\"",
                copyNumber
            );

        auto pos = text.find(oldName);

        if (pos != std::string::npos) {
            text.replace(
                pos,
                oldName.size(),
                newName
            );
        }

        if (
            !utils::file::writeString(
                destPath,
                text
            ).isOk()
        ) {
            FLAlertLayer::create(
                "Auto Deco",
                "Couldn't copy the design.",
                "OK"
            )->show();

            return;
        }

        FLAlertLayer::create(
            "Auto Deco",
            fmt::format(
                "Copied Design_{}.json → Design_{}.json",
                source,
                copyNumber
            ).c_str(),
            "OK"
        )->show();
    }

    void onDeco() {
        if (
            !m_editor ||
            !m_editor->m_editorLayer
        ) {
            return;
        }

        int targetID = m_savedID;

        if (targetID == 0) {
            auto selected =
                m_editor->m_selectedObjects;

            if (
                !selected ||
                selected->count() != 1
            ) {
                FLAlertLayer::create(
                    "Auto Deco",
                    "Pick one object first.",
                    "OK"
                )->show();

                return;
            }

            auto object =
                static_cast<GameObject*>(
                    selected->objectAtIndex(0)
                );

            if (!object)
                return;

            targetID = object->m_objectID;
            m_savedID = targetID;

            updateStatus();
        }

        auto objects =
            m_editor->m_editorLayer->m_objects;

        if (!objects)
            return;

        int created = 0;

        auto originalCount =
            objects->count();

        for (
            unsigned int i = 0;
            i < originalCount;
            i++
        ) {
            auto target =
                static_cast<GameObject*>(
                    objects->objectAtIndex(i)
                );

            if (
                !target ||
                target->m_objectID != targetID
            ) {
                continue;
            }

            auto size =
                target->getContentSize();

            auto pieces = getStyle(
                m_styleIndex,
                size.width *
                    target->getScaleX(),
                size.height *
                    target->getScaleY()
            );

            for (auto const& piece : pieces) {
                auto pos =
                    target->getPosition() +
                    CCPoint(
                        piece.x,
                        piece.y
                    );

                auto createdObject =
                    m_editor->m_editorLayer
                        ->createObject(
                            piece.id,
                            pos,
                            false
                        );

                if (!createdObject)
                    continue;

                createdObject->setScale(
                    piece.scale
                );

                createdObject->setRotation(
                    piece.rotation
                );

                created++;
            }
        }

        FLAlertLayer::create(
            "Auto Deco",
            fmt::format(
                "Decorated {} matching objects.",
                created
            ).c_str(),
            "OK"
        )->show();
    }
};

class $modify(
    AutoDecoEditorUI,
    EditorUI
) {
    struct Fields {
        AutoDecoPanel* panel = nullptr;
    };

    bool init(
        LevelEditorLayer* editorLayer
    ) {
        if (
            !EditorUI::init(
                editorLayer
            )
        ) {
            return false;
        }

        auto panel =
            AutoDecoPanel::create(this);

        if (!panel)
            return true;

        panel->setPosition(
            CCPoint(180.f, 75.f)
        );

        this->addChild(
            panel,
            9999
        );

        m_fields->panel = panel;

        log::info(
            "Auto Deco loaded"
        );

        return true;
    }
};
``` [❶](code://python)
