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

#ifndef ABSTRACTFILELISTENER_H
#define ABSTRACTFILELISTENER_H

#include "notificationitem.h"

#include <QObject>
#include <QVector>
#include <QString>

#include <memory>

class AbstractFileListenerPrivate;
class DatabaseInterface;
class MusicAudioTrack;
class AbstractFileListing;

class AbstractFileListener : public QObject
{
    Q_OBJECT

    Q_PROPERTY(DatabaseInterface* databaseInterface
               READ databaseInterface
               WRITE setDatabaseInterface
               NOTIFY databaseInterfaceChanged)

    Q_PROPERTY(int importedTracksCount
               READ importedTracksCount
               NOTIFY importedTracksCountChanged)

public:
    explicit AbstractFileListener(QObject *parent = nullptr);

    ~AbstractFileListener() override;

    DatabaseInterface* databaseInterface() const;

    AbstractFileListing* fileListing() const;

    int importedTracksCount() const;

Q_SIGNALS:

    void databaseInterfaceChanged();

    void newTrackFile(const MusicAudioTrack &newTrack);

    void indexingStarted();

    void indexingFinished();

    void configurationChanged();

    void clearDatabase(const QString &listenerName);

    void importedTracksCountChanged();

    void newNotification(NotificationItem notification);

    void closeNotification(QString notificationId);

public Q_SLOTS:

    void performInitialScan();

    void setDatabaseInterface(DatabaseInterface* databaseInterface);

    void applicationAboutToQuit();

    void quitListener();

    void resetImportedTracksCounter();

protected:

    void setFileListing(AbstractFileListing *fileIndexer);

    NotificationItem& currentNotification();

private:

    std::unique_ptr<AbstractFileListenerPrivate> d;

};

#endif // ABSTRACTFILELISTENER_H
