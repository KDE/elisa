/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtQml.Models 2.2
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0

import org.kde.elisa 1.0

FocusScope {
    id: listView

    property bool isSubPage: false
    property alias mainTitle: navigationBar.mainTitle
    property alias secondaryTitle: navigationBar.secondaryTitle
    property alias image: navigationBar.image
    property alias delegate: contentDirectoryView.delegate
    property alias contentModel: contentDirectoryView.model
    property alias expandedFilterView: navigationBar.expandedFilterView
    property alias showRating: navigationBar.showRating
    property alias allowArtistNavigation: navigationBar.allowArtistNavigation
    property var delegateWidth: scrollBar.visible ? contentDirectoryView.width - scrollBar.width : contentDirectoryView.width
    property alias currentIndex: contentDirectoryView.currentIndex
    property var stackView

    signal goBack()
    signal showArtist(var name)
    signal filterViewChanged(bool expandedFilterView)

    SystemPalette {
        id: myPalette
        colorGroup: SystemPalette.Active
    }

    Theme {
        id: elisaTheme
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        NavigationActionBar {
            id: navigationBar

            enableGoBack: listView.isSubPage
            sortOrder: contentModel.sortedAscending

            height: elisaTheme.navigationBarHeight
            Layout.preferredHeight: height
            Layout.minimumHeight: height
            Layout.maximumHeight: height
            Layout.fillWidth: true

            Binding {
                target: contentModel
                property: 'filterText'
                value: navigationBar.filterText
            }

            Binding {
                target: contentModel
                property: 'filterRating'
                value: navigationBar.filterRating
            }

            onEnqueue: contentModel.enqueueToPlayList()

            onFilterViewChanged: listView.filterViewChanged(expandedFilterView)

            onReplaceAndPlay: contentModel.replaceAndPlayOfPlayList()

            onGoBack: listView.goBack()

            onShowArtist: listView.showArtist(listView.contentModel.sourceModel.author)

            onSort: contentModel.sortModel(order)
        }

        Rectangle {
            color: myPalette.base

            Layout.fillHeight: true
            Layout.fillWidth: true

            ListView {
                id: contentDirectoryView
                anchors.topMargin: 20
                anchors.fill: parent

                focus: true

                ScrollBar.vertical: ScrollBar {
                    id: scrollBar
                }
                boundsBehavior: Flickable.StopAtBounds
                clip: true
            }
        }
    }
}

