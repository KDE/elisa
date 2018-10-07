/*
 * Copyright 2016-2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * Copyright 2018 Alexander Stippich <a.stippich@gmx.net>
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
import QtQml.Models 2.2
import QtQuick.Layouts 1.2

import org.kde.elisa 1.0

FocusScope {
    id: fileView

    property bool isSubPage: false
    property alias contentModel: contentDirectoryView.model
    property alias expandedFilterView: navigationBar.expandedFilterView

    function goBack() {
        contentModel.openParentFolder()
    }

    function loadFolderAndClear(data) {
        contentModel.openFolder(data)
        navigationBar.filterText = ""
    }

    SystemPalette {
        id: myPalette
        colorGroup: SystemPalette.Active
    }

    Theme {
        id: elisaTheme
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: false
        acceptedButtons: Qt.BackButton
        onClicked: contentModel.openParentFolder()
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        NavigationActionBar {
            id: navigationBar

            mainTitle: i18nc("Title of the file browser view", "Files")
            image: elisaTheme.folderIcon
            secondaryTitle: contentModel.url
            enableGoBack: contentModel.canGoBack
            sortOrder: contentModel.sortedAscending
            showRating: false

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

            onEnqueue: contentModel.enqueueToPlayList()
            onReplaceAndPlay: contentModel.replaceAndPlayOfPlayList()
            onGoBack: contentModel.openParentFolder()
            onSort: contentModel.sortModel(order)
        }

        Rectangle {
            color: myPalette.base

            Layout.fillHeight: true
            Layout.fillWidth: true
            clip: true

            GridView {
                id: contentDirectoryView
                anchors.topMargin: 20
                anchors.fill: parent

                focus: true

                ScrollBar.vertical: ScrollBar {
                    id: scrollBar
                }
                boundsBehavior: Flickable.StopAtBounds

                ScrollHelper {
                    id: scrollHelper
                    flickable: contentDirectoryView
                    anchors.fill: contentDirectoryView
                }

                add:  Transition {
                    PropertyAnimation {
                        property: "opacity"
                        from: 0
                        to: 1
                        duration: 100
                    }
                }

                remove:  Transition {
                    PropertyAnimation {
                        property: "opacity"
                        from: 0
                        to: 1
                        duration: 100
                    }
                }

                cellWidth: elisaTheme.gridDelegateWidth
                cellHeight:elisaTheme.gridDelegateHeight

                delegate: FileBrowserDelegate {
                    width: contentDirectoryView.cellWidth
                    height: contentDirectoryView.cellHeight
                    focus: true

                    isDirectory: model.directory
                    isPlayList: model.isPlaylist
                    fileName: model.name
                    fileUrl: model.containerData
                    imageUrl: model.imageUrl
                    contentModel: fileView.contentModel

                    onEnqueue: elisa.mediaPlayList.enqueue(data)
                    onReplaceAndPlay: contentModel.replaceAndPlayOfUrl(data)
                    onSelected: {
                        forceActiveFocus()
                        contentDirectoryView.currentIndex = model.index
                    }
                    onOpen: loadFolderAndClear(data)
                }
            }
        }
    }
}
