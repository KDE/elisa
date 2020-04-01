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
    property alias defaultIcon: gridView.defaultIcon
    property alias showRating: gridView.showRating
    property alias delegateDisplaySecondaryText: gridView.delegateDisplaySecondaryText
    property alias isSubPage: gridView.isSubPage
    property alias expandedFilterView: gridView.expandedFilterView
    property string genreFilterText
    property string artistFilter

    focus: true

    Accessible.role: Accessible.Pane
    Accessible.name: mainTitle

    function initializeModel()
    {
        realModel.initialize(elisa.musicManager, elisa.musicManager.viewDatabase,
                             modelType, filterType, genreFilterText, artistFilter, 0)
    }

    DataModel {
        id: realModel
    }

    GridViewProxyModel {
        id: proxyModel

        sourceModel: realModel
        playList: elisa.mediaPlayListProxyModel
    }

    GridBrowserView {
        id: gridView

        focus: true

        anchors.fill: parent

        contentModel: proxyModel

        onEnqueue: elisa.mediaPlayListProxyModel.enqueue(fullData, name,
                                               ElisaUtils.AppendPlayList,
                                               ElisaUtils.DoNotTriggerPlay)

        onReplaceAndPlay: elisa.mediaPlayListProxyModel.enqueue(fullData, name,
                                                      ElisaUtils.ReplacePlayList,
                                                      ElisaUtils.TriggerPlay)

        onOpen: viewManager.openChildView(innerMainTitle, innerSecondaryTitle, innerImage, databaseId, dataType)

        onGoBack: viewManager.goBack()

        Loader {
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
        if (elisa.musicManager) {
            initializeModel()
        }
    }
}
