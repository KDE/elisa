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

#include "musicaudiotrack.h"

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

};

LocalFileListing::LocalFileListing(QObject *parent) : AbstractFileListing(QStringLiteral("local"), parent), d(std::make_unique<LocalFileListingPrivate>())
{
}

LocalFileListing::~LocalFileListing()
= default;

QString LocalFileListing::rootPath() const
{
    return d->mRootPath;
}

void LocalFileListing::setRootPath(const QString &rootPath)
{
    if (d->mRootPath == rootPath) {
        return;
    }

    d->mRootPath = rootPath;
    Q_EMIT rootPathChanged();

    setSourceName(rootPath);
}

void LocalFileListing::executeInit(QHash<QUrl, QDateTime> allFiles)
{
    AbstractFileListing::executeInit(std::move(allFiles));
}

void LocalFileListing::triggerRefreshOfContent()
{
    Q_EMIT indexingStarted();

    AbstractFileListing::triggerRefreshOfContent();

    scanDirectoryTree(d->mRootPath);

    checkFilesToRemove();

    Q_EMIT indexingFinished();
}


#include "moc_localfilelisting.cpp"
