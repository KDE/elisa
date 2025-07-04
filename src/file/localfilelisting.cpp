/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "localfilelisting.h"

#include "filescanner.h"
#include "abstractfile/indexercommon.h"

#include <QThread>
#include <QHash>
#include <QStandardPaths>


#include <algorithm>

class LocalFileListingPrivate
{
public:

};

LocalFileListing::LocalFileListing(QObject *parent) : AbstractFileListing(parent), d(std::make_unique<LocalFileListingPrivate>())
{
}

LocalFileListing::~LocalFileListing()
= default;

void LocalFileListing::executeInit(const QHash<QUrl, QDateTime> &allFiles)
{
    if (!isActive()) {
        qCDebug(orgKdeElisaIndexer()) << "LocalFileListing::executeInit is inactive";
        return;
    }

    qCDebug(orgKdeElisaIndexer()) << "LocalFileListing::executeInit" << "with" << allFiles.size() << "files";
    AbstractFileListing::executeInit(allFiles);
}

void LocalFileListing::triggerRefreshOfContent()
{
    qCDebug(orgKdeElisaIndexer()) << "LocalFileListing::triggerRefreshOfContent";

    if (!isActive()) {
        qCDebug(orgKdeElisaIndexer()) << "LocalFileListing::triggerRefreshOfContent is inactive";
        return;
    }

    Q_EMIT indexingStarted();

    qCDebug(orgKdeElisaIndexer()) << "LocalFileListing::triggerRefreshOfContent" << allRootPaths();

    AbstractFileListing::triggerRefreshOfContent();

    const auto &rootPaths = allRootPaths();
    for (const auto &onePath : rootPaths) {
        scanDirectoryTree(onePath);
    }

    setWaitEndTrackRemoval(false);

    if (!waitEndTrackRemoval()) {
        Q_EMIT indexingFinished();
    }
}

void LocalFileListing::triggerStop()
{
    qCDebug(orgKdeElisaIndexer()) << "LocalFileListing::triggerStop";
    AbstractFileListing::triggerStop();
}

DataTypes::TrackDataType LocalFileListing::scanOneFile(const QUrl &scanFile, const QFileInfo &scanFileInfo, FileSystemWatchingModes watchForFileSystemChanges)
{
    auto trackData = AbstractFileListing::scanOneFile(scanFile, scanFileInfo, watchForFileSystemChanges);

    if (!trackData.isValid()) {
        qCDebug(orgKdeElisaIndexer()) << "LocalFileListing::scanOneFile" << scanFile << "invalid track";
    }

    return trackData;
}


#include "moc_localfilelisting.cpp"
