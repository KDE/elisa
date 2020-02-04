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
    property int databaseId
    property alias delegate: contentDirectoryView.delegate
    property bool showSection: false
    property alias contentModel: contentDirectoryView.model
    property alias expandedFilterView: navigationBar.expandedFilterView
    property alias showRating: navigationBar.showRating
    property alias allowArtistNavigation: navigationBar.allowArtistNavigation
    property var delegateWidth: scrollBar.visible ? contentDirectoryView.width - scrollBar.width : contentDirectoryView.width
    property alias currentIndex: contentDirectoryView.currentIndex
    property alias enableSorting: navigationBar.enableSorting
    property var stackView
    property alias showEnqueueButton: navigationBar.showEnqueueButton
    property alias showCreateRadioButton: navigationBar.showCreateRadioButton
    property alias navigationBar: navigationBar

    signal goBack()
    signal showArtist(var name)

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

            onReplaceAndPlay: contentModel.replaceAndPlayOfPlayList()

            onGoBack: listView.goBack()

            onShowArtist: listView.showArtist(listView.contentModel.sourceModel.author)

            onSort: contentModel.sortModel(order)
        }

        Rectangle {
            color: myPalette.base

            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.margins: 2

            ListView {
                id: contentDirectoryView
                anchors.fill: parent

                Accessible.role: Accessible.List
                Accessible.name: mainTitle
                Accessible.description: mainTitle

                activeFocusOnTab: true
                keyNavigationEnabled: true

                currentIndex: -1

                section.property: (showSection ? 'discNumber' : '')
                section.criteria: ViewSection.FullString
                section.labelPositioning: ViewSection.InlineLabels
                section.delegate: TracksDiscHeader {
                    discNumber: section
                    width: scrollBar.visible ? (!LayoutMirroring.enabled ? contentDirectoryView.width - scrollBar.width : contentDirectoryView.width) : contentDirectoryView.width
                }

                ScrollBar.vertical: ScrollBar {
                    id: scrollBar
                }
                boundsBehavior: Flickable.StopAtBounds
                clip: true

                ScrollHelper {
                    id: scrollHelper
                    flickable: contentDirectoryView
                    anchors.fill: contentDirectoryView
                }

                onCountChanged: if (count === 0) {
                                    currentIndex = -1;
                                }
            }
        }
    }
}

