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
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Window 2.2
import QtQml.Models 2.1
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0

import org.kde.elisa 1.0

FocusScope {
    id: rootElement

    property var stackView
    property alias contentModel: contentDirectoryView.model
    property alias filterState: navigationBar.state

    signal filterViewChanged(string filterState)

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

            mainTitle: i18nc("Title of the view of all tracks", "Tracks")
            secondaryTitle: ""
            image: elisaTheme.tracksIcon
            enableGoBack: false

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

            onFilterViewChanged: rootElement.filterViewChanged(filterState)

            onReplaceAndPlay: contentModel.replaceAndPlayOfPlayList()
        }

        Rectangle {
            color: myPalette.base

            Layout.fillHeight: true
            Layout.fillWidth: true

            ScrollView {
                anchors.fill: parent
                flickableItem.boundsBehavior: Flickable.StopAtBounds
                flickableItem.interactive: true

                ListView {
                    id: contentDirectoryView

                    focus: true

                    delegate: MediaTrackDelegate {
                        id: entry

                        width: contentDirectoryView.width
                        height: elisaTheme.trackDelegateHeight

                        focus: true

                        isAlternateColor: (index % 2) === 1

                        trackData: model.containerData

                        isFirstTrackOfDisc: false

                        isSingleDiscAlbum: model.isSingleDiscAlbum

                        onEnqueue: rootElement.enqueue(data)

                        onReplaceAndPlay: rootElement.replaceAndPlay(data)

                        onClicked: contentDirectoryView.currentIndex = index
                    }
                }
            }
        }
    }
}

