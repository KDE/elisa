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

#include "musiclistenersmanager.h"

#include "config-upnp-qt.h"

#if defined UPNPQT_FOUND && UPNPQT_FOUND
#include "upnp/upnplistener.h"
#endif

#if defined KF5Baloo_FOUND && KF5Baloo_FOUND
#include "baloo/baloolistener.h"
#endif

#include "databaseinterface.h"
#include "mediaplaylist.h"
#include "file/filelistener.h"
#include "file/localfilelisting.h"
#include "trackslistener.h"
#include "notificationitem.h"
#include "elisaapplication.h"
#include "elisa_settings.h"
#include "models/allalbumsmodel.h"
#include "models/allartistsmodel.h"
#include "models/alltracksmodel.h"
#include "models/albummodel.h"
#include "models/abstractmediaproxymodel.h"
#include "models/allalbumsproxymodel.h"
#include "models/allartistsproxymodel.h"
#include "models/alltracksproxymodel.h"
#include "models/singleartistproxymodel.h"
#include "models/singlealbumproxymodel.h"

#include <KI18n/KLocalizedString>

#include <QThread>
#include <QMutex>
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>
#include <QList>
#include <QScopedPointer>
#include <QPointer>
#include <QFileSystemWatcher>

#include <QAction>

#include <list>

class MusicListenersManagerPrivate
{
public:

    QThread mDatabaseThread;

    QThread mListenerThread;

#if defined UPNPQT_FOUND && UPNPQT_FOUND
    UpnpListener mUpnpListener;
#endif

#if defined KF5Baloo_FOUND && KF5Baloo_FOUND
    std::unique_ptr<BalooListener> mBalooListener;
#endif

    std::list<std::unique_ptr<FileListener>> mFileListener;

    DatabaseInterface mDatabaseInterface;

    QFileSystemWatcher mConfigFileWatcher;

    ElisaApplication *mElisaApplication = nullptr;

    AllAlbumsModel mAllAlbumsModel;

    AllArtistsModel mAllArtistsModel;

    AllTracksModel mAllTracksModel;

    AlbumModel mAlbumModel;

    int mImportedTracksCount = 0;

    int mTotalImportedTracksCount = 0;

    int mActiveMusicListenersCount = 0;

    bool mIndexingRunning = false;

    bool mIndexerBusy = false;

};

MusicListenersManager::MusicListenersManager(QObject *parent)
    : QObject(parent), d(std::make_unique<MusicListenersManagerPrivate>())
{
    d->mListenerThread.start();
    d->mDatabaseThread.start();

    d->mDatabaseInterface.moveToThread(&d->mDatabaseThread);

    connect(&d->mDatabaseInterface, &DatabaseInterface::requestsInitDone,
            this, &MusicListenersManager::databaseReady);

    const auto &localDataPaths = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
    auto databaseFileName = QString();
    if (!localDataPaths.isEmpty()) {
        QDir myDataDirectory;
        myDataDirectory.mkpath(localDataPaths.first());
        databaseFileName = localDataPaths.first() + QStringLiteral("/elisaDatabase.db");
    }

    QMetaObject::invokeMethod(&d->mDatabaseInterface, "init", Qt::QueuedConnection,
                              Q_ARG(QString, QStringLiteral("listeners")), Q_ARG(QString, databaseFileName));

    connect(&d->mDatabaseInterface, &DatabaseInterface::artistsAdded,
            this, &MusicListenersManager::artistsAdded);
    connect(&d->mDatabaseInterface, &DatabaseInterface::tracksAdded,
            this, &MusicListenersManager::tracksAdded);
    connect(&d->mDatabaseInterface, &DatabaseInterface::artistRemoved,
            this, &MusicListenersManager::artistRemoved);
    connect(&d->mDatabaseInterface, &DatabaseInterface::albumRemoved,
            this, &MusicListenersManager::albumRemoved);
    connect(&d->mDatabaseInterface, &DatabaseInterface::trackRemoved,
            this, &MusicListenersManager::trackRemoved);
    connect(&d->mDatabaseInterface, &DatabaseInterface::artistModified,
            this, &MusicListenersManager::artistModified);
    connect(&d->mDatabaseInterface, &DatabaseInterface::albumModified,
            this, &MusicListenersManager::albumModified);
    connect(&d->mDatabaseInterface, &DatabaseInterface::trackModified,
            this, &MusicListenersManager::trackModified);

    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
            this, &MusicListenersManager::applicationAboutToQuit);

    connect(Elisa::ElisaConfiguration::self(), &Elisa::ElisaConfiguration::configChanged,
            this, &MusicListenersManager::configChanged);

    connect(&d->mConfigFileWatcher, &QFileSystemWatcher::fileChanged,
            this, &MusicListenersManager::configChanged);

    auto initialRootPath = Elisa::ElisaConfiguration::rootPath();
    if (initialRootPath.isEmpty()) {
        auto systemMusicPaths = QStandardPaths::standardLocations(QStandardPaths::MusicLocation);
        for (const auto &musicPath : qAsConst(systemMusicPaths)) {
            initialRootPath.push_back(musicPath);
        }

        Elisa::ElisaConfiguration::setRootPath(initialRootPath);
        Elisa::ElisaConfiguration::self()->save();
    }

    d->mConfigFileWatcher.addPath(Elisa::ElisaConfiguration::self()->config()->name());

    d->mAllAlbumsModel.setAllArtists(&d->mAllArtistsModel);
    d->mAllArtistsModel.setAllAlbums(&d->mAllAlbumsModel);

    connect(&d->mDatabaseInterface, &DatabaseInterface::albumsAdded,
            &d->mAllAlbumsModel, &AllAlbumsModel::albumsAdded);
    connect(&d->mDatabaseInterface, &DatabaseInterface::albumModified,
            &d->mAllAlbumsModel, &AllAlbumsModel::albumModified);
    connect(&d->mDatabaseInterface, &DatabaseInterface::albumRemoved,
            &d->mAllAlbumsModel, &AllAlbumsModel::albumRemoved);

    connect(&d->mDatabaseInterface, &DatabaseInterface::artistsAdded,
            &d->mAllArtistsModel, &AllArtistsModel::artistsAdded);
    connect(&d->mDatabaseInterface, &DatabaseInterface::artistModified,
            &d->mAllArtistsModel, &AllArtistsModel::artistModified);
    connect(&d->mDatabaseInterface, &DatabaseInterface::artistRemoved,
            &d->mAllArtistsModel, &AllArtistsModel::artistRemoved);

    connect(&d->mDatabaseInterface, &DatabaseInterface::tracksAdded,
            &d->mAllTracksModel, &AllTracksModel::tracksAdded);
    connect(&d->mDatabaseInterface, &DatabaseInterface::trackModified,
            &d->mAllTracksModel, &AllTracksModel::trackModified);
    connect(&d->mDatabaseInterface, &DatabaseInterface::trackRemoved,
            &d->mAllTracksModel, &AllTracksModel::trackRemoved);

    connect(&d->mDatabaseInterface, &DatabaseInterface::albumModified,
            &d->mAlbumModel, &AlbumModel::albumModified);
    connect(&d->mDatabaseInterface, &DatabaseInterface::albumRemoved,
            &d->mAlbumModel, &AlbumModel::albumRemoved);
    connect(&d->mAlbumModel, &AlbumModel::requestAlbumData,
            &d->mDatabaseInterface, &DatabaseInterface::getAlbumFromAlbumId);
    connect(&d->mDatabaseInterface, &DatabaseInterface::sentAlbumData,
            &d->mAlbumModel, &AlbumModel::setAlbumData);
}

MusicListenersManager::~MusicListenersManager()
= default;

DatabaseInterface *MusicListenersManager::viewDatabase() const
{
    return &d->mDatabaseInterface;
}

void MusicListenersManager::subscribeForTracks(MediaPlayList *client)
{
    auto helper = std::make_unique<TracksListener>(&d->mDatabaseInterface);

    helper->moveToThread(&d->mDatabaseThread);
    connect(&d->mDatabaseThread, &QThread::finished, helper.get(), &QObject::deleteLater);

    connect(this, &MusicListenersManager::trackRemoved, helper.get(), &TracksListener::trackRemoved);
    connect(this, &MusicListenersManager::tracksAdded, helper.get(), &TracksListener::tracksAdded);
    connect(this, &MusicListenersManager::trackModified, helper.get(), &TracksListener::trackModified);
    connect(this, &MusicListenersManager::removeTracksInError, &d->mDatabaseInterface, &DatabaseInterface::removeTracksList);
    connect(helper.get(), &TracksListener::trackHasChanged, client, &MediaPlayList::trackChanged);
    connect(helper.get(), &TracksListener::trackHasBeenRemoved, client, &MediaPlayList::trackRemoved);
    connect(helper.get(), &TracksListener::albumAdded, client, &MediaPlayList::albumAdded);
    connect(client, &MediaPlayList::newTrackByIdInList, helper.get(), &TracksListener::trackByIdInList);
    connect(client, &MediaPlayList::newTrackByNameInList, helper.get(), &TracksListener::trackByNameInList);
    connect(client, &MediaPlayList::newTrackByFileNameInList, helper.get(), &TracksListener::trackByFileNameInList);
    connect(client, &MediaPlayList::newArtistInList, helper.get(), &TracksListener::newArtistInList);
    helper.release();
}

int MusicListenersManager::importedTracksCount() const
{
    return d->mImportedTracksCount;
}

bool MusicListenersManager::isIndexingRunning() const
{
    return d->mIndexingRunning;
}

ElisaApplication *MusicListenersManager::elisaApplication() const
{
    return d->mElisaApplication;
}

QAbstractItemModel *MusicListenersManager::allAlbumsModel() const
{
    return &d->mAllAlbumsModel;
}

QAbstractItemModel *MusicListenersManager::allArtistsModel() const
{
    return &d->mAllArtistsModel;
}

QAbstractItemModel *MusicListenersManager::allTracksModel() const
{
    return &d->mAllTracksModel;
}

QAbstractItemModel *MusicListenersManager::albumModel() const
{
    return &d->mAlbumModel;
}

bool MusicListenersManager::indexerBusy() const
{
    return d->mIndexerBusy;
}

void MusicListenersManager::databaseReady()
{
    d->mIndexerBusy = true;
    Q_EMIT indexerBusyChanged();

    configChanged();
}

void MusicListenersManager::applicationAboutToQuit()
{
    d->mDatabaseInterface.applicationAboutToQuit();

    Q_EMIT applicationIsTerminating();

    d->mDatabaseThread.exit();
    d->mDatabaseThread.wait();

    d->mListenerThread.exit();
    d->mListenerThread.wait();
}

void MusicListenersManager::showConfiguration()
{
    auto configureAction = d->mElisaApplication->action(QStringLiteral("options_configure"));

    configureAction->trigger();
}

void MusicListenersManager::resetImportedTracksCounter()
{
#if defined KF5Baloo_FOUND && KF5Baloo_FOUND
    if (d->mBalooListener) {
        d->mBalooListener->resetImportedTracksCounter();
    }
#endif

    for (const auto &itFileListener : d->mFileListener) {
        itFileListener->resetImportedTracksCounter();
    }
}

void MusicListenersManager::setElisaApplication(ElisaApplication *elisaApplication)
{
    if (d->mElisaApplication == elisaApplication) {
        return;
    }

    d->mElisaApplication = elisaApplication;
    emit elisaApplicationChanged();
}

void MusicListenersManager::playBackError(QUrl sourceInError, QMediaPlayer::Error playerError)
{
    qDebug() << "MusicListenersManager::playBackError" << sourceInError;

    if (playerError == QMediaPlayer::ResourceError) {
        Q_EMIT removeTracksInError({sourceInError});

        if (sourceInError.isLocalFile()) {
            Q_EMIT displayTrackError(sourceInError.toLocalFile());
        } else {
            Q_EMIT displayTrackError(sourceInError.toString());
        }
    }
}

void MusicListenersManager::configChanged()
{
    auto currentConfiguration = Elisa::ElisaConfiguration::self();

    d->mConfigFileWatcher.addPath(currentConfiguration->config()->name());

    currentConfiguration->load();

#if defined KF5Baloo_FOUND && KF5Baloo_FOUND
    if (currentConfiguration->balooIndexer() && !d->mBalooListener) {
        d->mBalooListener = std::make_unique<BalooListener>();
        d->mBalooListener->moveToThread(&d->mListenerThread);
        d->mBalooListener->setDatabaseInterface(&d->mDatabaseInterface);
        connect(this, &MusicListenersManager::applicationIsTerminating,
                d->mBalooListener.get(), &BalooListener::applicationAboutToQuit, Qt::DirectConnection);
        connect(d->mBalooListener.get(), &BalooListener::indexingStarted,
                this, &MusicListenersManager::monitorStartingListeners);
        connect(d->mBalooListener.get(), &BalooListener::indexingFinished,
                this, &MusicListenersManager::monitorEndingListeners);
        connect(d->mBalooListener.get(), &BalooListener::clearDatabase,
                &d->mDatabaseInterface, &DatabaseInterface::removeAllTracksFromSource);
        connect(d->mBalooListener.get(), &BalooListener::importedTracksCountChanged,
                this, &MusicListenersManager::computeImportedTracksCount);
        connect(d->mBalooListener.get(), &BalooListener::newNotification,
                this, &MusicListenersManager::newNotification);
        connect(d->mBalooListener.get(), &BalooListener::closeNotification,
                this, &MusicListenersManager::closeNotification);

        QMetaObject::invokeMethod(d->mBalooListener.get(), "performInitialScan", Qt::QueuedConnection);
    } else if (!currentConfiguration->balooIndexer() && d->mBalooListener) {
        QMetaObject::invokeMethod(d->mBalooListener.get(), "quitListener", Qt::QueuedConnection);
        d->mBalooListener.reset();
    }
#endif
#if defined UPNPQT_FOUND && UPNPQT_FOUND
    d->mUpnpListener.setDatabaseInterface(&d->mDatabaseInterface);
    d->mUpnpListener.moveToThread(&d->mDatabaseThread);
    connect(this, &MusicListenersManager::applicationIsTerminating,
            &d->mUpnpListener, &UpnpListener::applicationAboutToQuit, Qt::DirectConnection);
#endif

    if (currentConfiguration->elisaFilesIndexer())
    {
        const auto &allRootPaths = currentConfiguration->rootPath();
        for (auto itFileListener = d->mFileListener.begin(); itFileListener != d->mFileListener.end(); ) {
            const auto &currentRootPath = (*itFileListener)->localFileIndexer().rootPath();
            auto itPath = std::find(allRootPaths.begin(), allRootPaths.end(), currentRootPath);

            if (itPath == allRootPaths.end()) {
                d->mDatabaseInterface.removeAllTracksFromSource((*itFileListener)->fileListing()->sourceName());
                itFileListener = d->mFileListener.erase(itFileListener);
            } else {
                ++itFileListener;
            }
        }

        for (const auto &oneRootPath : allRootPaths) {
            auto itPath = std::find_if(d->mFileListener.begin(), d->mFileListener.end(),
                                       [&oneRootPath](const auto &value)->bool {return value->localFileIndexer().rootPath() == oneRootPath;});
            if (itPath == d->mFileListener.end()) {
                auto newFileIndexer = std::make_unique<FileListener>();

                newFileIndexer->setDatabaseInterface(&d->mDatabaseInterface);
                newFileIndexer->moveToThread(&d->mListenerThread);
                connect(this, &MusicListenersManager::applicationIsTerminating,
                        newFileIndexer.get(), &FileListener::applicationAboutToQuit, Qt::DirectConnection);
                connect(newFileIndexer.get(), &FileListener::indexingStarted,
                        this, &MusicListenersManager::monitorStartingListeners);
                connect(newFileIndexer.get(), &FileListener::indexingFinished,
                        this, &MusicListenersManager::monitorEndingListeners);
                connect(newFileIndexer.get(), &FileListener::importedTracksCountChanged,
                        this, &MusicListenersManager::computeImportedTracksCount);
                connect(newFileIndexer.get(), &FileListener::newNotification,
                        this, &MusicListenersManager::newNotification);
                connect(newFileIndexer.get(), &FileListener::closeNotification,
                        this, &MusicListenersManager::closeNotification);

                newFileIndexer->setRootPath(oneRootPath);

                QMetaObject::invokeMethod(newFileIndexer.get(), "performInitialScan", Qt::QueuedConnection);

                d->mFileListener.emplace_back(std::move(newFileIndexer));
            }
        }
    }
}

void MusicListenersManager::computeImportedTracksCount()
{
#if defined KF5Baloo_FOUND && KF5Baloo_FOUND
    if (d->mBalooListener) {
        d->mImportedTracksCount = d->mBalooListener->importedTracksCount();
    } else {
        d->mImportedTracksCount = 0;
    }
#else
    d->mImportedTracksCount = 0;
#endif

    for (const auto &itFileListener : d->mFileListener) {
        d->mImportedTracksCount += itFileListener->importedTracksCount();
    }

    if (d->mImportedTracksCount && d->mIndexerBusy) {
        d->mIndexerBusy = false;
        Q_EMIT indexerBusyChanged();
    }

    if (d->mImportedTracksCount >= 4) {
        Q_EMIT closeNotification(QStringLiteral("notEnoughTracks"));
    }

    Q_EMIT importedTracksCountChanged();
}

void MusicListenersManager::monitorStartingListeners()
{
    if (d->mActiveMusicListenersCount == 0) {
        d->mIndexingRunning = true;
        Q_EMIT indexingRunningChanged();
    }

    ++d->mActiveMusicListenersCount;
}

void MusicListenersManager::monitorEndingListeners(int tracksCount)
{
    --d->mActiveMusicListenersCount;

    d->mTotalImportedTracksCount += tracksCount;

    if (d->mActiveMusicListenersCount == 0) {
        if (d->mTotalImportedTracksCount < 4 && d->mElisaApplication) {
            NotificationItem notEnoughTracks;

            notEnoughTracks.setNotificationId(QStringLiteral("notEnoughTracks"));

            notEnoughTracks.setTargetObject(this);

            notEnoughTracks.setMessage(i18nc("No track found message", "No track have been found"));

            auto configureAction = d->mElisaApplication->action(QStringLiteral("options_configure"));

            notEnoughTracks.setMainButtonText(configureAction->text());
            notEnoughTracks.setMainButtonIconName(configureAction->icon().name());
            notEnoughTracks.setMainButtonMethodName(QStringLiteral("showConfiguration"));

            Q_EMIT newNotification(notEnoughTracks);
        }

        d->mIndexingRunning = false;
        Q_EMIT indexingRunningChanged();

        QMetaObject::invokeMethod(&d->mDatabaseInterface, "cleanInvalidTracks", Qt::QueuedConnection);
    }
}


#include "moc_musiclistenersmanager.cpp"
