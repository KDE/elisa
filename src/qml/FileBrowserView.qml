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

    property var viewType
    property bool isSubPage: false
    property alias expandedFilterView: navigationBar.expandedFilterView

    function goBack() {
        proxyModel.openParentFolder()
    }

    function loadFolderAndClear(data) {
        proxyModel.openFolder(data)
        navigationBar.filterText = ""
    }

    FileBrowserModel {
        id: realModel
    }

    FileBrowserProxyModel {
        id: proxyModel

        sourceModel: realModel

        onFilesToEnqueue: elisa.mediaPlayList.enqueue(newFiles, databaseIdType, enqueueMode, triggerPlay)
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: false
        acceptedButtons: Qt.BackButton
        onClicked: proxyModel.openParentFolder()
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        NavigationActionBar {
            id: navigationBar

            mainTitle: i18nc("Title of the file browser view", "Files")
            image: elisaTheme.folderIcon
            secondaryTitle: proxyModel.url
            enableGoBack: proxyModel.canGoBack
            sortOrder: proxyModel.sortedAscending
            showRating: false

            Layout.fillWidth: true

            Binding {
                target: proxyModel
                property: 'filterText'
                value: navigationBar.filterText
            }

            onEnqueue: proxyModel.enqueueToPlayList()
            onReplaceAndPlay: proxyModel.replaceAndPlayOfPlayList()
            onGoBack: proxyModel.openParentFolder()
            onSort: proxyModel.sortModel(order)
        }

        Rectangle {
            color: myPalette.base

            Layout.fillHeight: true
            Layout.fillWidth: true
            clip: true

            GridView {
                id: contentDirectoryView
                anchors.fill: parent

                activeFocusOnTab: true
                keyNavigationEnabled: true

                ScrollBar.vertical: ScrollBar {
                    id: scrollBar
                }
                boundsBehavior: Flickable.StopAtBounds

                currentIndex: -1

                Accessible.role: Accessible.List
                Accessible.name: proxyModel.url

                model: proxyModel

                TextMetrics {
                    id: secondaryLabelSize
                    text: 'example'
                }

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

                cellWidth: Math.floor(availableWidth / Math.max(Math.floor(availableWidth / elisaTheme.gridDelegateWidth), 2))
                cellHeight: elisaTheme.gridDelegateHeight +
                            (secondaryLabelSize.boundingRect.height - secondaryLabelSize.boundingRect.y)

                delegate: GridBrowserDelegate {
                    width: elisaTheme.gridDelegateWidth
                    height: contentDirectoryView.cellHeight

                    focus: true

                    isSelected: contentDirectoryView.currentIndex === index

                    mainText: model.name
                    delegateDisplaySecondaryText: false
                    fileUrl: model.fileUrl
                    imageUrl: model.imageUrl
                    showDetailsButton: !model.isDirectory && !model.isPlaylist
                    showEnqueueButton: !model.isDirectory && !model.isPlaylist
                    showPlayButton: !model.isDirectory
                    showOpenButton: model.isDirectory && !model.isPlaylist

                    onEnqueue: elisa.mediaPlayList.enqueue(0, model.fileUrl, ElisaUtils.FileName, ElisaUtils.AppendPlayList, ElisaUtils.DoNotTriggerPlay)
                    onReplaceAndPlay: {
                        if (model.isPlaylist) {
                            elisa.mediaPlayList.loadPlaylist(model.fileUrl)
                        } else {
                            elisa.mediaPlayList.enqueue(0, model.fileUrl, ElisaUtils.FileName, ElisaUtils.ReplacePlayList, ElisaUtils.TriggerPlay)
                        }
                    }
                    onSelected: {
                        forceActiveFocus()
                        contentDirectoryView.currentIndex = model.index
                    }

                    onActiveFocusChanged: {
                        if (activeFocus && contentDirectoryView.currentIndex !== model.index) {
                            contentDirectoryView.currentIndex = model.index
                        }
                    }

                    onOpen: isDirectory ? loadFolderAndClear(model.fileUrl) : elisa.mediaPlayList.enqueue(0, model.fileUrl, ElisaUtils.FileName, ElisaUtils.AppendPlayList, ElisaUtils.DoNotTriggerPlay)
                }
            }
        }
    }
}
