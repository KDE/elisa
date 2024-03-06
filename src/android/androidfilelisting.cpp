/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "androidfilelisting.h"

#include "android/androidcommon.h"

#include "elisa_settings.h"
#include "elisautils.h"

#include "filescanner.h"

#include <KLocalizedString>

#include <QThread>
#include <QFileInfo>
#include <QAtomicInt>

// C++ code
#include <jni.h>
#include <QCoreApplication>
#include <QJniObject>
#include <QtCore/private/qandroidextras_p.h>
#include <QOperatingSystemVersion>

#include <algorithm>

class AndroidFileListingPrivate
{
public:
    QHash<QString, DataTypes::ListTrackDataType> mAllAlbums;

    QHash<QUrl, QDateTime> mAllFiles;

    DataTypes::ListTrackDataType mNewTracks;

    QHash<QString, QUrl> mCovers;

    QAtomicInt mStopRequest = 0;
};

AndroidFileListing::AndroidFileListing(QObject *parent)
    : AbstractFileListing(parent), d(std::make_unique<AndroidFileListingPrivate>())
{
    AndroidFileListing::mCurrentInstance = this;

    registerNativeMethods();
}

AndroidFileListing::~AndroidFileListing() = default;

void AndroidFileListing::applicationAboutToQuit()
{
    AbstractFileListing::applicationAboutToQuit();
    d->mStopRequest = 1;
}

AndroidFileListing *AndroidFileListing::mCurrentInstance = nullptr;

AndroidFileListing *AndroidFileListing::currentInstance()
{
    return mCurrentInstance;
}

void AndroidFileListing::registerNativeMethods()
{
}

void AndroidFileListing::executeInit(QHash<QUrl, QDateTime> allFiles)
{
    qCInfo(orgKdeElisaAndroid()) << "AndroidFileListing::executeInit";
    AbstractFileListing::executeInit(std::move(allFiles));
}

void AndroidFileListing::triggerRefreshOfContent()
{
    qCInfo(orgKdeElisaAndroid()) << "AndroidFileListing::triggerRefreshOfContent";
    Q_EMIT indexingStarted();

    if (QOperatingSystemVersion::current() >= QOperatingSystemVersion(QOperatingSystemVersion::Android, 13)) {
        qCInfo(orgKdeElisaAndroid()) << "AndroidFileListing::triggerRefreshOfContent" << "detected android version >=13";
        const auto audioPermissionCheck = QtAndroidPrivate::checkPermission(QStringLiteral("android.permission.READ_MEDIA_AUDIO"));

        if (audioPermissionCheck.result() == QtAndroidPrivate::PermissionResult::Denied) {
            const auto audioPermissionRequest = QtAndroidPrivate::requestPermission(QStringLiteral("android.permission.READ_MEDIA_AUDIO"));
            if (audioPermissionRequest.result() == QtAndroidPrivate::PermissionResult::Denied) {
                qCInfo(orgKdeElisaAndroid()) << "AndroidFileListing::triggerRefreshOfContent" << "not scanning files due to missing permission";
                return;
            }
        }
    } else {
        qCInfo(orgKdeElisaAndroid()) << "AndroidFileListing::triggerRefreshOfContent" << "detected android version <13";
        const auto storagePermissionCheck = QtAndroidPrivate::checkPermission(QStringLiteral("android.permission.READ_EXTERNAL_STORAGE"));

        if (storagePermissionCheck.result() == QtAndroidPrivate::PermissionResult::Denied) {
            const auto storagePermissionRequest = QtAndroidPrivate::requestPermission(QStringLiteral("android.permission.READ_EXTERNAL_STORAGE"));
            if (storagePermissionRequest.result() == QtAndroidPrivate::PermissionResult::Denied) {
                qCInfo(orgKdeElisaAndroid()) << "AndroidFileListing::triggerRefreshOfContent" << "not scanning files due to missing permission";
                return;
            }
        }
    }

    AbstractFileListing::triggerRefreshOfContent();

    QJniObject musicList = QJniObject::callStaticObjectMethod(
        "org/kde/elisa/ElisaActivity", "listAudioFiles", "(Landroid/content/Context;)Ljava/util/ArrayList;", QNativeInterface::QAndroidApplication::context());

    auto nbTracks = musicList.callMethod<jint>("size");

    for (int i = 0; i < nbTracks; ++i) {
        auto newTrack = DataTypes::TrackDataType{};

        auto track = musicList.callObjectMethod("get", "(I)Ljava/lang/Object;", i);

        newTrack[DataTypes::TitleRole] = track.callObjectMethod("getTitle", "()Ljava/lang/String;").toString();
        newTrack[DataTypes::ArtistRole] = track.callObjectMethod("getArtist", "()Ljava/lang/String;").toString();
        newTrack[DataTypes::AlbumRole] = track.callObjectMethod("getAlbumName", "()Ljava/lang/String;").toString();
        newTrack[DataTypes::AlbumArtistRole] = track.callObjectMethod("getAlbumArtist", "()Ljava/lang/String;").toString();
        newTrack[DataTypes::TrackNumberRole] = track.callMethod<jint>("getTrackNumber");
        newTrack[DataTypes::DiscNumberRole] = track.callMethod<jint>("getDiscNumber");
        newTrack[DataTypes::DurationRole] = QTime::fromMSecsSinceStartOfDay(track.callMethod<jlong>("getDuration"));
        newTrack[DataTypes::ResourceRole] = QUrl::fromLocalFile(track.callObjectMethod("getResourceURI", "()Ljava/lang/String;").toString());

        const auto albumCoverStr = track.callObjectMethod("getAlbumCover", "()Ljava/lang/String;").toString();
        if (QOperatingSystemVersion::current() >= QOperatingSystemVersion(QOperatingSystemVersion::Android, 10)) {
            newTrack[DataTypes::ImageUrlRole] = QUrl(QLatin1String("image://android/") + albumCoverStr);
        } else {
            newTrack[DataTypes::ImageUrlRole] = QUrl::fromLocalFile(albumCoverStr);
        }

        newTrack[DataTypes::GenreRole] = track.callObjectMethod("getGenre", "()Ljava/lang/String;").toString();
        newTrack[DataTypes::ComposerRole] = track.callObjectMethod("getComposer", "()Ljava/lang/String;").toString();

        QFileInfo scanFileInfo(newTrack.resourceURI().toLocalFile());
        newTrack[DataTypes::FileModificationTime] = scanFileInfo.metadataChangeTime();

        newTrack[DataTypes::RatingRole] = 0;
        newTrack[DataTypes::ElementTypeRole] = ElisaUtils::Track;

        qCInfo(orgKdeElisaAndroid()) << "AndroidFileListing::triggerRefreshOfContent" << newTrack;

        d->mNewTracks.push_back(newTrack);
    }

    Q_EMIT tracksList(d->mNewTracks, d->mCovers);

    Q_EMIT indexingFinished();
}

DataTypes::TrackDataType AndroidFileListing::scanOneFile(const QUrl &scanFile, const QFileInfo &scanFileInfo, FileSystemWatchingModes watchForFileSystemChanges)
{
    Q_UNUSED(scanFile)
    Q_UNUSED(scanFileInfo)
    Q_UNUSED(watchForFileSystemChanges)
    auto newTrack = DataTypes::TrackDataType{};

    return newTrack;
}

#include "moc_androidfilelisting.cpp"
