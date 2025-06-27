/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef ABSTRACTFILELISTING_H
#define ABSTRACTFILELISTING_H

#include "elisaLib_export.h"
#include "datatypes.h"

#include <QObject>
#include <QString>
#include <QUrl>
#include <QHash>
#include <QDateTime>

#include <memory>

class AbstractFileListingPrivate;
class FileScanner;
class QFileInfo;

class ELISALIB_EXPORT AbstractFileListing : public QObject
{

    Q_OBJECT

public:

    enum FileSystemWatchingMode {
        DoNotWatchFileSystemChanges,
        WatchChangedDirectories = 1 << 0,
        WatchChangedFiles = 1 << 1,
    };

    using FileSystemWatchingModes = QFlags<FileSystemWatchingMode>;

    explicit AbstractFileListing(QObject *parent = nullptr);

    ~AbstractFileListing() override;

    virtual void applicationAboutToQuit();

    [[nodiscard]] const QStringList& allRootPaths() const;

    [[nodiscard]] virtual bool canHandleRootPaths() const;

Q_SIGNALS:

    void tracksList(const DataTypes::ListTrackDataType &tracks);

    void removedTracksList(const QList<QUrl> &removedTracks);

    void modifyTracksList(const DataTypes::ListTrackDataType &modifiedTracks);

    void indexingStarted();

    void indexingFinished();

    void askRestoredTracks();

    void errorWatchingFileSystemChanges();

public Q_SLOTS:

    /**
     * Re-scan all root directories after setting the indexed tracks
     */
    void setIndexedTracks(const QHash<QUrl, QDateTime> &allTracks);

    /**
     * Re-scan all root directories after clearing the indexed tracks
     */
    void resetAndRefreshContent();

    /**
     * Re-scan all root directories without clearing the index
     */
    void refreshContent();

    void init();

    void stop();

    void newTrackFile(const DataTypes::TrackDataType &partialTrack);

    void setAllRootPaths(const QStringList &allRootPaths);

    void databaseFinishedInsertingTracksList();

    void databaseFinishedRemovingTracksList();

protected Q_SLOTS:

    void directoryChanged(const QString &path);

    void fileChanged(const QString &modifiedFileName);

protected:

    virtual void executeInit(const QHash<QUrl, QDateTime> &allFiles);

    virtual void triggerRefreshOfContent();

    virtual void triggerStop();

    void scanDirectory(DataTypes::ListTrackDataType &newFiles, const QUrl &path, FileSystemWatchingModes watchForFileSystemChanges);

    virtual DataTypes::TrackDataType scanOneFile(const QUrl &scanFile, const QFileInfo &scanFileInfo, FileSystemWatchingModes watchForFileSystemChanges);

    void watchPath(const QString &pathName);

    void addFileInDirectory(const QUrl &newFile, const QUrl &directoryName, FileSystemWatchingModes watchForFileSystemChanges);

    void scanDirectoryTree(const QString &path);

    void setHandleNewFiles(bool handleThem);

    void emitNewFiles(const DataTypes::ListTrackDataType &tracks);

    void removeDirectory(const QUrl &removedDirectory, QList<QUrl> &allRemovedFiles);

    void removeFile(const QUrl &oneRemovedTrack, QList<QUrl> &allRemovedFiles);

    FileScanner& fileScanner();

    [[nodiscard]] bool waitEndTrackRemoval() const;

    void setWaitEndTrackRemoval(bool wait);

    [[nodiscard]] bool isActive() const;

    [[nodiscard]] bool fileModifiedSinceLastScan(const QUrl &path, const QUrl &parentPath, const QDateTime &lastModified) const;

private:

    std::unique_ptr<AbstractFileListingPrivate> d;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(AbstractFileListing::FileSystemWatchingModes)

#endif // ABSTRACTFILELISTING_H
