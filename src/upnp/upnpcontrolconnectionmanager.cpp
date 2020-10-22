/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "upnpcontrolconnectionmanager.h"

#include "upnpLogging.h"

#include <KDSoapClient/KDSoapPendingCall.h>
#include <KDSoapClient/KDSoapPendingCallWatcher.h>

#include <QLoggingCategory>

class UpnpControlConnectionManagerPrivate
{
public:

    QString mSourceProtocolInfo;

    QString mSinkProtocolInfo;

    QString mCurrentConnectionIDs;

    bool mHasPrepareForConnection;

    bool mHasConnectionComplete;

};

UpnpControlConnectionManager::UpnpControlConnectionManager(QObject *parent) : UpnpControlAbstractService(parent), d(new UpnpControlConnectionManagerPrivate)
{
}

UpnpControlConnectionManager::~UpnpControlConnectionManager()
= default;

const QString &UpnpControlConnectionManager::sourceProtocolInfo() const
{
    return d->mSourceProtocolInfo;
}

const QString &UpnpControlConnectionManager::sinkProtocolInfo() const
{
    return d->mSinkProtocolInfo;
}

const QString &UpnpControlConnectionManager::currentConnectionIDs() const
{
    return d->mCurrentConnectionIDs;
}

bool UpnpControlConnectionManager::hasPrepareForConnection() const
{
    return d->mHasPrepareForConnection;
}

bool UpnpControlConnectionManager::hasConnectionComplete() const
{
    return d->mHasConnectionComplete;
}

UpnpControlAbstractServiceReply *UpnpControlConnectionManager::getProtocolInfo()
{
    auto pendingAnswer = callAction(QStringLiteral("GetProtocolInfo"), {});

    return pendingAnswer;
}

UpnpControlAbstractServiceReply *UpnpControlConnectionManager::prepareForConnection(const QString &remoteProtocolInfo,
                                                                                    const QString &remoteUDN,
                                                                                    const QString &remoteServiceID,
                                                                                    int remotePeerConnectionID,
                                                                                    const QString &connectionDirection)
{
    Q_UNUSED(remoteProtocolInfo)

    const QString &remoteConnectionManager(remoteUDN + QStringLiteral("/") + remoteServiceID);

    auto pendingAnswer = callAction(QStringLiteral("PrepareForConnection"),
        {{{}, remoteConnectionManager}, {{}, remotePeerConnectionID}, {{}, connectionDirection}});

    return pendingAnswer;
}

UpnpControlAbstractServiceReply *UpnpControlConnectionManager::connectionComplete(int currentConnectionID)
{
    auto pendingAnswer = callAction(QStringLiteral("ConnectionComplete"), {{{}, currentConnectionID}});

    return pendingAnswer;
}

UpnpControlAbstractServiceReply *UpnpControlConnectionManager::getCurrentConnectionIDs()
{
    auto pendingAnswer = callAction(QStringLiteral("GetCurrentConnectionIDs"), {});

    return pendingAnswer;
}

UpnpControlAbstractServiceReply *UpnpControlConnectionManager::getCurrentConnectionInfo(int currentConnectionID)
{
    auto pendingAnswer = callAction(QStringLiteral("GetCurrentConnectionInfo"), {{{}, currentConnectionID}});

    return pendingAnswer;
}

#if 0
void UpnpControlConnectionManager::finishedGetProtocolInfoCall(KDSoapPendingCallWatcher *self)
{
    self->deleteLater();

    auto answer = self->returnMessage();
    auto allValues = answer.childValues();
    for (KDSoapValue oneValue : allValues) {
        if (oneValue.name() == QLatin1String("Source")) {
            d->mSourceProtocolInfo = oneValue.value().toString();
        }
        if (oneValue.name() == QLatin1String("Sink")) {
            d->mSinkProtocolInfo = oneValue.value().toString();
        }
    }

    qCDebug(orgKdeElisaUpnp()) << "SourceProtocolInfo:" << d->mSourceProtocolInfo;
    qCDebug(orgKdeElisaUpnp()) << "SinkProtocolInfo:" << d->mSinkProtocolInfo;

    Q_EMIT getProtocolInfoFinished(!self->returnMessage().isFault());
}

void UpnpControlConnectionManager::finishedPrepareForConnectionCall(KDSoapPendingCallWatcher *self)
{
    self->deleteLater();

    qCDebug(orgKdeElisaUpnp()) << self->returnValue();

    Q_EMIT prepareForConnectionFinished(!self->returnMessage().isFault());
}

void UpnpControlConnectionManager::finishedConnectionCompleteCall(KDSoapPendingCallWatcher *self)
{
    self->deleteLater();

    qCDebug(orgKdeElisaUpnp()) << self->returnValue();

    Q_EMIT connectionCompleteFinished(!self->returnMessage().isFault());
}

void UpnpControlConnectionManager::finishedGetCurrentConnectionIDsCall(KDSoapPendingCallWatcher *self)
{
    self->deleteLater();

    qCDebug(orgKdeElisaUpnp()) << self->returnValue();
    d->mCurrentConnectionIDs = self->returnValue().toString();
    Q_EMIT currentConnectionIDsChanged(d->mCurrentConnectionIDs);

    Q_EMIT getCurrentConnectionIDsFinished(!self->returnMessage().isFault());
}

void UpnpControlConnectionManager::finishedGetCurrentConnectionInfoCall(KDSoapPendingCallWatcher *self)
{
    self->deleteLater();

    auto answer = self->returnMessage();
    auto allValues = answer.childValues();
    int rcsID = -1;
    int avTransportID = -1;
    QString protocolInfo;
    QString connectionManager;
    int peerConnectionID = -1;
    QString direction;
    QString connectionStatus;

    for (KDSoapValue oneValue : allValues) {
        if (oneValue.name() == QLatin1String("RcsID")) {
            rcsID = oneValue.value().toInt();
        }
        if (oneValue.name() == QLatin1String("AVTransportID")) {
            avTransportID = oneValue.value().toInt();
        }
        if (oneValue.name() == QLatin1String("protocolInfo")) {
            protocolInfo = oneValue.value().toInt();
        }
        if (oneValue.name() == QLatin1String("PeerConnectionManager")) {
            connectionManager = oneValue.value().toInt();
        }
        if (oneValue.name() == QLatin1String("PeerConnectionID")) {
            peerConnectionID = oneValue.value().toInt();
        }
        if (oneValue.name() == QLatin1String("Direction")) {
            direction = oneValue.value().toInt();
        }
        if (oneValue.name() == QLatin1String("Status")) {
            connectionStatus = oneValue.value().toInt();
        }
    }

    qCDebug(orgKdeElisaUpnp()) << "RcsID:" << rcsID;
    qCDebug(orgKdeElisaUpnp()) << "AVTransportID:" << avTransportID;
    qCDebug(orgKdeElisaUpnp()) << "protocolInfo:" << protocolInfo;
    qCDebug(orgKdeElisaUpnp()) << "PeerConnectionManager:" << connectionManager;
    qCDebug(orgKdeElisaUpnp()) << "PeerConnectionID:" << peerConnectionID;
    qCDebug(orgKdeElisaUpnp()) << "Direction:" << direction;
    qCDebug(orgKdeElisaUpnp()) << "Status:" << connectionStatus;

    Q_EMIT getCurrentConnectionInfoFinished(rcsID, avTransportID, protocolInfo, connectionManager, peerConnectionID,
                                            direction, connectionStatus, !self->returnMessage().isFault());
}
#endif

void UpnpControlConnectionManager::parseServiceDescription(QIODevice *serviceDescriptionContent)
{
    UpnpControlAbstractService::parseServiceDescription(serviceDescriptionContent);

    const QList<QString> &allActions(actions());

    d->mHasPrepareForConnection = allActions.contains(QStringLiteral("PrepareForConnection"));
    Q_EMIT hasPrepareForConnectionChanged();

    d->mHasConnectionComplete = allActions.contains(QStringLiteral("ConnectionComplete"));
    Q_EMIT hasConnectionCompleteChanged();
}

void UpnpControlConnectionManager::parseEventNotification(const QString &eventName, const QString &eventValue)
{
    if (eventName == QLatin1String("SourceProtocolInfo")) {
        d->mSourceProtocolInfo = eventValue;
        Q_EMIT sourceProtocolInfoChanged(d->mSourceProtocolInfo);
    }
    if (eventName == QLatin1String("SinkProtocolInfo")) {
        d->mSinkProtocolInfo = eventValue;
        Q_EMIT sinkProtocolInfoChanged(d->mSinkProtocolInfo);
    }
    if (eventName == QLatin1String("CurrentConnectionIDs")) {
        d->mCurrentConnectionIDs = eventValue;
        Q_EMIT currentConnectionIDsChanged(d->mCurrentConnectionIDs);
    }
}

#include "moc_upnpcontrolconnectionmanager.cpp"
