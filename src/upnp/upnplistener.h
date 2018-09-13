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

#ifndef UPNPLISTENER_H
#define UPNPLISTENER_H

#include <QObject>
#include <QVector>
#include <QString>

#include <memory>

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

    explicit UpnpListener(QObject *parent = nullptr);

    ~UpnpListener() override;

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

    std::unique_ptr<UpnpListenerPrivate> d;

};

#endif // UPNPLISTENER_H
