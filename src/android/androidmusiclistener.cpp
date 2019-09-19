/*
 * Copyright 2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "androidmusiclistener.h"

#include "databaseinterface.h"
#include "musicaudiotrack.h"

#include <QDateTime>
#include <QMetaObject>

// C++ code
#include <jni.h>
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>
#include <QtAndroid>
#include <QDebug>

class AndroidMusicListenerPrivate
{
public:

    QString mSourceName;

    QHash<QUrl, QDateTime> mAllFiles;

    QList<MusicAudioTrack> mNewTracks;
    QHash<QString, QUrl> mCovers;

};

static void tracksAndroidScanStarted(JNIEnv */*env*/, jobject /*obj*/)
{
    AndroidMusicListener::currentInstance()->androidMusicTracksScanStarted();
}

static void sentMusicFile(JNIEnv */*env*/, jobject /*obj*/, jstring musicFile)
{
    QAndroidJniObject musicFileJavaString(musicFile);
    AndroidMusicListener::currentInstance()->newMusicTrack(musicFileJavaString.toString());
}

static void tracksAndroidScanFinished(JNIEnv */*env*/, jobject /*obj*/)
{
    AndroidMusicListener::currentInstance()->androidMusicTracksScanFinished();
}

static void albumsAndroidScanStarted(JNIEnv */*env*/, jobject /*obj*/)
{
    AndroidMusicListener::currentInstance()->androidMusicAlbumsScanStarted();
}

static void sentMusicAlbum(JNIEnv */*env*/, jobject /*obj*/, jstring musicAlbum)
{
    QAndroidJniObject musicAlbumJavaString(musicAlbum);
    AndroidMusicListener::currentInstance()->newMusicAlbum(musicAlbumJavaString.toString());
}

static void albumsAndroidScanFinished(JNIEnv */*env*/, jobject /*obj*/)
{
    AndroidMusicListener::currentInstance()->androidMusicAlbumsScanFinished();
}

AndroidMusicListener::AndroidMusicListener(QObject *parent) : QObject(parent), d(std::make_unique<AndroidMusicListenerPrivate>())
{
    AndroidMusicListener::mCurrentInstance = this;

    d->mSourceName = QStringLiteral("Android");

    registerNativeMethods();
}

DatabaseInterface *AndroidMusicListener::databaseInterface() const
{
    return nullptr;
}

void AndroidMusicListener::registerNativeMethods()
{
    JNINativeMethod methods[] {{"androidMusicScanTracksStarting", "()V", reinterpret_cast<void *>(tracksAndroidScanStarted)},
        {"sendMusicFile", "(Ljava/lang/String;)V", reinterpret_cast<void *>(sentMusicFile)},
        {"androidMusicScanTracksFinishing", "()V", reinterpret_cast<void *>(tracksAndroidScanFinished)},
        {"androidMusicScanAlbumsStarting", "()V", reinterpret_cast<void *>(albumsAndroidScanStarted)},
        {"sendMusicAlbum", "(Ljava/lang/String;)V", reinterpret_cast<void *>(sentMusicAlbum)},
        {"androidMusicScanAlbumsFinishing", "()V", reinterpret_cast<void *>(albumsAndroidScanFinished)},
    };

    QAndroidJniObject javaClass("org/kde/elisa/ElisaAndroidMusicScanner");
    QAndroidJniEnvironment env;
    jclass objectClass = env->GetObjectClass(javaClass.object<jobject>());
    env->RegisterNatives(objectClass,
                         methods,
                         sizeof(methods) / sizeof(methods[0]));
    env->DeleteLocalRef(objectClass);
}

AndroidMusicListener *AndroidMusicListener::mCurrentInstance = nullptr;

AndroidMusicListener *AndroidMusicListener::currentInstance()
{
    return mCurrentInstance;
}

void AndroidMusicListener::androidMusicTracksScanStarted()
{
}

void AndroidMusicListener::newMusicTrack(const QString &trackDescription)
{
    auto trackData = trackDescription.split(QStringLiteral("||"));
    auto newTrack = MusicAudioTrack{};
    newTrack.setTitle(trackData[1]);
    bool conversionOK = false;
    if (trackData[2] != QLatin1String("null")) {
        newTrack.setTrackNumber(trackData[2].toInt(&conversionOK));
        if (!conversionOK) {
            qInfo() << "newMusicTrack" << trackData[1] << trackData[2];
        }
    }
    if (trackData[3] != QLatin1String("null")) {
        newTrack.setYear(trackData[3].toInt(&conversionOK));
        if (!conversionOK) {
            qInfo() << "newMusicTrack" << trackData[1] << trackData[3];
        }
    }
    if (trackData[4] != QLatin1String("null")) {
        newTrack.setDuration(QTime::fromMSecsSinceStartOfDay(trackData[4].toInt()));
    }
    newTrack.setResourceURI(QUrl::fromLocalFile(trackData[5]));
    newTrack.setArtist(trackData[6]);
    newTrack.setAlbumName(trackData[8]);
    newTrack.setComposer(trackData[10]);

    d->mNewTracks.push_back(newTrack);
}

void AndroidMusicListener::androidMusicTracksScanFinished()
{
}

void AndroidMusicListener::androidMusicAlbumsScanStarted()
{
}

void AndroidMusicListener::newMusicAlbum(const QString &albumDescription)
{
    auto albumData = albumDescription.split(QStringLiteral("||"));

    if (albumData[2] != QLatin1String("null)")) {
        d->mCovers[albumData[1]] = QUrl::fromLocalFile(albumData[2]);
    }
}

void AndroidMusicListener::androidMusicAlbumsScanFinished()
{
    Q_EMIT tracksList(d->mNewTracks, d->mCovers, d->mSourceName);
}

void AndroidMusicListener::setDatabaseInterface(DatabaseInterface *model)
{
    if (model) {
        connect(this, &AndroidMusicListener::tracksList, model, &DatabaseInterface::insertTracksList);
        connect(this, &AndroidMusicListener::removedTracksList, model, &DatabaseInterface::removeTracksList);
        connect(this, &AndroidMusicListener::modifyTracksList, model, &DatabaseInterface::modifyTracksList);
        connect(this, &AndroidMusicListener::askRestoredTracks, model, &DatabaseInterface::askRestoredTracks);
        connect(model, &DatabaseInterface::restoredTracks, this, &AndroidMusicListener::restoredTracks);
    }

    Q_EMIT databaseInterfaceChanged();

    if (model) {
        QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
    }
}

void AndroidMusicListener::applicationAboutToQuit()
{
}

void AndroidMusicListener::quitListener()
{
    applicationAboutToQuit();

    Q_EMIT clearDatabase(d->mSourceName);
}

void AndroidMusicListener::restoredTracks(const QString &musicSource, QHash<QUrl, QDateTime> allFiles)
{
    if (d->mSourceName == musicSource) {
        d->mAllFiles = allFiles;

        QAndroidJniObject::callStaticMethod<void>("org/kde/elisa/ElisaAndroidMusicScanner",
                                                  "listAudioFiles",
                                                  "(Landroid/content/Context;)V",
                                                  QtAndroid::androidContext().object());
    }
}

void AndroidMusicListener::init()
{
    Q_EMIT askRestoredTracks(d->mSourceName);
}

AndroidMusicListener::~AndroidMusicListener() = default;


#include "moc_androidmusiclistener.cpp"
