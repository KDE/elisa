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

#include "localfilelisting.h"

#include "abstractfile/indexercommon.h"

#include <QThread>
#include <QHash>
#include <QFileInfo>
#include <QDir>
#include <QFileSystemWatcher>
#include <QStandardPaths>

#include <QDebug>

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

void LocalFileListing::executeInit(QHash<QUrl, QDateTime> allFiles)
{
    if (!isActive()) {
        qCDebug(orgKdeElisaIndexer()) << "LocalFileListing::executeInit is inactive";
        return;
    }

    qCDebug(orgKdeElisaIndexer()) << "LocalFileListing::executeInit" << "with" << allFiles.size() << "files";
    AbstractFileListing::executeInit(std::move(allFiles));
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

    checkFilesToRemove();

    if (!waitEndTrackRemoval()) {
        Q_EMIT indexingFinished();
    }
}

void LocalFileListing::triggerStop()
{
    qCDebug(orgKdeElisaIndexer()) << "LocalFileListing::triggerStop";
    AbstractFileListing::triggerStop();
}


#include "moc_localfilelisting.cpp"
