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

static void tracksAndroidScanStarted(JNIEnv */*env*/, jobject /*obj*/)
{
    AndroidFileListing::currentInstance()->androidMusicTracksScanStarted();
}

static void sentMusicFile(JNIEnv */*env*/, jobject /*obj*/, jstring musicFile)
{
    QAndroidJniObject musicFileJavaString(musicFile);
    AndroidFileListing::currentInstance()->newMusicTrack(musicFileJavaString.toString());
}

static void tracksAndroidScanFinished(JNIEnv */*env*/, jobject /*obj*/)
{
    AndroidFileListing::currentInstance()->androidMusicTracksScanFinished();
}

static void albumsAndroidScanStarted(JNIEnv */*env*/, jobject /*obj*/)
{
    AndroidFileListing::currentInstance()->androidMusicAlbumsScanStarted();
}

static void sentMusicAlbum(JNIEnv */*env*/, jobject /*obj*/, jstring musicAlbum)
{
    QAndroidJniObject musicAlbumJavaString(musicAlbum);
    AndroidFileListing::currentInstance()->newMusicAlbum(musicAlbumJavaString.toString());
}

static void albumsAndroidScanFinished(JNIEnv */*env*/, jobject /*obj*/)
{
    AndroidFileListing::currentInstance()->androidMusicAlbumsScanFinished();
}

static void readExternalStoragePermissionIsOk(JNIEnv */*env*/, jobject /*obj*/)
{
    AndroidFileListing::currentInstance()->readExternalStoragePermissionIsOk();
}

static void readExternalStoragePermissionIsKo(JNIEnv */*env*/, jobject /*obj*/)
{
    AndroidFileListing::currentInstance()->readExternalStoragePermissionIsKo();
}

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
    JNINativeMethod methods[] {{"androidMusicScanTracksStarting", "()V", reinterpret_cast<void *>(tracksAndroidScanStarted)},
        {"sendMusicFile", "(Ljava/lang/String;)V", reinterpret_cast<void *>(sentMusicFile)},
        {"androidMusicScanTracksFinishing", "()V", reinterpret_cast<void *>(tracksAndroidScanFinished)},
        {"androidMusicScanAlbumsStarting", "()V", reinterpret_cast<void *>(albumsAndroidScanStarted)},
        {"sendMusicAlbum", "(Ljava/lang/String;)V", reinterpret_cast<void *>(sentMusicAlbum)},
        {"androidMusicScanAlbumsFinishing", "()V", reinterpret_cast<void *>(albumsAndroidScanFinished)},
        {"readExternalStoragePermissionIsOk", "()V", reinterpret_cast<void *>(::readExternalStoragePermissionIsOk)},
        {"readExternalStoragePermissionIsKo", "()V", reinterpret_cast<void *>(::readExternalStoragePermissionIsKo)},
    };

    QAndroidJniObject javaClass("org/kde/elisa/ElisaActivity");
    QAndroidJniEnvironment env;
    jclass objectClass = env->GetObjectClass(javaClass.object<jobject>());
    env->RegisterNatives(objectClass,
                         methods,
                         sizeof(methods) / sizeof(methods[0]));
    env->DeleteLocalRef(objectClass);
}

void AndroidFileListing::readExternalStoragePermissionIsOk()
{
    QAndroidJniObject::callStaticMethod<void>("org/kde/elisa/ElisaActivity",
                                              "listAudioFiles",
                                              "(Landroid/content/Context;)V",
                                              QtAndroid::androidContext().object());
}

void AndroidFileListing::readExternalStoragePermissionIsKo()
{
    qCInfo(orgKdeElisaAndroid()) << "AndroidFileListing::readExternalStoragePermissionIsKo";
}

void AndroidFileListing::androidMusicTracksScanStarted()
{
    qCInfo(orgKdeElisaAndroid()) << "AndroidFileListing::androidMusicTracksScanStarted";
}

void AndroidFileListing::newMusicTrack(const QString &trackDescription)
{
    qCInfo(orgKdeElisaAndroid()) << "AndroidFileListing::newMusicTrack" << trackDescription;
    auto trackData = trackDescription.split(QStringLiteral("||"));
    auto newTrack = DataTypes::TrackDataType{};
    newTrack[DataTypes::TitleRole] = trackData[1];
    bool conversionOK = false;
    if (trackData[2] != QLatin1String("null")) {
        newTrack[DataTypes::TrackNumberRole] = trackData[2].toInt(&conversionOK);
        if (!conversionOK) {
            qInfo() << "newMusicTrack" << trackData[1] << trackData[2];
        }
    }

    if (trackData[3] != QLatin1String("null")) {
        newTrack[DataTypes::YearRole] = trackData[3].toInt(&conversionOK);
        if (!conversionOK) {
            qInfo() << "newMusicTrack" << trackData[1] << trackData[3];
        }
    }

    if (trackData[4] != QLatin1String("null")) {
        newTrack[DataTypes::DurationRole] = QTime::fromMSecsSinceStartOfDay(trackData[4].toInt());
    } else {
        newTrack[DataTypes::DurationRole] = QTime::fromMSecsSinceStartOfDay(1);
    }
    newTrack[DataTypes::ResourceRole] = QUrl::fromLocalFile(trackData[5]);
    newTrack[DataTypes::ArtistRole] = trackData[6];
    newTrack[DataTypes::AlbumRole] = trackData[8];
    newTrack[DataTypes::ComposerRole] = trackData[10];

    QFileInfo scanFileInfo(trackData[5]);
    newTrack[DataTypes::FileModificationTime] = scanFileInfo.metadataChangeTime();

    newTrack[DataTypes::RatingRole] = 0;
    newTrack[DataTypes::ElementTypeRole] = ElisaUtils::Track;

    qCInfo(orgKdeElisaAndroid()) << "AndroidFileListing::newMusicTrack" << newTrack;

    d->mNewTracks.push_back(newTrack);
}

void AndroidFileListing::androidMusicTracksScanFinished()
{
    qCInfo(orgKdeElisaAndroid()) << "AndroidFileListing::androidMusicTracksScanFinished";
}

void AndroidFileListing::androidMusicAlbumsScanStarted()
{
    qCInfo(orgKdeElisaAndroid()) << "AndroidFileListing::androidMusicAlbumsScanStarted";
}

void AndroidFileListing::newMusicAlbum(const QString &albumDescription)
{
    auto albumData = albumDescription.split(QStringLiteral("||"));

    if (albumData[2] != QLatin1String("null)")) {
        d->mCovers[albumData[1]] = QUrl::fromLocalFile(albumData[2]);
    }
}

void AndroidFileListing::androidMusicAlbumsScanFinished()
{
    qCInfo(orgKdeElisaAndroid()) << "AndroidFileListing::androidMusicAlbumsScanFinished";

    Q_EMIT tracksList(d->mNewTracks, d->mCovers);

    Q_EMIT indexingFinished();
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

    AbstractFileListing::triggerRefreshOfContent();

    QAndroidJniObject::callStaticMethod<void>("org/kde/elisa/ElisaActivity",
                                              "checkPermissions",
                                              "(Landroid/app/Activity;)V",
                                              QtAndroid::androidActivity().object());
}

DataTypes::TrackDataType AndroidFileListing::scanOneFile(const QUrl &scanFile, const QFileInfo &scanFileInfo, FileSystemWatchingModes watchForFileSystemChanges)
{
    auto newTrack = DataTypes::TrackDataType{};

    return newTrack;
}

#if 0
void AndroidMusicListener::init()
{
    Q_EMIT askRestoredTracks(d->mSourceName);
}
#endif

#include "moc_androidfilelisting.cpp"
