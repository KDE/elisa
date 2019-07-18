/*
 * Copyright 2016-2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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
import QtQuick.Layouts 1.2
import QtQuick.Window 2.2
import org.kde.elisa 1.0

RowLayout {
    id: contentViewContainer
    spacing: 0

    property bool showPlaylist
    property alias currentViewIndex: listViews.currentIndex

    signal toggleSearch()

    function goBack() {
        viewManager.goBack()
    }

    function openArtist(name) {
        viewManager.openChildView(name, '', elisaTheme.artistIcon, 0, ElisaUtils.Artist, ViewManager.NoDiscHeaders)
    }

    function openAlbum(album, artist, image, albumID, showDiscHeader) {
        image = !image ? elisaTheme.defaultAlbumImage : image;
        viewManager.openChildView(album, artist, image, albumID, ElisaUtils.Album, showDiscHeader);
    }

    function openNowPlaying() {
        viewManager.closeAllViews();
    }

    ViewManager {
        id: viewManager

        onSwitchOffAllViews: {
            listViews.setCurrentIndex(pageModel.indexFromViewType(viewType))

            while(browseStackView.depth > 1) {
                browseStackView.pop()
            }
        }

        onOpenGridView: {
            if (expectedDepth === 1) {
                listViews.setCurrentIndex(pageModel.indexFromViewType(viewType))
            }

            while(browseStackView.depth > expectedDepth) {
                browseStackView.pop()
            }

            browseStackView.push(dataGridView, {
                                     viewType: viewType,
                                     filterType: filterType,
                                     mainTitle: pageModel.viewMainTitle(viewType, mainTitle),
                                     secondaryTitle: secondaryTitle,
                                     image: pageModel.viewImageUrl(viewType, imageUrl),
                                     modelType: dataType,
                                     defaultIcon: viewDefaultIcon,
                                     showRating: viewShowRating,
                                     delegateDisplaySecondaryText: viewDelegateDisplaySecondaryText,
                                     genreFilterText: genreNameFilter,
                                     artistFilter: artistNameFilter,
                                     isSubPage: (browseStackView.depth >= 2),
                                     stackView: browseStackView,
                                     opacity: 0,
                                 })
        }

        onOpenListView: {
            listViews.setCurrentIndex(pageModel.indexFromViewType(viewType))

            while(browseStackView.depth > expectedDepth) {
                browseStackView.pop()
            }

            browseStackView.push(dataListView, {
                                     viewType: viewType,
                                     filterType: filterType,
                                     isSubPage: expectedDepth > 1,
                                     mainTitle: mainTitle,
                                     secondaryTitle: secondaryTitle,
                                     databaseId: databaseId,
                                     image: imageUrl,
                                     modelType: dataType,
                                     sortRole: sortRole,
                                     sortAscending: sortOrder,
                                     stackView: browseStackView,
                                     displaySingleAlbum: displaySingleAlbum,
                                     showSection: showDiscHeaders,
                                     opacity: 0,
                                 })
        }

        onSwitchFilesBrowserView: {
            listViews.setCurrentIndex(pageModel.indexFromViewType(viewType))

            while(browseStackView.depth > expectedDepth) {
                browseStackView.pop()
            }

            browseStackView.push(filesBrowserView, {
                                     viewType: viewType,
                                     mainTitle: mainTitle,
                                     image: imageUrl,
                                     opacity: 0,
                                 })
        }

        onSwitchContextView: {
             listViews.setCurrentIndex(pageModel.indexFromViewType(viewType))

            while(browseStackView.depth > expectedDepth) {
                browseStackView.pop()
            }

            browseStackView.push(albumContext, {
                                     viewType: viewType,
                                     mainTitle: mainTitle,
                                     image: imageUrl,
                                     opacity: 0,
                                 })
        }


        onPopOneView: {
            if (browseStackView.depth > 2) {
                browseStackView.pop() }
        }
    }

    ViewsModel {
        id: pageModel
    }

    ViewSelector {
        id: listViews

        model: pageModel

        Layout.fillHeight: true

        Behavior on Layout.maximumWidth {
            NumberAnimation {
                duration: 150
            }
        }

        onSwitchView: viewManager.openParentView(viewType, pageModel.viewMainTitle(viewType, ""), pageModel.viewImageUrl(viewType, ""))
    }

    Rectangle {
        id: viewSelectorSeparatorItem

        Layout.fillHeight: true
        width: 1

        color: myPalette.mid
    }

    ColumnLayout {
        Layout.fillHeight: true
        Layout.fillWidth: true

        spacing: 0

        TopNotification {
            id: invalidBalooConfiguration

            Layout.fillWidth: true

            musicManager: elisa.musicManager

            focus: true
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true

            RowLayout {
                anchors.fill: parent

                spacing: 0

                id: contentZone

                FocusScope {
                    id: mainContentView

                    focus: true

                    Layout.fillHeight: true

                    visible: Layout.minimumWidth != 0

                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.BackButton
                        onClicked: goBack()
                    }

                    Rectangle {
                        radius: 3
                        color: myPalette.base

                        anchors.fill: parent

                        StackView {
                            id: browseStackView

                            anchors.fill: parent

                            clip: true

                            initialItem: Item {
                            }

                            popEnter: Transition {
                                OpacityAnimator {
                                    from: 0.0
                                    to: 1.0
                                    duration: 300
                                }
                            }

                            popExit: Transition {
                                OpacityAnimator {
                                    from: 1.0
                                    to: 0.0
                                    duration: 300
                                }
                            }

                            pushEnter: Transition {
                                OpacityAnimator {
                                    from: 0.0
                                    to: 1.0
                                    duration: 300
                                }
                            }

                            pushExit: Transition {
                                OpacityAnimator {
                                    from: 1.0
                                    to: 0.0
                                    duration: 300
                                }
                            }

                            replaceEnter: Transition {
                                OpacityAnimator {
                                    from: 0.0
                                    to: 1.0
                                    duration: 300
                                }
                            }

                            replaceExit: Transition {
                                OpacityAnimator {
                                    from: 1.0
                                    to: 0.0
                                    duration: 300
                                }
                            }
                        }
                    }
                }

                Rectangle {
                    id: firstViewSeparatorItem

                    Layout.fillHeight: true

                    width: 1

                    color: myPalette.mid
                }

                MediaPlayListView {
                    id: playList

                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    onStartPlayback: elisa.audioControl.ensurePlay()

                    onPausePlayback: elisa.audioControl.playPause()
                }
            }
        }

        states: [
            State {
                name: "browsingViewsNoPlaylist"
                when: contentViewContainer.showPlaylist === false
                extend: "browsingViews"
                PropertyChanges {
                    target: mainContentView
                    Layout.minimumWidth: contentZone.width
                    Layout.maximumWidth: contentZone.width
                    Layout.preferredWidth: contentZone.width
                }
                PropertyChanges {
                    target: playList
                    Layout.minimumWidth: 0
                    Layout.maximumWidth: 0
                    Layout.preferredWidth: 0
                }
            },
            State {
                name: 'browsingViews'
                when: contentViewContainer.showPlaylist === true
                PropertyChanges {
                    target: mainContentView
                    Layout.minimumWidth: contentZone.width * 0.66
                    Layout.maximumWidth: contentZone.width * 0.68
                    Layout.preferredWidth: contentZone.width * 0.68
                }
                PropertyChanges {
                    target: firstViewSeparatorItem
                    Layout.minimumWidth: 1
                    Layout.maximumWidth: 1
                    Layout.preferredWidth: 1
                }
            }
        ]
        transitions: Transition {
            NumberAnimation {
                properties: "Layout.minimumWidth, Layout.maximumWidth, Layout.preferredWidth, opacity"
                easing.type: Easing.InOutQuad
                duration: 300
            }
        }
    }

    Component {
        id: dataGridView

        DataGridView {
            StackView.onActivated: viewManager.viewIsLoaded(viewType)
        }
    }

    Component {
        id: dataListView

        DataListView {
            StackView.onActivated: viewManager.viewIsLoaded(viewType)
        }
    }

    Component {
        id: filesBrowserView

        FileBrowserView {
            StackView.onActivated: viewManager.viewIsLoaded(viewType)
        }
    }

    Component {
        id: albumContext

        ContextView {
            StackView.onActivated: viewManager.viewIsLoaded(viewType)

            databaseId: elisa.manageHeaderBar.databaseId
            title: elisa.manageHeaderBar.title
            artistName: elisa.manageHeaderBar.artist
            albumName: elisa.manageHeaderBar.album
            albumArtUrl: elisa.manageHeaderBar.image
            fileUrl: elisa.manageHeaderBar.fileName
        }
    }
}
