/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef ANDROIDMUSICLISTENER_H
#define ANDROIDMUSICLISTENER_H

#include "datatypes.h"

#include <QObject>
#include <QList>
#include <QHash>
#include <QUrl>
#include <QString>

#include <memory>

class DatabaseInterface;
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

    void tracksList(const DataTypes::ListTrackDataType &tracks, const QHash<QString, QUrl> &covers);

    void removedTracksList(const QList<QUrl> &removedTracks);

    void askRestoredTracks(const QString &musicSource);

public Q_SLOTS:

    void setDatabaseInterface(DatabaseInterface* databaseInterface);

    void applicationAboutToQuit();

    void quitListener();

    void restoredTracks(QHash<QUrl, QDateTime> allFiles);

    void init();

private:

    static AndroidMusicListener* mCurrentInstance;

    std::unique_ptr<AndroidMusicListenerPrivate> d;

};

#endif // ANDROIDMUSICLISTENER_H
