/*
 *   Copyright 2015 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.1
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.0 as QtControls
import org.kde.kirigami 1.0

/**
 * A drawer specialization that will show a list of actions that are
 * specific of the current page shown by the application
 *
 * Example usage:
 * @code
 * import org.kde.kirigami 1.0 as Kirigami
 *
 * Kirigami.ApplicationWindow {
 *  [...]
 *     contextDrawer: Kirigami.ContextDrawer {
 *         id: contextDrawer
 *     }
 *  [...]
 * }
 * @endcode
 *
 * @code
 * import org.kde.kirigami 1.0 as Kirigami
 *
 * Kirigami.Page {
 *   [...]
 *     contextualActions: [
 *         Kirigami.Action {
 *             iconName: "edit"
 *             text: "Action text"
 *             onTriggered: {
 *                 // do stuff
 *             }
 *         },
 *         Kirigami.Action {
 *             iconName: "edit"
 *             text: "Action text"
 *             onTriggered: {
 *                 // do stuff
 *             }
 *         }
 *     ]
 *   [...]
 * }
 * @endcode
 *
 * @inherit AbstractDrawer
 */
OverlayDrawer {
    id: root

    /**
     * title: string
     * A title for the action list that will be shown to the user when opens the drawer
     */
    property string title: typeof i18n !== "undefined" ? i18n("Actions") : "Actions"

    /**
     * actions: list<Action>
     * This can be any type of object that a ListView can accept as model. 
     * It expects items compatible with either QAction or Kirigami Action
     */
    property var actions: pageStack.currentItem ? pageStack.currentItem.contextualActions : null
    enabled: menu.count > 0
    edge: Qt.RightEdge

    handleVisible: applicationWindow() ? applicationWindow().controlsVisible : true

    Connections {
        target: pageStack
        onCurrentItemChanged: {
            actions = pageStack.currentItem.contextualActions
        }
    }

    contentItem: QtControls.ScrollView {
        implicitWidth: Units.gridUnit * 20
        ListView {
            id: menu
            interactive: contentHeight > height
            model: {
                if (typeof root.actions == "undefined") {
                    return null;
                }
                if (root.actions.length == 0) {
                    return null;
                } else {
                    return root.actions[0].text !== undefined &&
                        root.actions[0].trigger !== undefined ?
                            root.actions :
                            root.actions[0];
                }
            }
            topMargin: menu.height - menu.contentHeight
            header: Item {
                height: heading.height
                width: menu.width
                Heading {
                    id: heading
                    anchors {
                        left: parent.left
                        right: parent.right
                        margins: Units.largeSpacing
                    }
                    elide: Text.ElideRight
                    level: 2
                    text: root.title
                }
            }
            delegate: BasicListItem {
                checked: modelData.checked
                icon: modelData.iconName
                supportsMouseEvents: true
                label: model ? model.text : modelData.text
                enabled: model ? model.enabled : modelData.enabled
                visible: model ? model.visible : modelData.visible
                opacity: enabled ? 1.0 : 0.6
                onClicked: {
                    if (modelData && modelData.trigger !== undefined) {
                        modelData.trigger();
                    // assume the model is a list of QAction or Action
                    } else if (menu.model.length > index) {
                        menu.model[index].trigger();
                    } else {
                        console.warning("Don't know how to trigger the action")
                    }
                    root.opened = false;
                }
            }
        }
    }
}
