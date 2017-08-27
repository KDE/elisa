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

#include "abstractfilelistener.h"

#include "abstractfilelisting.h"
#include "databaseinterface.h"
#include "notificationitem.h"

#include <QThread>

class AbstractFileListenerPrivate
{
public:

    explicit AbstractFileListenerPrivate()
    {
    }

    QThread mFileQueryThread;

    AbstractFileListing *mFileListing = nullptr;

};

AbstractFileListener::AbstractFileListener(QObject *parent)
    : QObject(parent), d(new AbstractFileListenerPrivate)
{
}

AbstractFileListener::~AbstractFileListener()
{
}

DatabaseInterface *AbstractFileListener::databaseInterface() const
{
    return nullptr;
}

void AbstractFileListener::setDatabaseInterface(DatabaseInterface *model)
{
    if (model) {
        connect(this, &AbstractFileListener::newTrackFile, d->mFileListing, &AbstractFileListing::newTrackFile);
        connect(d->mFileListing, &AbstractFileListing::tracksList, model, &DatabaseInterface::insertTracksList);
        connect(d->mFileListing, &AbstractFileListing::removedTracksList, model, &DatabaseInterface::removeTracksList);
        connect(d->mFileListing, &AbstractFileListing::modifyTracksList, model, &DatabaseInterface::modifyTracksList);

        QMetaObject::invokeMethod(d->mFileListing, "init", Qt::QueuedConnection);
    }

    Q_EMIT databaseInterfaceChanged();
}

void AbstractFileListener::applicationAboutToQuit()
{
    d->mFileListing->applicationAboutToQuit();

    d->mFileQueryThread.exit();
    d->mFileQueryThread.wait();
}

void AbstractFileListener::quitListener()
{
    applicationAboutToQuit();

    Q_EMIT clearDatabase(d->mFileListing->sourceName());
}

void AbstractFileListener::resetImportedTracksCounter()
{
    d->mFileListing->resetImportedTracksCounter();
}

void AbstractFileListener::setFileListing(AbstractFileListing *fileIndexer)
{
    d->mFileListing = fileIndexer;
    d->mFileQueryThread.start();
    d->mFileListing->moveToThread(&d->mFileQueryThread);
    connect(fileIndexer, &AbstractFileListing::indexingStarted,
            this, &AbstractFileListener::indexingStarted);
    connect(fileIndexer, &AbstractFileListing::indexingFinished,
            this, &AbstractFileListener::indexingFinished);
    connect(fileIndexer, &AbstractFileListing::notification,
            this, &AbstractFileListener::notification);
    connect(fileIndexer, &AbstractFileListing::importedTracksCountChanged,
            this, &AbstractFileListener::importedTracksCountChanged);
}

AbstractFileListing *AbstractFileListener::fileListing() const
{
    return d->mFileListing;
}

int AbstractFileListener::importedTracksCount() const
{
    if (!d->mFileListing) {
        return 0;
    }

    return d->mFileListing->importedTracksCount();
}

void AbstractFileListener::performInitialScan()
{
    d->mFileListing->refreshContent();
}


#include "moc_abstractfilelistener.cpp"
