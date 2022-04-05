/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2022 (c) Nate Graham <nate@kde.org>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.15

import org.kde.kirigami 2.12 as Kirigami
import org.kde.elisa 1.0

AbstractDataView {
    id: gridView

    property url defaultIcon
    property bool delegateDisplaySecondaryText: true

    signal enqueue(var fullData, string name)
    signal replaceAndPlay(var fullData, string name)
    signal open(var fullData)

    onEnqueue: proxyModel.enqueue(fullData, name,
                                  ElisaUtils.AppendPlayList,
                                  ElisaUtils.DoNotTriggerPlay)

    onReplaceAndPlay: proxyModel.enqueue(fullData, name,
                                         ElisaUtils.ReplacePlayList,
                                         ElisaUtils.TriggerPlay)

    onOpen: viewManager.openChildView(fullData)


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


    contentView: GridView {
        id: contentDirectoryView

        activeFocusOnTab: true
        keyNavigationEnabled: true

        reuseItems: true

        model: delegateModel

        // HACK: setting currentIndex to -1 in mobile for some reason causes segfaults, no idea why
        currentIndex: Kirigami.Settings.isMobile ? 0 : -1

        Accessible.role: Accessible.List
        Accessible.name: mainTitle

        cellWidth: {
            let columns = Math.max(Math.floor(gridView.viewWidth / elisaTheme.gridDelegateSize), 2);
            return Math.floor(gridView.viewWidth / columns);
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
