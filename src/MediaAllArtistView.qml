/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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
import QtMultimedia 5.4

import org.mgallien.QmlExtension 1.0

Item {
    property var playerControl
    property var playListModel
    property var musicDatabase
    property var stackView

    id: rootElement

    SystemPalette {
        id: myPalette
        colorGroup: SystemPalette.Active
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            id: filterRow

            Layout.fillWidth: true

            Text {
                text: i18nc("before the TextField input of the filter", "Filter: ")
            }

            TextField {
                id: filterTextInput

                placeholderText: i18nc("Placeholder text in the filter text box", "Filter")

                Layout.fillWidth: true

                Image {
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    anchors.margins: Screen.pixelDensity * 1
                    id: clearText
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                    visible: parent.text
                    source: 'image://icon/edit-clear'
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
        }

        Rectangle {
            color: myPalette.base

            Layout.fillHeight: true
            Layout.fillWidth: true

            ScrollView {
                anchors.fill: parent
                GridView {
                    id: contentDirectoryView

                    cellWidth: Screen.pixelDensity * 40.
                    cellHeight: Screen.pixelDensity * 54.5

                    model: DelegateModel {
                        id: delegateContentModel

                        model: AllArtistsModel {
                            id: allArtistsModel

                            databaseInterface: rootElement.musicDatabase
                            filter: filterTextInput.text
                        }

                        delegate: MediaArtistDelegate {
                            width: contentDirectoryView.cellWidth
                            height: contentDirectoryView.cellHeight

                            image: if (model.image)
                                       model.image
                                   else
                                       ""

                            name: if (model.name)
                                      model.name
                                  else
                                      ""
                            albumsNumber: if (model.albumsCount !== undefined)
                                              i18ncp("Number of albums for an artist", "1 album", "%1 albums", model.albumsCount)
                                          else
                                              i18nc("Number of albums for an artist", "0 album")

                            stackView: rootElement.stackView

                            playListModel: rootElement.playListModel
                            playerControl: rootElement.playerControl
                            musicDatabase: rootElement.musicDatabase
                        }
                    }

                    focus: true
                }
            }
        }
    }
}
