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
#include <QHash>
#include <QFileInfo>
#include <QDir>
#include <QAtomicInt>
#include <QScopedPointer>
#include <QDebug>
#include <QGuiApplication>

// C++ code
#include <jni.h>
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>
#include <QtAndroid>

#include <algorithm>
#include <memory>

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

    if (QtAndroid::checkPermission(QStringLiteral("android.permission.READ_EXTERNAL_STORAGE")) == QtAndroid::PermissionResult::Denied) {
        QtAndroid::requestPermissionsSync({QStringLiteral("android.permission.READ_EXTERNAL_STORAGE")});
    }

    if (QtAndroid::checkPermission(QStringLiteral("android.permission.READ_EXTERNAL_STORAGE")) == QtAndroid::PermissionResult::Denied) {
        qCInfo(orgKdeElisaAndroid()) << "AndroidFileListing::triggerRefreshOfContent" << "not scanning files due to missing permission";
        return;
    }

    AbstractFileListing::triggerRefreshOfContent();

    QAndroidJniObject musicList = QAndroidJniObject::callStaticObjectMethod("org/kde/elisa/ElisaActivity",
                                              "listAudioFiles",
                                              "(Landroid/content/Context;)Ljava/util/ArrayList;",
                                              QtAndroid::androidContext().object());

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
        newTrack[DataTypes::ImageUrlRole] = QUrl::fromLocalFile(track.callObjectMethod("getAlbumCover", "()Ljava/lang/String;").toString());
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
    auto newTrack = DataTypes::TrackDataType{};

    return newTrack;
}

#include "moc_androidfilelisting.cpp"
