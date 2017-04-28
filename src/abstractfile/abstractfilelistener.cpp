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

#include <QThread>

class AbstractFileListenerPrivate
{
public:

    explicit AbstractFileListenerPrivate(AbstractFileListing *aFileListing) : mFileListing(aFileListing)
    {
    }

    QThread mFileQueryThread;

    AbstractFileListing *mFileListing;

};

AbstractFileListener::AbstractFileListener(AbstractFileListing *aFileListing, QObject *parent) : QObject(parent), d(new AbstractFileListenerPrivate(aFileListing))
{
    d->mFileQueryThread.start();
    d->mFileListing->moveToThread(&d->mFileQueryThread);
}

AbstractFileListener::~AbstractFileListener()
{
    delete d->mFileListing;
    delete d;
}

DatabaseInterface *AbstractFileListener::databaseInterface() const
{
    return nullptr;
}

void AbstractFileListener::setDatabaseInterface(DatabaseInterface *model)
{
    if (model) {
        connect(this, &AbstractFileListener::databaseReady, d->mFileListing, &AbstractFileListing::databaseIsReady);
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

AbstractFileListing *AbstractFileListener::fileListing() const
{
    return d->mFileListing;
}


#include "moc_abstractfilelistener.cpp"
