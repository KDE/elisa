/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "localbaloofilelisting.h"

#include "musicaudiotrack.h"

#include <Baloo/Query>
#include <Baloo/File>

#include <KFileMetaData/Properties>
#include <KFileMetaData/UserMetaData>

#include <QDBusConnection>

#include <QThread>
#include <QDebug>
#include <QHash>
#include <QFileInfo>
#include <QDir>

#include <algorithm>

class LocalBalooFileListingPrivate
{
public:

    Baloo::Query mQuery;

    QHash<QString, QVector<MusicAudioTrack>> mAllAlbums;

    QHash<QString, QVector<MusicAudioTrack>> mNewAlbums;

    QList<MusicAudioTrack> mNewTracks;

    QHash<QString, QUrl> mAllAlbumCover;

};

LocalBalooFileListing::LocalBalooFileListing(QObject *parent) : AbstractFileListing(QStringLiteral("baloo"), parent), d(new LocalBalooFileListingPrivate)
{
    d->mQuery.addType(QStringLiteral("Audio"));
}

LocalBalooFileListing::~LocalBalooFileListing()
{
}

void LocalBalooFileListing::triggerRefreshOfContent()
{
    auto resultIterator = d->mQuery.exec();
    auto newFiles = QList<MusicAudioTrack>();

    while(resultIterator.next()) {
        const auto &newFileUrl = QUrl::fromLocalFile(resultIterator.filePath());
        auto scanFileInfo = QFileInfo(resultIterator.filePath());
        scanFileInfo.absoluteDir().absolutePath();

        addFileInDirectory(newFileUrl, QUrl::fromLocalFile(scanFileInfo.absoluteDir().absolutePath()));
        newFiles.push_back(scanOneFile(newFileUrl));
    }

    Q_EMIT tracksList(newFiles, d->mAllAlbumCover, sourceName());
}

MusicAudioTrack LocalBalooFileListing::scanOneFile(QUrl scanFile)
{
    auto newTrack = MusicAudioTrack();

    auto fileName = scanFile.toLocalFile();
    auto scanFileInfo = QFileInfo(fileName);

    watchPath(fileName);
    watchPath(scanFileInfo.absoluteDir().absolutePath());

    Baloo::File match(fileName);
    match.load();

    const auto &allProperties = match.properties();

    auto titleProperty = allProperties.find(KFileMetaData::Property::Title);
    auto durationProperty = allProperties.find(KFileMetaData::Property::Duration);
    auto artistProperty = allProperties.find(KFileMetaData::Property::Artist);
    auto albumProperty = allProperties.find(KFileMetaData::Property::Album);
    auto albumArtistProperty = allProperties.find(KFileMetaData::Property::AlbumArtist);
    auto trackNumberProperty = allProperties.find(KFileMetaData::Property::TrackNumber);
    auto fileData = KFileMetaData::UserMetaData(fileName);

    if (albumProperty != allProperties.end()) {
        auto albumValue = albumProperty->toString();
        auto &allTracks = d->mAllAlbums[albumValue];

        newTrack.setAlbumName(albumValue);

        if (artistProperty != allProperties.end()) {
            newTrack.setArtist(artistProperty->toString());
        }

        if (durationProperty != allProperties.end()) {
            newTrack.setDuration(QTime::fromMSecsSinceStartOfDay(1000 * durationProperty->toDouble()));
        }

        if (titleProperty != allProperties.end()) {
            newTrack.setTitle(titleProperty->toString());
        }

        if (trackNumberProperty != allProperties.end()) {
            newTrack.setTrackNumber(trackNumberProperty->toInt());
        }

        if (albumArtistProperty != allProperties.end()) {
            newTrack.setAlbumArtist(albumArtistProperty->toString());
        }

        if (newTrack.albumArtist().isEmpty()) {
            newTrack.setAlbumArtist(newTrack.artist());
        }

        if (newTrack.artist().isEmpty()) {
            newTrack.setArtist(newTrack.albumArtist());
        }

        newTrack.setRating(fileData.rating());

        newTrack.setResourceURI(scanFile);

        QFileInfo coverFilePath(scanFileInfo.dir().filePath(QStringLiteral("cover.jpg")));
        if (coverFilePath.exists()) {
            d->mAllAlbumCover[albumValue] = QUrl::fromLocalFile(coverFilePath.absoluteFilePath());
        }

        auto itTrack = std::find(allTracks.begin(), allTracks.end(), newTrack);
        if (itTrack == allTracks.end()) {
            allTracks.push_back(newTrack);
            d->mNewTracks.push_back(newTrack);
            d->mNewAlbums[newTrack.albumName()].push_back(newTrack);

            auto &newTracks = d->mAllAlbums[newTrack.albumName()];

            std::sort(allTracks.begin(), allTracks.end());
            std::sort(newTracks.begin(), newTracks.end());
        }

        newTrack.setValid(true);
    }

    if (!newTrack.isValid()) {
        newTrack = AbstractFileListing::scanOneFile(scanFile);
    }

    return newTrack;
}


#include "moc_localbaloofilelisting.cpp"
