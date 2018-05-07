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

#include "localbaloofilelisting.h"

#include "musicaudiotrack.h"
#include "notificationitem.h"
#include "elisa_settings.h"
#include "elisautils.h"

#include "baloo/scheduler.h"
#include "baloo/fileindexer.h"

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

    QHash<QString, QVector<MusicAudioTrack>> mAllAlbums;

    QDBusServiceWatcher mServiceWatcher;

    QScopedPointer<org::kde::baloo::fileindexer> mBalooIndexer;

    QScopedPointer<org::kde::baloo::scheduler> mBalooScheduler;

    QAtomicInt mStopRequest = 0;

    bool mIsRegistered = false;

    bool mIsRegistering = false;

};

LocalBalooFileListing::LocalBalooFileListing(QObject *parent)
    : AbstractFileListing(QStringLiteral("baloo"), parent), d(std::make_unique<LocalBalooFileListingPrivate>())
{
    d->mQuery.addType(QStringLiteral("Audio"));
    setHandleNewFiles(false);

    auto sessionBus = QDBusConnection::sessionBus();

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
    Q_EMIT closeNotification(QStringLiteral("balooInvalidConfiguration"));
}

void LocalBalooFileListing::applicationAboutToQuit()
{
    AbstractFileListing::applicationAboutToQuit();
    d->mStopRequest = 1;
}

void LocalBalooFileListing::newBalooFile(const QString &fileName)
{
    auto newFile = QUrl::fromLocalFile(fileName);

    auto newTrack = scanOneFile(newFile);

    if (newTrack.isValid()) {
        QFileInfo newFileInfo(fileName);

        addFileInDirectory(newFile, QUrl::fromLocalFile(newFileInfo.absoluteDir().absolutePath()));

        emitNewFiles({newTrack});
    }
}

void LocalBalooFileListing::registeredToBaloo(QDBusPendingCallWatcher *watcher)
{
    qDebug() << "LocalBalooFileListing::registeredToBaloo";

    if (!watcher) {
        return;
    }

    QDBusPendingReply<> reply = *watcher;
    if (reply.isError()) {
        qDebug() << "LocalBalooFileListing::executeInit" << reply.error().name() << reply.error().message();
        d->mIsRegistered = false;
    } else {
        d->mIsRegistered = true;
    }

    d->mIsRegistering = false;

    watcher->deleteLater();
}

void LocalBalooFileListing::registerToBaloo()
{
    if (d->mIsRegistering) {
        qDebug() << "LocalBalooFileListing::registerToBaloo" << "already registering";
        return;
    }

    qDebug() << "LocalBalooFileListing::registerToBaloo";

    d->mIsRegistering = true;

    auto sessionBus = QDBusConnection::sessionBus();

    d->mBalooIndexer.reset(new org::kde::baloo::fileindexer(QStringLiteral("org.kde.baloo"), QStringLiteral("/fileindexer"),
                                                            sessionBus, this));

    if (!d->mBalooIndexer->isValid()) {
        qDebug() << "LocalBalooFileListing::registerToBaloo" << "invalid org.kde.baloo/fileindexer interface";
        return;
    }

    connect(d->mBalooIndexer.data(), &org::kde::baloo::fileindexer::finishedIndexingFile,
            this, &LocalBalooFileListing::newBalooFile);

    d->mBalooScheduler.reset(new org::kde::baloo::scheduler(QStringLiteral("org.kde.baloo"), QStringLiteral("/scheduler"),
                                                            sessionBus, this));

    if (!d->mBalooScheduler->isValid()) {
        qDebug() << "LocalBalooFileListing::registerToBaloo" << "invalid org.kde.baloo/scheduler interface";
        return;
    }

    qDebug() << "LocalBalooFileListing::registerToBaloo" << "call registerMonitor";
    auto answer = d->mBalooIndexer->registerMonitor();

    if (answer.isError()) {
        qDebug() << "LocalBalooFileListing::executeInit" << answer.error().name() << answer.error().message();
    }

    auto pendingCallWatcher = new QDBusPendingCallWatcher(answer);

    connect(pendingCallWatcher, &QDBusPendingCallWatcher::finished, this, &LocalBalooFileListing::registeredToBaloo);
    if (pendingCallWatcher->isFinished()) {
        registeredToBaloo(pendingCallWatcher);
    }
}

void LocalBalooFileListing::renamedFiles(const QString &from, const QString &to, const QStringList &listFiles)
{
    qDebug() << "LocalBalooFileListing::renamedFiles" << from << to << listFiles;
}

void LocalBalooFileListing::serviceOwnerChanged(const QString &serviceName, const QString &oldOwner, const QString &newOwner)
{
    Q_UNUSED(oldOwner);
    Q_UNUSED(newOwner);

    qDebug() << "LocalBalooFileListing::serviceOwnerChanged" << serviceName << oldOwner << newOwner;

    if (serviceName == QStringLiteral("org.kde.baloo") && !newOwner.isEmpty()) {
        d->mIsRegistered = false;
        registerToBaloo();
    }
}

void LocalBalooFileListing::serviceRegistered(const QString &serviceName)
{
    qDebug() << "LocalBalooFileListing::serviceRegistered" << serviceName;

    if (serviceName == QStringLiteral("org.kde.baloo")) {
        registerToBaloo();
    }
}

void LocalBalooFileListing::serviceUnregistered(const QString &serviceName)
{
    qDebug() << "LocalBalooFileListing::serviceUnregistered" << serviceName;

    if (serviceName == QStringLiteral("org.kde.baloo")) {
        d->mIsRegistered = false;
    }
}

void LocalBalooFileListing::executeInit(QHash<QUrl, QDateTime> allFiles)
{
    AbstractFileListing::executeInit(std::move(allFiles));
}

void LocalBalooFileListing::triggerRefreshOfContent()
{
    if (!checkBalooConfiguration()) {
        return;
    }

    Q_EMIT indexingStarted();

    AbstractFileListing::triggerRefreshOfContent();

    auto resultIterator = d->mQuery.exec();
    auto newFiles = QList<MusicAudioTrack>();

    while(resultIterator.next() && d->mStopRequest == 0) {
        const auto &newFileUrl = QUrl::fromLocalFile(resultIterator.filePath());
        auto scanFileInfo = QFileInfo(resultIterator.filePath());
        const auto currentDirectory = QUrl::fromLocalFile(scanFileInfo.absoluteDir().absolutePath());

        addFileInDirectory(newFileUrl, currentDirectory);

        const auto &newTrack = scanOneFile(newFileUrl);

        if (newTrack.isValid()) {
            newFiles.push_back(newTrack);
            if (newFiles.size() > 500 && d->mStopRequest == 0) {
                emitNewFiles(newFiles);
                newFiles.clear();
            }
        }
    }

    if (!newFiles.isEmpty() && d->mStopRequest == 0) {
        emitNewFiles(newFiles);
    }

    checkFilesToRemove();

    Q_EMIT indexingFinished();
}

MusicAudioTrack LocalBalooFileListing::scanOneFile(const QUrl &scanFile)
{
    auto newTrack = MusicAudioTrack();

    auto localFileName = scanFile.toLocalFile();
    auto scanFileInfo = QFileInfo(localFileName);

    if (scanFileInfo.exists()) {
        watchPath(localFileName);
    } else {
        return newTrack;
    }

    auto itExistingFile = allFiles().find(scanFile);
    if (itExistingFile != allFiles().end()) {
        if (*itExistingFile >= scanFileInfo.fileTime(QFile::FileModificationTime)) {
            allFiles().erase(itExistingFile);
            return newTrack;
        }
    }

    Baloo::File match(localFileName);

    match.load();

    newTrack.setFileModificationTime(scanFileInfo.fileTime(QFile::FileModificationTime));
    newTrack.setResourceURI(scanFile);

    const auto &allProperties = match.properties();

    ElisaUtils::scanProperties(localFileName, allProperties, newTrack);

    if (!newTrack.isValid()) {
        newTrack = AbstractFileListing::scanOneFile(scanFile);
    }

    if (newTrack.isValid()) {
        AbstractFileListing::addCover(newTrack);
    }

    return newTrack;
}

bool LocalBalooFileListing::checkBalooConfiguration()
{
    bool problemDetected = false;
    Baloo::IndexerConfig balooConfiguration;

    problemDetected = problemDetected || !balooConfiguration.fileIndexingEnabled();
    problemDetected = problemDetected || balooConfiguration.onlyBasicIndexing();

    if (problemDetected) {
        NotificationItem balooInvalidConfiguration;

        balooInvalidConfiguration.setNotificationId(QStringLiteral("balooInvalidConfiguration"));

        balooInvalidConfiguration.setTargetObject(this);

        balooInvalidConfiguration.setMessage(i18nc("Notification about unusable Baloo Configuration", "Baloo configuration does not allow to discover your music"));

        balooInvalidConfiguration.setMainButtonText(i18nc("Text of button to modify Baloo Configuration", "Modify it"));
        balooInvalidConfiguration.setMainButtonIconName(QStringLiteral("configure"));
        balooInvalidConfiguration.setMainButtonMethodName(QStringLiteral("fixBalooConfiguration"));

        balooInvalidConfiguration.setSecondaryButtonText(i18nc("Text of button to disable Baloo indexer", "Disable Baloo support"));
        balooInvalidConfiguration.setSecondaryButtonIconName(QStringLiteral("configure"));
        balooInvalidConfiguration.setSecondaryButtonMethodName(QStringLiteral("disableBalooIndexer"));

        Q_EMIT newNotification(balooInvalidConfiguration);
    } else {
        Q_EMIT closeNotification(QStringLiteral("balooInvalidConfiguration"));
    }

    return !problemDetected;
}

void LocalBalooFileListing::fixBalooConfiguration()
{
    qDebug() << "LocalBalooFileListing::fixBalooConfiguration";

    Baloo::IndexerConfig balooConfiguration;

    if (!balooConfiguration.fileIndexingEnabled()) {
        balooConfiguration.setFileIndexingEnabled(true);
    }

    if (balooConfiguration.onlyBasicIndexing()) {
        balooConfiguration.setOnlyBasicIndexing(false);
    }

    balooConfiguration.refresh();

    if (checkBalooConfiguration()) {
        triggerRefreshOfContent();
    }
}

void LocalBalooFileListing::disableBalooIndexer()
{
    Elisa::ElisaConfiguration::self()->setBalooIndexer(false);
    Elisa::ElisaConfiguration::self()->save();
}


#include "moc_localbaloofilelisting.cpp"
