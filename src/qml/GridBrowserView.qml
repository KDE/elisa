/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtQml.Models 2.1
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.12 as Kirigami
import org.kde.elisa 1.0

import "mobile"

FocusScope {
    id: gridView

    property bool isSubPage: false
    property string mainTitle
    property string secondaryTitle
    property url image
    property AbstractProxyModel contentModel
    property alias showRating: navigationBar.showRating
    property bool delegateDisplaySecondaryText: true
    property alias expandedFilterView: navigationBar.expandedFilterView
    property bool haveTreeModel: false
    property alias sortRole: navigationBar.sortRole
    property alias sortRoles: navigationBar.sortRoles
    property alias sortRoleNames: navigationBar.sortRoleNames
    property alias sortOrderNames: navigationBar.sortOrderNames
    property alias sortOrder: navigationBar.sortOrder
    property var stackView
    property url defaultIcon
    property int depth: 1
    property alias viewManager: navigationBar.viewManager
    property bool suppressNoDataPlaceholderMessage: false

    signal enqueue(var fullData, string name)
    signal replaceAndPlay(var fullData, string name)
    signal open(var fullData)
    signal goBackRequested()

    function goToBack() {
        if (haveTreeModel) {
            delegateModel.rootIndex = delegateModel.parentModelIndex()
            --depth
        } else {
            gridView.goBackRequested()
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        NavigationActionBar {
            id: navigationBar

            z: 1 // on top of track list

            mainTitle: gridView.mainTitle
            secondaryTitle: gridView.secondaryTitle
            image: gridView.image
            enableGoBack: gridView.isSubPage || depth > 1

            Layout.fillWidth: true

            Loader {
                active: gridView.contentModel

                sourceComponent: Binding {
                    target: gridView.contentModel
                    property: 'filterText'
                    when: gridView.contentModel
                    value: navigationBar.filterText
                }
            }

            Loader {
                active: gridView.contentModel

                sourceComponent: Binding {
                    target: gridView.contentModel
                    property: 'filterRating'
                    when: gridView.contentModel
                    value: navigationBar.filterRating
                }
            }

            Loader {
                active: gridView.contentModel && navigationBar.enableSorting

                sourceComponent: Binding {
                    target: gridView.contentModel
                    property: 'sortRole'
                    when: gridView.contentModel && navigationBar.enableSorting
                    value: navigationBar.sortRole
                }
            }

            onEnqueue: contentModel.enqueueToPlayList(delegateModel.rootIndex)

            onReplaceAndPlay:contentModel.replaceAndPlayOfPlayList(delegateModel.rootIndex)

            onGoBack: {
                gridView.goToBack()
            }

            onSortOrderChanged: {
                if (!contentModel || !navigationBar.enableSorting) {
                    return
                }

                if ((contentModel.sortedAscending && sortOrder !== Qt.AscendingOrder) ||
                        (!contentModel.sortedAscending && sortOrder !== Qt.DescendingOrder)) {
                    contentModel.sortModel(sortOrder)
                }
            }
        }

        DelegateModel {
            id: delegateModel

            model: gridView.contentModel

            delegate: GridBrowserDelegate {
                width: Kirigami.Settings.isMobile ? contentDirectoryView.cellWidth : elisaTheme.gridDelegateSize
                height: contentDirectoryView.cellHeight

                focus: true

                isSelected: contentDirectoryView.currentIndex === index

                isPartial: false

                mainText: model.display
                fileUrl: model.url ? model.url : ""
                secondaryText: gridView.delegateDisplaySecondaryText && model.secondaryText ? model.secondaryText : ""
                imageUrl: model.imageUrl ? model.imageUrl : ''
                imageFallbackUrl: defaultIcon
                databaseId: model.databaseId
                delegateDisplaySecondaryText: gridView.delegateDisplaySecondaryText
                entryType: model.dataType
                hasChildren: model.hasChildren

                onEnqueue: gridView.enqueue(model.fullData, model.display)
                onReplaceAndPlay: gridView.replaceAndPlay(model.fullData, model.display)
                onOpen: {
                    if (haveTreeModel && !model.hasModelChildren) {
                        return
                    }

                    if (haveTreeModel) {
                        delegateModel.rootIndex = delegateModel.modelIndex(model.index)
                        ++depth
                    } else {
                        gridView.open(model.fullData)
                    }
                }
                onSelected: {
                    forceActiveFocus()
                    contentDirectoryView.currentIndex = model.index
                }

                onActiveFocusChanged: {
                    if (activeFocus && contentDirectoryView.currentIndex !== model.index) {
                        contentDirectoryView.currentIndex = model.index
                    }
                }
            }
        }

        FocusScope {

            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.leftMargin: Kirigami.Units.smallSpacing
            Layout.rightMargin: Kirigami.Units.smallSpacing

            ScrollView {
                id: scrollView
                readonly property int scrollBarWidth: ScrollBar.vertical.visible ? ScrollBar.vertical.width : 0
                readonly property int availableSpace: scrollView.width - scrollView.scrollBarWidth

                anchors.fill: parent

                GridView {
                    id: contentDirectoryView

                    activeFocusOnTab: true
                    keyNavigationEnabled: true

                    reuseItems: true

                    model: delegateModel

                    // HACK: setting currentIndex to -1 in mobile for some reason causes segfaults, no idea why
                    currentIndex: Kirigami.Settings.isMobile ? 0 : -1

                    Accessible.role: Accessible.List
                    Accessible.name: mainTitle

                    Kirigami.PlaceholderMessage {
                        anchors.centerIn: parent
                        width: parent.width - (Kirigami.Units.largeSpacing * 4)
                        visible: contentDirectoryView.count === 0 && !suppressNoDataPlaceholderMessage
                        text: i18n("Nothing to display")
                    }

                    cellWidth: {
                        let columns = Math.max(Math.floor(scrollView.availableSpace / elisaTheme.gridDelegateSize), 2);
                        return Math.floor(scrollView.availableSpace / columns);
                    }
                    cellHeight: {
                        if (Kirigami.Settings.isMobile) {
                            return cellWidth + Kirigami.Units.gridUnit * 2 + Kirigami.Units.largeSpacing;
                        } else {
                            return elisaTheme.gridDelegateSize + Kirigami.Units.gridUnit * 2 + Kirigami.Units.largeSpacing;
                        }
                    }
                }
            }
        }
    }
}
