/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef LOCALBALOOFILELISTING_H
#define LOCALBALOOFILELISTING_H

#include "../abstractfile/abstractfilelisting.h"

#include <QObject>
#include <QString>
#include <QUrl>
#include <QHash>

#include <memory>

class LocalBalooFileListingPrivate;
class QDBusPendingCallWatcher;

class LocalBalooFileListing : public AbstractFileListing
{

    Q_OBJECT

public:

    explicit LocalBalooFileListing(QObject *parent = nullptr);

    ~LocalBalooFileListing() override;

    void applicationAboutToQuit() override;

    [[nodiscard]] bool canHandleRootPaths() const override;

Q_SIGNALS:

public Q_SLOTS:

    void renamedFiles(const QString &from, const QString &to, const QStringList &listFiles);

    void serviceOwnerChanged(const QString &serviceName, const QString &oldOwner, const QString &newOwner);

    void serviceRegistered(const QString &serviceName);

    void serviceUnregistered(const QString &serviceName);

private Q_SLOTS:

    void newBalooFile(const QString &fileName);

    void registeredToBaloo(QDBusPendingCallWatcher *watcher);

    void registeredToBalooWatcher(QDBusPendingCallWatcher *watcher);

private:

    void registerToBaloo();

    void executeInit(QHash<QUrl, QDateTime> allFiles) override;

    void triggerRefreshOfContent() override;

    void triggerStop() override;

    DataTypes::TrackDataType scanOneFile(const QUrl &scanFile, const QFileInfo &scanFileInfo, FileSystemWatchingModes watchForFileSystemChanges) override;

    std::unique_ptr<LocalBalooFileListingPrivate> d;

};

#endif // LOCALBALOOFILELISTING_H
