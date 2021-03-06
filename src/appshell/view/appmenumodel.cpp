//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2021 MuseScore BVBA and others
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================
#include "appmenumodel.h"

#include "translation.h"

using namespace mu::appshell;
using namespace mu::uicomponents;
using namespace mu::notation;
using namespace mu::workspace;
using namespace mu::actions;

AppMenuModel::AppMenuModel(QObject* parent)
    : QObject(parent)
{
}

void AppMenuModel::load()
{
    m_items.clear();
    m_items << fileItem()
            << editItem()
            << viewItem()
            << addItem()
            << formatItem()
            << toolsItem()
            << helpItem();

    globalContext()->currentMasterNotationChanged().onNotify(this, [this]() {
        load();

        currentMasterNotation()->notation()->notationChanged().onNotify(this, [this]() {
            load();
        });

        currentMasterNotation()->notation()->interaction()->selectionChanged().onNotify(this, [this]() {
            load();
        });
    });

    emit itemsChanged();
}

void AppMenuModel::handleAction(const QString& actionCode)
{
    actionsDispatcher()->dispatch(codeFromQString(actionCode));
}

QVariantList AppMenuModel::items()
{
    QVariantList menuItems;

    for (const MenuItem& menuItem: m_items) {
        menuItems << menuItem.toMap();
    }

    return menuItems;
}

IMasterNotationPtr AppMenuModel::currentMasterNotation() const
{
    return globalContext()->currentMasterNotation();
}

INotationPtr AppMenuModel::currentNotation() const
{
    return currentMasterNotation() ? currentMasterNotation()->notation() : nullptr;
}

MenuItem AppMenuModel::fileItem()
{
    MenuItemList fileItems {
        makeAction("file-new"),
        makeAction("file-open"),
        makeAction("file-import"),
        makeSeparator(),
        makeAction("file-close", scoreOpened()),
        makeAction("file-save", needSaveScore()),
        makeAction("file-save-as", scoreOpened()),
        makeAction("file-save-a-copy", scoreOpened()), // need implement
        makeAction("file-save-selection", scoreOpened()), // need implement
        makeAction("file-save-online", scoreOpened()), // need implement
        makeSeparator(),
        makeAction("file-import-pdf", scoreOpened()), // need implement
        makeAction("file-export", scoreOpened()), // need implement
        makeSeparator(),
        makeAction("edit-info", scoreOpened()),
        makeAction("parts", scoreOpened()),
        makeSeparator(),
        makeAction("print", scoreOpened()), // need implement
        makeSeparator(),
        makeAction("quit")
    };

    return makeMenu(trc("appshell", "&File"), fileItems);
}

MenuItem AppMenuModel::editItem()
{
    MenuItemList editItems {
        makeAction("undo", canUndo()),
        makeAction("redo", canRedo()),
        makeSeparator(),
        makeAction("cut", selectedElementOnScore()),
        makeAction("copy", selectedElementOnScore()),
        makeAction("paste", selectedElementOnScore()),
        makeAction("paste-half", selectedElementOnScore()),
        makeAction("paste-double", selectedElementOnScore()),
        makeAction("paste-special", selectedElementOnScore()),
        makeAction("swap", selectedElementOnScore()),
        makeAction("delete", selectedElementOnScore()),
        makeSeparator(),
        makeAction("select-all", scoreOpened()),
        makeAction("select-similar", selectedElementOnScore()),
        makeAction("find", scoreOpened()),
        makeSeparator(),
        makeAction("preference-dialog") // need implement
    };

    return makeMenu(trc("appshell", "&Edit"), editItems);
}

MenuItem AppMenuModel::viewItem()
{
    MenuItemList viewItems {
        makeAction("toggle-palette", canUndo()), // need implement
        makeAction("masterpalette", canRedo()), // need implement
        makeAction("inspector", selectedElementOnScore()), // need implement
        makeAction("toggle-playpanel", selectedElementOnScore()), // need implement
        makeAction("toggle-navigator", selectedElementOnScore()), // need implement
        makeAction("toggle-timeline", selectedElementOnScore()), // need implement
        makeAction("toggle-mixer", selectedElementOnScore()), // need implement
        makeAction("synth-control", selectedElementOnScore()), // need implement
        makeAction("toggle-selection-window", selectedElementOnScore()), // need implement
        makeAction("toggle-piano", selectedElementOnScore()), // need implement
        makeAction("toggle-scorecmp-tool", scoreOpened()), // need implement
        makeSeparator(),
        makeAction("zoomin", selectedElementOnScore()),
        makeAction("zoomout", scoreOpened()),
        makeSeparator(),
        makeMenu(trc("appshell", "W&orkspaces"), workspacesItems()),
        makeSeparator(),
        makeAction("split-h"), // need implement
        makeAction("split-v"), // need implement
        makeSeparator(),
        makeAction("show-invisible"), // need implement
        makeAction("show-unprintable"), // need implement
        makeAction("show-frames"), // need implement
        makeAction("show-pageborders"), // need implement
        makeAction("mark-irregular"), // need implement
        makeSeparator(),
        makeAction("fullscreen") // need implement
    };

    return makeMenu(trc("appshell", "&View"), viewItems);
}

MenuItem AppMenuModel::addItem()
{
    MenuItemList addItems {
        makeMenu(trc("appshell", "N&otes"), notesItems()),
        makeMenu(trc("appshell", "&Intervals"), intervalsItems()),
        makeMenu(trc("appshell", "T&uplets"), tupletsItems()),
        makeSeparator(),
        makeMenu(trc("appshell", "&Measures"), measuresItems()),
        makeMenu(trc("appshell", "&Frames"), framesItems()),
        makeMenu(trc("appshell", "&Text"), textItems()),
        makeMenu(trc("appshell", "&Lines"), linesItems()),
    };

    return makeMenu(trc("appshell", "&Add"), addItems, scoreOpened());
}

MenuItem AppMenuModel::formatItem()
{
    MenuItemList stretchItems {
        makeAction("stretch+"), // need implement
        makeAction("stretch-"), // need implement
        makeAction("reset-stretch") // need implement
    };

    MenuItemList formatItems {
        makeAction("edit-style"),
        makeAction("page-settings"), // need implement
        makeSeparator(),
        makeMenu(trc("appshell", "&Stretch"), stretchItems),
        makeSeparator(),
        makeAction("reset-text-style-overrides"), // need implement
        makeAction("reset-beammode"), // need implement
        makeAction("reset"), // need implement
        makeSeparator(),
        makeAction("load-style"), // need implement
        makeAction("save-style") // need implement
    };

    return makeMenu(trc("appshell", "F&ormat"), formatItems, scoreOpened());
}

MenuItem AppMenuModel::toolsItem()
{
    MenuItemList voicesItems {
        makeAction("voice-x12"),
        makeAction("voice-x13"),
        makeAction("voice-x14"),
        makeAction("voice-x23"),
        makeAction("voice-x24"),
        makeAction("voice-x34")
    };

    MenuItemList measuresItems {
        makeAction("split-measure"),
        makeAction("join-measures"),
    };

    MenuItemList toolsItems {
        makeAction("transpose"),
        makeSeparator(),
        makeAction("explode"), // need implement
        makeAction("implode"), // need implement
        makeAction("realize-chord-symbols"), // need implement
        makeMenu(trc("appshell", "&Voices"), voicesItems),
        makeMenu(trc("appshell", "&Measures"), measuresItems),
        makeAction("time-delete"), // need implement
        makeSeparator(),
        makeAction("slash-fill"), // need implement
        makeAction("slash-rhythm"), // need implement
        makeSeparator(),
        makeAction("pitch-spell"), // need implement
        makeAction("reset-groupings"), // need implement
        makeAction("resequence-rehearsal-marks"), // need implement
        makeAction("unroll-repeats"), // need implement
        makeSeparator(),
        makeAction("copy-lyrics-to-clipboard"), // need implement
        makeAction("fotomode"), // need implement
        makeAction("del-empty-measures"), // need implement
    };

    return makeMenu(trc("appshell", "&Tools"), toolsItems, scoreOpened());
}

MenuItem AppMenuModel::helpItem()
{
    MenuItemList toursItems {
        makeAction("show-tours"), // need implement
        makeAction("reset-tours") // need implement
    };

    MenuItemList helpItems {
        makeAction("online-handbook"), // need implement
        makeMenu(trc("appshell", "&Tours"), toursItems),
        makeSeparator(),
        makeAction("about"), // need implement
        makeAction("about-qt"), // need implement
        makeAction("about-musicxml"), // need implement
    };

    if (configuration()->isAppUpdatable()) {
        helpItems << makeAction("check-update"); // need implement
    }

    helpItems << makeSeparator()
              << makeAction("ask-help") // need implement
              << makeAction("report-bug") // need implement
              << makeAction("leave-feedback") // need implement
              << makeSeparator()
              << makeAction("revert-factory"); // need implement

    return makeMenu(trc("appshell", "&Help"), helpItems, scoreOpened());
}

MenuItemList AppMenuModel::notesItems() const
{
    MenuItemList items {
        makeAction("note-input", true, isNoteInputMode()),
        makeSeparator(),
        makeAction("note-c", selectedElementOnScore()),
        makeAction("note-d", selectedElementOnScore()),
        makeAction("note-e", selectedElementOnScore()),
        makeAction("note-f", selectedElementOnScore()),
        makeAction("note-g", selectedElementOnScore()),
        makeAction("note-a", selectedElementOnScore()),
        makeAction("note-b", selectedElementOnScore()),
        makeSeparator(),
        makeAction("chord-c", selectedElementOnScore()),
        makeAction("chord-d", selectedElementOnScore()),
        makeAction("chord-e", selectedElementOnScore()),
        makeAction("chord-f", selectedElementOnScore()),
        makeAction("chord-g", selectedElementOnScore()),
        makeAction("chord-a", selectedElementOnScore()),
        makeAction("chord-b", selectedElementOnScore())
    };

    return items;
}

MenuItemList AppMenuModel::intervalsItems() const
{
    MenuItemList items {
        makeAction("interval1", true, selectedElementOnScore()),
        makeAction("interval2", true, selectedElementOnScore()),
        makeAction("interval3", true, selectedElementOnScore()),
        makeAction("interval4", true, selectedElementOnScore()),
        makeAction("interval5", true, selectedElementOnScore()),
        makeAction("interval6", true, selectedElementOnScore()),
        makeAction("interval7", true, selectedElementOnScore()),
        makeAction("interval8", true, selectedElementOnScore()),
        makeAction("interval9", true, selectedElementOnScore()),
        makeSeparator(),
        makeAction("interval-2", true, selectedElementOnScore()),
        makeAction("interval-3", true, selectedElementOnScore()),
        makeAction("interval-4", true, selectedElementOnScore()),
        makeAction("interval-5", true, selectedElementOnScore()),
        makeAction("interval-6", true, selectedElementOnScore()),
        makeAction("interval-7", true, selectedElementOnScore()),
        makeAction("interval-8", true, selectedElementOnScore()),
        makeAction("interval-9", true, selectedElementOnScore())
    };

    return items;
}

MenuItemList AppMenuModel::tupletsItems() const
{
    MenuItemList items {
        makeAction("duplet", true, selectedElementOnScore()),
        makeAction("triplet", true, selectedElementOnScore()),
        makeAction("quadruplet", true, selectedElementOnScore()),
        makeAction("quintuplet", true, selectedElementOnScore()),
        makeAction("sextuplet", true, selectedElementOnScore()),
        makeAction("septuplet", true, selectedElementOnScore()),
        makeAction("octuplet", true, selectedElementOnScore()),
        makeAction("nonuplet", true, selectedElementOnScore()),
        makeAction("tuplet-dialog", true, selectedElementOnScore())
    };

    return items;
}

MenuItemList AppMenuModel::measuresItems() const
{
    MenuItemList items {
        makeAction("insert-measure", true, selectedElementOnScore()),
        makeAction("insert-measures", true, selectedElementOnScore()),
        makeSeparator(),
        makeAction("append-measure", true, selectedElementOnScore()),
        makeAction("append-measures", true, selectedElementOnScore())
    };

    return items;
}

MenuItemList AppMenuModel::framesItems() const
{
    MenuItemList items {
        makeAction("insert-hbox", true, selectedElementOnScore()),
        makeAction("insert-vbox", true, selectedElementOnScore()),
        makeAction("insert-textframe", true, selectedElementOnScore()),
        makeSeparator(),
        makeAction("append-hbox", true, selectedElementOnScore()),
        makeAction("append-vbox", true, selectedElementOnScore()),
        makeAction("append-textframe", true, selectedElementOnScore())
    };

    return items;
}

MenuItemList AppMenuModel::textItems() const
{
    MenuItemList items {
        makeAction("title-text", true, selectedElementOnScore()),
        makeAction("subtitle-text", true, selectedElementOnScore()),
        makeAction("composer-text", true, selectedElementOnScore()),
        makeAction("poet-text", true, selectedElementOnScore()),
        makeAction("part-text", true, selectedElementOnScore()),
        makeSeparator(),
        makeAction("system-text", true, selectedElementOnScore()),
        makeAction("staff-text", true, selectedElementOnScore()),
        makeAction("expression-text", true, selectedElementOnScore()),
        makeAction("rehearsalmark-text", true, selectedElementOnScore()),
        makeAction("instrument-change-text", true, selectedElementOnScore()),
        makeAction("fingering-text", true, selectedElementOnScore()),
        makeSeparator(),
        makeAction("sticking-text", true, selectedElementOnScore()),
        makeAction("chord-text", true, selectedElementOnScore()),
        makeAction("roman-numeral-text", true, selectedElementOnScore()),
        makeAction("nashville-number-text", true, selectedElementOnScore()),
        makeAction("lyrics", true, selectedElementOnScore()),
        makeAction("figured-bass", true, selectedElementOnScore()),
        makeAction("tempo", true, selectedElementOnScore())
    };

    return items;
}

MenuItemList AppMenuModel::linesItems() const
{
    MenuItemList items {
        makeAction("add-slur", true, selectedElementOnScore()),
        makeAction("add-hairpin", true, selectedElementOnScore()),
        makeAction("add-hairpin-reverse", true, selectedElementOnScore()),
        makeAction("add-8va", true, selectedElementOnScore()),
        makeAction("add-8vb", true, selectedElementOnScore()),
        makeAction("add-noteline", true, selectedElementOnScore()),
    };

    return items;
}

MenuItemList AppMenuModel::workspacesItems() const
{
    MenuItemList items;

    RetVal<IWorkspacePtrList> workspaces = workspacesManager()->workspaces();
    if (!workspaces.ret) {
        return items;
    }

    IWorkspacePtr currentWorkspace = workspacesManager()->currentWorkspace().val;

    std::sort(workspaces.val.begin(), workspaces.val.end(), [](const IWorkspacePtr& workspace1, const IWorkspacePtr& workspace2) {
        return workspace1->name() < workspace2->name();
    });

    for (const IWorkspacePtr& workspace : workspaces.val) {
        MenuItem item = actionsRegister()->action("select-workspace"); // need implement
        item.title = workspace->title();
        item.data = ActionData::make_arg1<std::string>(workspace->name());

        bool isCurrentWorkspace = workspace == currentWorkspace;
        items << makeAction(item.code, true, isCurrentWorkspace);
    }

    items << makeSeparator()
          << makeAction("new-workspace") // need implement
          << makeAction("edit-workspace") // need implement
          << makeAction("delete-workspace") // need implement
          << makeAction("reset-workspace"); // need implement

    return items;
}

MenuItem AppMenuModel::makeMenu(const std::string& title, const MenuItemList& actions, bool enabled)
{
    MenuItem item;
    item.title = title;
    item.subitems = actions;
    item.enabled = enabled;
    return item;
}

MenuItem AppMenuModel::makeAction(const ActionCode& actionCode, bool enabled, bool checked, const std::string& section) const
{
    ActionItem action = actionsRegister()->action(actionCode);
    if (!action.isValid()) {
        return MenuItem();
    }

    MenuItem item = action;
    item.enabled = enabled;
    item.checked = checked;
    item.section = section;

    shortcuts::Shortcut shortcut = shortcutsRegister()->shortcut(action.code);
    if (shortcut.isValid()) {
        item.shortcut = shortcut.sequence;
    }

    return item;
}

MenuItem AppMenuModel::makeSeparator() const
{
    MenuItem item;
    item.title = std::string();
    return item;
}

bool AppMenuModel::needSaveScore() const
{
    return currentMasterNotation() && currentMasterNotation()->needSave().val;
}

bool AppMenuModel::scoreOpened() const
{
    return currentNotation() != nullptr;
}

bool AppMenuModel::canUndo() const
{
    return currentNotation() ? currentNotation()->undoStack()->canUndo() : false;
}

bool AppMenuModel::canRedo() const
{
    return currentNotation() ? currentNotation()->undoStack()->canRedo() : false;
}

bool AppMenuModel::selectedElementOnScore() const
{
    return currentNotation() ? !currentNotation()->interaction()->selection()->isNone() : false;
}

bool AppMenuModel::isNoteInputMode() const
{
    return currentNotation() ? currentNotation()->interaction()->noteInput()->isNoteInputMode() : false;
}
