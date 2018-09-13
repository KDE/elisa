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

#include "abstractfilelistener.h"

#include "abstractfilelisting.h"
#include "databaseinterface.h"
#include "notificationitem.h"

#include <QThread>

class AbstractFileListenerPrivate
{
public:

    QThread mFileQueryThread;

    AbstractFileListing *mFileListing = nullptr;

};

AbstractFileListener::AbstractFileListener(QObject *parent)
    : QObject(parent), d(std::make_unique<AbstractFileListenerPrivate>())
{
}

AbstractFileListener::~AbstractFileListener()
{
    d->mFileQueryThread.quit();
    d->mFileQueryThread.wait();
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
        connect(d->mFileListing, &AbstractFileListing::askRestoredTracks,
                model, &DatabaseInterface::askRestoredTracks);
        connect(model, &DatabaseInterface::restoredTracks,
                d->mFileListing, &AbstractFileListing::restoredTracks);

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

void AbstractFileListener::setFileListing(AbstractFileListing *fileIndexer)
{
    d->mFileListing = fileIndexer;
    d->mFileQueryThread.start();
    d->mFileListing->moveToThread(&d->mFileQueryThread);
    connect(fileIndexer, &AbstractFileListing::indexingStarted,
            this, &AbstractFileListener::indexingStarted);
    connect(fileIndexer, &AbstractFileListing::indexingFinished,
            this, &AbstractFileListener::indexingFinished);
    connect(fileIndexer, &AbstractFileListing::newNotification,
            this, &AbstractFileListener::newNotification);
    connect(fileIndexer, &AbstractFileListing::closeNotification,
            this, &AbstractFileListener::closeNotification);
}

AbstractFileListing *AbstractFileListener::fileListing() const
{
    return d->mFileListing;
}


#include "moc_abstractfilelistener.cpp"
