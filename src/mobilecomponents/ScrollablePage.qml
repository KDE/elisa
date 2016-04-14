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
 * ScrollablePage is a container for all the app pages: everything pushed to the
 * ApplicationWindow stackView should be a Page or ScrollablePage instabnce.
 * This Page subclass is for content that has to be scrolled around, such as
 * bigger content than the screen that would normally go in a Flickable
 * or a ListView.
 * Scrolling and scrolling indicators will be automatically managed
 *
 *
 * @code
 * ScrollablePage {
 *     id: root
 *     //The rectangle will automatically bescrollable
 *     Rectangle {
 *         width: root.width
 *         height: 99999
 *     }
 * }
 * @endcode
 *
 * @code
 * ScrollablePage {
 *     id: root
 *
 *     //support for the popular "pull down to refresh" behavior in mobile apps
 *     supportsRefreshing: true
 *
 *     //The ListView will automatically receive proper scroll indicators
 *     ListView {
 *         model: myModel
 *         delegate: BasicListItem { ... }
 *     }
 * }
 * @endcode
 *
 * @inherit Page
 */
Page {
    id: root

    /**
     * refreshing: bool
     * If true the list is asking for refresh and will show a loading spinner.
     * it will automatically be set to true when the user pulls down enough the list.
     * This signals the application logic to start its refresh procedure.
     * The application itself will have to set back this property to false when done.
     */
    property alias refreshing: scrollView.refreshing

    /**
     * supportsRefreshing: bool
     * If true the list supports the "pull down to refresh" behavior.
     * default is false.
     */
    property alias supportsRefreshing: scrollView.supportsRefreshing

    /**
     * flickable: Flickable
     * The main Flickable item of this page
     */
    property alias flickable: scrollView.flickableItem

    /**
     * The main content Item of this page.
     * In the case of a ListView or GridView, both contentItem and flickable
     * will be a pointer to the ListView (or GridView)
     */
    default property alias contentItem: scrollView.contentItem

    /**
     * leftPadding: int
     * default contents padding at left
     */
    property alias leftPadding: scrollView.leftPadding

    /**
     * topPadding: int
     * default contents padding at top
     */
    property alias topPadding: scrollView.topPadding

    /**
     * rightPadding: int
     * default contents padding at right
     */
    property alias rightPadding: scrollView.rightPadding

    /**
     * bottomPadding: int
     * default contents padding at bottom
     */
    property alias bottomPadding: scrollView.bottomPadding

    children: [
        RefreshableScrollView {
            id: scrollView
            topPadding: (applicationWindow() && applicationWindow().header ? applicationWindow().header.preferredHeight : 0) + (contentItem == flickable ? 0 : Units.gridUnit)
            leftPadding: contentItem == flickable ? 0 : Units.gridUnit
            rightPadding: contentItem == flickable ? 0 : Units.gridUnit
            bottomPadding: contentItem == flickable ? 0 : Units.gridUnit
            anchors {
                fill: parent
            }
        },

        Item {
            id: overlay
            anchors.fill: parent
            property Item oldContentItem
        }
    ]

    //HACK to get the contentItem as the last one, all the other eventual items as an overlay
    //no idea if is the way the user expects
    onContentItemChanged: {
         if (overlay.oldContentItem) {
             overlay.oldContentItem.parent = overlay
         }
         overlay.oldContentItem = root.contentItem
    }
  /*  Component.onCompleted: {
        if (0&&contentItem == flickable) {
            leftPadding = 0;
            topPadding = 0;
            rightPadding = 0;
            bottomPadding = 0;

        } else {
            leftPadding = Units.gridUnit;
            topPadding = Units.gridUnit;
            rightPadding = Units.gridUnit;
            bottomPadding = Units.gridUnit;
        }
    }*/
    
}
