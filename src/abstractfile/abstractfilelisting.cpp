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

#include "abstractfilelisting.h"

#include "musicaudiotrack.h"

#include <KFileMetaData/Properties>
#include <KFileMetaData/ExtractorCollection>
#include <KFileMetaData/Extractor>
#include <KFileMetaData/SimpleExtractionResult>
#include <KFileMetaData/UserMetaData>

#include <QThread>
#include <QHash>
#include <QFileInfo>
#include <QDir>
#include <QFileSystemWatcher>
#include <QMimeDatabase>
#include <QSet>
#include <QPair>

#include <algorithm>

class AbstractFileListingPrivate
{
public:

    explicit AbstractFileListingPrivate(const QString &sourceName) : mSourceName(sourceName)
    {
    }

    QFileSystemWatcher mFileSystemWatcher;

    QHash<QString, QUrl> mAllAlbumCover;

    QHash<QUrl, QSet<QPair<QUrl, bool>>> mDiscoveredFiles;

    QString mSourceName;

    bool mHandleNewFiles = true;

};

AbstractFileListing::AbstractFileListing(const QString &sourceName, QObject *parent) : QObject(parent), d(new AbstractFileListingPrivate(sourceName))
{
    connect(&d->mFileSystemWatcher, &QFileSystemWatcher::directoryChanged,
            this, &AbstractFileListing::directoryChanged);
    connect(&d->mFileSystemWatcher, &QFileSystemWatcher::fileChanged,
            this, &AbstractFileListing::fileChanged);
}

AbstractFileListing::~AbstractFileListing()
{
}

void AbstractFileListing::init()
{
    executeInit();
}

void AbstractFileListing::databaseIsReady()
{
    refreshContent();
}

void AbstractFileListing::newTrackFile(MusicAudioTrack partialTrack)
{
    const auto &newTrack = scanOneFile(partialTrack.resourceURI());

    if (newTrack.isValid() && newTrack != partialTrack) {
        Q_EMIT modifyTracksList({newTrack}, d->mAllAlbumCover);
    }
}

void AbstractFileListing::scanDirectory(QList<MusicAudioTrack> &newFiles, const QUrl &path)
{
    QDir rootDirectory(path.toLocalFile());
    rootDirectory.refresh();

    if (rootDirectory.exists()) {
        watchPath(path.toLocalFile());
    }

    auto &currentDirectoryListingFiles = d->mDiscoveredFiles[path];

    auto currentFilesList = QSet<QUrl>();

    rootDirectory.refresh();
    const auto entryList = rootDirectory.entryInfoList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);
    for (auto oneEntry : entryList) {
        auto newFilePath = QUrl::fromLocalFile(oneEntry.canonicalFilePath());

        if (oneEntry.isDir() || oneEntry.isFile()) {
            currentFilesList.insert(newFilePath);
        }
    }

    auto removedTracks = QList<QPair<QUrl, bool>>();
    for (const auto &removedFilePath : currentDirectoryListingFiles) {
        auto itFilePath = std::find(currentFilesList.begin(), currentFilesList.end(), removedFilePath.first);

        if (itFilePath != currentFilesList.end()) {
            continue;
        }

        removedTracks.push_back(removedFilePath);
    }

    auto allRemovedTracks = QList<QUrl>();
    for (const auto &oneRemovedTrack : removedTracks) {
        if (oneRemovedTrack.second) {
            allRemovedTracks.push_back(oneRemovedTrack.first);
        } else {
            removeFile(oneRemovedTrack.first, allRemovedTracks);
        }
    }
    for (const auto &oneRemovedTrack : removedTracks) {
        currentDirectoryListingFiles.remove(oneRemovedTrack);
        currentDirectoryListingFiles.remove(oneRemovedTrack);
    }

    if (!allRemovedTracks.isEmpty()) {
        Q_EMIT removedTracksList(allRemovedTracks);
    }

    if (!d->mHandleNewFiles) {
        return;
    }

    for (auto newFilePath : currentFilesList) {
        QFileInfo oneEntry(newFilePath.toLocalFile());

        auto itFilePath = std::find(currentDirectoryListingFiles.begin(), currentDirectoryListingFiles.end(), QPair<QUrl, bool>{newFilePath, oneEntry.isFile()});

        if (itFilePath != currentDirectoryListingFiles.end()) {
            continue;
        }

        if (oneEntry.isDir()) {
            addFileInDirectory(newFilePath, path);
            scanDirectory(newFiles, newFilePath);
            continue;
        }
        if (!oneEntry.isFile()) {
            continue;
        }

        auto newTrack = scanOneFile(newFilePath);

        if (newTrack.isValid()) {
            addCover(newTrack);

            addFileInDirectory(newTrack.resourceURI(), path);
            newFiles.push_back(newTrack);
        }
    }
}

const QString &AbstractFileListing::sourceName() const
{
    return d->mSourceName;
}

void AbstractFileListing::directoryChanged(const QString &path)
{
    const auto directoryEntry = d->mDiscoveredFiles.find(QUrl::fromLocalFile(path));
    if (directoryEntry == d->mDiscoveredFiles.end()) {
        return;
    }

    scanDirectoryTree(path);
}

void AbstractFileListing::fileChanged(const QString &modifiedFileName)
{
    auto modifiedFile = QUrl::fromLocalFile(modifiedFileName);

    auto modifiedTrack = scanOneFile(modifiedFile);

    if (modifiedTrack.isValid()) {
        Q_EMIT modifyTracksList({modifiedTrack}, d->mAllAlbumCover);
    }
}

void AbstractFileListing::executeInit()
{
}

void AbstractFileListing::triggerRefreshOfContent()
{
}

void AbstractFileListing::refreshContent()
{
    triggerRefreshOfContent();
}

MusicAudioTrack AbstractFileListing::scanOneFile(QUrl scanFile)
{
    MusicAudioTrack newTrack;

    QMimeDatabase mimeDb;
    QString mimetype = mimeDb.mimeTypeForFile(scanFile.toLocalFile()).name();

    KFileMetaData::ExtractorCollection extractors;
    QList<KFileMetaData::Extractor*> exList = extractors.fetchExtractors(mimetype);

    if (exList.isEmpty()) {
        return newTrack;
    }

    QFileInfo scanFileInfo(scanFile.toLocalFile());
    if (scanFileInfo.exists()) {
        watchPath(scanFile.toLocalFile());
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
    auto fileData = KFileMetaData::UserMetaData(scanFile.toLocalFile());

    if (albumProperty != allProperties.end()) {
        auto albumValue = albumProperty->toString();

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

        newTrack.setResourceURI(scanFile);

        newTrack.setRating(fileData.rating());

        newTrack.setValid(true);
    }

    return newTrack;
}

void AbstractFileListing::watchPath(const QString &pathName)
{
    d->mFileSystemWatcher.addPath(pathName);
}

void AbstractFileListing::addFileInDirectory(const QUrl &newFile, const QUrl &directoryName)
{
    const auto directoryEntry = d->mDiscoveredFiles.find(directoryName);
    if (directoryEntry == d->mDiscoveredFiles.end()) {
        watchPath(directoryName.toLocalFile());

        QDir currentDirectory(directoryName.toLocalFile());
        if (currentDirectory.cdUp()) {
            const auto parentDirectoryName = currentDirectory.absolutePath();
            const auto parentDirectory = QUrl::fromLocalFile(parentDirectoryName);
            const auto parentDirectoryEntry = d->mDiscoveredFiles.find(parentDirectory);
            if (parentDirectoryEntry == d->mDiscoveredFiles.end()) {
                watchPath(parentDirectoryName);
            }

            auto &parentCurrentDirectoryListingFiles = d->mDiscoveredFiles[parentDirectory];

            parentCurrentDirectoryListingFiles.insert({directoryName, false});
        }
    }
    auto &currentDirectoryListingFiles = d->mDiscoveredFiles[directoryName];

    QFileInfo isAFile(newFile.toLocalFile());
    currentDirectoryListingFiles.insert({newFile, isAFile.isFile()});
}

void AbstractFileListing::scanDirectoryTree(const QString &path)
{
    auto newFiles = QList<MusicAudioTrack>();

    scanDirectory(newFiles, QUrl::fromLocalFile(path));

    if (!newFiles.isEmpty()) {
        emitNewFiles(newFiles);
    }
}

void AbstractFileListing::setHandleNewFiles(bool handleThem)
{
    d->mHandleNewFiles = handleThem;
}

void AbstractFileListing::emitNewFiles(const QList<MusicAudioTrack> &tracks)
{
    Q_EMIT tracksList(tracks, d->mAllAlbumCover, d->mSourceName);
}

void AbstractFileListing::addCover(const MusicAudioTrack &newTrack)
{
    auto itCover = d->mAllAlbumCover.find(newTrack.albumName());
    if (itCover != d->mAllAlbumCover.end()) {
        return;
    }

    QFileInfo trackFilePath(newTrack.resourceURI().toLocalFile());
    QFileInfo coverFilePath(trackFilePath.dir().filePath(QStringLiteral("cover.jpg")));

    if (coverFilePath.exists()) {
        d->mAllAlbumCover[newTrack.albumName()] = QUrl::fromLocalFile(coverFilePath.absoluteFilePath());
    }
}

void AbstractFileListing::removeDirectory(const QUrl &removedDirectory, QList<QUrl> &allRemovedFiles)
{
    auto itRemovedDirectory = d->mDiscoveredFiles.find(removedDirectory);

    if (itRemovedDirectory == d->mDiscoveredFiles.end()) {
        return;
    }

    for (auto itFile : *itRemovedDirectory) {
        if (itFile.first.isValid() && !itFile.first.isEmpty()) {
            removeFile(itFile.first, allRemovedFiles);
            if (itFile.second) {
                allRemovedFiles.push_back(itFile.first);
            }
        }
    }

    d->mDiscoveredFiles.erase(itRemovedDirectory);
}

void AbstractFileListing::removeFile(const QUrl &oneRemovedTrack, QList<QUrl> &allRemovedFiles)
{
    auto itRemovedDirectory = d->mDiscoveredFiles.find(oneRemovedTrack);
    if (itRemovedDirectory != d->mDiscoveredFiles.end()) {
        removeDirectory(oneRemovedTrack, allRemovedFiles);
    }
}


#include "moc_abstractfilelisting.cpp"
