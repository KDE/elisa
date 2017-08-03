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

#include "baloo/scheduler.h"
#include "baloo/fileindexer.h"

#include <Baloo/Query>
#include <Baloo/File>

#include <KFileMetaData/Properties>
#include <KFileMetaData/UserMetaData>

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QDBusServiceWatcher>

#include <QThread>
#include <QHash>
#include <QFileInfo>
#include <QDir>
#include <QAtomicInt>
#include <QDebug>

#include <algorithm>

class LocalBalooFileListingPrivate
{
public:

    Baloo::Query mQuery;

    QHash<QString, QVector<MusicAudioTrack>> mAllAlbums;

    QHash<QString, QVector<MusicAudioTrack>> mNewAlbums;

    QList<MusicAudioTrack> mNewTracks;

    QHash<QString, QUrl> mAllAlbumCover;

    QAtomicInt mStopRequest = 0;

    QDBusServiceWatcher mServiceWatcher;

    bool mIsRegistered = false;

    bool mIsRegistering = false;

};

LocalBalooFileListing::LocalBalooFileListing(QObject *parent)
    : AbstractFileListing(QStringLiteral("baloo"), parent), d(new LocalBalooFileListingPrivate)
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
}

void LocalBalooFileListing::applicationAboutToQuit()
{
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

    sessionBus.connect(QStringLiteral("org.kde.baloo"), QStringLiteral("/fileindexer"),
                       QStringLiteral("org.kde.baloo.fileindexer"), QStringLiteral("finishedIndexingFile"), this, SLOT(newBalooFile(QString)));

    auto methodCall = QDBusMessage::createMethodCall(QStringLiteral("org.kde.baloo"), QStringLiteral("/fileindexer"),
                                                     QStringLiteral("org.kde.baloo.fileindexer"), QStringLiteral("registerMonitor"));

    qDebug() << "LocalBalooFileListing::registerToBaloo" << "call registerMonitor";
    auto answer = sessionBus.call(methodCall);

    if (answer.type() != QDBusMessage::ReplyMessage) {
        qDebug() << "LocalBalooFileListing::executeInit" << answer.errorName() << answer.errorMessage();
    }


    QDBusMessage registerBalooWatcher = QDBusMessage::createMethodCall(QStringLiteral("org.kde.baloo"),
                                                                       QStringLiteral("/"),
                                                                       QStringLiteral("org.kde.baloo.main"),
                                                                       QStringLiteral("registerBalooWatcher"));
    registerBalooWatcher.setArguments({QStringLiteral("org.mpris.MediaPlayer2.elisa/org/kde/BalooWatcherApplication")});

    auto pendingCall = sessionBus.asyncCall(registerBalooWatcher);
    qDebug() << "LocalBalooFileListing::registerToBaloo" << "call registerBalooWatcher";
    auto pendingCallWatcher = new QDBusPendingCallWatcher(pendingCall);

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

void LocalBalooFileListing::executeInit()
{
}

void LocalBalooFileListing::triggerRefreshOfContent()
{
    Q_EMIT indexingStarted();

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
        }
    }

    if (!newFiles.isEmpty() && d->mStopRequest == 0) {
        emitNewFiles(newFiles);
    }

    Q_EMIT indexingFinished();
}

MusicAudioTrack LocalBalooFileListing::scanOneFile(const QUrl &scanFile)
{
    auto newTrack = MusicAudioTrack();

    auto fileName = scanFile.toLocalFile();
    auto scanFileInfo = QFileInfo(fileName);

    if (scanFileInfo.exists()) {
        watchPath(fileName);
    }

    Baloo::File match(fileName);
    match.load();

    const auto &allProperties = match.properties();

    auto titleProperty = allProperties.find(KFileMetaData::Property::Title);
    auto durationProperty = allProperties.find(KFileMetaData::Property::Duration);
    auto artistProperty = allProperties.find(KFileMetaData::Property::Artist);
    auto albumProperty = allProperties.find(KFileMetaData::Property::Album);
    auto albumArtistProperty = allProperties.find(KFileMetaData::Property::AlbumArtist);
    auto trackNumberProperty = allProperties.find(KFileMetaData::Property::TrackNumber);
    auto discNumberProperty = allProperties.find(KFileMetaData::Property::DiscNumber);
    auto fileData = KFileMetaData::UserMetaData(fileName);

    if (albumProperty != allProperties.end()) {
        auto albumValue = albumProperty->toString();
        auto &allTracks = d->mAllAlbums[albumValue];

        newTrack.setAlbumName(albumValue);

        if (artistProperty != allProperties.end()) {
            newTrack.setArtist(artistProperty->toString());
        }

        if (durationProperty != allProperties.end()) {
            newTrack.setDuration(QTime::fromMSecsSinceStartOfDay(1000 * durationProperty->toDouble()));
        }

        if (titleProperty != allProperties.end()) {
            newTrack.setTitle(titleProperty->toString());
        }

        if (trackNumberProperty != allProperties.end()) {
            newTrack.setTrackNumber(trackNumberProperty->toInt());
        }

        if (discNumberProperty != allProperties.end()) {
            newTrack.setDiscNumber(discNumberProperty->toInt());
        }

        if (albumArtistProperty != allProperties.end()) {
            newTrack.setAlbumArtist(albumArtistProperty->toString());
        }

        if (newTrack.artist().isEmpty()) {
            newTrack.setArtist(newTrack.albumArtist());
        }

        newTrack.setRating(fileData.rating());

        newTrack.setResourceURI(scanFile);

        QFileInfo coverFilePath(scanFileInfo.dir().filePath(QStringLiteral("cover.jpg")));
        if (coverFilePath.exists()) {
            d->mAllAlbumCover[albumValue] = QUrl::fromLocalFile(coverFilePath.absoluteFilePath());
        }

        auto itTrack = std::find(allTracks.begin(), allTracks.end(), newTrack);
        if (itTrack == allTracks.end()) {
            allTracks.push_back(newTrack);
            d->mNewTracks.push_back(newTrack);
            d->mNewAlbums[newTrack.albumName()].push_back(newTrack);

            auto &newTracks = d->mAllAlbums[newTrack.albumName()];

            std::sort(allTracks.begin(), allTracks.end());
            std::sort(newTracks.begin(), newTracks.end());
        }

        if (newTrack.title().isEmpty()) {
            return newTrack;
        }

        if (newTrack.artist().isEmpty()) {
            return newTrack;
        }

        if (newTrack.albumName().isEmpty()) {
            return newTrack;
        }

        if (!newTrack.duration().isValid()) {
            return newTrack;
        }

        newTrack.setValid(true);
    }

    if (!newTrack.isValid()) {
        newTrack = AbstractFileListing::scanOneFile(scanFile);
    }

    if (newTrack.isValid()) {
        addCover(newTrack);
    }

    return newTrack;
}


#include "moc_localbaloofilelisting.cpp"
