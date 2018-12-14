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

    ViewManager {
        id: viewManager

        onSwitchAllAlbumsView: {
            listViews.currentIndex = 1
            localArtistsLoader.opacity = 0
            localTracksLoader.opacity = 0
            localAlbumsLoader.opacity = 1
            localGenresLoader.opacity = 0
            localFilesLoader.opacity = 0
        }

        onSwitchOneAlbumView: {
            currentStackView.push(albumView, {
                                      mainTitle: mainTitle,
                                      secondaryTitle: secondaryTitle,
                                      image: imageUrl,
                                      stackView: currentStackView,
                                  })
            oneAlbumViewIsLoaded()
        }

        onSwitchAllArtistsView: {
            listViews.currentIndex = 2
            localArtistsLoader.opacity = 1
            localTracksLoader.opacity = 0
            localAlbumsLoader.opacity = 0
            localGenresLoader.opacity = 0
            localFilesLoader.opacity = 0
        }

        onSwitchOneArtistView: {
            currentStackView.push(innerAlbumView, {
                                      mainTitle: mainTitle,
                                      secondaryTitle: secondaryTitle,
                                      image: imageUrl,
                                      stackView: currentStackView,
                                      artistFilter: mainTitle,
                                  })
            oneArtistViewIsLoaded()
        }

        onSwitchOneArtistFromGenreView: {
            currentStackView.push(innerAlbumView, {
                                      mainTitle: mainTitle,
                                      secondaryTitle: secondaryTitle,
                                      image: imageUrl,
                                      stackView: currentStackView,
                                      artistFilter: mainTitle,
                                  })
            oneArtistViewIsLoaded()
        }

        onSwitchAllTracksView: {
            listViews.currentIndex = 3
            localArtistsLoader.opacity = 0
            localTracksLoader.opacity = 1
            localAlbumsLoader.opacity = 0
            localGenresLoader.opacity = 0
            localFilesLoader.opacity = 0
        }

        onSwitchAllGenresView: {
            listViews.currentIndex = 4
            localArtistsLoader.opacity = 0
            localTracksLoader.opacity = 0
            localAlbumsLoader.opacity = 0
            localGenresLoader.opacity = 1
            localFilesLoader.opacity = 0
        }

        onSwitchFilesBrowserView: {
            listViews.currentIndex = 5
            localArtistsLoader.opacity = 0
            localTracksLoader.opacity = 0
            localAlbumsLoader.opacity = 0
            localGenresLoader.opacity = 0
            localFilesLoader.opacity = 1
        }

        onSwitchAllArtistsFromGenreView: {
            currentStackView.push(innerArtistView, {
                                      mainTitle: genreName,
                                      secondaryTitle: '',
                                      image: elisaTheme.artistIcon,
                                      stackView: currentStackView,
                                      genreFilterText: genreName,
                                  })
            allArtistsFromGenreViewIsLoaded()
        }

        onSwitchOffAllViews: {
            localArtistsLoader.opacity = 0
            localTracksLoader.opacity = 0
            localAlbumsLoader.opacity = 0
            localGenresLoader.opacity = 0
            localFilesLoader.opacity = 0
        }
    }

    ViewSelector {
        id: listViews

        Layout.fillHeight: true

        Layout.maximumWidth: mainWindow.width * 0.11
        maximumSize: mainWindow.width * 0.11

        Behavior on Layout.maximumWidth {
            NumberAnimation {
                duration: 150
            }
        }

        onSwitchView: if (index === 1) {
                          viewManager.openAllAlbums()
                      } else if (index === 2) {
                          viewManager.openAllArtists()
                      } else if (index === 3) {
                          viewManager.openAllTracks()
                      } else if (index === 4) {
                          viewManager.openAllGenres()
                      } else if (index === 5) {
                          viewManager.openFilesBrowser()
                      } else {
                          viewManager.closeAllViews()
                      }
    }

    Rectangle {
        id: viewSelectorSeparatorItem

        border.width: 1
        border.color: myPalette.mid
        color: myPalette.mid
        visible: true

        Layout.bottomMargin: elisaTheme.layoutVerticalMargin
        Layout.topMargin: elisaTheme.layoutVerticalMargin

        Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

        Layout.fillHeight: true

        Layout.preferredWidth: 1
        Layout.minimumWidth: 1
        Layout.maximumWidth: 1
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

                    Layout.minimumWidth: 0
                    Layout.maximumWidth: 0
                    Layout.preferredWidth: 0


                    visible: Layout.minimumWidth != 0

                    Rectangle {
                        border {
                            color: (mainContentView.activeFocus ? myPalette.highlight : myPalette.base)
                            width: 1
                        }

                        radius: 3
                        color: myPalette.base

                        anchors.fill: parent

                        Loader {
                            id: localAlbumsLoader

                            active: opacity > 0

                            visible: opacity > 0

                            anchors.fill: parent

                            onLoaded: viewManager.allAlbumsViewIsLoaded(item.stackView)

                            sourceComponent: AlbumsView {
                            }

                            Behavior on opacity {
                                NumberAnimation {
                                    easing.type: Easing.InOutQuad
                                    duration: 300
                                }
                            }
                        }

                        Loader {
                            id: localArtistsLoader

                            active: opacity > 0

                            visible: opacity > 0

                            opacity: 0

                            anchors.fill: parent

                            onLoaded: viewManager.allArtistsViewIsLoaded(item.stackView)

                            sourceComponent: ArtistsView {
                            }

                            Behavior on opacity {
                                NumberAnimation {
                                    easing.type: Easing.InOutQuad
                                    duration: 300
                                }
                            }
                        }

                        Loader {
                            id: localTracksLoader

                            active: opacity > 0

                            visible: opacity > 0

                            opacity: 0

                            anchors.fill: parent

                            onLoaded: viewManager.allTracksViewIsLoaded(item)

                            sourceComponent: TracksView {
                            }

                            Behavior on opacity {
                                NumberAnimation {
                                    easing.type: Easing.InOutQuad
                                    duration: 300
                                }
                            }
                        }

                        Loader {
                            id: localGenresLoader

                            active: opacity > 0

                            visible: opacity > 0

                            opacity: 0

                            anchors.fill: parent

                            onLoaded: viewManager.allGenresViewIsLoaded(item.stackView)

                            sourceComponent: GenresView {
                            }

                            Behavior on opacity {
                                NumberAnimation {
                                    easing.type: Easing.InOutQuad
                                    duration: 300
                                }
                            }
                        }

                        Loader {
                            id: localFilesLoader
                            anchors.fill: parent

                            active: opacity > 0

                            visible: opacity > 0

                            opacity: 0

                            anchors {
                                fill: parent

                                leftMargin: elisaTheme.layoutHorizontalMargin
                                rightMargin: elisaTheme.layoutHorizontalMargin
                            }

                            onLoaded: viewManager.filesBrowserViewIsLoaded(item)

                            sourceComponent: FileBrowserView {
                                id: localFiles

                                focus: true

                                contentModel: elisa.fileBrowserProxyModel
                            }

                            Behavior on opacity {
                                NumberAnimation {
                                    easing.type: Easing.InOutQuad
                                    duration: 300
                                }
                            }
                        }

                        Behavior on border.color {
                            ColorAnimation {
                                duration: 300
                            }
                        }
                    }
                }

                Rectangle {
                    id: firstViewSeparatorItem

                    border.width: 1
                    border.color: myPalette.mid
                    color: myPalette.mid
                    visible: true

                    Layout.bottomMargin: elisaTheme.layoutVerticalMargin
                    Layout.topMargin: elisaTheme.layoutVerticalMargin

                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

                    Layout.fillHeight: true

                    Layout.preferredWidth: 1
                    Layout.minimumWidth: 1
                    Layout.maximumWidth: 1
                }

                MediaPlayListView {
                    id: playList

                    playListModel: elisa.mediaPlayList

                    Layout.fillHeight: true
                    Layout.leftMargin: elisaTheme.layoutHorizontalMargin
                    Layout.rightMargin: elisaTheme.layoutHorizontalMargin

                    Layout.minimumWidth: contentZone.width
                    Layout.maximumWidth: contentZone.width
                    Layout.preferredWidth: contentZone.width

                    onStartPlayback: elisa.audioControl.ensurePlay()

                    onPausePlayback: elisa.audioControl.playPause()

                    onDisplayError: messageNotification.showNotification(errorText)
                }

                Rectangle {
                    id: viewSeparatorItem

                    border.width: 1
                    border.color: myPalette.mid
                    color: myPalette.mid
                    visible: Layout.minimumWidth != 0

                    Layout.bottomMargin: elisaTheme.layoutVerticalMargin
                    Layout.topMargin: elisaTheme.layoutVerticalMargin

                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

                    Layout.fillHeight: true

                    Layout.preferredWidth: 1
                    Layout.minimumWidth: 1
                    Layout.maximumWidth: 1
                }

                ContextView {
                    id: albumContext

                    Layout.fillHeight: true

                    Layout.minimumWidth: contentZone.width
                    Layout.maximumWidth: contentZone.width
                    Layout.preferredWidth: contentZone.width

                    visible: Layout.minimumWidth != 0

                    artistName: elisa.manageHeaderBar.artist
                    albumName: elisa.manageHeaderBar.album
                    albumArtUrl: elisa.manageHeaderBar.image
                }
            }
        }

        states: [
            State {
                name: 'playList'
                when: listViews.currentIndex === 0
                PropertyChanges {
                    target: mainContentView
                    Layout.fillWidth: false
                    Layout.minimumWidth: 0
                    Layout.maximumWidth: 0
                    Layout.preferredWidth: 0
                }
                PropertyChanges {
                    target: firstViewSeparatorItem
                    Layout.minimumWidth: 0
                    Layout.maximumWidth: 0
                    Layout.preferredWidth: 0
                }
                PropertyChanges {
                    target: playList
                    Layout.minimumWidth: contentZone.width / 2
                    Layout.maximumWidth: contentZone.width / 2
                    Layout.preferredWidth: contentZone.width / 2
                }
                PropertyChanges {
                    target: viewSeparatorItem
                    Layout.minimumWidth: 1
                    Layout.maximumWidth: 1
                    Layout.preferredWidth: 1
                }
                PropertyChanges {
                    target: albumContext
                    Layout.minimumWidth: contentZone.width / 2
                    Layout.maximumWidth: contentZone.width / 2
                    Layout.preferredWidth: contentZone.width / 2
                }
            },
            State {
                name: "browsingViewsNoPlaylist"
                when: listViews.currentIndex !== 0 && contentViewContainer.showPlaylist !== true
                extend: "browsingViews"
                PropertyChanges {
                    target: mainContentView
                    Layout.fillWidth: true
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
                when: listViews.currentIndex !== 0
                PropertyChanges {
                    target: mainContentView
                    Layout.fillWidth: true
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
                PropertyChanges {
                    target: playList
                    Layout.minimumWidth: contentZone.width * 0.33
                    Layout.maximumWidth: contentZone.width * 0.33
                    Layout.preferredWidth: contentZone.width * 0.33
                }
                PropertyChanges {
                    target: viewSeparatorItem
                    Layout.minimumWidth: 0
                    Layout.maximumWidth: 0
                    Layout.preferredWidth: 0
                }
                PropertyChanges {
                    target: albumContext
                    Layout.minimumWidth: 0
                    Layout.maximumWidth: 0
                    Layout.preferredWidth: 0
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
        id: innerAlbumView

        OneArtistView {
        }
    }

    Component {
        id: innerArtistView

        OneGenreView {
        }
    }

    Component {
        id: albumView

        AlbumView {
        }
    }
}
