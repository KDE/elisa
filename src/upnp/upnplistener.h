/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

class UpnpListenerPrivate;

class DatabaseInterface;
class UpnpDiscoverAllMusic;
class UpnpSsdpEngine;

class UpnpListener : public QObject
{
    Q_OBJECT

    Q_PROPERTY(DatabaseInterface* databaseInterface
               READ databaseInterface
               WRITE setDatabaseInterface
               NOTIFY databaseInterfaceChanged)

    Q_PROPERTY(UpnpDiscoverAllMusic* upnpManager
               READ upnpManager
               WRITE setUpnpManager
               NOTIFY upnpManagerChanged)

    Q_PROPERTY(UpnpSsdpEngine* ssdpEngine
               READ ssdpEngine
               WRITE setSsdpEngine
               NOTIFY ssdpEngineChanged)

public:

    explicit UpnpListener(QObject *parent = 0);

    virtual ~UpnpListener();

    DatabaseInterface* databaseInterface() const;

    UpnpDiscoverAllMusic* upnpManager() const;

    UpnpSsdpEngine* ssdpEngine() const;

Q_SIGNALS:

    void refreshContent();

    void databaseInterfaceChanged();

    void upnpManagerChanged();

    void ssdpEngineChanged();

public Q_SLOTS:

void setDatabaseInterface(DatabaseInterface* databaseInterface);

void setUpnpManager(UpnpDiscoverAllMusic* upnpManager);

void setSsdpEngine(UpnpSsdpEngine* ssdpEngine);

private:

    UpnpListenerPrivate *d = nullptr;

};

#endif // UPNPLISTENER_H
