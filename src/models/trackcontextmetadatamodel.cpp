/*
   SPDX-FileCopyrightText: 2019 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "trackcontextmetadatamodel.h"

TrackContextMetaDataModel::TrackContextMetaDataModel(QObject *parent) : TrackMetadataModel(parent)
{
}

void TrackContextMetaDataModel::filterDataFromTrackData()
{
    removeMetaData(DataTypes::TitleRole);
    removeMetaData(DataTypes::ArtistRole);
    removeMetaData(DataTypes::AlbumRole);
    removeMetaData(DataTypes::AlbumArtistRole);

    if (dataFromType(DataTypes::IsSingleDiscAlbumRole).toBool() &&
            dataFromType(DataTypes::DiscNumberRole).toInt() == 1) {
        removeMetaData(DataTypes::DiscNumberRole);
    }
}

void TrackContextMetaDataModel::fillLyricsDataFromTrack()
{
}


#include "moc_trackcontextmetadatamodel.cpp"
