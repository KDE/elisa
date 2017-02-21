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

    QThread mFileQueryThread;

    LocalFileListing mFileListing;

};

FileListener::FileListener(QObject *parent) : QObject(parent), d(new FileListenerPrivate)
{
    d->mFileQueryThread.start();
    d->mFileListing.moveToThread(&d->mFileQueryThread);
}

FileListener::~FileListener()
{
    delete d;
}

DatabaseInterface *FileListener::databaseInterface() const
{
    return nullptr;
}

void FileListener::setDatabaseInterface(DatabaseInterface *model)
{
    if (model) {
        connect(this, &FileListener::databaseReady, &d->mFileListing, &LocalFileListing::databaseIsReady);
        connect(&d->mFileListing, &LocalFileListing::initialTracksListRequired, this, &FileListener::initialTracksListRequired);
        connect(this, &FileListener::initialTracksList, &d->mFileListing, &LocalFileListing::initialTracksList);
        connect(&d->mFileListing, &LocalFileListing::tracksList, model, &DatabaseInterface::insertTracksList);
        connect(&d->mFileListing, &LocalFileListing::removedTracksList, model, &DatabaseInterface::removeTracksList);

        QMetaObject::invokeMethod(&d->mFileListing, "init", Qt::QueuedConnection);
    }

    Q_EMIT databaseInterfaceChanged();
}

void FileListener::applicationAboutToQuit()
{
    d->mFileQueryThread.exit();
    d->mFileQueryThread.wait();
}


#include "moc_filelistener.cpp"
