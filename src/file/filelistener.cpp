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

#include "filelistener.h"

#include "localfilelisting.h"
#include "databaseinterface.h"

#include <QThread>

class FileListenerPrivate
{
public:

    LocalFileListing mLocalFileIndexer;

};

FileListener::FileListener(QObject *parent) : AbstractFileListener(parent), d(new FileListenerPrivate)
{
    setFileListing(&d->mLocalFileIndexer);
    connect(&d->mLocalFileIndexer, &LocalFileListing::rootPathChanged,
            this, &FileListener::rootPathChanged);
}

FileListener::~FileListener()
{
    delete d;
}

const LocalFileListing& FileListener::localFileIndexer() const
{
    return d->mLocalFileIndexer;
}

QString FileListener::rootPath() const
{
    return d->mLocalFileIndexer.rootPath();
}

void FileListener::setRootPath(const QString &rootPath)
{
    d->mLocalFileIndexer.setRootPath(rootPath);
}


#include "moc_filelistener.cpp"
