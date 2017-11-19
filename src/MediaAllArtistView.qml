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
    property var playerControl
    property var playListModel
    property var artistsModel
    property var stackView
    property var contentDirectoryModel
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
            text: i18nc("Title of the view of all artists", "Artists")

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

                horizontalAlignment: TextInput.AlignLeft

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
                    mirror: LayoutMirroring.enabled

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
                focus: true

                anchors.fill: parent
                flickableItem.boundsBehavior: Flickable.StopAtBounds

                GridView {
                    id: contentDirectoryView

                    focus: true

                    TextMetrics {
                        id: textLineHeight
                        text: 'Artist'
                    }

                    cellWidth: elisaTheme.gridDelegateWidth
                    cellHeight: elisaTheme.gridDelegateWidth + elisaTheme.layoutVerticalMargin * 2 + textLineHeight.height

                    model: DelegateModel {
                        id: delegateContentModel

                        model: SortFilterProxyModel {
                            sourceModel: artistsModel

                            filterRole: AllArtistsModel.NameRole

                            filterRegExp: new RegExp(filterTextInput.text, 'i')
                        }

                        delegate: MediaArtistDelegate {
                            width: contentDirectoryView.cellWidth
                            height: contentDirectoryView.cellHeight

                            focus: true

                            musicListener: rootElement.musicListener

                            image: if (model.image)
                                       model.image
                                   else
                                       ""

                            name: if (model.name)
                                      model.name
                                  else
                                      ""

                            stackView: rootElement.stackView

                            playListModel: rootElement.playListModel
                            playerControl: rootElement.playerControl
                            contentDirectoryModel: rootElement.contentDirectoryModel

                            onArtistClicked: contentDirectoryView.currentIndex = index
                        }
                    }
                }
            }
        }
    }
}
