/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0

FocusScope {
    id: viewHeader

    property var filterType
    property alias mainTitle: gridView.mainTitle
    property alias secondaryTitle: gridView.secondaryTitle
    property alias image: gridView.image
    property var modelType
    property AbstractItemModel realModel
    property AbstractProxyModel proxyModel
    property alias defaultIcon: gridView.defaultIcon
    property alias showRating: gridView.showRating
    property alias delegateDisplaySecondaryText: gridView.delegateDisplaySecondaryText
    property alias isSubPage: gridView.isSubPage
    property alias expandedFilterView: gridView.expandedFilterView
    property alias haveTreeModel: gridView.haveTreeModel
    property var filter
    property alias sortRole: gridView.sortRole
    property alias sortRoles: gridView.sortRoles
    property alias sortRoleNames: gridView.sortRoleNames
    property alias sortOrderNames: gridView.sortOrderNames
    property alias sortOrder: gridView.sortOrder
    property bool modelIsInitialized: false
    property alias viewManager: gridView.viewManager

    focus: true

    Accessible.role: Accessible.Pane
    Accessible.name: mainTitle

    function initializeModel()
    {
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
        proxyModel.playList = Qt.binding(function() { return ElisaApplication.mediaPlayListProxyModel })
        gridView.contentModel = proxyModel

        proxyModel.sortModel(sortOrder)

        realModel.initializeByData(ElisaApplication.musicManager, ElisaApplication.musicManager.viewDatabase,
                                   modelType, filterType, filter)

        modelIsInitialized = true
    }

    function goToBack() {
        gridView.goToBack()
    }

    GridBrowserView {
        id: gridView

        focus: true

        anchors.fill: parent

        onEnqueue: proxyModel.enqueue(fullData, name,
                                      ElisaUtils.AppendPlayList,
                                      ElisaUtils.DoNotTriggerPlay)

        onReplaceAndPlay: proxyModel.enqueue(fullData, name,
                                             ElisaUtils.ReplacePlayList,
                                             ElisaUtils.TriggerPlay)

        onOpen: viewManager.openChildView(fullData)

        suppressNoDataPlaceholderMessage: busyIndicatorLoader.active

        onGoBackRequested: {
            viewManager.goBack()
        }

        Loader {
            id: busyIndicatorLoader
            anchors.centerIn: parent
            height: Kirigami.Units.gridUnit * 5
            width: height

            visible: realModel ? realModel.isBusy : true
            active: realModel ? realModel.isBusy : true

            sourceComponent: BusyIndicator {
                anchors.centerIn: parent
            }
        }
    }

    Connections {
        target: ElisaApplication

        function onMusicManagerChanged() {
            initializeModel()
        }
    }

    Component.onCompleted: {
        initializeModel()
    }
}
