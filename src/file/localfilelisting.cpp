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

#include "localfilelisting.h"

#include "musicaudiotrack.h"

#include <KFileMetaData/Properties>
#include <KFileMetaData/ExtractorCollection>
#include <KFileMetaData/Extractor>
#include <KFileMetaData/SimpleExtractionResult>

#include <QThread>
#include <QHash>
#include <QFileInfo>
#include <QDir>
#include <QFileSystemWatcher>
#include <QMimeDatabase>
#include <QStandardPaths>

#include <QDebug>

#include <algorithm>

class LocalFileListingPrivate
{
public:

    QString mRootPath;

    QFileSystemWatcher mFileSystemWatcher;

    QHash<QString, QVector<MusicAudioTrack>> mAllAlbums;

    QHash<QString, QUrl> mAllAlbumCover;

    QHash<QUrl, QString> mAlbumNameFromTrackFile;

    int mCptTracks = 0;

    QHash<QString, QList<QUrl>> mDiscoveredFiles;

    QHash<QString, QList<QUrl>> mDiscoveredDirectories;

    bool mNewTracks = false;

};

LocalFileListing::LocalFileListing(QObject *parent) : QObject(parent), d(new LocalFileListingPrivate)
{
    connect(&d->mFileSystemWatcher, &QFileSystemWatcher::directoryChanged,
            this, &LocalFileListing::directoryChanged);
    connect(&d->mFileSystemWatcher, &QFileSystemWatcher::fileChanged,
            this, &LocalFileListing::fileChanged);

    const auto &musicLocations(QStandardPaths::standardLocations(QStandardPaths::MusicLocation));

    if (musicLocations.isEmpty()) {
        return;
    }

    d->mRootPath = musicLocations.first();
}

LocalFileListing::~LocalFileListing()
{
}

QString LocalFileListing::rootPath() const
{
    return d->mRootPath;
}

void LocalFileListing::init()
{
}

void LocalFileListing::setRootPath(QString rootPath)
{
    if (d->mRootPath == rootPath) {
        return;
    }

    d->mRootPath = rootPath;
    Q_EMIT rootPathChanged();
}

void LocalFileListing::scanDirectory(const QString &path)
{
    QDir rootDirectory(path);
    rootDirectory.refresh();

    if (rootDirectory.exists()) {
        d->mFileSystemWatcher.addPath(path);
    }

    auto &currentDirectoryListingFiles = d->mDiscoveredFiles[path];
    auto &currentDirectoryListingDirectories = d->mDiscoveredDirectories[path];

    auto currentFilesList = QList<QUrl>();

    rootDirectory.refresh();
    const auto entryList = rootDirectory.entryInfoList();
    for (auto oneEntry : entryList) {
        auto newFilePath = oneEntry.canonicalFilePath();

        if (newFilePath == path) {
            continue;
        }
        if (newFilePath.size() < path.size()) {
            continue;
        }

        if (oneEntry.isDir() || oneEntry.isFile()) {
            currentFilesList.push_back(QUrl::fromLocalFile(newFilePath));
        }
    }

    for (const auto &removedDirectoryPath : currentDirectoryListingDirectories) {
        auto itFilePath = std::find(currentFilesList.begin(), currentFilesList.end(), removedDirectoryPath);

        if (itFilePath != currentFilesList.end()) {
            continue;
        }

        scanDirectory(removedDirectoryPath.toLocalFile());

        auto itRemovedDirectory = std::find(currentDirectoryListingDirectories.begin(), currentDirectoryListingDirectories.end(), removedDirectoryPath);
        currentDirectoryListingDirectories.erase(itRemovedDirectory);

        auto itRemovedFullDirectory = d->mDiscoveredDirectories.find(removedDirectoryPath.toLocalFile());
        d->mDiscoveredDirectories.erase(itRemovedFullDirectory);
    }
    auto removedTracks = QList<QUrl>();
    for (const auto &removedFilePath : currentDirectoryListingFiles) {
        auto itFilePath = std::find(currentFilesList.begin(), currentFilesList.end(), removedFilePath);

        if (itFilePath != currentFilesList.end()) {
            continue;
        }

        removedTracks.push_back(removedFilePath);
    }
    for (const auto &oneRemovedTrack : removedTracks) {
        auto itAlbumName = d->mAlbumNameFromTrackFile.find(oneRemovedTrack);

        if (itAlbumName != d->mAlbumNameFromTrackFile.end()) {
            auto itAlbum = d->mAllAlbums.find(*itAlbumName);
            if (itAlbum != d->mAllAlbums.end()) {
                for (auto itTrack = itAlbum->begin(); itTrack != itAlbum->end(); ) {
                    if (itTrack->resourceURI() == oneRemovedTrack) {
                        itTrack = itAlbum->erase(itTrack);
                    } else {
                        ++itTrack;
                    }
                }
            }
            if (itAlbum->isEmpty()) {
                d->mAllAlbums.erase(itAlbum);
            }
        }

        auto itRemovedTrack = std::find(currentDirectoryListingFiles.begin(), currentDirectoryListingFiles.end(), oneRemovedTrack);
        currentDirectoryListingFiles.erase(itRemovedTrack);
    }

    if (!removedTracks.isEmpty()) {
        Q_EMIT removedTracksList(removedTracks);
    }

    for (auto newFilePath : currentFilesList) {
        auto itFilePath = std::find(currentDirectoryListingFiles.begin(), currentDirectoryListingFiles.end(), newFilePath);

        if (itFilePath != currentDirectoryListingFiles.end()) {
            continue;
        }

        QFileInfo oneEntry(newFilePath.toLocalFile());
        if (oneEntry.isDir()) {
            if (d->mDiscoveredDirectories.find(newFilePath.toLocalFile()) == d->mDiscoveredDirectories.end()) {
                currentDirectoryListingDirectories.push_back(newFilePath);
                scanDirectory(newFilePath.toLocalFile());
            }
            continue;
        }
        if (!oneEntry.isFile()) {
            continue;
        }

        currentDirectoryListingFiles.push_back(newFilePath);

        auto newTrack = scanOneFile(newFilePath);

        if (newTrack.isValid()) {
            QFileInfo trackFilePath(newTrack.resourceURI().toLocalFile());
            QFileInfo coverFilePath(trackFilePath.dir().filePath(QStringLiteral("cover.jpg")));
            if (coverFilePath.exists()) {
                d->mAllAlbumCover[newTrack.albumName()] = QUrl::fromLocalFile(coverFilePath.absoluteFilePath());
            }

            auto &allTracks = d->mAllAlbums[newTrack.albumName()];

            d->mNewTracks = true;
            allTracks.push_back(newTrack);
            d->mAlbumNameFromTrackFile[newTrack.resourceURI()] = newTrack.albumName();
        }
    }
    if (currentDirectoryListingDirectories.isEmpty()) {
        auto itRemovedFullDirectory = d->mDiscoveredDirectories.find(path);
        d->mDiscoveredDirectories.erase(itRemovedFullDirectory);
    }
}

void LocalFileListing::directoryChanged(const QString &path)
{
    d->mNewTracks = false;

    scanDirectory(path);

    if (d->mNewTracks) {
        Q_EMIT tracksList(d->mAllAlbums, d->mAllAlbumCover);
    }
}

void LocalFileListing::fileChanged(const QString &modifiedFileName)
{
    auto modifiedFile = QUrl::fromLocalFile(modifiedFileName);
    auto itAlbumName = d->mAlbumNameFromTrackFile.find(modifiedFile);

    if (itAlbumName != d->mAlbumNameFromTrackFile.end()) {
        auto itAlbum = d->mAllAlbums.find(*itAlbumName);
        if (itAlbum != d->mAllAlbums.end()) {
            for (auto itTrack : *itAlbum) {
                if (itTrack.resourceURI() == modifiedFile) {
                    auto modifiedTrack = scanOneFile(modifiedFile);

                    if (modifiedTrack.isValid()) {
                        auto removedTracks = QList<QUrl>{modifiedFile};
                        Q_EMIT removedTracksList(removedTracks);

                        QHash<QString, QVector<MusicAudioTrack>> modifiedAlbum;

                        auto &currentAlbum = modifiedAlbum[modifiedTrack.albumName()];
                        currentAlbum.push_back(modifiedTrack);

                        Q_EMIT tracksList(modifiedAlbum, d->mAllAlbumCover);
                    }

                    break;
                }
            }
        }
    }
}

void LocalFileListing::refreshContent()
{
    directoryChanged(d->mRootPath);
}

MusicAudioTrack LocalFileListing::scanOneFile(QUrl scanFile)
{
    MusicAudioTrack newTrack;

    QMimeDatabase mimeDb;
    QString mimetype = mimeDb.mimeTypeForFile(scanFile.toLocalFile()).name();

    QFileInfo scanFileInfo(scanFile.toLocalFile());
    if (scanFileInfo.exists()) {
        d->mFileSystemWatcher.addPath(scanFile.toLocalFile());
    }

    KFileMetaData::ExtractorCollection extractors;
    QList<KFileMetaData::Extractor*> exList = extractors.fetchExtractors(mimetype);

    if (exList.isEmpty()) {
        qDebug() << "LocalFileListing::scanOneFile" << "empty extractors list" << scanFile;
        return newTrack;
    }

    KFileMetaData::Extractor* ex = exList.first();
    KFileMetaData::SimpleExtractionResult result(scanFile.toLocalFile(), mimetype,
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

        newTrack.setResourceURI(scanFile);

        newTrack.setValid(true);
    }

    return newTrack;
}


#include "moc_localfilelisting.cpp"
