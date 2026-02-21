/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef UPNPCONTROLCONNECTIONMANAGER_H
#define UPNPCONTROLCONNECTIONMANAGER_H

#include "upnpcontrolabstractservice.h"
#include "upnpbasictypes.h"

#include <QQmlEngine>

#include <memory>

class UpnpControlConnectionManagerPrivate;

class UpnpControlConnectionManager : public UpnpControlAbstractService
{
    Q_OBJECT

    QML_ELEMENT

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

    [[nodiscard]] const QString& sourceProtocolInfo() const;

    [[nodiscard]] const QString& sinkProtocolInfo() const;

    [[nodiscard]] const QString& currentConnectionIDs() const;

    [[nodiscard]] bool hasPrepareForConnection() const;

    [[nodiscard]] bool hasConnectionComplete() const;

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

    void parseEventNotification(const QString &eventName, const QString &eventValue) override;

private:

    std::unique_ptr<UpnpControlConnectionManagerPrivate> d;

};

#endif // UPNPCONTROLCONNECTIONMANAGER_H
