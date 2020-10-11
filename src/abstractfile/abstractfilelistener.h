/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef ABSTRACTFILELISTENER_H
#define ABSTRACTFILELISTENER_H

#include <QObject>
#include <QString>

#include "datatypes.h"

#include <memory>

class AbstractFileListenerPrivate;
class DatabaseInterface;
class AbstractFileListing;

class AbstractFileListener : public QObject
{
    Q_OBJECT

    Q_PROPERTY(DatabaseInterface* databaseInterface
               READ databaseInterface
               WRITE setDatabaseInterface
               NOTIFY databaseInterfaceChanged)

public:
    explicit AbstractFileListener(QObject *parent = nullptr);

    ~AbstractFileListener() override;

    [[nodiscard]] DatabaseInterface* databaseInterface() const;

    [[nodiscard]] AbstractFileListing* fileListing() const;

    [[nodiscard]] bool canHandleRootPaths() const;

Q_SIGNALS:

    void databaseInterfaceChanged();

    void newTrackFile(const DataTypes::TrackDataType &newTrack);

    void indexingStarted();

    void indexingFinished();

    void configurationChanged();

    void clearDatabase();

public Q_SLOTS:

    void setDatabaseInterface(DatabaseInterface* databaseInterface);

    void applicationAboutToQuit();

    void quitListener();

    void setAllRootPaths(const QStringList &allRootPaths);

protected:

    void setFileListing(AbstractFileListing *fileIndexer);

private:

    std::unique_ptr<AbstractFileListenerPrivate> d;

};

#endif // ABSTRACTFILELISTENER_H
