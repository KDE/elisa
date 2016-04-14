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
import org.kde.kirigami 1.0
import "private"

/**
 * Page is a container for all the app pages: everything pushed to the
 * ApplicationWindow stackView should be a Page instabnce (or a subclass,
 * such as ScrollablePage)
 * @see ScrollablePage
 */
Item {
    id: root

    /**
     * title: string
     * Title for the page
     */
    property string title

    /**
     * leftPadding: int
     * default contents padding at left
     */
    property int leftPadding: Units.gridUnit

    /**
     * topPadding: int
     * default contents padding at top
     */
    property int topPadding: Units.gridUnit + (applicationWindow() && applicationWindow().header ? applicationWindow().header.preferredHeight : 0)

    /**
     * rightPadding: int
     * default contents padding at right
     */
    property int rightPadding: Units.gridUnit

    /**
     * bottomPadding: int
     * default contents padding at bottom
     */
    property int bottomPadding: Units.gridUnit

    /**
     * contentData: Item
     * The main items contained in this Page
     */
    default property alias contentData: container.data

    /**
     * flickable: Flickable
     * if the central element of the page is a Flickable
     * (ListView and Gridview as well) you can set it there.
     * normally, you wouldn't need to do that, but just use the
     * ScrollablePage element instead
     * @see ScrollablePage
     * Use this if your flickable has some non standard properties, such as not covering the whole Page
     */
    property Flickable flickable

    /**
     * contextualActions: list<QtObject>
     * Defines the contextual actions for the page:
     * an easy way to assign actions in the right sliding panel
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
     */
    property list<QtObject> contextualActions

    /**
     * mainAction: Action
     * An optional single action for the action button.
     * it can be a Kirigami.Action or a QAction
     *
     * Example usage:
     *
     * @code
     * import org.kde.kirigami 1.0 as Kirigami
     * Kirigami.Page {
     *     mainAction: Kirigami.Action {
     *         iconName: "edit"
     *         onTriggered: {
     *             // do stuff
     *         }
     *     }
     * }
     * @endcode
     */
    property QtObject mainAction

    /**
     * background: Item
     * This property holds the background item.
     * Note: If the background item has no explicit size specified,
     * it automatically follows the control's size.
     * In most cases, there is no need to specify width or
     * height for a background item.
     */
    property Item background

    //HACK: trigger a signal that will convince the ApplicationHeader to reload titles
    //FIXME: when the 5.6 branch is merged, do it properly as PageRow will have a proper model
    onTitleChanged: applicationWindow().pageStack.contentChildrenChanged();

    onBackgroundChanged: {
        background.z = -1;
        background.parent = root;
        background.anchors.fill = root;
    }

    /**
     * emitted When the application requests a Back action
     * For instance a global "back" shortcut or the Android
     * Back button has been pressed.
     * The page can manage the back event by itself,
     * and if it set event.accepted = true, it will stop the main
     * application to manage the back event.
     */
    signal backRequested(var event);

    /**
     * actionButton: ActionButton
     * Readonly.
     * ActionButton can't be instantiated directly by the user.
     * This is the Action button shown in the middle bottom of the page.
     * It will open the side drawers by dragging it around.
     * Also, it is possible to assign an Action to it, dependent from the page.
     *
     * Example usage:
     *
     * @code
     * import org.kde.kirigami 1.0 as Kirigami
     * Kirigami.Page {
     *     mainAction: Kirigami.Action {
     *         iconName: "edit"
     *         onTriggered: {
     *             // do stuff
     *         }
     *     }
     * }
     * @endcode
     *
     * When that page will be the current one in the app, the action button
     * will have the icon of the page's mainAction and when clicked it will
     * trigger it.
     */
    property alias actionButton: __actionButton
    ActionButton {
        id: __actionButton
        z: 9999
        action: root.mainAction
        anchors.bottom: parent.bottom
        x: parent.width/2 - width/2
    }

    Item {
        id: container
        anchors {
            fill: parent
            leftMargin: leftPadding
            topMargin: topPadding
            rightMargin: rightPadding
            bottomMargin: bottomPadding
        }
    }

    Layout.fillWidth: true
}
