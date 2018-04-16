/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQml.Models 2.2
import org.kde.elisa 1.0
import QtQuick.Layouts 1.2

FocusScope {
    id: topListing

    property var albumName
    property var artistName
    property var albumArtUrl
    property bool isSingleDiscAlbum
    property alias expandedFilterView: navigationBar.expandedFilterView
    property var albumId
    property alias contentModel: contentDirectoryView.model

    signal showArtist(var name)
    signal goBack();
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

            height: elisaTheme.navigationBarHeight

            Layout.preferredHeight: height
            Layout.minimumHeight: height
            Layout.maximumHeight: height
            Layout.fillWidth: true

            mainTitle: (topListing.artistName ? topListing.artistName : '')
            secondaryTitle: topListing.albumName
            image: (topListing.albumArtUrl ? topListing.albumArtUrl : elisaTheme.defaultAlbumImage)
            allowArtistNavigation: true

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

            onGoBack: topListing.goBack()

            onFilterViewChanged: topListing.filterViewChanged(expandedFilterView)

            onShowArtist: topListing.showArtist(topListing.contentModel.sourceModel.author)

            onEnqueue: contentModel.enqueueToPlayList()

            onReplaceAndPlay: contentModel.replaceAndPlayOfPlayList()
        }

        ListView {
            id: contentDirectoryView
            Layout.topMargin: 20
            Layout.fillHeight: true
            Layout.fillWidth: true
            contentWidth: parent.width
            focus: true

            ScrollBar.vertical: ScrollBar {
                id: scrollBar
            }
            boundsBehavior: Flickable.StopAtBounds
            clip: true

            delegate: MediaAlbumTrackDelegate {
                id: entry

                height: ((model.isFirstTrackOfDisc && !isSingleDiscAlbum) ? elisaTheme.delegateHeight*2 : elisaTheme.delegateHeight)
                width: scrollBar.visible ? contentDirectoryView.width - scrollBar.width : contentDirectoryView.width

                focus: true

                mediaTrack.trackData: model.containerData

                mediaTrack.isFirstTrackOfDisc: model.isFirstTrackOfDisc

                mediaTrack.isSingleDiscAlbum: model.isSingleDiscAlbum

                mediaTrack.onEnqueue: elisa.mediaPlayList.enqueue(data)

                mediaTrack.onReplaceAndPlay: elisa.mediaPlayList.replaceAndPlay(data)

                mediaTrack.isAlternateColor: (index % 2) === 1

                mediaTrack.onClicked: contentDirectoryView.currentIndex = index
            }
        }
    }
}
