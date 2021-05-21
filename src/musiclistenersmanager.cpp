/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "musiclistenersmanager.h"

#include "config-upnp-qt.h"

#include "indexersManager.h"

#if defined UPNPQT_FOUND && UPNPQT_FOUND
#include "upnp/upnplistener.h"
#endif

#if defined KF5Baloo_FOUND && KF5Baloo_FOUND
#include "baloo/baloolistener.h"
#include "baloo/baloodetector.h"
#endif

#if defined Qt5AndroidExtras_FOUND && Qt5AndroidExtras_FOUND
#include "android/androidmusiclistener.h"
#endif

#include "databaseinterface.h"
#include "mediaplaylist.h"
#include "file/filelistener.h"
#include "file/localfilelisting.h"
#include "trackslistener.h"
#include "elisaapplication.h"
#include "elisa_settings.h"
#include "modeldataloader.h"

#include <KI18n/KLocalizedString>

#include <QThread>
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>
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
    BalooDetector mBalooDetector;

    BalooListener mBalooListener;
#endif

    FileListener mFileListener;

#if defined Qt5AndroidExtras_FOUND && Qt5AndroidExtras_FOUND
    AndroidMusicListener mAndroidMusicListener;
#endif

    DatabaseInterface mDatabaseInterface;

    std::unique_ptr<TracksListener> mTracksListener;

    QFileSystemWatcher mConfigFileWatcher;

    QStringList mPreviousRootPathValue;

    ElisaApplication *mElisaApplication = nullptr;

    int mImportedTracksCount = 0;

    bool mIndexerBusy = false;

    bool mFileSystemIndexerActive = false;

    bool mBalooIndexerActive = false;

    bool mBalooIndexerAvailable = false;

    bool mAndroidIndexerActive = false;

    bool mAndroidIndexerAvailable = false;

};

MusicListenersManager::MusicListenersManager(QObject *parent)
    : QObject(parent), d(std::make_unique<MusicListenersManagerPrivate>())
{
    connect(&d->mDatabaseInterface, &DatabaseInterface::tracksAdded,
            this, &MusicListenersManager::increaseImportedTracksCount);

#if defined KF5Baloo_FOUND && KF5Baloo_FOUND
    connect(&d->mBalooDetector, &BalooDetector::balooAvailabilityChanged,
            this, &MusicListenersManager::balooAvailabilityChanged);
#endif

    connect(&d->mDatabaseInterface, &DatabaseInterface::requestsInitDone,
            this, &MusicListenersManager::databaseReady);
    connect(this, &MusicListenersManager::clearDatabase,
            &d->mDatabaseInterface, &DatabaseInterface::clearData);
    connect(&d->mDatabaseInterface, &DatabaseInterface::cleanedDatabase,
            this, &MusicListenersManager::cleanedDatabase);

    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
            this, &MusicListenersManager::applicationAboutToQuit);

    connect(&d->mConfigFileWatcher, &QFileSystemWatcher::fileChanged,
            this, &MusicListenersManager::configChanged);

    d->mListenerThread.start();
    d->mDatabaseThread.start();

    d->mDatabaseInterface.moveToThread(&d->mDatabaseThread);

    const auto &localDataPaths = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
    auto databaseFileName = QString();
    if (!localDataPaths.isEmpty()) {
        QDir myDataDirectory;
        myDataDirectory.mkpath(localDataPaths.first());
        databaseFileName = localDataPaths.first() + QStringLiteral("/elisaDatabase.db");
    }

    QMetaObject::invokeMethod(&d->mDatabaseInterface, "init", Qt::QueuedConnection,
                              Q_ARG(QString, QStringLiteral("listeners")), Q_ARG(QString, databaseFileName));

    qCInfo(orgKdeElisaIndexersManager) << "Local file system indexer is inactive";
    qCInfo(orgKdeElisaIndexersManager) << "Baloo indexer is unavailable";
    qCInfo(orgKdeElisaIndexersManager) << "Baloo indexer is inactive";
}

MusicListenersManager::~MusicListenersManager()
{
    d->mListenerThread.quit();
    d->mListenerThread.wait();

    d->mDatabaseThread.quit();
    d->mDatabaseThread.wait();
}

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
    connect(client, &MediaPlayList::newUrlInList, d->mTracksListener.get(), &TracksListener::newUrlInList);
    connect(client, &MediaPlayList::newTrackByNameInList, d->mTracksListener.get(), &TracksListener::trackByNameInList);
}

int MusicListenersManager::importedTracksCount() const
{
    return d->mImportedTracksCount;
}

ElisaApplication *MusicListenersManager::elisaApplication() const
{
    return d->mElisaApplication;
}

TracksListener *MusicListenersManager::tracksListener() const
{
    return d->mTracksListener.get();
}

bool MusicListenersManager::indexerBusy() const
{
    return d->mIndexerBusy;
}

bool MusicListenersManager::fileSystemIndexerActive() const
{
    return d->mFileSystemIndexerActive;
}

bool MusicListenersManager::balooIndexerActive() const
{
    return d->mBalooIndexerActive;
}

bool MusicListenersManager::balooIndexerAvailable() const
{
    return d->mBalooIndexerAvailable;
}

bool MusicListenersManager::androidIndexerActive() const
{
    return d->mAndroidIndexerActive;
}

bool MusicListenersManager::androidIndexerAvailable() const
{
    return d->mAndroidIndexerAvailable;
}

auto MusicListenersManager::initializeRootPath()
{
    auto initialRootPath = QStringList{};
    auto systemMusicPaths = QStandardPaths::standardLocations(QStandardPaths::MusicLocation);
    for (const auto &musicPath : qAsConst(systemMusicPaths)) {
        initialRootPath.push_back(musicPath);
    }

    Elisa::ElisaConfiguration::setRootPath(initialRootPath);
    Elisa::ElisaConfiguration::self()->save();

    return initialRootPath;
}

void MusicListenersManager::databaseReady()
{
    auto initialRootPath = Elisa::ElisaConfiguration::rootPath();
    if (initialRootPath.isEmpty()) {
        initializeRootPath();
    }

    d->mConfigFileWatcher.addPath(Elisa::ElisaConfiguration::self()->config()->name());

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
    Q_EMIT elisaApplicationChanged();
}

void MusicListenersManager::playBackError(const QUrl &sourceInError, QMediaPlayer::Error playerError)
{
    qCDebug(orgKdeElisaIndexersManager) << "MusicListenersManager::playBackError" << sourceInError;

    if (playerError == QMediaPlayer::ResourceError) {
        Q_EMIT removeTracksInError({sourceInError});

        if (sourceInError.isLocalFile()) {
            Q_EMIT displayTrackError(sourceInError.toLocalFile());
        } else {
            Q_EMIT displayTrackError(sourceInError.toString());
        }
    }
}

void MusicListenersManager::deleteElementById(ElisaUtils::PlayListEntryType entryType, qulonglong databaseId)
{
    switch(entryType)
    {
    case ElisaUtils::Radio:
        QMetaObject::invokeMethod(&d->mDatabaseInterface, "removeRadio", Qt::QueuedConnection,
                                  Q_ARG(qulonglong, databaseId));
        break;
    case ElisaUtils::Album:
    case ElisaUtils::Artist:
    case ElisaUtils::Genre:
    case ElisaUtils::Lyricist:
    case ElisaUtils::Composer:
    case ElisaUtils::Track:
    case ElisaUtils::FileName:
    case ElisaUtils::Unknown:
    case ElisaUtils::Container:
        break;
    }
}

void MusicListenersManager::connectModel(ModelDataLoader *dataLoader)
{
    dataLoader->moveToThread(&d->mDatabaseThread);
}

void MusicListenersManager::resetMusicData()
{
    Q_EMIT clearDatabase();
}

void MusicListenersManager::configChanged()
{
    auto currentConfiguration = Elisa::ElisaConfiguration::self();

    d->mConfigFileWatcher.addPath(currentConfiguration->config()->name());

    currentConfiguration->load();
    currentConfiguration->read();

    bool configurationHasChanged = false;
#if defined KF5Baloo_FOUND && KF5Baloo_FOUND
    if (d->mBalooIndexerAvailable && d->mBalooIndexerActive && d->mBalooListener.canHandleRootPaths() && !currentConfiguration->forceUsageOfFastFileSearch()) {
        configurationHasChanged = true;
    } else if (d->mBalooIndexerAvailable && !d->mBalooIndexerActive && d->mBalooListener.canHandleRootPaths() && currentConfiguration->forceUsageOfFastFileSearch()) {
        configurationHasChanged = true;
    }
#endif

    auto inputRootPath = currentConfiguration->rootPath();
    configurationHasChanged = configurationHasChanged || (d->mPreviousRootPathValue != inputRootPath);

    if (configurationHasChanged) {
        d->mPreviousRootPathValue = inputRootPath;
    } else {
        qCDebug(orgKdeElisaIndexersManager()) << "music paths configuration and scanning has not changed";
        return;
    }

    //resolve symlinks
    QStringList allRootPaths;
    for (const auto &onePath : qAsConst(inputRootPath)) {
        auto workPath = onePath;
        if (workPath.startsWith(QLatin1String("file:/"))) {
            auto urlPath = QUrl{workPath};
            workPath = urlPath.toLocalFile();
        }

        QFileInfo pathFileInfo(workPath);
        auto directoryPath = pathFileInfo.canonicalFilePath();
        if (!directoryPath.isEmpty()) {
            if (directoryPath.rightRef(1) != QLatin1Char('/'))
            {
                directoryPath.append(QLatin1Char('/'));
            }
            allRootPaths.push_back(directoryPath);
        }
    }

    if (allRootPaths.isEmpty()) {
        allRootPaths = initializeRootPath();
    }

    d->mFileListener.setAllRootPaths(allRootPaths);

#if defined KF5Baloo_FOUND && KF5Baloo_FOUND
    d->mBalooListener.setAllRootPaths(allRootPaths);
#endif

    if (!d->mBalooIndexerActive && !d->mFileSystemIndexerActive) {
        testBalooIndexerAvailability();
    }

#if defined KF5Baloo_FOUND && KF5Baloo_FOUND
    if (d->mBalooIndexerAvailable && !d->mBalooIndexerActive && d->mBalooListener.canHandleRootPaths() && currentConfiguration->forceUsageOfFastFileSearch()) {
        qCDebug(orgKdeElisaIndexersManager()) << "trigger start of baloo file indexer";
        QMetaObject::invokeMethod(d->mFileListener.fileListing(), "stop", Qt::BlockingQueuedConnection);
        d->mFileSystemIndexerActive = false;
        startBalooIndexing();
    } else if ((!d->mFileSystemIndexerActive && d->mBalooIndexerActive && !d->mBalooListener.canHandleRootPaths()) ||
               !currentConfiguration->forceUsageOfFastFileSearch()) {
        if (d->mBalooIndexerActive) {
            qCDebug(orgKdeElisaIndexersManager()) << "trigger stop of baloo file indexer";
            QMetaObject::invokeMethod(d->mBalooListener.fileListing(), "stop", Qt::BlockingQueuedConnection);
        }
        d->mBalooIndexerActive = false;
        startLocalFileSystemIndexing();
    }
#endif

    if (d->mBalooIndexerActive) {
        qCInfo(orgKdeElisaIndexersManager()) << "trigger init of baloo file indexer";
#if defined KF5Baloo_FOUND && KF5Baloo_FOUND
        QMetaObject::invokeMethod(d->mBalooListener.fileListing(), "init", Qt::QueuedConnection);
#endif
    } else if (d->mFileSystemIndexerActive) {
        qCInfo(orgKdeElisaIndexersManager()) << "trigger init of local file indexer";
        QMetaObject::invokeMethod(d->mFileListener.fileListing(), "init", Qt::QueuedConnection);
    } else if (d->mAndroidIndexerActive) {
#if defined Q_OS_ANDROID
        QMetaObject::invokeMethod(d->mAndroidMusicListener.fileListing(), "init", Qt::QueuedConnection);
#endif
    }

#if defined UPNPQT_FOUND && UPNPQT_FOUND
    d->mUpnpListener.setDatabaseInterface(&d->mDatabaseInterface);
    d->mUpnpListener.moveToThread(&d->mDatabaseThread);
    connect(this, &MusicListenersManager::applicationIsTerminating,
            &d->mUpnpListener, &UpnpListener::applicationAboutToQuit, Qt::DirectConnection);
#endif

}

void MusicListenersManager::increaseImportedTracksCount(const DataTypes::ListTrackDataType &allTracks)
{
    d->mImportedTracksCount += allTracks.size();

    Q_EMIT importedTracksCountChanged();
}

void MusicListenersManager::decreaseImportedTracksCount()
{
    --d->mImportedTracksCount;

    Q_EMIT importedTracksCountChanged();
}

void MusicListenersManager::monitorStartingListeners()
{
    d->mIndexerBusy = true;
    Q_EMIT indexerBusyChanged();
}

void MusicListenersManager::monitorEndingListeners()
{
    d->mIndexerBusy = false;
    Q_EMIT indexerBusyChanged();
}

void MusicListenersManager::cleanedDatabase()
{
    d->mImportedTracksCount = 0;
    Q_EMIT importedTracksCountChanged();
    Q_EMIT clearedDatabase();
}

void MusicListenersManager::balooAvailabilityChanged()
{
#if defined KF5Baloo_FOUND && KF5Baloo_FOUND
    if (!d->mBalooDetector.balooAvailability() || !d->mBalooListener.canHandleRootPaths()) {
        if (d->mBalooDetector.balooAvailability()) {
            qCInfo(orgKdeElisaIndexersManager) << "Baloo indexer is available";
            d->mBalooIndexerAvailable = true;
        }
#else
    if (true) {
#endif

#if defined KF5Baloo_FOUND && KF5Baloo_FOUND
        if (!d->mBalooListener.canHandleRootPaths() && d->mBalooDetector.balooAvailability())
        {
            qCInfo(orgKdeElisaIndexersManager()) << "Baloo cannot handle all configured paths: falling back to plain filex indexer";
        }
#endif

        if (!d->mFileSystemIndexerActive) {
            startLocalFileSystemIndexing();
        }
        if (!d->mAndroidIndexerActive) {
            startAndroidIndexing();
        }

        return;
    }

    qCInfo(orgKdeElisaIndexersManager) << "Baloo indexer is available";
    d->mBalooIndexerAvailable = true;
    Q_EMIT balooIndexerAvailableChanged();
    startBalooIndexing();
}

void MusicListenersManager::testBalooIndexerAvailability()
{
#if defined KF5Baloo_FOUND && KF5Baloo_FOUND
    d->mBalooDetector.checkBalooAvailability();
#else
    qCInfo(orgKdeElisaIndexersManager) << "Baloo indexer is unavailable";
    d->mBalooIndexerAvailable = false;
    Q_EMIT balooIndexerAvailableChanged();

    qCInfo(orgKdeElisaIndexersManager) << "Baloo indexer is inactive";
    d->mBalooIndexerActive = false;
    Q_EMIT balooIndexerActiveChanged();

    startLocalFileSystemIndexing();
    startAndroidIndexing();
#endif
}

void MusicListenersManager::startLocalFileSystemIndexing()
{
    if (d->mFileSystemIndexerActive) {
        return;
    }

    d->mFileListener.setDatabaseInterface(&d->mDatabaseInterface);
    d->mFileListener.moveToThread(&d->mListenerThread);
    connect(this, &MusicListenersManager::applicationIsTerminating,
            &d->mFileListener, &FileListener::applicationAboutToQuit, Qt::DirectConnection);
    connect(&d->mFileListener, &FileListener::indexingStarted,
            this, &MusicListenersManager::monitorStartingListeners);
    connect(&d->mFileListener, &FileListener::indexingFinished,
            this, &MusicListenersManager::monitorEndingListeners);

    qCInfo(orgKdeElisaIndexersManager) << "Local file system indexer is active";

    d->mFileSystemIndexerActive = true;
    Q_EMIT fileSystemIndexerActiveChanged();
}

void MusicListenersManager::startAndroidIndexing()
{
    qCInfo(orgKdeElisaIndexersManager) << "MusicListenersManager::startAndroidIndexing";
#if defined Q_OS_ANDROID
    if (d->mAndroidIndexerActive) {
        return;
    }

    d->mAndroidMusicListener.setDatabaseInterface(&d->mDatabaseInterface);
    d->mAndroidMusicListener.moveToThread(&d->mListenerThread);
    connect(this, &MusicListenersManager::applicationIsTerminating,
            &d->mAndroidMusicListener, &AndroidMusicListener::applicationAboutToQuit, Qt::DirectConnection);
    connect(&d->mAndroidMusicListener, &AndroidMusicListener::indexingStarted,
            this, &MusicListenersManager::monitorStartingListeners);
    connect(&d->mAndroidMusicListener, &AndroidMusicListener::indexingFinished,
            this, &MusicListenersManager::monitorEndingListeners);

    qCInfo(orgKdeElisaIndexersManager) << "Android indexer is active";

    d->mAndroidIndexerActive = true;
    Q_EMIT androidIndexerActiveChanged();
#endif
}

void MusicListenersManager::startBalooIndexing()
{
#if defined KF5Baloo_FOUND && KF5Baloo_FOUND
    d->mBalooListener.moveToThread(&d->mListenerThread);
    d->mBalooListener.setDatabaseInterface(&d->mDatabaseInterface);
    connect(this, &MusicListenersManager::applicationIsTerminating,
            &d->mBalooListener, &BalooListener::applicationAboutToQuit, Qt::DirectConnection);
    connect(&d->mBalooListener, &BalooListener::indexingStarted,
            this, &MusicListenersManager::monitorStartingListeners);
    connect(&d->mBalooListener, &BalooListener::indexingFinished,
            this, &MusicListenersManager::monitorEndingListeners);
    connect(&d->mBalooListener, &BalooListener::clearDatabase,
            &d->mDatabaseInterface, &DatabaseInterface::clearData);

    qCInfo(orgKdeElisaIndexersManager) << "Baloo indexer is active";

    d->mBalooIndexerActive = true;
    Q_EMIT balooIndexerActiveChanged();
#endif
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
        Q_EMIT tracksListenerChanged();
    }
}


#include "moc_musiclistenersmanager.cpp"
