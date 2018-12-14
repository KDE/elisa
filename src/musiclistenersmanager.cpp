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
#include "modeldataloader.h"

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

    std::unique_ptr<TracksListener> mTracksListener;

    QFileSystemWatcher mConfigFileWatcher;

    ElisaApplication *mElisaApplication = nullptr;

    int mImportedTracksCount = 0;

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

    connect(&d->mDatabaseInterface, &DatabaseInterface::tracksAdded,
            this, &MusicListenersManager::increaseImportedTracksCount);
}

MusicListenersManager::~MusicListenersManager()
= default;

DatabaseInterface *MusicListenersManager::viewDatabase() const
{
    return &d->mDatabaseInterface;
}

void MusicListenersManager::subscribeForTracks(MediaPlayList *client)
{
    createTracksListener();
    connect(d->mTracksListener.get(), &TracksListener::trackHasChanged, client, &MediaPlayList::trackChanged);
    connect(d->mTracksListener.get(), &TracksListener::trackHasBeenRemoved, client, &MediaPlayList::trackRemoved);
    connect(d->mTracksListener.get(), &TracksListener::tracksListAdded, client, &MediaPlayList::tracksListAdded);
    connect(client, &MediaPlayList::newEntryInList, d->mTracksListener.get(), &TracksListener::newEntryInList);
    connect(client, &MediaPlayList::newTrackByNameInList, d->mTracksListener.get(), &TracksListener::trackByNameInList);
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

void MusicListenersManager::setElisaApplication(ElisaApplication *elisaApplication)
{
    if (d->mElisaApplication == elisaApplication) {
        return;
    }

    d->mElisaApplication = elisaApplication;
    emit elisaApplicationChanged();
}

void MusicListenersManager::playBackError(const QUrl &sourceInError, QMediaPlayer::Error playerError)
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

void MusicListenersManager::connectModel(ModelDataLoader *dataLoader)
{
    dataLoader->moveToThread(&d->mDatabaseThread);
    dataLoader->setDatabase(&d->mDatabaseInterface);
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
        connect(d->mBalooListener.get(), &BalooListener::newNotification,
                this, &MusicListenersManager::newNotification);
        connect(d->mBalooListener.get(), &BalooListener::closeNotification,
                this, &MusicListenersManager::closeNotification);
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
                connect(newFileIndexer.get(), &FileListener::newNotification,
                        this, &MusicListenersManager::newNotification);
                connect(newFileIndexer.get(), &FileListener::closeNotification,
                        this, &MusicListenersManager::closeNotification);

                newFileIndexer->setRootPath(oneRootPath);

                d->mFileListener.emplace_back(std::move(newFileIndexer));
            }
        }
    }
}

void MusicListenersManager::increaseImportedTracksCount(const DatabaseInterface::ListTrackDataType &allTracks)
{
    d->mImportedTracksCount += allTracks.size();

    if (d->mImportedTracksCount && d->mIndexerBusy) {
        d->mIndexerBusy = false;
        Q_EMIT indexerBusyChanged();
    }

    //if (d->mImportedTracksCount >= 4) {
        Q_EMIT closeNotification(QStringLiteral("notEnoughTracks"));
    //}

    Q_EMIT importedTracksCountChanged();
}

void MusicListenersManager::decreaseImportedTracksCount()
{
    --d->mImportedTracksCount;

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

void MusicListenersManager::monitorEndingListeners()
{
    --d->mActiveMusicListenersCount;

    if (d->mActiveMusicListenersCount == 0) {
        /*if (d->mImportedTracksCount < 4 && d->mElisaApplication) {
            NotificationItem notEnoughTracks;

            notEnoughTracks.setNotificationId(QStringLiteral("notEnoughTracks"));

            notEnoughTracks.setTargetObject(this);

            notEnoughTracks.setMessage(i18nc("No track found message", "No track have been found"));

            auto configureAction = d->mElisaApplication->action(QStringLiteral("options_configure"));

            notEnoughTracks.setMainButtonText(configureAction->text());
            notEnoughTracks.setMainButtonIconName(configureAction->icon().name());
            notEnoughTracks.setMainButtonMethodName(QStringLiteral("showConfiguration"));

            Q_EMIT newNotification(notEnoughTracks);
        }*/

        d->mIndexingRunning = false;
        Q_EMIT indexingRunningChanged();
    }
}

void MusicListenersManager::createTracksListener()
{
    if (!d->mTracksListener) {
        d->mTracksListener = std::make_unique<TracksListener>(&d->mDatabaseInterface);
        d->mTracksListener->moveToThread(&d->mDatabaseThread);

        connect(this, &MusicListenersManager::removeTracksInError,
                &d->mDatabaseInterface, &DatabaseInterface::removeTracksList);

        connect(&d->mDatabaseInterface, &DatabaseInterface::trackRemoved, d->mTracksListener.get(), &TracksListener::trackRemoved);
        connect(&d->mDatabaseInterface, &DatabaseInterface::tracksAdded, d->mTracksListener.get(), &TracksListener::tracksAdded);
        connect(&d->mDatabaseInterface, &DatabaseInterface::trackModified, d->mTracksListener.get(), &TracksListener::trackModified);
    }
}


#include "moc_musiclistenersmanager.cpp"
