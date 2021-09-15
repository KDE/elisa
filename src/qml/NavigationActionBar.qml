/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQml 2.2
import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.8 as Kirigami
import org.kde.elisa 1.0

Item {
    id: navigationBar

    property string mainTitle
    property string secondaryTitle
    property url image
    property bool allowArtistNavigation: false
    property bool showEnqueueButton: true
    property bool showCreateRadioButton

    property string labelText
    property bool showRating: !Kirigami.Settings.isMobile
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

    property bool isWidescreen: mainWindow.width >= elisaTheme.viewSelectorSmallSizeThreshold

    implicitHeight: layout.height

    SortMenu {
        id: sortMenu
        onSortOrderChanged: if (viewManager) viewManager.sortOrderChanged(sortOrder)
        onSortRoleChanged: if (viewManager) viewManager.sortRoleChanged(sortRole)
    }

    // shared actions between mobile and desktop
    Component {
        id: sortMenuButton
        FlatButtonWithToolTip {
            Kirigami.Theme.colorSet: Kirigami.Settings.isMobile ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
            Kirigami.Theme.inherit: false
            objectName: 'sortMenuButton'
            display: AbstractButton.TextOnly
            onClicked: {
                sortMenu.sortOrder = navigationBar.sortOrder
                sortMenu.popup(parent)
            }

            // Custom content item for now to replicate the look of a button
            // that opens a dropdown menu
            // TODO: Port more fully to Kirigami so we get this for free
            contentItem: RowLayout {
                spacing: Kirigami.Units.largeSpacing

                Label {
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    text: if (sortMenu.sortRoleName !== "") {
                        i18nc("Sort By Menu Title with no sort selected", "Sort: %1", sortMenu.sortRoleName)
                    } else {
                        i18nc("Sort By Menu Title with no sort selected", "Sort")
                    }
                }
                Kirigami.Icon {
                    Layout.rightMargin: Kirigami.Units.largeSpacing
                    implicitWidth: Kirigami.Units.iconSizes.small
                    implicitHeight: Kirigami.Units.iconSizes.small
                    source: 'arrow-down'
                    isMask: true
                }
            }
        }
    }

    Component {
        id: createRadioButton
        FlatButtonWithToolTip {
            Kirigami.Theme.colorSet: Kirigami.Settings.isMobile ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
            Kirigami.Theme.inherit: false
            objectName: 'createRadioButton'
            text: i18nc("Create a new radio", "Create a radio")
            icon.name: "list-add"
            display: Kirigami.Settings.isMobile && navigationBar.isWidescreen ? AbstractButton.TextBesideIcon : AbstractButton.IconOnly
            onClicked: createRadio()
        }
    }
    Component {
        id: enqueueButton
        FlatButtonWithToolTip {
            Kirigami.Theme.colorSet: Kirigami.Settings.isMobile ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
            Kirigami.Theme.inherit: false
            objectName: 'enqueueButton'
            text: i18nc("Add current list to playlist", "Enqueue")
            icon.name: "list-add"
            display: Kirigami.Settings.isMobile && navigationBar.isWidescreen ? AbstractButton.TextBesideIcon : AbstractButton.IconOnly
            onClicked: enqueue()
        }
    }
    Component {
        id: replaceAndPlayButton
        FlatButtonWithToolTip {
            Kirigami.Theme.colorSet: Kirigami.Settings.isMobile ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
            Kirigami.Theme.inherit: false
            objectName: 'replaceAndPlayButton'
            text: i18n("Play")
            icon.name: "media-playback-start"
            display: Kirigami.Settings.isMobile && navigationBar.isWidescreen ? AbstractButton.TextBesideIcon : AbstractButton.IconOnly
            onClicked: replaceAndPlay()
        }
    }
    Component {
        id: showArtistButton
        FlatButtonWithToolTip {
            Kirigami.Theme.colorSet: Kirigami.Settings.isMobile ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
            Kirigami.Theme.inherit: false
            objectName: 'showArtistButton'
            text: i18nc("Button to navigate to the artist of the album", "Display Artist")
            icon.name: "view-media-artist"
            display: Kirigami.Settings.isMobile && navigationBar.isWidescreen ? AbstractButton.TextBesideIcon : AbstractButton.IconOnly
            onClicked: showArtist()
        }
    }
    Component {
        id: showPlaylistButton
        FlatButtonWithToolTip {
            Kirigami.Theme.colorSet: Kirigami.Settings.isMobile ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
            Kirigami.Theme.inherit: false
            id: showPlaylistButton
            text: i18nc("show the playlist", "Show Playlist")
            icon.name: "view-media-playlist"
            display: navigationBar.isWidescreen ? AbstractButton.TextBesideIcon : AbstractButton.IconOnly
            onClicked: {
                if (navigationBar.isWidescreen) {
                    contentView.showPlaylist = !contentView.showPlaylist;
                } else {
                    playlistDrawer.open();
                }
            }
        }
    }

    // header layout
    ColumnLayout {
        id: layout
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        spacing: 0

        HeaderFooterToolbar {
            id: mainHeader
            toolbarType: Kirigami.Settings.isMobile || filterRow.visible ? HeaderFooterToolbar.ToolbarType.Other
                                                                        : HeaderFooterToolbar.ToolbarType.Header
            Layout.fillWidth: true

            // on mobile, the header is translucent
            color: Kirigami.Settings.isMobile ? Qt.rgba(myPalette.window.r, myPalette.window.g, myPalette.window.b, 0.3) : myPalette.window
            Kirigami.Theme.colorSet: Kirigami.Settings.isMobile ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
            Kirigami.Theme.inherit: false

            contentItems: [
                FlatButtonWithToolTip {
                    id: showSidebarButton
                    objectName: 'showSidebarButton'
                    visible: Kirigami.Settings.isMobile
                    text: i18nc("open the sidebar", "Open sidebar")
                    icon.name: "application-menu"
                    onClicked: mainWindow.globalDrawer.open()
                },
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
                    visible: image.toString().length > 0 && !Kirigami.Settings.isMobile // on mobile, we want more header space
                    source: image

                    Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                    Layout.preferredWidth: Kirigami.Units.iconSizes.medium
                    Layout.preferredHeight: Kirigami.Units.iconSizes.medium
                    sourceSize.height: Layout.preferredHeight
                    sourceSize.width: Layout.preferredWidth

                    fillMode: Image.PreserveAspectFit
                    asynchronous: true

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
                    }
                    LabelWithToolTip {
                        id: authorLabel
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        Layout.fillWidth: true

                        visible: secondaryTitle !== ""
                        opacity: 0.6

                        text: secondaryTitle
                        elide: Text.ElideRight

                    }
                },
                Loader {
                    sourceComponent: sortMenuButton
                    active: !Kirigami.Settings.isMobile && enableSorting && !showCreateRadioButton
                    Layout.maximumHeight: parent.height
                    Layout.preferredWidth: item ? item.implicitContentWidth : 0
                },
                Loader {
                    sourceComponent: createRadioButton
                    active: !Kirigami.Settings.isMobile && showCreateRadioButton
                    Layout.maximumHeight: parent.height
                },
                Loader {
                    sourceComponent: enqueueButton
                    active: !Kirigami.Settings.isMobile && !showCreateRadioButton
                    Layout.maximumHeight: parent.height
                },
                Loader {
                    sourceComponent: replaceAndPlayButton
                    active: !Kirigami.Settings.isMobile && !showCreateRadioButton
                    Layout.maximumHeight: parent.height
                },
                Loader {
                    sourceComponent: showArtistButton
                    active: !Kirigami.Settings.isMobile && allowArtistNavigation && !showCreateRadioButton
                    Layout.maximumHeight: parent.height
                },
                FlatButtonWithToolTip {
                    Kirigami.Theme.colorSet: Kirigami.Settings.isMobile ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
                    Kirigami.Theme.inherit: false
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

        // on mobile, move header buttons into a second row (there's limited horizontal space for track names and etc.)
        Loader {
            active: Kirigami.Settings.isMobile
            visible: active

            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.gridUnit * 2

            sourceComponent: HeaderFooterToolbar {
                id: mobileActions
                toolbarType: Kirigami.Settings.isMobile || filterRow.visible ? HeaderFooterToolbar.ToolbarType.Other : HeaderFooterToolbar.ToolbarType.Header

                // on mobile, the header is translucent
                color: Kirigami.Settings.isMobile ? Qt.rgba(myPalette.window.r, myPalette.window.g, myPalette.window.b, 0.3) : myPalette.window
                Kirigami.Theme.colorSet: Kirigami.Settings.isMobile ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
                Kirigami.Theme.inherit: false

                contentItems: [
                    Loader {
                        sourceComponent: sortMenuButton
                        active: enableSorting && !showCreateRadioButton
                        Layout.maximumHeight: parent.height
                        Layout.preferredWidth: item ? item.implicitContentWidth : 0
                    },
                    Item {
                        Layout.fillWidth: true
                    },
                    Loader {
                        sourceComponent: createRadioButton
                        active: showCreateRadioButton
                        Layout.maximumHeight: parent.height
                    },
                    Loader {
                        sourceComponent: enqueueButton
                        active: !showCreateRadioButton
                        Layout.maximumHeight: parent.height
                    },
                    Loader {
                        sourceComponent: replaceAndPlayButton
                        active: !showCreateRadioButton
                        Layout.maximumHeight: parent.height
                    },
                    Loader {
                        sourceComponent: showArtistButton
                        active: allowArtistNavigation && !showCreateRadioButton
                        Layout.maximumHeight: parent.height
                    },
                    Loader {
                        sourceComponent: showPlaylistButton
                        Layout.maximumHeight: parent.height
                    }
                ]
            }
        }

        // filter bar
        HeaderFooterToolbar {
            id: filterRow
            toolbarType: Kirigami.Settings.isMobile ? HeaderFooterToolbar.ToolbarType.Other : HeaderFooterToolbar.ToolbarType.Header

            color: Kirigami.Settings.isMobile ? Qt.rgba(myPalette.window.r, myPalette.window.g, myPalette.window.b, 0.3) : myPalette.window
            Kirigami.Theme.colorSet: Kirigami.Settings.isMobile ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
            Kirigami.Theme.inherit: false

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
                    visible: showRating
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
    }

    // darken background
    Rectangle {
        anchors.fill: background
        visible: Kirigami.Settings.isMobile

        z: -1
        color: "black"
        opacity: 0.8
    }

    // mobile blurred background image
    Loader {
        id: background
        active: Kirigami.Settings.isMobile
        visible: active

        anchors.fill: layout
        z: -1

        sourceComponent: ImageWithFallback {
            source: ElisaApplication.manageHeaderBar.image
            fallback: elisaTheme.defaultBackgroundImage
            asynchronous: true

            fillMode: Image.PreserveAspectCrop

            // make the FastBlur effect more strong
            sourceSize.height: 10

            layer.enabled: true
            layer.effect: HueSaturation {
                cached: true

                lightness: -0.5
                saturation: 0.9

                layer.enabled: true
                layer.effect: FastBlur {
                    cached: true
                    radius: 64
                    transparentBorder: false
                }
            }
        }
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
