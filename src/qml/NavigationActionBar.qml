/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

import QtQml 2.2
import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4 as Controls1

ColumnLayout {
    id: navigationBar

    spacing: 0

    property string mainTitle
    property string secondaryTitle
    property url image
    property bool allowArtistNavigation: false

    property string labelText
    property bool showRating: true
    property alias filterText: filterTextInput.text
    property alias filterRating: ratingFilter.starRating
    property bool enableGoBack: true
    property bool expandedFilterView: persistentSettings.expandedFilterView
    property bool enableSorting: true
    property bool sortOrder
    property var findAction: elisa.action("edit_find")

    signal enqueue();
    signal replaceAndPlay();
    signal goBack();
    signal showArtist();
    signal sort(var order);

    Controls1.Action {
        id: goPreviousAction
        text: i18nc("navigate back in the views stack", "Back")
        iconName: (Qt.application.layoutDirection == Qt.RightToLeft) ? "go-next" : "go-previous"
        onTriggered: goBack()
        enabled: enableGoBack
    }

    Controls1.Action {
        id: replaceAndPlayAction
        text: i18n("Play now, replacing contents of Playlist")
        iconName: "media-playback-start"
        onTriggered: replaceAndPlay()
    }

    Controls1.Action {
        id: enqueueAction
        text: i18nc("Add current list to playlist", "Enqueue")
        iconName: "media-track-add-amarok"
        onTriggered: enqueue()
    }

    Controls1.Action {
        id: showFilterAction
        shortcut: findAction.shortcut
        text: !navigationBar.expandedFilterView ? i18nc("Show filters in the navigation bar", "Show Search Options") : i18nc("Hide filters in the navigation bar", "Hide Search Options")
        iconName: 'search'
        checkable: true
        checked: filterRow.opacity == 1.0
        onTriggered: {
            persistentSettings.expandedFilterView = !persistentSettings.expandedFilterView
            expandedFilterView = persistentSettings.expandedFilterView
            if (expandedFilterView) {
                filterTextInput.forceActiveFocus()
            }
        }
    }

    Controls1.Action {
        id: sortAction
        text: i18nc("Toggle between ascending and descending order", "Toggle sort order")
        iconName: sortOrder ? "view-sort-ascending" : "view-sort-descending"
        onTriggered: sortOrder ? sort(Qt.DescendingOrder) : sort(Qt.AscendingOrder)
    }

    Controls1.Action {
        id: showArtistAction
        text: i18nc("Button to navigate to the artist of the album", "Display Artist")
        iconName: "view-media-artist"
        onTriggered: showArtist()
    }

    HeaderFooterToolbar {
        type: filterRow.visible? "other" : "header"
        contentItems: [

            Controls1.ToolButton {
                action: goPreviousAction
                id: goPreviousButton
                objectName: 'goPreviousButton'

                Keys.onReturnPressed: action.trigger()
                Accessible.onPressAction: action.trigger()

                activeFocusOnTab: true
                focus: enableGoBack
                visible: enableGoBack
            },
            Item {
                id: spacer
                width: elisaTheme.layoutHorizontalMargin
                visible: goPreviousButton.visible
            },
            Image {
                id: mainIcon
                source: image

                height: authorAndAlbumLayout.height
                width: height
                sourceSize.height: height
                sourceSize.width: width
                fillMode: Image.PreserveAspectFit
                asynchronous: true

                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
            },
            Item {
                width: elisaTheme.layoutHorizontalMargin
                visible: mainIcon.visible
            },
            ColumnLayout {
                id: authorAndAlbumLayout

                LabelWithToolTip {
                    id: albumLabel
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    Layout.fillWidth: true

                    text: mainTitle
                    font.pointSize: authorLabel.visible ?
                        Math.round(elisaTheme.defaultFontPointSize * 1.2) :
                        elisaTheme.headerTitleFontSize
                    font.weight: authorLabel.visible ? Font.Bold : Font.Normal
                    elide: Text.ElideRight
                }
                LabelWithToolTip {
                    id: authorLabel

                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    Layout.fillWidth: true

                    text: secondaryTitle
                    font.pointSize: elisaTheme.defaultFontPointSize
                    elide: Text.ElideRight

                    visible: secondaryTitle !== ""
                }
            },
            Controls1.ToolButton {
                action: enqueueAction
                objectName: 'enqueueButton'

                Keys.onReturnPressed: action.trigger()
                Accessible.onPressAction: action.trigger()

                activeFocusOnTab: true
                focus: true
            },
            Controls1.ToolButton {
                action: replaceAndPlayAction
                objectName: 'replaceAndPlayButton'

                Keys.onReturnPressed: action.trigger()
                Accessible.onPressAction: action.trigger()

                activeFocusOnTab: true
            },
            Controls1.ToolButton {
                action: showArtistAction
                objectName: 'showArtistButton'

                Keys.onReturnPressed: action.trigger()
                Accessible.onPressAction: action.trigger()

                activeFocusOnTab: true

                visible: allowArtistNavigation
            },
            Controls1.ToolButton {
                action: sortAction
                objectName: 'sortAscendingButton'

                Keys.onReturnPressed: action.trigger()
                Accessible.onPressAction: action.trigger()

                activeFocusOnTab: true

                visible: enableSorting
            },
            Controls1.ToolButton {
                action: showFilterAction
                objectName: 'showFilterButton'

                Keys.onReturnPressed: action.trigger()
                Accessible.onPressAction: action.trigger()

                activeFocusOnTab: true
            }
        ]
    }

    HeaderFooterToolbar {
        type: "header"
        id: filterRow

        visible: opacity > 0.0

        opacity: 0

        contentItems: [
            TextField {
                id: filterTextInput
                objectName: 'filterTextInput'

                Layout.fillWidth: true

                Accessible.role: Accessible.EditableText

                placeholderText: i18n("Search for album name, artist, etc.")

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

            },
            Item {
                width: elisaTheme.layoutHorizontalMargin
            },
            LabelWithToolTip {
                text: i18n("Filter by rating: ")

                visible: showRating
            },
            RatingStar {
                id: ratingFilter
                objectName: 'ratingFilter'

                visible: showRating
                hoverWidgetOpacity: 1

                readOnly: false

                starSize: elisaTheme.ratingStarSize
            }
        ]
    }

    states: [
        State {
            name: 'collapsed'
            when: !expandedFilterView
            PropertyChanges {
                target: filterRow
                opacity: 0.0
            }
        },
        State {
            name: 'expanded'
            when: expandedFilterView
            PropertyChanges {
                target: filterRow
                opacity: 1.0
            }
        }
    ]
    transitions: Transition {
        from: "expanded,collapsed"
        PropertyAnimation {
            properties: "opacity"
            easing.type: Easing.Linear
            duration: 250
        }
    }
}
