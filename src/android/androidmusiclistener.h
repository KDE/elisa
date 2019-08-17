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

#ifndef ANDROIDMUSICLISTENER_H
#define ANDROIDMUSICLISTENER_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QUrl>
#include <QString>

#include <memory>

class DatabaseInterface;
class MusicAudioTrack;
class AndroidMusicListenerPrivate;

class AndroidMusicListener : public QObject
{

    Q_OBJECT

    Q_PROPERTY(DatabaseInterface* databaseInterface
               READ databaseInterface
               WRITE setDatabaseInterface
               NOTIFY databaseInterfaceChanged)

public:

    explicit AndroidMusicListener(QObject *parent = nullptr);

    ~AndroidMusicListener() override;

    DatabaseInterface* databaseInterface() const;

    static void registerNativeMethods();

    static AndroidMusicListener* currentInstance();

    void androidMusicTracksScanStarted();

    void newMusicTrack(const QString &trackDescription);

    void androidMusicTracksScanFinished();

    void androidMusicAlbumsScanStarted();

    void newMusicAlbum(const QString &albumDescription);

    void androidMusicAlbumsScanFinished();

Q_SIGNALS:

    void databaseInterfaceChanged();

    void indexingStarted();

    void indexingFinished();

    void clearDatabase(const QString &listenerName);

    void tracksList(const QList<MusicAudioTrack> &tracks, const QHash<QString, QUrl> &covers, const QString &musicSource);

    void removedTracksList(const QList<QUrl> &removedTracks);

    void modifyTracksList(const QList<MusicAudioTrack> &modifiedTracks, const QHash<QString, QUrl> &covers, const QString &musicSource);

    void askRestoredTracks(const QString &musicSource);

public Q_SLOTS:

    void setDatabaseInterface(DatabaseInterface* databaseInterface);

    void applicationAboutToQuit();

    void quitListener();

    void restoredTracks(const QString &musicSource, QHash<QUrl, QDateTime> allFiles);

    void init();

private:

    static AndroidMusicListener* mCurrentInstance;

    std::unique_ptr<AndroidMusicListenerPrivate> d;

};

#endif // ANDROIDMUSICLISTENER_H
