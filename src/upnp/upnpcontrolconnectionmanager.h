/*
 * Copyright 2015 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef UPNPCONTROLCONNECTIONMANAGER_H
#define UPNPCONTROLCONNECTIONMANAGER_H

#include "upnpQt_export.h"

#include "upnpcontrolabstractservice.h"
#include "upnpbasictypes.h"

#include <memory>

class UpnpControlConnectionManagerPrivate;

class UPNPQT_EXPORT UpnpControlConnectionManager : public UpnpControlAbstractService
{
    Q_OBJECT

    Q_PROPERTY(QString sourceProtocolInfo
               READ sourceProtocolInfo
               NOTIFY sourceProtocolInfoChanged)

    Q_PROPERTY(QString sinkProtocolInfo
               READ sinkProtocolInfo
               NOTIFY sinkProtocolInfoChanged)

    Q_PROPERTY(QString currentConnectionIDs
               READ currentConnectionIDs
               NOTIFY currentConnectionIDsChanged)

    Q_PROPERTY(bool hasPrepareForConnection
               READ hasPrepareForConnection
               NOTIFY hasPrepareForConnectionChanged)

    Q_PROPERTY(bool hasConnectionComplete
               READ hasConnectionComplete
               NOTIFY hasConnectionCompleteChanged)

public:

    explicit UpnpControlConnectionManager(QObject *parent = nullptr);

    ~UpnpControlConnectionManager() override;

    const QString& sourceProtocolInfo() const;

    const QString& sinkProtocolInfo() const;

    const QString& currentConnectionIDs() const;

    bool hasPrepareForConnection() const;

    bool hasConnectionComplete() const;

public Q_SLOTS:

    UpnpControlAbstractServiceReply *getProtocolInfo();

    UpnpControlAbstractServiceReply *prepareForConnection(const QString &remoteProtocolInfo, const QString &remoteUDN, const QString &remoteServiceID, int remotePeerConnectionID, const QString &connectionDirection);

    UpnpControlAbstractServiceReply *connectionComplete(int currentConnectionID);

    UpnpControlAbstractServiceReply *getCurrentConnectionIDs();

    UpnpControlAbstractServiceReply *getCurrentConnectionInfo(int currentConnectionID);

Q_SIGNALS:

    void sourceProtocolInfoChanged(const QString &protocolInfo);

    void sinkProtocolInfoChanged(const QString &protocolInfo);

    void currentConnectionIDsChanged(const QString &ids);

    void hasPrepareForConnectionChanged();

    void hasConnectionCompleteChanged();

private Q_SLOTS:

protected:

    void parseServiceDescription(QIODevice *serviceDescriptionContent) override;

    void parseEventNotification(const QString &eventName, const QString &eventValue) Q_DECL_OVERRIDE;

private:

    std::unique_ptr<UpnpControlConnectionManagerPrivate> d;

};

#endif // UPNPCONTROLCONNECTIONMANAGER_H
