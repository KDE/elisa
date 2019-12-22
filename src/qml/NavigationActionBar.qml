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
import QtQuick.Controls 2.3
import org.kde.kirigami 2.8 as Kirigami

ColumnLayout {
    id: navigationBar

    spacing: 0

    property string mainTitle
    property string secondaryTitle
    property url image
    property bool allowArtistNavigation: false
    property bool showEnqueueButton: true
    property bool showCreateRadioButton

    property string labelText
    property bool showRating: true
    property alias filterText: filterTextInput.text
    property alias filterRating: ratingFilter.starRating
    property bool enableGoBack: true
    property bool expandedFilterView
    property bool enableSorting: true
    property bool sortOrder


    signal enqueue();
    signal replaceAndPlay();
    signal createRadio();
    signal goBack();
    signal showArtist();
    signal sort(var order);

    Action {
        id: goPreviousAction
        text: i18nc("navigate back in the views stack", "Back")
        icon.name: (Qt.application.layoutDirection == Qt.RightToLeft) ? "go-next" : "go-previous"
        onTriggered: goBack()
        enabled: enableGoBack
    }

    Action {
        id: replaceAndPlayAction
        text: i18n("Play now, replacing contents of Playlist")
        icon.name: "media-playback-start"
        onTriggered: replaceAndPlay()
    }

    Action {
        id: enqueueAction
        text: i18nc("Add current list to playlist", "Enqueue")
        icon.name: "list-add"
        onTriggered: enqueue()
    }

    Action {
        id: createRadioAction
        text: i18nc("Create a new radio", "Create a radio")
        icon.name: "media-track-add-amarok"
        onTriggered: createRadio()
    }

    Action {
        id: showFilterAction
        text: !navigationBar.expandedFilterView ? i18nc("Show filters in the navigation bar", "Show Search Options") : i18nc("Hide filters in the navigation bar", "Hide Search Options")
        icon.name: 'search'
        checkable: true
        checked: expandedFilterView
        onTriggered: persistentSettings.expandedFilterView = !persistentSettings.expandedFilterView
    }

    Action {
        id: sortAction
        text: i18nc("Toggle between ascending and descending order", "Toggle sort order")
        icon.name: sortOrder ? "view-sort-ascending" : "view-sort-descending"
        onTriggered: sortOrder ? sort(Qt.DescendingOrder) : sort(Qt.AscendingOrder)
    }

    Action {
        id: showArtistAction
        text: i18nc("Button to navigate to the artist of the album", "Display Artist")
        icon.name: "view-media-artist"
        onTriggered: showArtist()
    }

    HeaderFooterToolbar {
        type: filterRow.visible? "other" : "header"
        contentItems: [

            FlatButtonWithToolTip {
                action: goPreviousAction
                id: goPreviousButton
                objectName: 'goPreviousButton'

                icon.height: elisaTheme.smallControlButtonSize
                icon.width: elisaTheme.smallControlButtonSize

                focus: enableGoBack
                visible: enableGoBack
            },
            Item {
                id: spacer
                Layout.preferredWidth: elisaTheme.layoutHorizontalMargin
                visible: goPreviousButton.visible
            },
            Image {
                id: mainIcon
                source: image

                Layout.preferredHeight: authorAndAlbumLayout.height
                Layout.preferredWidth: height
                sourceSize.height: height
                sourceSize.width: width
                fillMode: Image.PreserveAspectFit
                asynchronous: true

                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
            },
            Item {
                Layout.preferredWidth: elisaTheme.layoutHorizontalMargin
                visible: mainIcon.visible
            },
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                id: authorAndAlbumLayout

                LabelWithToolTip {
                    id: albumLabel
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    Layout.fillWidth: true

                    text: mainTitle
                    level: authorLabel.visible ? 4 : 1
                    font.weight: authorLabel.visible ? Font.Bold : Font.Normal
                    elide: Text.ElideRight
                }
                LabelWithToolTip {
                    id: authorLabel

                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    Layout.fillWidth: true

                    text: secondaryTitle
                    elide: Text.ElideRight

                    visible: secondaryTitle !== ""
                }
            },
            FlatButtonWithToolTip {
                action: createRadioAction
                objectName: 'createRadioButton'

                icon.height: elisaTheme.smallControlButtonSize
                icon.width: elisaTheme.smallControlButtonSize

                focus: true

                visible: showCreateRadioButton
            },
            FlatButtonWithToolTip {
                action: enqueueAction
                objectName: 'enqueueButton'

                icon.height: elisaTheme.smallControlButtonSize
                icon.width: elisaTheme.smallControlButtonSize

                focus: true

                visible: !showCreateRadioButton
            },
            FlatButtonWithToolTip {
                action: replaceAndPlayAction
                objectName: 'replaceAndPlayButton'

                icon.height: elisaTheme.smallControlButtonSize
                icon.width: elisaTheme.smallControlButtonSize

                visible: !showCreateRadioButton
            },
            FlatButtonWithToolTip {
                action: showArtistAction
                objectName: 'showArtistButton'

                icon.height: elisaTheme.smallControlButtonSize
                icon.width: elisaTheme.smallControlButtonSize

                visible: allowArtistNavigation && !showCreateRadioButton
            },
            FlatButtonWithToolTip {
                action: sortAction
                objectName: 'sortAscendingButton'

                icon.height: elisaTheme.smallControlButtonSize
                icon.width: elisaTheme.smallControlButtonSize

                visible: enableSorting && !showCreateRadioButton
            },
            FlatButtonWithToolTip {
                action: showFilterAction
                objectName: 'showFilterButton'

                icon.height: elisaTheme.smallControlButtonSize
                icon.width: elisaTheme.smallControlButtonSize

                visible: !showCreateRadioButton
            }
        ]
    }

    HeaderFooterToolbar {
        type: "header"
        id: filterRow

        visible: opacity > 0.0

        opacity: 0

        contentItems: [
            Kirigami.SearchField {
                id: filterTextInput
                objectName: 'filterTextInput'

                Layout.fillWidth: true
                focusSequence: null

                Accessible.role: Accessible.EditableText

                placeholderText: i18n("Search for album name, artist, etc.")
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
            StateChangeScript {
                script: filterTextInput.forceActiveFocus()
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
