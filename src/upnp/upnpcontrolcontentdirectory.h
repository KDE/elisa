/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef UPNPCONTROLCONTENTDIRECTORY_H
#define UPNPCONTROLCONTENTDIRECTORY_H

#include "upnpcontrolabstractservice.h"
#include "upnpbasictypes.h"

#include <QQmlEngine>

#include <memory>

class UpnpControlContentDirectoryPrivate;
class UpnpControlAbstractServiceReply;

class UpnpControlContentDirectory : public UpnpControlAbstractService
{
    Q_OBJECT

    QML_ELEMENT

    Q_PROPERTY(QString searchCapabilities
               READ searchCapabilities
               NOTIFY searchCapabilitiesChanged)

    Q_PROPERTY(QString transferIDs
               READ transferIDs
               NOTIFY transferIDsChanged)

    Q_PROPERTY(bool hasTransferIDs
               READ hasTransferIDs
               NOTIFY hasTransferIDsChanged)

    Q_PROPERTY(QString sortCapabilities
               READ sortCapabilities
               NOTIFY sortCapabilitiesChanged)

    Q_PROPERTY(int systemUpdateID
               READ systemUpdateID
               NOTIFY systemUpdateIDChanged)

public:

    explicit UpnpControlContentDirectory(QObject *parent = nullptr);

    ~UpnpControlContentDirectory() override;

    [[nodiscard]] const QString& searchCapabilities() const;

    [[nodiscard]] const QString& transferIDs() const;

    [[nodiscard]] bool hasTransferIDs() const;

    [[nodiscard]] const QString& sortCapabilities() const;

    [[nodiscard]] int systemUpdateID() const;

public Q_SLOTS:

    UpnpControlAbstractServiceReply* getSearchCapabilities();

    UpnpControlAbstractServiceReply* getSortCapabilities();

    UpnpControlAbstractServiceReply* getSystemUpdateID();

    UpnpControlAbstractServiceReply* search(const QString &objectID, const QString &searchCriteria,
                const QString &filter, int startingIndex,
                int requestedCount, const QString &sortCriteria);

    UpnpControlAbstractServiceReply* browse(const QString &objectID, const QString &browseFlag,
                const QString &filter, int startingIndex,
                int requestedCount, const QString &sortCriteria);

Q_SIGNALS:

    void searchCapabilitiesChanged(const QString &capabilities);

    void transferIDsChanged(const QString &ids);

    void hasTransferIDsChanged();

    void sortCapabilitiesChanged(const QString &capabilities);

    void systemUpdateIDChanged(int id);

private Q_SLOTS:

protected:

    void parseServiceDescription(QIODevice *serviceDescriptionContent) override;

    void parseEventNotification(const QString &eventName, const QString &eventValue) override;

private:

    std::unique_ptr<UpnpControlContentDirectoryPrivate> d;

};

#endif // UPNPCONTROLCONTENTDIRECTORY_H

