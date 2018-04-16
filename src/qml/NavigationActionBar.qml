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

import QtQml 2.2
import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4 as Controls1

FocusScope {
    id: navigationBar

    property string mainTitle
    property string secondaryTitle
    property url image
    property bool allowArtistNavigation: false

    property string labelText
    property bool showRating: true
    property alias filterText: filterTextInput.text
    property alias filterRating: ratingFilter.starRating
    property bool enableGoBack: true
    property bool expandedFilterView: false

    signal enqueue();
    signal replaceAndPlay();
    signal goBack();
    signal showArtist();
    signal filterViewChanged(bool expandedFilterView);

    Controls1.Action {
        id: goPreviousAction
        text: i18nc("navigate back in the views stack", "Back")
        iconName: (Qt.application.layoutDirection == Qt.RightToLeft) ? "go-next" : "go-previous"
        onTriggered: goBack()
    }

    Controls1.Action {
        id: showFilterAction
        text: !navigationBar.expandedFilterView ? i18nc("Show filters in the navigation bar", "Show Search Options") : i18nc("Hide filters in the navigation bar", "Hide Search Options")
        iconName: !navigationBar.expandedFilterView ? "go-down-search" : "go-up-search"
        onTriggered: filterViewChanged(!navigationBar.expandedFilterView)
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        anchors.margins: {
            top: elisaTheme.layoutVerticalMargin
            bottom: elisaTheme.layoutVerticalMargin
        }

        RowLayout {
            spacing: 0
            Layout.alignment: Qt.AlignTop

            Controls1.ToolButton {
                action: goPreviousAction
                objectName: 'goPreviousButton'
                Layout.leftMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                Layout.rightMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                visible: enableGoBack
            }

            Image {
                id: mainIcon
                source: image

                asynchronous: true

                sourceSize.height: elisaTheme.coverImageSize / 2
                sourceSize.width: elisaTheme.coverImageSize / 2

                fillMode: Image.PreserveAspectFit

                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

                Layout.preferredHeight: elisaTheme.coverImageSize / 2
                Layout.minimumHeight: elisaTheme.coverImageSize / 2
                Layout.maximumHeight: elisaTheme.coverImageSize / 2
                Layout.preferredWidth: elisaTheme.coverImageSize / 2
                Layout.minimumWidth: elisaTheme.coverImageSize / 2
                Layout.maximumWidth: elisaTheme.coverImageSize / 2
                Layout.leftMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                Layout.rightMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
            }

            ColumnLayout {
                Layout.preferredHeight: elisaTheme.coverImageSize / 2
                Layout.minimumHeight: elisaTheme.coverImageSize / 2
                Layout.maximumHeight: elisaTheme.coverImageSize / 2

                spacing: 0

                Layout.fillWidth: true
                Layout.leftMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                Layout.rightMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0

                TextMetrics {
                    id: albumTextSize
                    text: albumLabel.text
                    font.pointSize: albumLabel.font.pointSize
                    font.bold: albumLabel.font.bold
                }

                LabelWithToolTip {
                    id: albumLabel

                    text: secondaryTitle

                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

                    elide: Text.ElideRight

                    color: myPalette.text

                    font {
                        pointSize: elisaTheme.defaultFontPointSize * 2
                    }

                    visible: secondaryTitle !== ""
                }

                TextMetrics {
                    id: authorTextSize
                    text: authorLabel.text
                    font.pointSize: authorLabel.font.pointSize
                    font.bold: authorLabel.font.bold
                }

                LabelWithToolTip {
                    id: authorLabel

                    text: mainTitle

                    color: myPalette.text

                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

                    font {
                        pointSize: (secondaryTitle !== "" ? elisaTheme.defaultFontPointSize : elisaTheme.defaultFontPointSize * 2)
                    }

                    elide: Text.ElideRight
                }

                Item {
                    id: emptyBottomFiller

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 0

                    Controls1.Button {
                        objectName: 'enqueueButton'
                        text: i18nc("Add current list to playlist", "Enqueue")
                        iconName: "media-track-add-amarok"

                        onClicked: enqueue()

                        Layout.leftMargin: 0
                        Layout.rightMargin: 0
                    }

                    Controls1.Button {
                        objectName: 'replaceAndPlayButton'
                        text: i18nc("Clear playlist and play", "Replace and Play")
                        tooltip: i18nc("Clear playlist and add current list to it", "Replace PlayList and Play Now")
                        iconName: "media-playback-start"

                        onClicked: replaceAndPlay()

                        Layout.leftMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                        Layout.rightMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                    }

                    Controls1.Button {
                        objectName: 'showArtistButton'
                        id: showArtistButton

                        visible: allowArtistNavigation
                        text: i18nc("Button to navigate to the artist of the album", "Display Artist")
                        iconName: "view-media-artist"

                        onClicked: showArtist()

                        Layout.leftMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                        Layout.rightMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    Controls1.ToolButton {
                        action: showFilterAction
                        objectName: 'showFilterButton'

                        Layout.alignment: Qt.AlignRight
                        Layout.leftMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                        Layout.rightMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
                    }
                }
            }
        }

        RowLayout {
            id: filterRow

            spacing: 0

            visible: opacity > 0.0

            opacity: 0

            Layout.fillWidth: true
            Layout.topMargin: elisaTheme.layoutVerticalMargin * 2
            Layout.leftMargin: !LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
            Layout.rightMargin: LayoutMirroring.enabled ? elisaTheme.layoutHorizontalMargin : 0
            Layout.alignment: Qt.AlignTop

            LabelWithToolTip {
                text: i18nc("before the TextField input of the filter", "Search: ")

                font.bold: true

                Layout.bottomMargin: 0

                color: myPalette.text
            }

            TextField {
                id: filterTextInput
                objectName: 'filterTextInput'

                horizontalAlignment: TextInput.AlignLeft

                placeholderText: i18nc("Placeholder text in the filter text box", "Album name, artist, etc.")

                Layout.bottomMargin: 0
                Layout.preferredWidth: navigationBar.width / 2

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

            LabelWithToolTip {
                text: i18nc("before the Rating widget input of the filter", "Rating: ")

                visible: showRating

                font.bold: true

                color: myPalette.text

                Layout.bottomMargin: 0
                Layout.leftMargin: !LayoutMirroring.enabled ? (elisaTheme.layoutHorizontalMargin * 2) : 0
                Layout.rightMargin: LayoutMirroring.enabled ? (elisaTheme.layoutHorizontalMargin * 2) : 0
            }

            RatingStar {
                id: ratingFilter
                objectName: 'ratingFilter'

                visible: showRating

                readOnly: false

                starSize: elisaTheme.ratingStarSize

                Layout.bottomMargin: 0
            }
        }
    }

    states: [
        State {
            name: 'collapsed'
            when: !expandedFilterView
            PropertyChanges {
                target: navigationBar
                height: elisaTheme.navigationBarHeight
            }
            PropertyChanges {
                target: filterRow
                opacity: 0.0
            }
        },
        State {
            name: 'expanded'
            when: expandedFilterView
            PropertyChanges {
                target: navigationBar
                height: elisaTheme.navigationBarHeight + elisaTheme.navigationBarFilterHeight
            }
            PropertyChanges {
                target: filterRow
                opacity: 1.0
            }
        }
    ]
    transitions: Transition {
        from: "expanded,collapsed"
        PropertyAnimation {
            properties: "height"
            easing.type: Easing.Linear
            duration: 250
        }
        PropertyAnimation {
            properties: "opacity"
            easing.type: Easing.Linear
            duration: 250
        }
    }
}
