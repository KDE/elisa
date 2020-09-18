/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQml 2.2
import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3
import org.kde.kirigami 2.8 as Kirigami
import org.kde.elisa 1.0

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
    property alias sortRole: sortMenu.sortRole
    property alias sortRoles: sortMenu.sortRoles
    property alias sortRoleNames: sortMenu.sortRoleNames
    property alias sortOrder: sortMenu.sortOrder
    property alias sortOrderNames: sortMenu.sortOrderNames

    property ViewManager viewManager

    signal enqueue();
    signal replaceAndPlay();
    signal createRadio();
    signal goBack();
    signal showArtist();

    HeaderFooterToolbar {
        toolbarType: filterRow.visible ? HeaderFooterToolbar.ToolbarType.Other
                                       : HeaderFooterToolbar.ToolbarType.Header
        Layout.fillWidth: true
        contentItems: [

            FlatButtonWithToolTip {
                id: goPreviousButton
                objectName: 'goPreviousButton'
                visible: enableGoBack
                text: i18nc("navigate back in the views stack", "Back")
                icon.name: (Qt.application.layoutDirection == Qt.RightToLeft) ? "go-next" : "go-previous"
                onClicked: goBack()
            },
            Image {
                id: mainIcon
                visible: image.toString().length > 0
                source: image

                Layout.fillHeight: true
                Layout.topMargin: Kirigami.Units.smallSpacing
                Layout.bottomMargin: Kirigami.Units.smallSpacing
                Layout.preferredWidth: height
                sourceSize.height: height
                sourceSize.width: width
                fillMode: Image.PreserveAspectFit
                asynchronous: true

                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
            },
            Item {
                Layout.preferredWidth: Kirigami.Units.smallSpacing
                visible: mainIcon.visible
            },
            ColumnLayout {
                id: authorAndAlbumLayout
                Layout.fillWidth: true
                Layout.fillHeight: true

                spacing: 0

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
                id: sortMenuButton

                objectName: 'sortMenuButton'
                visible: enableSorting && !showCreateRadioButton
                text: if (sortMenu.sortRoleName !== "") {
                          i18nc("Sort By Menu Title with no sort selected", "Sort: %1", sortMenu.sortRoleName)
                      } else {
                          i18nc("Sort By Menu Title with no sort selected", "Sort")
                      }
                display: AbstractButton.TextOnly
                icon.name: "view-sort"
                onClicked: {
                    sortMenu.sortOrder = navigationBar.sortOrder
                    sortMenu.open()
                }

                indicator: Image {
                     source: 'image://icon/arrow-down'
                     width: Kirigami.Units.iconSizes.small
                     height: Kirigami.Units.iconSizes.small
                     sourceSize.width: width
                     sourceSize.height: height

                     anchors.right: sortMenuButton.right
                     anchors.verticalCenter: sortMenuButton.verticalCenter
                     anchors.rightMargin: Kirigami.Units.largeSpacing
                }

                Layout.preferredWidth: textSize.width + implicitIndicatorWidth + Kirigami.Units.largeSpacing * 2

                SortMenu {
                    id: sortMenu

                    onSortOrderChanged: if (viewManager) viewManager.sortOrderChanged(sortOrder)

                    onSortRoleChanged: if (viewManager) viewManager.sortRoleChanged(sortRole)
                }

                TextMetrics {
                    id: textSize

                    text: sortMenuButton.text
                    font: sortMenuButton.font
                }
            },
            FlatButtonWithToolTip {
                objectName: 'createRadioButton'
                visible: showCreateRadioButton
                text: i18nc("Create a new radio", "Create a radio")
                icon.name: "list-add"
                onClicked: createRadio()
            },
            FlatButtonWithToolTip {
                objectName: 'enqueueButton'
                visible: !showCreateRadioButton
                text: i18nc("Add current list to playlist", "Enqueue")
                icon.name: "list-add"
                onClicked: enqueue()
            },
            FlatButtonWithToolTip {
                objectName: 'replaceAndPlayButton'
                visible: !showCreateRadioButton
                text: i18n("Play now, replacing contents of Playlist")
                icon.name: "media-playback-start"
                onClicked: replaceAndPlay()
            },
            FlatButtonWithToolTip {
                objectName: 'showArtistButton'
                visible: allowArtistNavigation && !showCreateRadioButton
                text: i18nc("Button to navigate to the artist of the album", "Display Artist")
                icon.name: "view-media-artist"
                onClicked: showArtist()
            },
            FlatButtonWithToolTip {
                objectName: 'showFilterButton'
                visible: !showCreateRadioButton
                text: !navigationBar.expandedFilterView ? i18nc("Show filters in the navigation bar", "Show Search Options") : i18nc("Hide filters in the navigation bar", "Hide Search Options")
                icon.name: 'search'
                checkable: true
                checked: expandedFilterView
                onClicked: persistentSettings.expandedFilterView = !persistentSettings.expandedFilterView
            }
        ]
    }

    HeaderFooterToolbar {
        id: filterRow
        toolbarType: HeaderFooterToolbar.ToolbarType.Header
        Layout.fillWidth: true

        visible: opacity > 0.0

        opacity: 0

        contentItems: [
            Kirigami.SearchField {
                id: filterTextInput
                objectName: 'filterTextInput'

                Layout.fillWidth: true
                focusSequence: ""

                selectByMouse: true

                Accessible.role: Accessible.EditableText

                placeholderText: i18n("Search for album name, artist, etc.")

                Keys.onEscapePressed: persistentSettings.expandedFilterView = false;
            },
            Item {
                width: Kirigami.Units.largeSpacing
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
            StateChangeScript {
                // Focus main content view since that's probably what the user
                // wants to interact with next
                script: contentDirectoryView.forceActiveFocus();
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
            duration: Kirigami.Units.longDuration
        }
    }
}
