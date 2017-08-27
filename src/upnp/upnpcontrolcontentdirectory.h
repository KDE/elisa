/*
 * Copyright 2015-2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef UPNPCONTROLCONTENTDIRECTORY_H
#define UPNPCONTROLCONTENTDIRECTORY_H

#include "upnpcontrolabstractservice.h"
#include "upnpbasictypes.h"

#include <memory>

class UpnpControlContentDirectoryPrivate;
class UpnpControlAbstractServiceReply;

class UpnpControlContentDirectory : public UpnpControlAbstractService
{
    Q_OBJECT

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

    const QString& searchCapabilities() const;

    const QString& transferIDs() const;

    bool hasTransferIDs() const;

    const QString& sortCapabilities() const;

    int systemUpdateID() const;

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

    void parseEventNotification(const QString &eventName, const QString &eventValue) Q_DECL_OVERRIDE;

private:

    std::unique_ptr<UpnpControlContentDirectoryPrivate> d;

};

#endif // UPNPCONTROLCONTENTDIRECTORY_H

