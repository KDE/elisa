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

import org.mgallien.QmlExtension 1.0

Item {
    property var playerControl
    property var playListModel
    property var tracksModel
    property var stackView
    property var musicListener

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

        TextMetrics {
            id: titleHeight
            text: viewTitleHeight.text
            font
            {
                pixelSize: viewTitleHeight.font.pixelSize
                bold: viewTitleHeight.font.bold
            }
        }

        LabelWithToolTip {
            id: viewTitleHeight
            text: i18nc("Title of the view of all tracks", "Tracks")

            color: myPalette.text
            font.pixelSize: elisaTheme.defaultFontPixelSize * 2

            Layout.leftMargin: elisaTheme.layoutHorizontalMargin
            Layout.rightMargin: elisaTheme.layoutHorizontalMargin
            Layout.topMargin: elisaTheme.layoutVerticalMargin
            Layout.bottomMargin: titleHeight.height + elisaTheme.layoutVerticalMargin
        }

        RowLayout {
            id: filterRow

            spacing: 0

            Layout.fillWidth: true
            Layout.bottomMargin: titleHeight.height + elisaTheme.layoutVerticalMargin * 2
            Layout.leftMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
            Layout.rightMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0

            LabelWithToolTip {
                text: i18nc("before the TextField input of the filter", "Filter: ")

                font.bold: true

                color: myPalette.text
            }

            TextField {
                id: filterTextInput

                placeholderText: i18nc("Placeholder text in the filter text box", "Filter")

                Layout.preferredWidth: rootElement.width / 2

                Image {
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    anchors.margins: elisaTheme.filterClearButtonMargin
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

                font.bold: true

                color: myPalette.text

                Layout.bottomMargin: 0
                Layout.leftMargin: !LayoutMirroring.enabled ? (elisaTheme.layoutHorizontalMargin * 2) : 0
                Layout.rightMargin: LayoutMirroring.enabled ? (elisaTheme.layoutHorizontalMargin * 2) : 0
            }

            RatingStar {
                id: ratingFilter

                readOnly: false

                starSize: elisaTheme.ratingStarSize

                Layout.bottomMargin: 0
            }
        }

        Rectangle {
            color: myPalette.base

            Layout.fillHeight: true
            Layout.fillWidth: true

            ScrollView {
                anchors.fill: parent
                flickableItem.boundsBehavior: Flickable.StopAtBounds

                ListView {
                    id: contentDirectoryView

                    TextMetrics {
                        id: textLineHeight
                        text: 'Artist'
                    }

                    model: DelegateModel {
                        id: delegateContentModel

                        model: AlbumFilterProxyModel {
                            sourceModel: rootElement.tracksModel

                            filterText: filterTextInput.text

                            filterRating: ratingFilter.starRating
                        }

                        delegate: MediaTracksDelegate {
                            width: contentDirectoryView.width
                            height: elisaTheme.trackDelegateHeight

                            isAlternateColor: (index % 2) === 1

                            title: if (model != undefined && model.title !== undefined)
                                       model.title
                                   else
                                       ''

                            artist: if (model != undefined && model.artist !== undefined)
                                        model.artist
                                    else
                                        ''

                            albumName: if (model != undefined && model.album !== undefined)
                                             model.album
                                         else
                                             ''

                            duration: if (model != undefined && model.duration !== undefined)
                                          model.duration
                                      else
                                          ''

                            trackNumber: if (model != undefined && model.trackNumber !== undefined)
                                             model.trackNumber
                                         else
                                             ''

                            discNumber: if (model != undefined && model.discNumber !== undefined)
                                             model.discNumber
                                         else
                                             ''

                            rating: if (model != undefined && model.rating !== undefined)
                                             model.rating
                                         else
                                             0

                            trackData: if (model != undefined && model.trackData !== undefined)
                                           model.trackData
                                       else
                                           ''

                            coverImage: if (model != undefined && model.image !== undefined)
                                            model.image
                                        else
                                            ''

                            playList: rootElement.playListModel
                            playerControl: rootElement.playerControl
                        }
                    }

                    focus: true
                }
            }
        }
    }
}
