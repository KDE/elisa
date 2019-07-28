/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "abstractfilelisting.h"

#include "config-upnp-qt.h"

#include "abstractfile/indexercommon.h"

#include "musicaudiotrack.h"
#include "notificationitem.h"
#include "filescanner.h"

#include <QThread>
#include <QHash>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QFileSystemWatcher>
#include <QMimeDatabase>
#include <QSet>
#include <QPair>
#include <QAtomicInt>
#include <QDebug>

#include <QtGlobal>

#include <algorithm>
#include <utility>

class AbstractFileListingPrivate
{
public:

    QStringList mAllRootPaths;

    QFileSystemWatcher mFileSystemWatcher;

    QHash<QString, QUrl> mAllAlbumCover;

    QHash<QUrl, QSet<QPair<QUrl, bool>>> mDiscoveredFiles;

    FileScanner mFileScanner;

    QMimeDatabase mMimeDb;

    QHash<QUrl, QDateTime> mAllFiles;

    QAtomicInt mStopRequest = 0;

    int mImportedTracksCount = 0;

    int mNotificationUpdateInterval = 1;

    int mNewFilesEmitInterval = 1;

    bool mHandleNewFiles = true;

    bool mWaitEndTrackRemoval = false;

    bool mErrorWatchingFileSystemChanges = false;

};

AbstractFileListing::AbstractFileListing(QObject *parent) : QObject(parent), d(std::make_unique<AbstractFileListingPrivate>())
{
    connect(&d->mFileSystemWatcher, &QFileSystemWatcher::directoryChanged,
            this, &AbstractFileListing::directoryChanged);
    connect(&d->mFileSystemWatcher, &QFileSystemWatcher::fileChanged,
            this, &AbstractFileListing::fileChanged);
}

AbstractFileListing::~AbstractFileListing()
= default;

void AbstractFileListing::init()
{
    Q_EMIT askRestoredTracks();
}

void AbstractFileListing::newTrackFile(const MusicAudioTrack &partialTrack)
{
    auto scanFileInfo = QFileInfo(partialTrack.resourceURI().toLocalFile());
    const auto &newTrack = scanOneFile(partialTrack.resourceURI(), scanFileInfo);

    if (newTrack.isValid() && newTrack != partialTrack) {
        Q_EMIT modifyTracksList({newTrack}, d->mAllAlbumCover);
    }
}

void AbstractFileListing::restoredTracks(QHash<QUrl, QDateTime> allFiles)
{
    executeInit(std::move(allFiles));

    refreshContent();
}

void AbstractFileListing::setAllRootPaths(const QStringList &allRootPaths)
{
    if (d->mAllRootPaths == allRootPaths) {
        return;
    }

    d->mAllRootPaths = allRootPaths;
}

void AbstractFileListing::databaseFinishedInsertingTracksList()
{
}

void AbstractFileListing::databaseFinishedRemovingTracksList()
{
    if (waitEndTrackRemoval()) {
        Q_EMIT indexingFinished();
        setWaitEndTrackRemoval(false);
    }
}

void AbstractFileListing::applicationAboutToQuit()
{
    d->mStopRequest = 1;
}

const QStringList &AbstractFileListing::allRootPaths() const
{
    return d->mAllRootPaths;
}

void AbstractFileListing::scanDirectory(QList<MusicAudioTrack> &newFiles, const QUrl &path)
{
    if (d->mStopRequest == 1) {
        return;
    }

    QDir rootDirectory(path.toLocalFile());
    rootDirectory.refresh();

    if (rootDirectory.exists()) {
        watchPath(path.toLocalFile());
    }

    auto &currentDirectoryListingFiles = d->mDiscoveredFiles[path];

    auto currentFilesList = QSet<QUrl>();

    rootDirectory.refresh();
    const auto entryList = rootDirectory.entryInfoList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);
    for (const auto &oneEntry : entryList) {
        auto newFilePath = QUrl::fromLocalFile(oneEntry.canonicalFilePath());

        if (oneEntry.isDir() || oneEntry.isFile()) {
            currentFilesList.insert(newFilePath);
        }
    }

    auto removedTracks = QVector<QPair<QUrl, bool>>();
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

    for (const auto &newFilePath : currentFilesList) {
        QFileInfo oneEntry(newFilePath.toLocalFile());

        auto itFilePath = std::find(currentDirectoryListingFiles.begin(), currentDirectoryListingFiles.end(), QPair<QUrl, bool>{newFilePath, oneEntry.isFile()});

        if (itFilePath != currentDirectoryListingFiles.end()) {
            continue;
        }

        if (oneEntry.isDir()) {
            addFileInDirectory(newFilePath, path);
            scanDirectory(newFiles, newFilePath);

            if (d->mStopRequest == 1) {
                break;
            }

            continue;
        }
        if (!oneEntry.isFile()) {
            continue;
        }

        auto newTrack = scanOneFile(newFilePath, oneEntry);

        if (newTrack.isValid() && d->mStopRequest == 0) {
            addCover(newTrack);

            addFileInDirectory(newTrack.resourceURI(), path);
            newFiles.push_back(newTrack);

            ++d->mImportedTracksCount;
            if (d->mImportedTracksCount % d->mNotificationUpdateInterval == 0) {
                d->mNotificationUpdateInterval = std::min(50, 1 + d->mNotificationUpdateInterval * 2);
            }

            if (newFiles.size() > d->mNewFilesEmitInterval && d->mStopRequest == 0) {
                d->mNewFilesEmitInterval = std::min(50, 1 + d->mNewFilesEmitInterval * d->mNewFilesEmitInterval);
                emitNewFiles(newFiles);
                newFiles.clear();
            }
        }

        if (d->mStopRequest == 1) {
            break;
        }
    }
}

void AbstractFileListing::directoryChanged(const QString &path)
{
    const auto directoryEntry = d->mDiscoveredFiles.find(QUrl::fromLocalFile(path));
    if (directoryEntry == d->mDiscoveredFiles.end()) {
        return;
    }

    Q_EMIT indexingStarted();

    scanDirectoryTree(path);

    Q_EMIT indexingFinished();
}

void AbstractFileListing::fileChanged(const QString &modifiedFileName)
{
    QFileInfo modifiedFileInfo(modifiedFileName);
    auto modifiedFile = QUrl::fromLocalFile(modifiedFileName);

    auto modifiedTrack = scanOneFile(modifiedFile, modifiedFileInfo);

    if (modifiedTrack.isValid()) {
        Q_EMIT modifyTracksList({modifiedTrack}, d->mAllAlbumCover);
    }
}

void AbstractFileListing::executeInit(QHash<QUrl, QDateTime> allFiles)
{
    d->mAllFiles = std::move(allFiles);
}

void AbstractFileListing::triggerRefreshOfContent()
{
    d->mImportedTracksCount = 0;
}

void AbstractFileListing::refreshContent()
{
    triggerRefreshOfContent();
}

MusicAudioTrack AbstractFileListing::scanOneFile(const QUrl &scanFile, const QFileInfo &scanFileInfo)
{
    MusicAudioTrack newTrack;

    qCDebug(orgKdeElisaIndexer) << "AbstractFileListing::scanOneFile" << scanFile;

    auto localFileName = scanFile.toLocalFile();

    const auto &fileMimeType = d->mMimeDb.mimeTypeForFile(localFileName);
    if (!fileMimeType.name().startsWith(QStringLiteral("audio/"))) {
        return newTrack;
    }

    if (scanFileInfo.exists()) {
        auto itExistingFile = d->mAllFiles.find(scanFile);
        if (itExistingFile != d->mAllFiles.end()) {
            if (*itExistingFile >= scanFileInfo.fileTime(QFile::FileModificationTime)) {
                d->mAllFiles.erase(itExistingFile);
                return newTrack;
            }
        }
    }

    newTrack = d->mFileScanner.scanOneFile(scanFile, d->mMimeDb);

    if (newTrack.isValid()) {
        newTrack.setHasEmbeddedCover(checkEmbeddedCoverImage(localFileName));
        newTrack.setFileModificationTime(scanFileInfo.fileTime(QFile::FileModificationTime));

        if (scanFileInfo.exists()) {
            watchPath(scanFile.toLocalFile());
        }
    }

    return newTrack;
}

void AbstractFileListing::watchPath(const QString &pathName)
{
    if (!d->mFileSystemWatcher.addPath(pathName)) {
        qCDebug(orgKdeElisaIndexer) << "AbstractFileListing::watchPath" << "fail for" << pathName;

        if (!d->mErrorWatchingFileSystemChanges) {
            d->mErrorWatchingFileSystemChanges = true;
            Q_EMIT errorWatchingFileSystemChanges();
        }
    }
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

    if (!newFiles.isEmpty() && d->mStopRequest == 0) {
        emitNewFiles(newFiles);
    }
}

void AbstractFileListing::setHandleNewFiles(bool handleThem)
{
    d->mHandleNewFiles = handleThem;
}

void AbstractFileListing::emitNewFiles(const QList<MusicAudioTrack> &tracks)
{
    Q_EMIT tracksList(tracks, d->mAllAlbumCover);
}

void AbstractFileListing::addCover(const MusicAudioTrack &newTrack)
{
    auto itCover = d->mAllAlbumCover.find(newTrack.albumName());
    if (itCover != d->mAllAlbumCover.end()) {
        return;
    }

    auto coverUrl = d->mFileScanner.searchForCoverFile(newTrack.resourceURI().toLocalFile());
    if (!coverUrl.isEmpty()) {
        d->mAllAlbumCover[newTrack.resourceURI().toString()] = coverUrl;
    }
}

void AbstractFileListing::removeDirectory(const QUrl &removedDirectory, QList<QUrl> &allRemovedFiles)
{
    const auto itRemovedDirectory = d->mDiscoveredFiles.find(removedDirectory);

    if (itRemovedDirectory == d->mDiscoveredFiles.end()) {
        return;
    }

    const auto &currentRemovedDirectory = *itRemovedDirectory;
    for (const auto &itFile : currentRemovedDirectory) {
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

QHash<QUrl, QDateTime> &AbstractFileListing::allFiles()
{
    return d->mAllFiles;
}

void AbstractFileListing::checkFilesToRemove()
{
    QList<QUrl> allRemovedFiles;

    for (auto itFile = d->mAllFiles.begin(); itFile != d->mAllFiles.end(); ++itFile) {
        allRemovedFiles.push_back(itFile.key());
    }

    if (!allRemovedFiles.isEmpty()) {
        setWaitEndTrackRemoval(true);
        Q_EMIT removedTracksList(allRemovedFiles);
    }
}

FileScanner &AbstractFileListing::fileScanner()
{
    return d->mFileScanner;
}

bool AbstractFileListing::checkEmbeddedCoverImage(const QString &localFileName)
{
    auto images = d->mFileScanner.findEmbeddedCoverImage(localFileName);
    if (images.contains(KFileMetaData::EmbeddedImageData::FrontCover)) {
        if (!images[KFileMetaData::EmbeddedImageData::FrontCover].isEmpty()) {
            return true;
        }
    }
    return false;
}

bool AbstractFileListing::waitEndTrackRemoval() const
{
    return d->mWaitEndTrackRemoval;
}

void AbstractFileListing::setWaitEndTrackRemoval(bool wait)
{
    d->mWaitEndTrackRemoval = wait;
}

const QMimeDatabase &AbstractFileListing::mimeDatabase() const
{
    return d->mMimeDb;
}


#include "moc_abstractfilelisting.cpp"
