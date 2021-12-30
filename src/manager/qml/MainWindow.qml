/*
   SPDX-FileCopyrightText: 2021 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import org.kde.kirigami 2.18 as Kirigami
import org.kde.elisa 1.0

Kirigami.ApplicationWindow {
    id: mainWindow

    visible: true

    LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
    LayoutMirroring.childrenInherit: true

    title: i18nc("Window title", "Elisa Manager")

    pageStack.initialPage: EntriesList {
        id: allAlbumsPage
    }

    SystemPalette {
        id: myPalette
        colorGroup: SystemPalette.Active
    }

    ViewsListData {
        id: viewsData

        manager: ElisaApplication.musicManager
        database: ElisaApplication.musicManager.viewDatabase
        embeddedCategory: ElisaApplication.embeddedView
    }

    ViewManager {
        id: viewManager

        viewsData: viewsData

        onOpenGridView: {
            console.info('open view:', configurationData.mainTitle)
            allAlbumsPage.title = configurationData.mainTitle

            allAlbumsPage.model = configurationData.associatedProxyModel
            allAlbumsPage.realModel = configurationData.model

            allAlbumsPage.model.sourceModel = allAlbumsPage.realModel
            allAlbumsPage.model.sourceModel.dataType = configurationData.dataType
            allAlbumsPage.model.sourceModel.playList = Qt.binding(function() { return ElisaApplication.mediaPlayListProxyModel })

            allAlbumsPage.model.sortModel(configurationData.sortOrder)

            allAlbumsPage.fallbackIcon = configurationData.viewDefaultIcon

            allAlbumsPage.realModel.initializeByData(ElisaApplication.musicManager,
                                                     ElisaApplication.musicManager.viewDatabase,
                                                     configurationData.dataType,
                                                     configurationData.filterType,
                                                     configurationData.dataFilter)

//            browseStackView.push(dataGridView, {
//                                     filterType: configurationData.filterType,
//                                     mainTitle: configurationData.mainTitle,
//                                     secondaryTitle: configurationData.secondaryTitle,
//                                     image: configurationData.imageUrl,
//                                     modelType: configurationData.dataType,
//                                     realModel: configurationData.model,
//                                     proxyModel: configurationData.associatedProxyModel,
//                                     defaultIcon: configurationData.viewDefaultIcon,
//                                     showRating: configurationData.viewShowRating,
//                                     delegateDisplaySecondaryText: configurationData.viewDelegateDisplaySecondaryText,
//                                     filter: configurationData.dataFilter,
//                                     isSubPage: (browseStackView.depth >= 2),
//                                     haveTreeModel: configurationData.isTreeModel,
//                                     stackView: configurationData.browseStackView,
//                                     sortRole: configurationData.sortRole,
//                                     sortRoles: configurationData.sortRoles,
//                                     sortRoleNames: configurationData.sortRoleNames,
//                                     sortOrder: configurationData.sortOrder,
//                                     sortOrderNames: configurationData.sortOrderNames,
//                                     viewManager: viewManager,
//                                     opacity: 1,
//                                 })
        }

        onOpenListView: {
//            browseStackView.push(dataListView, {
//                                     filterType: configurationData.filterType,
//                                     isSubPage: configurationData.expectedDepth > 1,
//                                     mainTitle: configurationData.mainTitle,
//                                     secondaryTitle: configurationData.secondaryTitle,
//                                     filter: configurationData.dataFilter,
//                                     image: configurationData.imageUrl,
//                                     modelType: configurationData.dataType,
//                                     realModel: configurationData.model,
//                                     proxyModel: configurationData.associatedProxyModel,
//                                     sortRole: configurationData.sortRole,
//                                     sortAscending: configurationData.sortOrder,
//                                     stackView: browseStackView,
//                                     displaySingleAlbum: configurationData.displaySingleAlbum,
//                                     showSection: configurationData.showDiscHeaders,
//                                     radioCase: configurationData.radioCase,
//                                     haveTreeModel: configurationData.isTreeModel,
//                                     sortRole: configurationData.sortRole,
//                                     sortRoles: configurationData.sortRoles,
//                                     sortRoleNames: configurationData.sortRoleNames,
//                                     sortOrder: configurationData.sortOrder,
//                                     sortOrderNames: configurationData.sortOrderNames,
//                                     viewManager: viewManager,
//                                     opacity: 1,
//                                 })
        }

        onSwitchContextView: {
//            browseStackView.push(albumContext, {
//                                     mainTitle: mainTitle,
//                                     image: imageUrl,
//                                     opacity: 1,
//                                 })
        }

        onPopOneView: {
        }
    }

    Component.onCompleted:
    {
        ElisaApplication.initialize()
        viewManager.openView(3)
    }
}
