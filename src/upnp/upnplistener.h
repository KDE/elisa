/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef UPNPLISTENER_H
#define UPNPLISTENER_H

#include "datatypes.h"

#include <QObject>
#include <QVector>
#include <QString>

#include <memory>

class UpnpListenerPrivate;

class DatabaseInterface;

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

    void initialTracksList(QString musicSource, DataTypes::ListUpnpTrackDataType initialList);

public Q_SLOTS:

    void setDatabaseInterface(DatabaseInterface* databaseInterface);

    void applicationAboutToQuit();

private:

    std::unique_ptr<UpnpListenerPrivate> d;

};

#endif // UPNPLISTENER_H
