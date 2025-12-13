/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2022 (c) Nate Graham <nate@kde.org>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick

import org.kde.kirigami as Kirigami
import org.kde.elisa

AbstractDataView {
    id: listView

    property int databaseId
    property bool showSection: false

    // Otherwise tracks are sorted in the wrong order
    sortModel: !displaySingleAlbum

    function openMetaDataView(databaseId, url, entryType) {
        metadataLoader.setSource(Kirigami.Settings.isMobile ? "mobile/MobileMediaTrackMetadataView.qml" : "MediaTrackMetadataView.qml",
            {
                fileName: url,
                modelType: entryType,
                showImage: entryType !== ElisaUtils.Radio,
                showTrackFileName: entryType !== ElisaUtils.Radio,
                showDeleteButton: entryType === ElisaUtils.Radio,
                editableMetadata: true,
                canAddMoreMetadata: entryType !== ElisaUtils.Radio,
            });
        metadataLoader.active = true;
    }
    function openCreateRadioView()
    {
        metadataLoader.setSource(Kirigami.Settings.isMobile ? "mobile/MobileMediaTrackMetadataView.qml" : "MediaTrackMetadataView.qml",
            {
                modelType: ElisaUtils.Radio,
                isCreating: true,
                showImage: false,
                showTrackFileName: false,
                showDeleteButton: true,
                editableMetadata: true,
                canAddMoreMetadata: false,
            });
        metadataLoader.active = true;
    }

    Loader {
        id: metadataLoader
        active: false
        onLoaded: {
            // on mobile, the metadata loader is a page
            // on desktop, it's a window
            if (Kirigami.Settings.isMobile) {
                mainWindow.pageStack.layers.push(item);
            } else {
                (item as MediaTrackMetadataView).show();
            }
        }
    }

    delegate: TrackBrowserDelegate {
        id: entry

        required property int index
        required property var model

        width: contentDirectoryView.width

        focus: true

        trackUrl: model.url
        dataType: model.dataType
        title: model.display ? model.display : ""
        artist: model.artist ? model.artist : ""
        album: model.album ? model.album : ""
        albumArtist: model.albumArtist ? model.albumArtist : ""
        duration: model.duration ? model.duration : ""
        imageUrl: model.imageUrl ? model.imageUrl : ""
        trackNumber: model.trackNumber ? model.trackNumber : -1
        discNumber: model.discNumber ? model.discNumber : -1
        rating: model.rating
        hideDiscNumber: !listView.displaySingleAlbum && model.isSingleDiscAlbum
        isSelected: contentDirectoryView.currentIndex === index
        isAlternateColor: (index % 2) === 1
        detailedView: !listView.displaySingleAlbum

        onTrackRatingChanged: (url, rating) => {
            ElisaApplication.musicManager.updateSingleFileMetaData(url, DataTypes.RatingRole, rating)
        }

        onEnqueue: ElisaApplication.mediaPlayListProxyModel.enqueue(model.fullData, model.display,
                                                            ElisaUtils.AppendPlayList,
                                                            ElisaUtils.DoNotTriggerPlay)

        onPlayNext: ElisaApplication.mediaPlayListProxyModel.enqueue(model.fullData, model.display,
                                                                     ElisaUtils.AfterCurrentTrack,
                                                                     ElisaUtils.DoNotTriggerPlay)

        onReplaceAndPlay: url => {
            // If we're a radio type then only enqueue the current track
            if (dataType === ElisaUtils.Radio) {
                ElisaApplication.mediaPlayListProxyModel.enqueue(model.fullData, model.display,
                                                                 ElisaUtils.ReplacePlayList,
                                                                 ElisaUtils.TriggerPlay);
            } else {
                listView.contentModel.replaceAndPlayOfPlayListFromTrackUrl(contentDirectoryView.model.rootIndex, url);
            }
        }

        onClicked: {
            forceActiveFocus()
            contentDirectoryView.currentIndex = model.index
        }

        onActiveFocusChanged: {
            if (activeFocus && contentDirectoryView.currentIndex !== model.index) {
                contentDirectoryView.currentIndex = model.index
            }
        }

        onCallOpenMetaDataView: (url, entryType) => {
            listView.openMetaDataView(listView.databaseId, url, entryType)
        }
    }


    contentView: ListView {
        id: contentDirectoryView

        activeFocusOnTab: true
        keyNavigationEnabled: true

        reuseItems: true

        model: listView.delegateModel

        // HACK: setting currentIndex to -1 in mobile for some reason causes segfaults, no idea why
        currentIndex: Kirigami.Settings.isMobile ? 0 : -1

        Accessible.role: Accessible.List
        Accessible.name: listView.mainTitle
        Accessible.description: listView.secondaryTitle

        section.property: listView.showSection ? "discNumber" : ""
        section.criteria: ViewSection.FullString
        section.labelPositioning: ViewSection.InlineLabels
        section.delegate: TracksDiscHeader {
            required property string section
            discNumber: section
            width: contentDirectoryView.width
        }

        onCountChanged: if (count === 0) {
            currentIndex = -1;
        }
    }

    Connections {
        target: listView.navigationBar // lives in parent item

        function onCreateRadio() {
            listView.openCreateRadioView()
        }

        function onShowArtist(name: string) {
            listView.viewManager.openArtistView(name)
        }
    }
}
