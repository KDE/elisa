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

#include "localbaloofilelisting.h"

#include "baloo/baloocommon.h"

#include "elisa_settings.h"
#include "elisautils.h"

#include "baloo/scheduler.h"
#include "baloo/fileindexer.h"
#include "baloo/main.h"

#include "baloowatcherapplicationadaptor.h"

#include "filescanner.h"

#include <Baloo/Query>
#include <Baloo/File>
#include <Baloo/IndexerConfig>

#include <KFileMetaData/Properties>
#include <KFileMetaData/UserMetaData>

#include <KLocalizedString>

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QDBusServiceWatcher>

#include <QThread>
#include <QHash>
#include <QFileInfo>
#include <QDir>
#include <QAtomicInt>
#include <QScopedPointer>
#include <QDebug>
#include <QGuiApplication>

#include <algorithm>
#include <memory>

class LocalBalooFileListingPrivate
{
public:

    Baloo::Query mQuery;

    QDBusServiceWatcher mServiceWatcher;

    QScopedPointer<org::kde::baloo::main> mBalooMainInterface;

    QScopedPointer<org::kde::baloo::fileindexer> mBalooIndexer;

    QScopedPointer<org::kde::baloo::scheduler> mBalooScheduler;

    BalooWatcherApplicationAdaptor *mDbusAdaptor = nullptr;

    QAtomicInt mStopRequest = 0;

    bool mIsRegisteredToBaloo = false;

    bool mIsRegisteringToBaloo = false;

    bool mIsRegisteredToBalooWatcher = false;

    bool mIsRegisteringToBalooWatcher = false;

};

LocalBalooFileListing::LocalBalooFileListing(QObject *parent)
    : AbstractFileListing(parent), d(std::make_unique<LocalBalooFileListingPrivate>())
{
    d->mQuery.addType(QStringLiteral("Audio"));
    setHandleNewFiles(false);

    auto sessionBus = QDBusConnection::sessionBus();

    d->mDbusAdaptor = new BalooWatcherApplicationAdaptor(this);

    sessionBus.registerObject(QStringLiteral("/org/kde/BalooWatcherApplication"), d->mDbusAdaptor, QDBusConnection::ExportAllContents);

    connect(&d->mServiceWatcher, &QDBusServiceWatcher::serviceRegistered,
            this, &LocalBalooFileListing::serviceRegistered);
    connect(&d->mServiceWatcher, &QDBusServiceWatcher::serviceOwnerChanged,
            this, &LocalBalooFileListing::serviceOwnerChanged);
    connect(&d->mServiceWatcher, &QDBusServiceWatcher::serviceUnregistered,
            this, &LocalBalooFileListing::serviceUnregistered);

    d->mServiceWatcher.setConnection(sessionBus);
    d->mServiceWatcher.addWatchedService(QStringLiteral("org.kde.baloo"));

    if (sessionBus.interface()->isServiceRegistered(QStringLiteral("org.kde.baloo"))) {
        registerToBaloo();
    }
}

LocalBalooFileListing::~LocalBalooFileListing()
{
}

void LocalBalooFileListing::applicationAboutToQuit()
{
    AbstractFileListing::applicationAboutToQuit();
    d->mStopRequest = 1;
}

bool LocalBalooFileListing::canHandleRootPaths() const
{
    return false;
}

void LocalBalooFileListing::newBalooFile(const QString &fileName)
{
    qCDebug(orgKdeElisaBaloo()) << "LocalBalooFileListing::newBalooFile" << fileName;

    auto scanFileInfo = QFileInfo(fileName);

    if (!scanFileInfo.exists()) {
        return;
    }

    if (!fileScanner().shouldScanFile(fileName)) {
        return;
    }

    Q_EMIT indexingStarted();

    auto newFile = QUrl::fromLocalFile(fileName);

    auto newTrack = scanOneFile(newFile, scanFileInfo);

    if (newTrack.isValid()) {
        QFileInfo newFileInfo(fileName);

        addFileInDirectory(newFile, QUrl::fromLocalFile(newFileInfo.absoluteDir().absolutePath()));

        emitNewFiles({newTrack});
    }

    Q_EMIT indexingFinished();
}

void LocalBalooFileListing::registeredToBaloo(QDBusPendingCallWatcher *watcher)
{
    qCDebug(orgKdeElisaBaloo) << "LocalBalooFileListing::registeredToBaloo";

    if (!watcher) {
        return;
    }

    QDBusPendingReply<> reply = *watcher;
    if (reply.isError()) {
        qCDebug(orgKdeElisaBaloo) << "LocalBalooFileListing::registeredToBaloo" << reply.error().name() << reply.error().message();
        d->mIsRegisteredToBaloo = false;
    } else {
        d->mIsRegisteredToBaloo = true;
    }

    d->mIsRegisteringToBaloo = false;

    watcher->deleteLater();
}

void LocalBalooFileListing::registeredToBalooWatcher(QDBusPendingCallWatcher *watcher)
{
    qCDebug(orgKdeElisaBaloo) << "LocalBalooFileListing::registeredToBalooWatcher";

    if (!watcher) {
        return;
    }

    QDBusPendingReply<> reply = *watcher;
    if (reply.isError()) {
        qCDebug(orgKdeElisaBaloo) << "LocalBalooFileListing::registeredToBalooWatcher" << reply.error().name() << reply.error().message();
        d->mIsRegisteredToBalooWatcher = false;
    } else {
        d->mIsRegisteredToBalooWatcher = true;
    }

    d->mIsRegisteringToBalooWatcher = false;

    watcher->deleteLater();
}

void LocalBalooFileListing::registerToBaloo()
{
    if (d->mIsRegisteringToBaloo || d->mIsRegisteringToBalooWatcher) {
        qCDebug(orgKdeElisaBaloo) << "LocalBalooFileListing::registerToBaloo" << "already registering";
        return;
    }

    qCDebug(orgKdeElisaBaloo) << "LocalBalooFileListing::registerToBaloo";

    d->mIsRegisteringToBaloo = true;
    d->mIsRegisteringToBalooWatcher = true;

    auto sessionBus = QDBusConnection::sessionBus();

    d->mBalooMainInterface.reset(new org::kde::baloo::main(QStringLiteral("org.kde.baloo"), QStringLiteral("/"),
                                                           sessionBus, this));

    if (!d->mBalooMainInterface->isValid()) {
        qCDebug(orgKdeElisaBaloo) << "LocalBalooFileListing::registerToBaloo" << "invalid org.kde.baloo/main interface";
        return;
    }

    d->mBalooIndexer.reset(new org::kde::baloo::fileindexer(QStringLiteral("org.kde.baloo"), QStringLiteral("/fileindexer"),
                                                            sessionBus, this));

    if (!d->mBalooIndexer->isValid()) {
        qCDebug(orgKdeElisaBaloo) << "LocalBalooFileListing::registerToBaloo" << "invalid org.kde.baloo/fileindexer interface";
        return;
    }

    connect(d->mBalooIndexer.data(), &org::kde::baloo::fileindexer::finishedIndexingFile,
            this, &LocalBalooFileListing::newBalooFile);

    d->mBalooScheduler.reset(new org::kde::baloo::scheduler(QStringLiteral("org.kde.baloo"), QStringLiteral("/scheduler"),
                                                            sessionBus, this));

    if (!d->mBalooScheduler->isValid()) {
        qCDebug(orgKdeElisaBaloo) << "LocalBalooFileListing::registerToBaloo" << "invalid org.kde.baloo/scheduler interface";
        return;
    }

    qCDebug(orgKdeElisaBaloo) << "LocalBalooFileListing::registerToBaloo" << "call registerMonitor";
    auto answer = d->mBalooIndexer->registerMonitor();

    if (answer.isError()) {
        qCDebug(orgKdeElisaBaloo) << "LocalBalooFileListing::executeInit" << answer.error().name() << answer.error().message();
    }

    auto pendingCallWatcher = new QDBusPendingCallWatcher(answer);

    connect(pendingCallWatcher, &QDBusPendingCallWatcher::finished, this, &LocalBalooFileListing::registeredToBaloo);
    if (pendingCallWatcher->isFinished()) {
        registeredToBaloo(pendingCallWatcher);
    }

    auto pendingCall = d->mBalooMainInterface->registerBalooWatcher(QStringLiteral("org.mpris.MediaPlayer2.elisa/org/kde/BalooWatcherApplication"));
    qCDebug(orgKdeElisaBaloo) << "LocalBalooFileListing::registerToBaloo" << "call registerBalooWatcher";
    auto pendingCallWatcher2 = new QDBusPendingCallWatcher(pendingCall);

    connect(pendingCallWatcher2, &QDBusPendingCallWatcher::finished, this, &LocalBalooFileListing::registeredToBalooWatcher);
    if (pendingCallWatcher2->isFinished()) {
        registeredToBalooWatcher(pendingCallWatcher2);
    }
}

void LocalBalooFileListing::renamedFiles(const QString &from, const QString &to, const QStringList &listFiles)
{
    qCDebug(orgKdeElisaBaloo) << "LocalBalooFileListing::renamedFiles" << from << to << listFiles;
}

void LocalBalooFileListing::serviceOwnerChanged(const QString &serviceName, const QString &oldOwner, const QString &newOwner)
{
    Q_UNUSED(oldOwner);
    Q_UNUSED(newOwner);

    qCDebug(orgKdeElisaBaloo) << "LocalBalooFileListing::serviceOwnerChanged" << serviceName << oldOwner << newOwner;

    if (serviceName == QLatin1String("org.kde.baloo") && !newOwner.isEmpty()) {
        d->mIsRegisteredToBaloo = false;
        d->mIsRegisteredToBalooWatcher = false;
        registerToBaloo();
    }
}

void LocalBalooFileListing::serviceRegistered(const QString &serviceName)
{
    qCDebug(orgKdeElisaBaloo) << "LocalBalooFileListing::serviceRegistered" << serviceName;

    if (serviceName == QLatin1String("org.kde.baloo")) {
        registerToBaloo();
    }
}

void LocalBalooFileListing::serviceUnregistered(const QString &serviceName)
{
    qCDebug(orgKdeElisaBaloo) << "LocalBalooFileListing::serviceUnregistered" << serviceName;

    if (serviceName == QLatin1String("org.kde.baloo")) {
        d->mIsRegisteredToBaloo = false;
        d->mIsRegisteredToBalooWatcher = false;
    }
}

void LocalBalooFileListing::stop()
{
    qCInfo(orgKdeElisaBaloo()) << "LocalBalooFileListing::stop";
}

void LocalBalooFileListing::executeInit(QHash<QUrl, QDateTime> allFiles)
{
    AbstractFileListing::executeInit(std::move(allFiles));
}

void LocalBalooFileListing::triggerRefreshOfContent()
{
    qCDebug(orgKdeElisaBaloo()) << "LocalBalooFileListing::triggerRefreshOfContent";
    Q_EMIT indexingStarted();

    AbstractFileListing::triggerRefreshOfContent();

    const auto &rootPaths = allRootPaths();
    bool hasSingleRootPath = (rootPaths.size() == 1);
    auto singleRootPath = rootPaths.at(0);

    auto resultIterator = d->mQuery.exec();
    auto newFiles = DataTypes::ListTrackDataType();

    while(resultIterator.next() && d->mStopRequest == 0) {
        const auto &fileName = resultIterator.filePath();

        if (hasSingleRootPath) {
            if (!fileName.startsWith(singleRootPath)) {
                qCDebug(orgKdeElisaBaloo()) << "LocalBalooFileListing::triggerRefreshOfContent" << fileName << "does not match root paths";
                continue;
            }
        } else {
            bool isIncluded = false;
            for (const auto &oneRootPath : rootPaths) {
                if (fileName.startsWith(oneRootPath)) {
                    isIncluded = true;
                    break;
                }
            }
            if (!isIncluded) {
                qCDebug(orgKdeElisaBaloo()) << "LocalBalooFileListing::triggerRefreshOfContent" << fileName << "does not match root paths";
                continue;
            }
        }

        const auto &newFileUrl = QUrl::fromLocalFile(resultIterator.filePath());

        auto scanFileInfo = QFileInfo(fileName);

        if (!scanFileInfo.exists()) {
            qCDebug(orgKdeElisaBaloo()) << "LocalBalooFileListing::triggerRefreshOfContent" << fileName << "file does not exists";
            continue;
        }

        auto itExistingFile = allFiles().find(newFileUrl);
        if (itExistingFile != allFiles().end()) {
            if (*itExistingFile >= scanFileInfo.metadataChangeTime()) {
                allFiles().erase(itExistingFile);
                qCDebug(orgKdeElisaBaloo()) << "LocalBalooFileListing::triggerRefreshOfContent" << fileName << "file not modified since last scan";
                continue;
            }
        }

        const auto currentDirectory = QUrl::fromLocalFile(scanFileInfo.absoluteDir().absolutePath());

        addFileInDirectory(newFileUrl, currentDirectory);

        const auto &newTrack = scanOneFile(newFileUrl, scanFileInfo);

        if (newTrack.isValid()) {
            newFiles.push_back(newTrack);
            if (newFiles.size() > 500 && d->mStopRequest == 0) {
                qCDebug(orgKdeElisaBaloo()) << "LocalBalooFileListing::triggerRefreshOfContent" << "insert new tracks in database" << newFiles.count();
                emitNewFiles(newFiles);
                newFiles.clear();
            }
        } else {
            qCDebug(orgKdeElisaBaloo()) << "LocalBalooFileListing::triggerRefreshOfContent" << fileName << "invalid track" << newTrack;
        }
    }

    if (!newFiles.isEmpty() && d->mStopRequest == 0) {
        qCDebug(orgKdeElisaBaloo()) << "LocalBalooFileListing::triggerRefreshOfContent" << "insert new tracks in database" << newFiles.count();
        emitNewFiles(newFiles);
    }

    setWaitEndTrackRemoval(false);

    checkFilesToRemove();

    if (!waitEndTrackRemoval()) {
        Q_EMIT indexingFinished();
    }
}

DataTypes::TrackDataType LocalBalooFileListing::scanOneFile(const QUrl &scanFile, const QFileInfo &scanFileInfo)
{
    DataTypes::TrackDataType trackData;

    auto localFileName = scanFile.toLocalFile();

    Baloo::File match(localFileName);

    match.load();
    fileScanner().scanProperties(match, trackData);
    trackData[DataTypes::FileModificationTime] = scanFileInfo.metadataChangeTime();
    trackData[DataTypes::ResourceRole] = scanFile;

    if (!trackData.isValid()) {
        qCDebug(orgKdeElisaBaloo) << "LocalBalooFileListing::scanOneFile" << scanFile << "falling back to plain file metadata analysis";
        trackData = AbstractFileListing::scanOneFile(scanFile, scanFileInfo);
    }

    if (trackData.isValid()) {
        trackData[DataTypes::HasEmbeddedCover] = checkEmbeddedCoverImage(localFileName);
        addCover(trackData);
    } else {
        qCDebug(orgKdeElisaBaloo) << "LocalBalooFileListing::scanOneFile" << scanFile << "invalid track";
    }

    return trackData;
}


#include "moc_localbaloofilelisting.cpp"
