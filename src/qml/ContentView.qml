/*
 * Copyright 2016-2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.2
import QtQuick.Window 2.2
import org.kde.elisa 1.0

RowLayout {
    id: contentViewContainer
    spacing: 0

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
        }

        onSwitchOneAlbumView: {
            elisa.singleAlbumProxyModel.loadAlbumData(databaseId)
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
        }

        onSwitchOneArtistView: {
            elisa.singleArtistProxyModel.setArtistFilterText(mainTitle)
            currentStackView.push(innerAlbumView, {
                                      mainTitle: mainTitle,
                                      secondaryTitle: secondaryTitle,
                                      image: imageUrl,
                                      stackView: currentStackView,
                                  })
            oneArtistViewIsLoaded()
        }

        onSwitchAllTracksView: {
            listViews.currentIndex = 3
            localArtistsLoader.opacity = 0
            localTracksLoader.opacity = 1
            localAlbumsLoader.opacity = 0
        }

        onSwitchOffAllViews: {
            localArtistsLoader.opacity = 0
            localTracksLoader.opacity = 0
            localAlbumsLoader.opacity = 0
        }
    }

    ViewSelector {
        id: listViews

        Layout.fillHeight: true
        Layout.preferredWidth: mainWindow.width * 0.11
        Layout.maximumWidth: mainWindow.width * 0.11

        onSwitchView: if (index === 1) {
                          viewManager.openAllAlbums()
                      } else if (index === 2) {
                          viewManager.openAllArtists()
                      } else if (index === 3) {
                          viewManager.openAllTracks()
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
                            anchors.fill: parent

                            anchors.leftMargin: parent.width / 3
                            anchors.rightMargin: parent.width / 3
                            anchors.topMargin: parent.height / 3
                            anchors.bottomMargin: parent.height / 3

                            z: 2

                            sourceComponent: BusyIndicator {
                                id: busyScanningMusic
                                hoverEnabled: false

                                anchors.fill: parent

                                opacity: 0.8

                                visible: true
                                running: true

                                z: 2
                            }

                            active: elisa.musicManager.indexerBusy
                        }

                        Loader {
                            id: localAlbumsLoader

                            active: opacity > 0

                            visible: opacity > 0

                            anchors.fill: parent

                            onLoaded: viewManager.allAlbumsViewIsLoaded(item.stackView)

                            sourceComponent: MediaBrowser {
                                id: localAlbums

                                focus: true

                                anchors {
                                    fill: parent

                                    leftMargin: elisaTheme.layoutHorizontalMargin
                                    rightMargin: elisaTheme.layoutHorizontalMargin
                                }

                                firstPage: GridBrowserView {
                                    id: allAlbumsView

                                    focus: true

                                    contentModel: elisa.allAlbumsProxyModel

                                    image: elisaTheme.albumIcon
                                    mainTitle: i18nc("Title of the view of all albums", "Albums")

                                    onOpen: {
                                        viewManager.openOneAlbum(localAlbums.stackView, innerMainTitle, innerSecondaryTitle, innerImage, databaseId)
                                    }

                                    onGoBack: viewManager.goBack()

                                    Binding {
                                        target: allAlbumsView
                                        property: 'expandedFilterView'
                                        value: persistentSettings.expandedFilterView
                                    }

                                    onFilterViewChanged: persistentSettings.expandedFilterView = expandedFilterView
                                }
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

                            sourceComponent: MediaBrowser {
                                id: localArtists

                                focus: true

                                anchors {
                                    fill: parent

                                    leftMargin: elisaTheme.layoutHorizontalMargin
                                    rightMargin: elisaTheme.layoutHorizontalMargin
                                }

                                firstPage: GridBrowserView {
                                    id: allArtistsView
                                    focus: true

                                    showRating: false
                                    delegateDisplaySecondaryText: false

                                    contentModel: elisa.allArtistsProxyModel

                                    image: elisaTheme.artistIcon
                                    mainTitle: i18nc("Title of the view of all artists", "Artists")

                                    onOpen: {
                                        viewManager.openOneArtist(localArtists.stackView, innerMainTitle, innerImage, 0)
                                    }

                                    onGoBack: viewManager.goBack()

                                    Binding {
                                        target: allArtistsView
                                        property: 'expandedFilterView'
                                        value: persistentSettings.expandedFilterView
                                    }

                                    onFilterViewChanged: persistentSettings.expandedFilterView = expandedFilterView
                                }
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

                            sourceComponent: MediaBrowser {
                                id: localTracks

                                focus: true

                                anchors {
                                    fill: parent

                                    leftMargin: elisaTheme.layoutHorizontalMargin
                                    rightMargin: elisaTheme.layoutHorizontalMargin
                                }

                                firstPage: ListBrowserView {
                                    id: allTracksView
                                    focus: true

                                    contentModel: elisa.allTracksProxyModel

                                    delegate: MediaTrackDelegate {
                                        id: entry

                                        width: allTracksView.delegateWidth
                                        height: elisaTheme.trackDelegateHeight

                                        focus: true

                                        trackData: model.containerData

                                        isFirstTrackOfDisc: false

                                        isSingleDiscAlbum: model.isSingleDiscAlbum

                                        onEnqueue: elisa.mediaPlayList.enqueue(data)

                                        onReplaceAndPlay: elisa.mediaPlayList.replaceAndPlay(data)

                                        onClicked: contentDirectoryView.currentIndex = index
                                    }

                                    image: elisaTheme.tracksIcon
                                    mainTitle: i18nc("Title of the view of all tracks", "Tracks")

                                    Binding {
                                        target: allTracksView
                                        property: 'expandedFilterView'
                                        value: persistentSettings.expandedFilterView
                                    }

                                    onFilterViewChanged: persistentSettings.expandedFilterView = expandedFilterView
                                }
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

        GridBrowserView {
            id: innerAlbumGridView

            contentModel: elisa.singleArtistProxyModel

            isSubPage: true

            onOpen: {
                viewManager.openOneAlbumFromArtist(stackView, innerMainTitle, innerSecondaryTitle, innerImage, databaseId)
            }

            onGoBack: viewManager.goBack()

            Binding {
                target: innerAlbumGridView
                property: 'expandedFilterView'
                value: persistentSettings.expandedFilterView
            }

            onFilterViewChanged: persistentSettings.expandedFilterView = expandedFilterView
        }
    }

    Component {
        id: albumView

        ListBrowserView {
            id: albumGridView

            contentModel: elisa.singleAlbumProxyModel

            isSubPage: true

            delegate: MediaAlbumTrackDelegate {
                id: entry

                width: albumGridView.delegateWidth
                height: ((model.isFirstTrackOfDisc && !isSingleDiscAlbum) ? elisaTheme.delegateHeight*2 : elisaTheme.delegateHeight)

                focus: true

                mediaTrack.trackData: model.containerData

                mediaTrack.isFirstTrackOfDisc: model.isFirstTrackOfDisc

                mediaTrack.isSingleDiscAlbum: model.isSingleDiscAlbum

                mediaTrack.onEnqueue: elisa.mediaPlayList.enqueue(data)

                mediaTrack.onReplaceAndPlay: elisa.mediaPlayList.replaceAndPlay(data)

                mediaTrack.isAlternateColor: (index % 2) === 1

                mediaTrack.onClicked: contentDirectoryView.currentIndex = index
            }

            allowArtistNavigation: true

            onShowArtist: {
                viewManager.openOneArtist(stackView, name, elisaTheme.artistIcon, 0)
            }

            onGoBack: viewManager.goBack()

            expandedFilterView: true

            Binding {
                target: albumGridView
                property: 'expandedFilterView'
                value: persistentSettings.expandedFilterView
            }

            onFilterViewChanged: persistentSettings.expandedFilterView = expandedFilterView
        }
    }
}
