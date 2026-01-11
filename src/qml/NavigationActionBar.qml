/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2023 (c) ivan tkachenko <me@ratijas.tk>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQml
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Effects as FX
import org.kde.kirigami as Kirigami
import org.kde.elisa

Item {
    id: navigationBar

    property string mainTitle
    property string secondaryTitle
    property url image
    property bool allowArtistNavigation: false
    property bool showEnqueueButton: true
    property bool showCreateRadioButton
    property bool canToggleViewStyle: false
    property int /*ViewManager::ViewStyle*/ viewStyle: ViewManager.ListStyle

    property string labelText
    property bool showRating: !Kirigami.Settings.isMobile
    property alias filterText: filterTextInput.text
    property alias filterRating: ratingFilter.starRating
    property bool enableGoBack: true
    property bool expandedFilterView
    property bool canEnqueueView: false
    property bool enableSorting: true
    property alias sortRole: sortMenu.sortRole
    property alias sortRoles: sortMenu.sortRoles
    property alias sortRoleNames: sortMenu.sortRoleNames
    property alias sortOrder: sortMenu.sortOrder
    property alias sortOrderNames: sortMenu.sortOrderNames

    property ViewManager viewManager

    signal enqueue();
    signal replaceAndPlay();
    signal playNext();
    signal createRadio();
    signal goBack();
    signal showArtist(string name)

    property bool isWidescreen: mainWindow.width >= Theme.viewSelectorSmallSizeThreshold

    required property bool displaySingleAlbum

    implicitHeight: toolbar.height

    onViewStyleChanged: {
        if (viewManager) {
            viewManager.setViewStyle(viewStyle);
        }
    }

    SortMenu {
        id: sortMenu
        onSortOrderChanged: if (navigationBar.viewManager) navigationBar.viewManager.sortOrderChanged(sortOrder)
        onSortRoleChanged: if (navigationBar.viewManager) navigationBar.viewManager.sortRoleChanged(sortRole)
    }

    // shared actions between mobile and desktop
    Component {
        id: sortMenuComponent
        FlatButtonWithToolTip {
            function openMenu() {
                if (sortMenu.visible) {
                    sortMenu.dismiss()
                } else {
                    sortMenu.sortOrder = navigationBar.sortOrder
                    sortMenu.popup(this, x, y + height)
                }
            }

            display: AbstractButton.TextBesideIcon
            down: sortMenu.visible || pressed
            Accessible.role: Accessible.ButtonMenu

            menu: sortMenu

            icon.name: "view-sort"
            text: sortMenu.sortRoleName !== ""
                ? i18nc("@label:listbox Sort By Menu Title with no sort selected", "Sort: %1", sortMenu.sortRoleName)
                : i18nc("@label:listbox Sort By Menu Title with no sort selected", "Sort")

            onPressed: openMenu()
            Keys.onReturnPressed: openMenu()
            Keys.onEnterPressed: openMenu()
        }
    }

    Component {
        id: createRadioButton
        FlatButtonWithToolTip {
            Kirigami.Theme.colorSet: Kirigami.Settings.isMobile ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
            Kirigami.Theme.inherit: false
            objectName: "createRadioButton"
            text: i18nc("@action:button", "Add Radio")
            ToolTip.text: i18nc("@info:tooltip", "Add radio")
            icon.name: "list-add"
            display: Kirigami.Settings.isMobile && navigationBar.isWidescreen ? AbstractButton.TextBesideIcon : AbstractButton.IconOnly
            onClicked: navigationBar.createRadio()
        }
    }
    Component {
        id: enqueueButton
        FlatButtonWithToolTip {
            Kirigami.Theme.colorSet: Kirigami.Settings.isMobile ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
            Kirigami.Theme.inherit: false
            objectName: "enqueueButton"
            text: i18nc("@action:button", "Add to Playlist")
            ToolTip.text: i18nc("@info:tooltip", "Add to playlist")
            icon.name: "list-add"
            enabled: navigationBar.canEnqueueView
            display: Kirigami.Settings.isMobile && navigationBar.isWidescreen ? AbstractButton.TextBesideIcon : AbstractButton.IconOnly
            onClicked: navigationBar.enqueue()
        }
    }
    Component {
        id: replaceAndPlayButton
        FlatButtonWithToolTip {
            Kirigami.Theme.colorSet: Kirigami.Settings.isMobile ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
            Kirigami.Theme.inherit: false
            objectName: "replaceAndPlayButton"
            text: i18nc("@action:button", "Play Now, Replacing Current Playlist")
            ToolTip.text: i18nc("@info:tooltip", "Play now, replacing current playlist")
            icon.name: "media-playback-start"
            enabled: navigationBar.canEnqueueView
            display: Kirigami.Settings.isMobile && navigationBar.isWidescreen ? AbstractButton.TextBesideIcon : AbstractButton.IconOnly
            onClicked: navigationBar.replaceAndPlay()
        }
    }
    Component {
        id: playNextButton
        FlatButtonWithToolTip {
            Kirigami.Theme.colorSet: Kirigami.Settings.isMobile ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
            Kirigami.Theme.inherit: false
            text: i18nc("@action:button Add as the next track(s) in the playlist", "Play Next")
            ToolTip.text: i18nc("@info:tooltip", "Play next in playlist")
            icon.name: "media-playlist-append-next"
            display: Kirigami.Settings.isMobile && navigationBar.isWidescreen ? AbstractButton.TextBesideIcon : AbstractButton.IconOnly
            onClicked: navigationBar.playNext()
        }
    }
    Component {
        id: showArtistButton
        FlatButtonWithToolTip {
            Kirigami.Theme.colorSet: Kirigami.Settings.isMobile ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
            Kirigami.Theme.inherit: false
            objectName: "showArtistButton"
            text: i18nc("@action:button navigate to the view for artist of this album", "Display Artist")
            ToolTip.text: i18nc("@info:tooltip navigate to the view for artist of this album", "Display Artist")
            icon.name: "view-media-artist"
            display: Kirigami.Settings.isMobile && navigationBar.isWidescreen ? AbstractButton.TextBesideIcon : AbstractButton.IconOnly
            onClicked: if (navigationBar.secondaryTitle) { navigationBar.showArtist(navigationBar.secondaryTitle) }
        }
    }
    Component {
        id: showPlaylistButton
        FlatButtonWithToolTip {
            Kirigami.Theme.colorSet: Kirigami.Settings.isMobile ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
            Kirigami.Theme.inherit: false
            action: Kirigami.Action {
                fromQAction: ElisaApplication.action("toggle_playlist")
            }
            text: i18nc("@action:button", "Show Playlist")
            ToolTip.text: i18nc("@info:tooltip", "Show playlist")
            icon.name: "view-media-playlist"
            display: navigationBar.isWidescreen ? AbstractButton.TextBesideIcon : AbstractButton.IconOnly
        }
    }
    Component {
        id: toggleViewStyleButton
        FlatButtonWithToolTip {
            Kirigami.Theme.colorSet: Kirigami.Settings.isMobile ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
            Kirigami.Theme.inherit: false
            text: navigationBar.viewStyle === ViewManager.ListStyle
                ? i18nc("@action:button", "View as Icon Grid")
                : i18nc("@action:button", "View as List")
            ToolTip.text: navigationBar.viewStyle === ViewManager.ListStyle
                ? i18nc("@info:tooltip", "View as icon grid")
                : i18nc("@info:tooltip", "View as list")
            icon.name: {
                switch (navigationBar.viewStyle) {
                case ViewManager.ListStyle:
                    return "view-list-icons-symbolic";
                case ViewManager.GridStyle:
                    return "view-list-details-symbolic";
                }
            }

            onClicked: {
                switch (navigationBar.viewStyle) {
                case ViewManager.ListStyle:
                    navigationBar.viewStyle = ViewManager.GridStyle;
                    break;
                case ViewManager.GridStyle:
                    navigationBar.viewStyle = ViewManager.ListStyle;
                    break;
                }
            }
        }
    }

    ToolBar {
        id: toolbar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top

        // on mobile, the header is translucent
        Kirigami.Theme.colorSet: Kirigami.Settings.isMobile ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
        Kirigami.Theme.inherit: false

        Binding {
            toolbar.background.visible: false
            when: Kirigami.Settings.isMobile
        }

        contentItem: Column {
            spacing: Kirigami.Units.smallSpacing

            // header layout
            RowLayout {
                id: mainHeader

                width: parent.width
                spacing: Kirigami.Units.smallSpacing

                FlatButtonWithToolTip {
                    id: showSidebarButton
                    objectName: "showSidebarButton"
                    visible: Kirigami.Settings.isMobile
                    text: i18nc("@action:button", "Open sidebar")
                    icon.name: "open-menu-symbolic"
                    onClicked: mainWindow.globalDrawer.open()
                }
                FlatButtonWithToolTip {
                    id: goPreviousButton
                    objectName: "goPreviousButton"
                    visible: navigationBar.enableGoBack
                    text: i18nc("@action:button navigate back in the view's stack", "Back")
                    icon.name: (Application.layoutDirection === Qt.RightToLeft) ? "go-next" : "go-previous"
                    onClicked: navigationBar.goBack()
                }
                Kirigami.Icon {
                    id: mainIcon
                    visible: navigationBar.image.toString().length > 0
                             && !Kirigami.Settings.isMobile // On mobile, we want more header space
                             && navigationBar.enableGoBack // For top-level pages, the icon is redundant
                    source: navigationBar.image

                    Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                    Layout.preferredWidth: Kirigami.Units.iconSizes.medium
                    Layout.preferredHeight: Kirigami.Units.iconSizes.medium
                }
                ColumnLayout {
                    id: authorAndAlbumLayout
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.leftMargin: mainIcon.visible ? Kirigami.Units.smallSpacing : Kirigami.Units.largeSpacing
                    Layout.minimumHeight: Theme.toolBarHeaderMinimumHeight

                    spacing: 0

                    LabelWithToolTip {
                        id: albumLabel
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        Layout.fillWidth: true

                        text: navigationBar.mainTitle
                        level: authorLabel.visible ? 4 : 1
                    }
                    LabelWithToolTip {
                        id: authorLabel
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        Layout.fillWidth: true

                        visible: navigationBar.secondaryTitle !== ""
                        opacity: Theme.subtitleOpacity

                        text: navigationBar.secondaryTitle
                        elide: Text.ElideRight

                        MouseArea {
                            width: Math.min(parent.implicitWidth, parent.width)
                            height: parent.height
                            cursorShape: Qt.PointingHandCursor
                            onClicked: navigationBar.showArtist(navigationBar.secondaryTitle)
                        }
                    }
                }
                Loader {
                    sourceComponent: sortMenuComponent
                    active: !Kirigami.Settings.isMobile && navigationBar.enableSorting && !navigationBar.showCreateRadioButton
                }
                Loader {
                    sourceComponent: createRadioButton
                    active: !Kirigami.Settings.isMobile && navigationBar.showCreateRadioButton
                }
                Loader {
                    sourceComponent: enqueueButton
                    active: !Kirigami.Settings.isMobile && !navigationBar.showCreateRadioButton
                }
                Loader {
                    sourceComponent: replaceAndPlayButton
                    active: !Kirigami.Settings.isMobile && !navigationBar.showCreateRadioButton
                }
                Loader {
                    sourceComponent: playNextButton
                    active: !Kirigami.Settings.isMobile && !navigationBar.showCreateRadioButton
                    Layout.maximumHeight: parent.height
                }
                Loader {
                    sourceComponent: toggleViewStyleButton
                    active: !Kirigami.Settings.isMobile && navigationBar.canToggleViewStyle
                }
                FlatButtonWithToolTip {
                    Kirigami.Theme.colorSet: Kirigami.Settings.isMobile ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
                    Kirigami.Theme.inherit: false
                    objectName: "showFilterButton"
                    text: !navigationBar.expandedFilterView
                    ? i18nc("@action:button displayed as @info:tooltip Show filters in the navigation bar", "Show search and filter toolbar")
                    : i18nc("@action:button displayed as @info:tooltip Hide filters in the navigation bar", "Hide search and filter toolbar")
                    icon.name: "search"
                    checkable: true
                    checked: navigationBar.expandedFilterView
                    onClicked: persistentSettings.expandedFilterView = !persistentSettings.expandedFilterView;
                }
            }

            // on mobile, move header buttons into a second row (there's limited horizontal space for track names and etc.)
            Loader {
                active: Kirigami.Settings.isMobile
                visible: active

                width: parent.width

                sourceComponent: RowLayout {
                    id: mobileActions

                    spacing: Kirigami.Units.smallSpacing

                    Loader {
                        sourceComponent: sortMenuComponent
                        active: navigationBar.enableSorting && !navigationBar.showCreateRadioButton
                        Layout.maximumHeight: parent.height
                    }
                    Item {
                        Layout.fillWidth: true
                    }
                    Loader {
                        sourceComponent: createRadioButton
                        active: navigationBar.showCreateRadioButton
                    }
                    Loader {
                        sourceComponent: enqueueButton
                        active: !navigationBar.showCreateRadioButton
                    }
                    Loader {
                        sourceComponent: replaceAndPlayButton
                        active: !navigationBar.showCreateRadioButton
                    }
                    Loader {
                        sourceComponent: playNextButton
                        active: !navigationBar.showCreateRadioButton
                        Layout.maximumHeight: parent.height
                    }
                    Loader {
                        sourceComponent: showArtistButton
                        active: navigationBar.allowArtistNavigation && !navigationBar.showCreateRadioButton
                    }
                    Loader {
                        sourceComponent: toggleViewStyleButton
                        active: navigationBar.canToggleViewStyle
                    }
                    Loader {
                        sourceComponent: showPlaylistButton
                    }
                }
            }

            // filter bar
            RowLayout {
                id: filterRow

                width: parent.width
                visible: opacity > 0.0

                opacity: navigationBar.expandedFilterView ? 1 : 0
                Behavior on opacity {
                    NumberAnimation {
                        easing.type: Easing.Linear
                        duration: Kirigami.Units.longDuration
                    }
                }
                onVisibleChanged: {
                    if (visible) {
                        filterTextInput.forceActiveFocus();
                    }
                }

                spacing: Kirigami.Units.smallSpacing

                Kirigami.SearchField {
                    id: filterTextInput
                    objectName: "filterTextInput"

                    Layout.fillWidth: true
                    focusSequence: ""

                    selectByMouse: true

                    Accessible.role: Accessible.EditableText

                    placeholderText: navigationBar.showCreateRadioButton
                        ? i18nc("@info:placeholder radio view filter", "Search for radio station")
                        : (navigationBar.displaySingleAlbum
                            ? i18nc("@info:placeholder track view filter single album", "Search for track name, artist, etc.")
                            : i18nc("@info:placeholder track view filter general", "Search for album name, artist, etc."))

                    Keys.onEscapePressed: persistentSettings.expandedFilterView = false;
                }
                Item {
                    implicitWidth: Kirigami.Units.largeSpacing
                    visible: navigationBar.showRating && !ElisaApplication.useFavoriteStyleRatings
                }
                LabelWithToolTip {
                    text: i18nc("@label:chooser", "Filter by rating: ")

                    visible: navigationBar.showRating && !ElisaApplication.useFavoriteStyleRatings
                }
                RatingStar {
                    id: ratingFilter
                    objectName: "ratingFilter"

                    visible: navigationBar.showRating && !ElisaApplication.useFavoriteStyleRatings

                    readOnly: false
                }
                FlatButtonWithToolTip {
                    visible: navigationBar.showRating && ElisaApplication.useFavoriteStyleRatings

                    flat: false
                    display: AbstractButton.TextBesideIcon

                    text: i18nc("@action:button", "Only Show Favorites")
                    icon.name: "starred"
                    checkable: true
                    checked: ratingFilter.starRating === 10
                    onToggled: {
                        if (checked) {
                            ratingFilter.starRating = 10;
                        } else {
                            ratingFilter.starRating = 0;
                        }
                    }
                }
            }
        }
    }

    // mobile blurred background image
    Loader {
        active: Kirigami.Settings.isMobile
        visible: active

        anchors.fill: toolbar
        z: -1

        sourceComponent: Item {
            // darken background
            Rectangle {
                anchors.fill: parent
                z: -1

                color: "black"
                opacity: 0.8
            }

            ImageWithFallback {
                anchors.fill: parent

                source: ElisaApplication.manageHeaderBar.image
                fallback: Theme.defaultBackgroundImage
                asynchronous: true

                sourceSize.width: Screen.width
                fillMode: Image.PreserveAspectCrop

                layer.enabled: GraphicsInfo.api !== GraphicsInfo.Software
                layer.effect: FX.MultiEffect {
                    autoPaddingEnabled: false
                    blurEnabled: true
                    blur: 1
                    blurMax: 64
                    blurMultiplier: 2

                    brightness: -0.3
                }
            }

            Rectangle {
                visible: GraphicsInfo.api === GraphicsInfo.Software
                anchors.fill: parent
                color: "black"
                opacity: 0.8
            }

            Rectangle {
                anchors.fill: parent
                z: 1

                color: palette.window
                opacity: 0.3
            }
        }
    }
}
