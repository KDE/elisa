/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2022 (c) Nate Graham <nate@kde.org>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.15

import QtQml.Models

import org.kde.kirigami 2.12 as Kirigami
import org.kde.elisa

AbstractDataView {
    id: gridView

    property url defaultIcon
    property bool delegateDisplaySecondaryText: true

    canToggleViewStyle: true
    viewStyle: ViewManager.GridStyle
    readonly property bool showingGridViewStyle: viewStyle === ViewManager.GridStyle

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

    function playNext(fullData: var, name: string) {
        proxyModel.enqueue(
            fullData,
            name,
            ElisaUtils.AfterCurrentTrack,
            ElisaUtils.DoNotTriggerPlay
        );
    }

    onOpen: fullData => viewManager.openChildView(fullData)

    delegate: Package {
        id: delegatePackage

        required property int index
        required property string display
        required property string secondaryText
        required property url url
        required property url imageUrl
        required property var multipleImageUrls
        required property bool hasChildren
        required property var fullData

        GridBrowserDelegate {
            Package.name: 'grid'

            width: Kirigami.Settings.isMobile ? contentDirectoryView.cellWidth : elisaTheme.gridDelegateSize
            height: contentDirectoryView.cellHeight

            focus: true

            isSelected: contentDirectoryView.currentIndex === delegatePackage.index

            mainText: delegatePackage.display
            fileUrl: delegatePackage.url ? delegatePackage.url : ""
            secondaryText: delegatePackage.secondaryText ? delegatePackage.secondaryText : ""
            imageUrl: delegatePackage.imageUrl ? delegatePackage.imageUrl : ""
            multipleImageUrls: delegatePackage.multipleImageUrls
            imageFallbackUrl: defaultIcon
            displaySecondaryText: gridView.delegateDisplaySecondaryText
            hasChildren: delegatePackage.hasChildren

            onEnqueue: gridView.enqueue(delegatePackage.fullData, delegatePackage.display)
            onReplaceAndPlay: gridView.replaceAndPlay(delegatePackage.fullData, delegatePackage.display)
            onPlayNext: gridView.playNext(delegatePackage.fullData, delegatePackage.display)
            onOpen: gridView.open(delegatePackage.fullData)

            onActiveFocusChanged: {
                if (activeFocus && gridView.currentIndex !== delegatePackage.index) {
                    gridView.currentIndex = delegatePackage.index;
                }
            }
        }

        ListBrowserDelegate {
            Package.name: 'list'

            width: contentDirectoryView.width

            focus: true

            isSelected: contentDirectoryView.currentIndex === delegatePackage.index

            index: delegatePackage.index
            mainText: delegatePackage.display
            fileUrl: delegatePackage.url ? delegatePackage.url : ""
            secondaryText: delegatePackage.secondaryText ? delegatePackage.secondaryText : ""
            imageUrl: delegatePackage.imageUrl ? delegatePackage.imageUrl : ""
            multipleImageUrls: delegatePackage.multipleImageUrls
            imageFallbackUrl: defaultIcon
            displaySecondaryText: gridView.delegateDisplaySecondaryText
            hasChildren: delegatePackage.hasChildren

            onEnqueue: gridView.enqueue(delegatePackage.fullData, delegatePackage.display)
            onReplaceAndPlay: gridView.replaceAndPlay(delegatePackage.fullData, delegatePackage.display)
            onPlayNext: gridView.playNext(delegatePackage.fullData, delegatePackage.display)
            onOpen: gridView.open(delegatePackage.fullData)

            onActiveFocusChanged: {
                if (activeFocus && contentDirectoryView.currentIndex !== delegatePackage.index) {
                    contentDirectoryView.currentIndex = delegatePackage.index;
                }
            }
        }
    }

    contentView: GridView {
        id: contentDirectoryView

        activeFocusOnTab: true
        keyNavigationEnabled: true

        reuseItems: true

        model: gridView.showingGridViewStyle ? gridView.delegateModel.parts.grid : gridView.delegateModel.parts.list

        // HACK: setting currentIndex to -1 in mobile for some reason causes segfaults, no idea why
        currentIndex: Kirigami.Settings.isMobile ? 0 : -1

        Accessible.role: Accessible.List
        Accessible.name: mainTitle

        header: Loader {
            active: (gridView.realModel ? !gridView.realModel.isBusy : false) &&
                gridView.isSubPage && (gridView.modelType === ElisaUtils.Artist || gridView.modelType === ElisaUtils.Album)

            sourceComponent: ListBrowserDelegate {
                index: 0
                height: elisaTheme.listDelegateHeight
                width: contentDirectoryView.width
                hideActions: true
                hasChildren: true
                mainText: i18nc("@item:intable View all tracks", "All tracks")
                secondaryText: i18ncp("@item:intable number of tracks/songs in all albums", "%1 track", "%1 tracks", gridView.contentModel.tracksCount)
                imageFallbackUrl: elisaTheme.trackIcon
                onOpen: gridView.viewManager.openTracksView(gridView.mainTitle)
            }
        }

        cellWidth: {
            if (!gridView.showingGridViewStyle) {
                return width;
            }

            let columns = Math.max(Math.floor(width / elisaTheme.gridDelegateSize), 2);
            return Math.floor(width / columns);
        }
        cellHeight: {
            if (!gridView.showingGridViewStyle) {
                return elisaTheme.listDelegateHeight;
            }

            if (Kirigami.Settings.isMobile) {
                return cellWidth + Kirigami.Units.gridUnit * 2 + Kirigami.Units.largeSpacing;
            } else {
                return elisaTheme.gridDelegateSize + Kirigami.Units.gridUnit * 2 + Kirigami.Units.largeSpacing;
            }
        }
    }
}
