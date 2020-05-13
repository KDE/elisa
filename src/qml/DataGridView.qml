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
    property string genreFilterText
    property string artistFilter
    property url pathFilter
    property bool modelIsInitialized: false

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

        if (!elisa.musicManager) {
            return
        }

        if (modelIsInitialized) {
            return
        }

        proxyModel.sourceModel = realModel
        proxyModel.dataType = modelType
        proxyModel.playList = elisa.mediaPlayListProxyModel
        gridView.contentModel = proxyModel

        realModel.initialize(elisa.musicManager, elisa.musicManager.viewDatabase,
                             modelType, filterType, genreFilterText, artistFilter, 0,
                             pathFilter)

        modelIsInitialized = true
    }

    function goToBack() {
        gridView.goToBack()
    }

    GridBrowserView {
        id: gridView

        focus: true

        anchors.fill: parent

        onEnqueue: elisa.mediaPlayListProxyModel.enqueue(fullData, name,
                                               ElisaUtils.AppendPlayList,
                                               ElisaUtils.DoNotTriggerPlay)

        onReplaceAndPlay: elisa.mediaPlayListProxyModel.enqueue(fullData, name,
                                                      ElisaUtils.ReplacePlayList,
                                                      ElisaUtils.TriggerPlay)

        onOpen: viewManager.openChildView(innerMainTitle, innerSecondaryTitle, innerImage, databaseId, dataType)

        onGoBackRequested: {
            viewManager.goBack()
        }

        Loader {
            id: busyIndicatorLoader
            anchors.centerIn: parent
            height: Kirigami.Units.gridUnit * 5
            width: height

            visible: realModel.isBusy
            active: realModel.isBusy

            sourceComponent: BusyIndicator {
                anchors.centerIn: parent
            }
        }
    }

    Connections {
        target: elisa

        onMusicManagerChanged: initializeModel()
    }

    Component.onCompleted: {
        initializeModel()
    }
}
