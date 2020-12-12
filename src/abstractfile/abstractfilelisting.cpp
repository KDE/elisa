/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "abstractfilelisting.h"

#include "config-upnp-qt.h"

#include "abstractfile/indexercommon.h"

#include "filescanner.h"

#include <QThread>
#include <QHash>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QFileSystemWatcher>
#include <QSet>
#include <QPair>
#include <QAtomicInt>


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

    QHash<QUrl, QDateTime> mAllFiles;

    QAtomicInt mStopRequest = 0;

    int mImportedTracksCount = 0;

    int mNewFilesEmitInterval = 1;

    bool mHandleNewFiles = true;

    bool mWaitEndTrackRemoval = false;

    bool mErrorWatchingFileSystemChanges = false;

    bool mIsActive = false;

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
    qCDebug(orgKdeElisaIndexer()) << "AbstractFileListing::init";

    d->mIsActive = true;

    Q_EMIT askRestoredTracks();
}

void AbstractFileListing::stop()
{
    d->mIsActive = false;

    triggerStop();
}

void AbstractFileListing::newTrackFile(const DataTypes::TrackDataType &partialTrack)
{
    auto scanFileInfo = QFileInfo(partialTrack.resourceURI().toLocalFile());
    const auto &newTrack = scanOneFile(partialTrack.resourceURI(), scanFileInfo, WatchChangedDirectories | WatchChangedFiles);

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

bool AbstractFileListing::canHandleRootPaths() const
{
    return true;
}

void AbstractFileListing::scanDirectory(DataTypes::ListTrackDataType &newFiles, const QUrl &path, FileSystemWatchingModes watchForFileSystemChanges)
{
    if (d->mStopRequest == 1) {
        return;
    }

    QDir rootDirectory(path.toLocalFile());
    rootDirectory.refresh();

    if (rootDirectory.exists()) {
        if (watchForFileSystemChanges & WatchChangedDirectories) {
            watchPath(path.toLocalFile());
        }
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
            addFileInDirectory(newFilePath, path, WatchChangedDirectories | WatchChangedFiles);
            scanDirectory(newFiles, newFilePath, WatchChangedDirectories | WatchChangedFiles);

            if (d->mStopRequest == 1) {
                break;
            }

            continue;
        }
        if (!oneEntry.isFile()) {
            continue;
        }

        auto itExistingFile = allFiles().find(newFilePath);
        if (itExistingFile != allFiles().end()) {
            if (*itExistingFile >= oneEntry.metadataChangeTime()) {
                allFiles().erase(itExistingFile);
                qCDebug(orgKdeElisaIndexer()) << "AbstractFileListing::scanDirectory" << newFilePath << "file not modified since last scan";
                continue;
            }
        }

        auto newTrack = scanOneFile(newFilePath, oneEntry, WatchChangedDirectories | WatchChangedFiles);

        if (newTrack.isValid() && d->mStopRequest == 0) {
            addCover(newTrack);

            addFileInDirectory(newTrack.resourceURI(), path, WatchChangedDirectories | WatchChangedFiles);
            newFiles.push_back(newTrack);

            ++d->mImportedTracksCount;

            if (newFiles.size() > d->mNewFilesEmitInterval && d->mStopRequest == 0) {
                d->mNewFilesEmitInterval = std::min(50, 1 + d->mNewFilesEmitInterval * d->mNewFilesEmitInterval);
                emitNewFiles(newFiles);
                newFiles.clear();
            }
        } else {
            qCDebug(orgKdeElisaIndexer()) << "AbstractFileListing::scanDirectory" << newFilePath << "is not a valid track";
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

    auto modifiedTrack = scanOneFile(modifiedFile, modifiedFileInfo, WatchChangedDirectories | WatchChangedFiles);

    if (modifiedTrack.isValid()) {
        Q_EMIT modifyTracksList({modifiedTrack}, d->mAllAlbumCover);
    }
}

void AbstractFileListing::executeInit(QHash<QUrl, QDateTime> allFiles)
{
    d->mAllFiles = std::move(allFiles);
}

void AbstractFileListing::triggerStop()
{
}

void AbstractFileListing::triggerRefreshOfContent()
{
    d->mImportedTracksCount = 0;
}

void AbstractFileListing::refreshContent()
{
    triggerRefreshOfContent();
}

DataTypes::TrackDataType AbstractFileListing::scanOneFile(const QUrl &scanFile, const QFileInfo &scanFileInfo, FileSystemWatchingModes watchForFileSystemChanges)
{
    DataTypes::TrackDataType newTrack;

    qCDebug(orgKdeElisaIndexer) << "AbstractFileListing::scanOneFile" << scanFile;

    auto localFileName = scanFile.toLocalFile();

    if (!d->mFileScanner.shouldScanFile(localFileName)) {
        qCDebug(orgKdeElisaIndexer) << "AbstractFileListing::scanOneFile" << "invalid mime type";
        return newTrack;
    }

    if (scanFileInfo.exists()) {
        auto itExistingFile = d->mAllFiles.find(scanFile);
        if (itExistingFile != d->mAllFiles.end()) {
            if (*itExistingFile >= scanFileInfo.metadataChangeTime()) {
                d->mAllFiles.erase(itExistingFile);
                qCDebug(orgKdeElisaIndexer) << "AbstractFileListing::scanOneFile" << "not changed file";
                return newTrack;
            }
        }
    }

    newTrack = d->mFileScanner.scanOneFile(scanFile, scanFileInfo);

    if (newTrack.isValid() && scanFileInfo.exists()) {
        if (watchForFileSystemChanges & WatchChangedFiles) {
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

void AbstractFileListing::addFileInDirectory(const QUrl &newFile, const QUrl &directoryName, FileSystemWatchingModes watchForFileSystemChanges)
{
    const auto directoryEntry = d->mDiscoveredFiles.find(directoryName);
    if (directoryEntry == d->mDiscoveredFiles.end()) {
        if (watchForFileSystemChanges & WatchChangedDirectories) {
            watchPath(directoryName.toLocalFile());
        }

        QDir currentDirectory(directoryName.toLocalFile());
        if (currentDirectory.cdUp()) {
            const auto parentDirectoryName = currentDirectory.absolutePath();
            const auto parentDirectory = QUrl::fromLocalFile(parentDirectoryName);
            const auto parentDirectoryEntry = d->mDiscoveredFiles.find(parentDirectory);
            if (parentDirectoryEntry == d->mDiscoveredFiles.end()) {
                if (watchForFileSystemChanges & WatchChangedDirectories) {
                    watchPath(parentDirectoryName);
                }
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
    auto newFiles = DataTypes::ListTrackDataType();

    qCDebug(orgKdeElisaIndexer()) << "AbstractFileListing::scanDirectoryTree" << path;

    scanDirectory(newFiles, QUrl::fromLocalFile(path), WatchChangedDirectories | WatchChangedFiles);

    if (!newFiles.isEmpty() && d->mStopRequest == 0) {
        emitNewFiles(newFiles);
    }
}

void AbstractFileListing::setHandleNewFiles(bool handleThem)
{
    d->mHandleNewFiles = handleThem;
}

void AbstractFileListing::emitNewFiles(const DataTypes::ListTrackDataType &tracks)
{
    Q_EMIT tracksList(tracks, d->mAllAlbumCover);
}

void AbstractFileListing::addCover(const DataTypes::TrackDataType &newTrack)
{
    auto itCover = d->mAllAlbumCover.find(newTrack.album());
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

    qCDebug(orgKdeElisaIndexer()) << "AbstractFileListing::checkFilesToRemove" << allRemovedFiles.size();

    if (!allRemovedFiles.isEmpty()) {
        setWaitEndTrackRemoval(true);
        Q_EMIT removedTracksList(allRemovedFiles);
    }
}

FileScanner &AbstractFileListing::fileScanner()
{
    return d->mFileScanner;
}

bool AbstractFileListing::waitEndTrackRemoval() const
{
    return d->mWaitEndTrackRemoval;
}

void AbstractFileListing::setWaitEndTrackRemoval(bool wait)
{
    d->mWaitEndTrackRemoval = wait;
}

bool AbstractFileListing::isActive() const
{
    return d->mIsActive;
}


#include "moc_abstractfilelisting.cpp"
