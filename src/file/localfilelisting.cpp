/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * Copyright (C) 2014 Vishesh Handa <me@vhanda.in>
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

#include "localfilelisting.h"

#include "musicaudiotrack.h"

#include <KFileMetaData/Properties>
#include <KFileMetaData/ExtractorCollection>
#include <KFileMetaData/Extractor>
#include <KFileMetaData/SimpleExtractionResult>

#include <QtCore/QThread>
#include <QtCore/QDebug>
#include <QtCore/QHash>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QMimeDatabase>

#include <QtCore/QDebug>

#include <algorithm>

class LocalFileListingPrivate
{
public:

    QString mRootPath;

    QFileSystemWatcher mFileSystemWatcher;

    QHash<QString, QVector<MusicAudioTrack>> mAllAlbums;

    QHash<QString, QUrl> mAllAlbumCover;

    int mCptTracks = 0;

    QHash<QString, QList<QString>> mDiscoveredFiles;

};

LocalFileListing::LocalFileListing(QObject *parent) : QObject(parent), d(new LocalFileListingPrivate)
{
    d->mRootPath = QStringLiteral("/home/mgallien/Musique");

    connect(&d->mFileSystemWatcher, &QFileSystemWatcher::directoryChanged,
            this, &LocalFileListing::directoryChanged);
    connect(&d->mFileSystemWatcher, &QFileSystemWatcher::fileChanged,
            this, &LocalFileListing::fileChanged);
}

LocalFileListing::~LocalFileListing()
{
}

void LocalFileListing::init()
{
    scanDirectory(d->mRootPath);

    for (auto oneAlbum : d->mAllAlbums) {
        QHash<QString, QVector<MusicAudioTrack>> oneAlbumContainer;
        oneAlbumContainer[oneAlbum.first().albumName()] = oneAlbum;
        Q_EMIT tracksList(oneAlbumContainer, d->mAllAlbumCover);
    }
}

void LocalFileListing::scanDirectory(const QString &path)
{
    QDir rootDirectory(path);

    qDebug() << "LocalFileListing::scanDirectory" << path;

    d->mFileSystemWatcher.addPath(path);
    d->mDiscoveredFiles[path];

    auto currentFilesList = QList<QString>();

    const auto entryList = rootDirectory.entryInfoList();
    for (auto oneEntry : entryList) {
        auto newFilePath = oneEntry.canonicalFilePath();

        if (newFilePath == path) {
            continue;
        }
        if (newFilePath.size() < path.size()) {
            continue;
        }

        if (oneEntry.isDir()) {
            if (d->mDiscoveredFiles.find(newFilePath) == d->mDiscoveredFiles.end()) {
                scanDirectory(newFilePath);
            }
            continue;
        }
        if (oneEntry.isFile()) {
            currentFilesList.push_back(newFilePath);
        }
    }

    std::sort(currentFilesList.begin(), currentFilesList.end());

    auto removedTracks = QList<QString>();
    for (auto removedFilePath : d->mDiscoveredFiles[path]) {
        qDebug() << "is" << removedFilePath << "still there";
        auto itFilePath = std::find(currentFilesList.begin(), currentFilesList.end(), removedFilePath);

        if (itFilePath != currentFilesList.end()) {
            qDebug() << "yes";
            continue;
        }

        qDebug() << "no";
        removedTracks.push_back(removedFilePath);
    }

    qDebug() << "LocalFileListing::scanDirectory" << removedTracks;
    Q_EMIT removedTracksList(removedTracks);

    for (auto newFilePath : currentFilesList) {
        auto itFilePath = std::find(d->mDiscoveredFiles[path].begin(), d->mDiscoveredFiles[path].end(), newFilePath);

        if (itFilePath != d->mDiscoveredFiles[path].end()) {
            continue;
        }

        d->mDiscoveredFiles[path].push_back(newFilePath);

        QMimeDatabase mimeDb;
        QString mimetype = mimeDb.mimeTypeForFile(newFilePath).name();

        d->mFileSystemWatcher.addPath(newFilePath);

        KFileMetaData::ExtractorCollection extractors;
        QList<KFileMetaData::Extractor*> exList = extractors.fetchExtractors(mimetype);

        KFileMetaData::Extractor* ex = exList.first();
        KFileMetaData::SimpleExtractionResult result(newFilePath, mimetype,
                                                     KFileMetaData::ExtractionResult::ExtractMetaData);

        ex->extract(&result);

        const auto &allProperties = result.properties();

        auto titleProperty = allProperties.find(KFileMetaData::Property::Title);
        auto durationProperty = allProperties.find(KFileMetaData::Property::Duration);
        auto artistProperty = allProperties.find(KFileMetaData::Property::Artist);
        auto albumProperty = allProperties.find(KFileMetaData::Property::Album);
        auto albumArtistProperty = allProperties.find(KFileMetaData::Property::AlbumArtist);
        auto trackNumberProperty = allProperties.find(KFileMetaData::Property::TrackNumber);

        if (albumProperty != allProperties.end()) {
            auto albumValue = albumProperty->toString();
            auto &allTracks = d->mAllAlbums[albumValue];

            MusicAudioTrack newTrack;

            newTrack.setAlbumName(albumValue);
            ++d->mCptTracks;

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

            newTrack.setResourceURI(QUrl::fromLocalFile(newFilePath));
            QFileInfo trackFilePath(newFilePath);
            QFileInfo coverFilePath(trackFilePath.dir().filePath(QStringLiteral("cover.jpg")));
            if (coverFilePath.exists()) {
                d->mAllAlbumCover[albumValue] = QUrl::fromLocalFile(coverFilePath.absoluteFilePath());
            }

            allTracks.push_back(newTrack);
        }

        if (albumProperty != allProperties.end()) {
            auto albumValue = albumProperty->toString();
            auto &allTracks = d->mAllAlbums[albumValue];

            std::sort(allTracks.begin(), allTracks.end());
        }
    }
}

void LocalFileListing::directoryChanged(const QString &path)
{
    qDebug() << "LocalFileListing::directoryChanged" << path;
    scanDirectory(path);

    for (auto oneAlbum : d->mAllAlbums) {
        QHash<QString, QVector<MusicAudioTrack>> oneAlbumContainer;
        oneAlbumContainer[oneAlbum.first().albumName()] = oneAlbum;
        Q_EMIT tracksList(oneAlbumContainer, d->mAllAlbumCover);
    }
}

void LocalFileListing::fileChanged(const QString &path)
{
    qDebug() << "LocalFileListing::fileChanged" << path;
}

void LocalFileListing::refreshContent()
{
    init();
}


#include "moc_localfilelisting.cpp"
