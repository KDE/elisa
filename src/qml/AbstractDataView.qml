/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2022 (c) Nate Graham <nate@kde.org>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQml.Models
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.elisa

FocusScope {
    id: abstractView

    // Subclasses must set these
    property alias delegate: delegateModel.delegate
    property alias contentView: scrollView.contentItem

    // Instances should set these as needed
    property AbstractItemModel realModel
    property AbstractProxyModel proxyModel
    property var modelType
    property var filterType
    property var filter
    property string mainTitle
    property string secondaryTitle
    property url image
    property bool sortModel: true
    property bool isSubPage: false
    property bool modelIsInitialized: false
    property bool allowArtistNavigation: false
    property bool showCreateRadioButton: false
    property bool showEnqueueButton: false
    property bool displaySingleAlbum: false
    property alias showRating: navigationBar.showRating
    property alias sortRole: navigationBar.sortRole
    property alias sortRoles: navigationBar.sortRoles
    property alias sortRoleNames: navigationBar.sortRoleNames
    property alias sortOrderNames: navigationBar.sortOrderNames
    property alias sortOrder: navigationBar.sortOrder
    property alias viewManager: navigationBar.viewManager
    property alias expandedFilterView: navigationBar.expandedFilterView
    property alias canToggleViewStyle: navigationBar.canToggleViewStyle
    property alias viewStyle: navigationBar.viewStyle

    // Inner items exposed to subclasses for various purposes
    readonly property alias delegateModel: delegateModel
    readonly property alias navigationBar: navigationBar

    // Other properties
    property AbstractProxyModel contentModel
    property int depth: 1

    focus: true

    Accessible.role: Accessible.Pane
    Accessible.name: mainTitle

    function initializeModel() {
        if (!proxyModel) {
            return
        }

        if (!realModel) {
            return
        }

        if (!ElisaApplication.musicManager) {
            return
        }

        if (modelIsInitialized) {
            return
        }

        proxyModel.sourceModel = realModel
        proxyModel.dataType = modelType
        proxyModel.playList = Qt.binding(() => ElisaApplication.mediaPlayListProxyModel)
        abstractView.contentModel = proxyModel

        if (sortModel) {
            proxyModel.sortModel(sortOrder)
        }

        realModel.initializeByData(ElisaApplication.musicManager, ElisaApplication.musicManager.viewDatabase,
                                   modelType, filterType, filter)

        modelIsInitialized = true
    }

    // Model
    DelegateModel {
        id: delegateModel
        model: abstractView.contentModel
    }

    // Main view components
    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        NavigationActionBar {
            id: navigationBar

            z: 1 // on top of track list

            mainTitle: abstractView.mainTitle
            secondaryTitle: abstractView.secondaryTitle
            image: abstractView.image
            enableSorting: abstractView.sortModel
            enableGoBack: abstractView.isSubPage || abstractView.depth > 1
            canEnqueueView: delegateModel.count > 0
            allowArtistNavigation: abstractView.isSubPage 
                && abstractView.modelType === ElisaUtils.Track
                && abstractView.displaySingleAlbum
            showCreateRadioButton: abstractView.modelType === ElisaUtils.Radio
            showEnqueueButton: abstractView.modelType !== ElisaUtils.Radio
            displaySingleAlbum: abstractView.displaySingleAlbum

            Layout.fillWidth: true

            Binding {
                target: abstractView.contentModel
                property: "filterText"
                when: abstractView.contentModel
                value: navigationBar.filterText
            }

            Binding {
                target: abstractView.contentModel
                property: "filterRating"
                when: abstractView.contentModel
                value: navigationBar.filterRating
            }

            Binding {
                target: abstractView.contentModel
                property: "sortRole"
                when: abstractView.contentModel && navigationBar.enableSorting
                value: navigationBar.sortRole
            }

            onEnqueue: abstractView.contentModel.enqueueAll(ElisaUtils.AppendPlayList, ElisaUtils.DoNotTriggerPlay)

            onReplaceAndPlay: abstractView.contentModel.enqueueAll(ElisaUtils.ReplacePlayList, ElisaUtils.TriggerPlay)

            onPlayNext: abstractView.contentModel.enqueueAll(ElisaUtils.AfterCurrentTrack, ElisaUtils.DoNotTriggerPlay)

            onGoBack: abstractView.viewManager.goBack()

            onSortOrderChanged: {
                if (!abstractView.contentModel || !navigationBar.enableSorting) {
                    return
                }

                if ((abstractView.contentModel.sortedAscending && sortOrder !== Qt.AscendingOrder) ||
                    (!abstractView.contentModel.sortedAscending && sortOrder !== Qt.DescendingOrder)) {
                    abstractView.contentModel.sortModel(sortOrder)
                }
            }
        }

        ScrollView {
            id: scrollView

            Layout.fillHeight: true
            Layout.fillWidth: true

            // HACK: workaround for https://bugreports.qt.io/browse/QTBUG-83890
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            // Content view goes here, set as the contentItem: property
        }
    }

    // Loading spinner
    Loader {
        id: busyIndicatorLoader
        anchors.centerIn: parent
        height: Kirigami.Units.gridUnit * 5
        width: height

        active: abstractView.realModel ? abstractView.realModel.isBusy : true
        visible: active && status === Loader.Ready

        sourceComponent: BusyIndicator {
            anchors.centerIn: parent
        }
    }

    // "Nothing here" placeholder message
    Loader {
        anchors.centerIn: parent
        anchors.verticalCenterOffset: Math.round(navigationBar.height / 2)
        width: parent.width - (Kirigami.Units.largeSpacing * 4)
        active: delegateModel.count === 0 && !busyIndicatorLoader.active
        visible: active && status === Loader.Ready

        sourceComponent: Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            icon.name: "edit-none"
            text: navigationBar.filterText.length > 0 ? i18nc("@info:placeholder", "No matches") : i18nc("@info:placeholder", "Nothing to display")
        }
    }

    Connections {
        target: ElisaApplication

        function onMusicManagerChanged() {
            abstractView.initializeModel()
        }
    }

    Component.onCompleted: {
        initializeModel()
    }
}
