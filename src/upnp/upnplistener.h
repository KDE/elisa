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

#ifndef UPNPLISTENER_H
#define UPNPLISTENER_H

#include <QObject>
#include <QVector>
#include <QString>

class UpnpListenerPrivate;

class DatabaseInterface;
class UpnpDiscoverAllMusic;
class UpnpSsdpEngine;
class MusicAudioTrack;

class UpnpListener : public QObject
{
    Q_OBJECT

    Q_PROPERTY(DatabaseInterface* databaseInterface
               READ databaseInterface
               WRITE setDatabaseInterface
               NOTIFY databaseInterfaceChanged)

public:

    explicit UpnpListener(QObject *parent = 0);

    virtual ~UpnpListener();

    DatabaseInterface* databaseInterface() const;

Q_SIGNALS:

    void databaseInterfaceChanged();

    void databaseReady();

    void initialTracksListRequired(QString musicSource);

    void initialTracksList(QString musicSource, QList<MusicAudioTrack> initialList);

public Q_SLOTS:

    void setDatabaseInterface(DatabaseInterface* databaseInterface);

    void applicationAboutToQuit();

private:

    UpnpListenerPrivate *d = nullptr;

};

#endif // UPNPLISTENER_H
