/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2022 (c) Nate Graham <nate@kde.org>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.15

import org.kde.kirigami 2.12 as Kirigami
import org.kde.elisa

AbstractDataView {
    id: gridView

    property url defaultIcon
    property bool delegateDisplaySecondaryText: true

    signal enqueue(var fullData, string name)
    signal replaceAndPlay(var fullData, string name)
    signal open(var fullData)

    onEnqueue: (fullData, name) => {
        proxyModel.enqueue(
            fullData,
            name,
            ElisaUtils.AppendPlayList,
            ElisaUtils.DoNotTriggerPlay
        );
    }

    onReplaceAndPlay: (fullData, name) => {
        proxyModel.enqueue(
            fullData,
            name,
            ElisaUtils.ReplacePlayList,
            ElisaUtils.TriggerPlay
        );
    }

    onOpen: fullData => viewManager.openChildView(fullData)

    delegate: GridBrowserDelegate {
        width: Kirigami.Settings.isMobile ? contentDirectoryView.cellWidth : elisaTheme.gridDelegateSize
        height: contentDirectoryView.cellHeight

        focus: true

        isSelected: contentDirectoryView.currentIndex === index

        mainText: model.display
        fileUrl: model.url ? model.url : ""
        secondaryText: gridView.delegateDisplaySecondaryText && model.secondaryText ? model.secondaryText : ""
        imageUrl: model.imageUrl ? model.imageUrl : ''
        multipleImageUrls: model.multipleImageUrls
        imageFallbackUrl: defaultIcon
        displaySecondaryText: gridView.delegateDisplaySecondaryText
        hasChildren: model.hasChildren

        onEnqueue: gridView.enqueue(model.fullData, model.display)
        onReplaceAndPlay: gridView.replaceAndPlay(model.fullData, model.display)
        onOpen: gridView.open(model.fullData)

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
            const columns = Math.max(Math.floor(width / elisaTheme.gridDelegateSize), 2);
            return Math.floor(width / columns);
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
