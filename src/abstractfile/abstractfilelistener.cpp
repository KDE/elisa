/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "abstractfilelistener.h"

#include "abstractfilelisting.h"
#include "databaseinterface.h"

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
        connect(d->mFileListing, &AbstractFileListing::modifyTracksList, model, &DatabaseInterface::insertTracksList);
        connect(d->mFileListing, &AbstractFileListing::askRestoredTracks,
                model, &DatabaseInterface::askRestoredTracks);
        connect(model, &DatabaseInterface::restoredTracks,
                d->mFileListing, &AbstractFileListing::restoredTracks);
        connect(model, &DatabaseInterface::cleanedDatabase,
                d->mFileListing, &AbstractFileListing::refreshContent);
        connect(model, &DatabaseInterface::finishRemovingTracksList,
                d->mFileListing, &AbstractFileListing::databaseFinishedRemovingTracksList);
        connect(model, &DatabaseInterface::finishInsertingTracksList,
                d->mFileListing, &AbstractFileListing::databaseFinishedInsertingTracksList);
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

    Q_EMIT clearDatabase();
}

void AbstractFileListener::setAllRootPaths(const QStringList &allRootPaths)
{
    d->mFileListing->setAllRootPaths(allRootPaths);
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
}

AbstractFileListing *AbstractFileListener::fileListing() const
{
    return d->mFileListing;
}

bool AbstractFileListener::canHandleRootPaths() const
{
    return d->mFileListing->canHandleRootPaths();
}


#include "moc_abstractfilelistener.cpp"
