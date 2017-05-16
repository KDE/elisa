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

import QtQuick 2.5
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Window 2.2
import QtQml.Models 2.1
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0

import org.mgallien.QmlExtension 1.0

Item {
    property var rootIndex
    property StackView stackView
    property MediaPlayList playListModel
    property var musicListener
    property var playerControl
    property var contentDirectoryModel

    id: rootElement

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

        RowLayout {
            id: filterRow

            Layout.fillWidth: true
            Layout.topMargin: elisaTheme.layoutVerticalMargin
            Layout.bottomMargin: elisaTheme.layoutVerticalMargin

            LabelWithToolTip {
                text: i18nc("before the TextField input of the filter", "Filter: ")

                color: myPalette.text
            }

            TextField {
                id: filterTextInput

                placeholderText: i18nc("Placeholder text in the filter text box", "Filter")

                Layout.fillWidth: true

                Image {
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    anchors.margins: elisaTheme.layoutVerticalMargin
                    id: clearText
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                    visible: parent.text
                    source: Qt.resolvedUrl(elisaTheme.clearIcon)
                    height: parent.height
                    width: parent.height
                    sourceSize.width: parent.height
                    sourceSize.height: parent.height

                    MouseArea {
                        id: clear
                        anchors { horizontalCenter: parent.horizontalCenter; verticalCenter: parent.verticalCenter }
                        height: parent.parent.height
                        width: parent.parent.height
                        onClicked: {
                            parent.parent.text = ""
                            parent.parent.forceActiveFocus()
                        }
                    }
                }
            }

            LabelWithToolTip {
                text: i18nc("before the Rating widget input of the filter", "Rating: ")

                color: myPalette.text

                Layout.leftMargin: elisaTheme.layoutHorizontalMargin
            }

            RatingStar {
                id: ratingFilter

                readOnly: false

                starSize: elisaTheme.ratingStarSize
            }
        }

        Rectangle {
            color: myPalette.base

            Layout.fillHeight: true
            Layout.fillWidth: true

            ScrollView {
                anchors.fill: parent
                flickableItem.boundsBehavior: Flickable.StopAtBounds
                GridView {
                    id: contentDirectoryView

                    TextMetrics {
                        id: textLineHeight
                        text: 'Album'
                    }

                    cellWidth: elisaTheme.gridDelegateWidth
                    cellHeight: elisaTheme.gridDelegateWidth + elisaTheme.layoutVerticalMargin * 4 + textLineHeight.height * 3

                    model: DelegateModel {
                        id: delegateContentModel

                        model: AlbumFilterProxyModel {
                            sourceModel: rootElement.contentDirectoryModel

                            filterText: filterTextInput.text

                            filterRating: ratingFilter.starRating
                        }

                        delegate: MediaAlbumDelegate {
                            width: contentDirectoryView.cellWidth
                            height: contentDirectoryView.cellHeight

                            musicListener: rootElement.musicListener
                            image: model.image
                            title: if (model.title)
                                       model.title
                                   else
                                       ""
                            artist: if (model.artist)
                                        model.artist
                                    else
                                        ""
                            trackNumber: if (model.count !== undefined)
                                             i18np("1 track", "%1 tracks", model.count)
                                         else
                                             i18nc("Number of tracks for an album", "0 track")

                            isSingleDiscAlbum: model.isSingleDiscAlbum

                            albumData: model.albumData

                            stackView: rootElement.stackView

                            playListModel: rootElement.playListModel
                            playerControl: rootElement.playerControl
                        }
                    }

                    focus: true
                }
            }
        }
    }
}
