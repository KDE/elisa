/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2022 (c) Nate Graham <nate@kde.org>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.15

import org.kde.kirigami 2.12 as Kirigami
import org.kde.elisa 1.0

AbstractDataView {
    id: listView

    property int databaseId
    property bool showSection: false
    property bool radioCase: false

    // Otherwise tracks are sorted in the wrong order
    sortModel: !displaySingleAlbum

    signal showArtist(var name)

    onShowArtist: {
        viewManager.openArtistView(name)
    }

    function openMetaDataView(databaseId, url, entryType) {
        metadataLoader.setSource(Kirigami.Settings.isMobile ? "mobile/MobileMediaTrackMetadataView.qml" : "MediaTrackMetadataView.qml",
                                 {
                                     "fileName": url,
                                     "modelType": entryType,
                                     "showImage": entryType !== ElisaUtils.Radio,
                                     "showTrackFileName": entryType !== ElisaUtils.Radio,
                                     "showDeleteButton": entryType === ElisaUtils.Radio,
                                     "editableMetadata": true,
                                     "canAddMoreMetadata": entryType !== ElisaUtils.Radio,
                                 });
        metadataLoader.active = true
    }
    function openCreateRadioView()
    {
        metadataLoader.setSource(Kirigami.Settings.isMobile ? "mobile/MobileMediaTrackMetadataView.qml" : "MediaTrackMetadataView.qml",
                                 {
                                     "modelType": ElisaUtils.Radio,
                                     "isCreating": true,
                                     "showImage": false,
                                     "showTrackFileName": false,
                                     "showDeleteButton": true,
                                     "editableMetadata": true,
                                     "canAddMoreMetadata": false,
                                 });
        metadataLoader.active = true
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
                item.show();
            }
        }
    }


    delegate: ListBrowserDelegate {
        id: entry

        width: contentDirectoryView.width

        focus: true

        trackUrl: model.url
        dataType: model.dataType
        title: model.display ? model.display : ''
        artist: model.artist ? model.artist : ''
        album: model.album ? model.album : ''
        albumArtist: model.albumArtist ? model.albumArtist : ''
        duration: model.duration ? model.duration : ''
        imageUrl: model.imageUrl ? model.imageUrl : ''
        trackNumber: model.trackNumber ? model.trackNumber : -1
        discNumber: model.discNumber ? model.discNumber : -1
        rating: model.rating
        hideDiscNumber: !listView.displaySingleAlbum && model.isSingleDiscAlbum
        isSelected: contentDirectoryView.currentIndex === index
        isAlternateColor: (index % 2) === 1
        detailedView: !listView.displaySingleAlbum

        onTrackRatingChanged: {
            ElisaApplication.musicManager.updateSingleFileMetaData(url, DataTypes.RatingRole, rating)
        }

        onEnqueue: ElisaApplication.mediaPlayListProxyModel.enqueue(model.fullData, model.display,
                                                            ElisaUtils.AppendPlayList,
                                                            ElisaUtils.DoNotTriggerPlay)

        onReplaceAndPlay: ElisaApplication.mediaPlayListProxyModel.enqueue(model.fullData, model.display,
                                                                ElisaUtils.ReplacePlayList,
                                                                ElisaUtils.TriggerPlay)

        onClicked: {
            forceActiveFocus()
            contentDirectoryView.currentIndex = model.index
        }

        onActiveFocusChanged: {
            if (activeFocus && contentDirectoryView.currentIndex !== model.index) {
                contentDirectoryView.currentIndex = model.index
            }
        }

        onCallOpenMetaDataView: {
            openMetaDataView(databaseId, url, entryType)
        }
    }


    contentView: ListView {
        id: contentDirectoryView

        activeFocusOnTab: true
        keyNavigationEnabled: true

        reuseItems: true

        model: delegateModel

        // HACK: setting currentIndex to -1 in mobile for some reason causes segfaults, no idea why
        currentIndex: Kirigami.Settings.isMobile ? 0 : -1

        Accessible.role: Accessible.List
        Accessible.name: mainTitle
        Accessible.description: secondaryTitle

        section.property: (showSection ? 'discNumber' : '')
        section.criteria: ViewSection.FullString
        section.labelPositioning: ViewSection.InlineLabels
        section.delegate: TracksDiscHeader {
            discNumber: section
            width: contentDirectoryView.width
        }

        onCountChanged: if (count === 0) {
            currentIndex = -1;
        }
    }

    Connections {
        target: navigationBar // lives in parent item

        function onCreateRadio() {
            openCreateRadioView()
        }
    }
}
