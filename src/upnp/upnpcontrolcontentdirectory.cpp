/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "upnpcontrolcontentdirectory.h"

#include "upnpLogging.h"

#include "upnpcontrolabstractservicereply.h"

#include <KDSoapClient/KDSoapPendingCall.h>
#include <KDSoapClient/KDSoapPendingCallWatcher.h>

#include <QLoggingCategory>

class UpnpControlContentDirectoryPrivate
{
public:

    QString mSearchCapabilities;

    QString mTransferIDs;

    QString mSortCapabilities;

    int mSystemUpdateID;

    bool mHasTransferIDs;

};

UpnpControlContentDirectory::UpnpControlContentDirectory(QObject *parent) : UpnpControlAbstractService(parent), d(new UpnpControlContentDirectoryPrivate)
{
}

UpnpControlContentDirectory::~UpnpControlContentDirectory()
= default;

const QString &UpnpControlContentDirectory::searchCapabilities() const
{
    return d->mSearchCapabilities;
}

const QString &UpnpControlContentDirectory::transferIDs() const
{
    return d->mTransferIDs;
}

bool UpnpControlContentDirectory::hasTransferIDs() const
{
    return d->mHasTransferIDs;
}

const QString &UpnpControlContentDirectory::sortCapabilities() const
{
    return d->mSortCapabilities;
}

int UpnpControlContentDirectory::systemUpdateID() const
{
    return d->mSystemUpdateID;
}

UpnpControlAbstractServiceReply *UpnpControlContentDirectory::getSearchCapabilities()
{
    auto pendingAnswer = callAction(QStringLiteral("GetSearchCapabilities"), {});

    return pendingAnswer;
}

UpnpControlAbstractServiceReply *UpnpControlContentDirectory::getSortCapabilities()
{
    auto pendingAnswer = callAction(QStringLiteral("GetSortCapabilities"), {});

    return pendingAnswer;
}

UpnpControlAbstractServiceReply *UpnpControlContentDirectory::getSystemUpdateID()
{
    auto pendingAnswer = callAction(QStringLiteral("GetSystemUpdateID"), {});

    return pendingAnswer;
}

UpnpControlAbstractServiceReply *UpnpControlContentDirectory::search(const QString &objectID, const QString &searchCriteria,
                                         const QString &filter, int startingIndex,
                                         int requestedCount, const QString &sortCriteria)
{
    qDebug() << "UpnpControlContentDirectory::search" << objectID << searchCriteria << filter << startingIndex << requestedCount << sortCriteria;
    auto pendingAnswer = callAction(QStringLiteral("Search"), {{{}, objectID},
                                                               {{}, searchCriteria},
                                                               {{}, filter},
                                                               {{}, startingIndex},
                                                               {{}, requestedCount},
                                                               {{}, sortCriteria}});

    return pendingAnswer;
}

UpnpControlAbstractServiceReply *UpnpControlContentDirectory::browse(const QString &objectID, const QString &browseFlag,
                                         const QString &filter, int startingIndex,
                                         int requestedCount, const QString &sortCriteria)
{
    auto pendingAnswer = callAction(QStringLiteral("Browse"), {{QStringLiteral("ObjectID"), objectID},
                                                               {QStringLiteral("BrowseFlag"), browseFlag},
                                                               {QStringLiteral("Filter"), filter},
                                                               {QStringLiteral("StartingIndex"), startingIndex},
                                                               {QStringLiteral("RequestedCount"), requestedCount},
                                                               {QStringLiteral("SortCriteria"), sortCriteria}});

    return pendingAnswer;
}

#if 0
void UpnpControlContentDirectory::finishedGetSearchCapabilitiesCall(KDSoapPendingCallWatcher *self)
{
    self->deleteLater();

    auto answer = self->returnMessage();
    auto allValues = answer.childValues();
    QString searchCaps;

    for (KDSoapValue oneValue : allValues) {
        if (oneValue.name() == QLatin1String("SearchCaps")) {
            searchCaps = oneValue.value().toString();
        }
    }

    qCDebug(orgKdeElisaUpnp()) << "SearchCaps:" << searchCaps;

    //Q_EMIT getSearchCapabilitiesFinished(searchCaps, !self->returnMessage().isFault());
}

void UpnpControlContentDirectory::finishedGetSortCapabilitiesCall(KDSoapPendingCallWatcher *self)
{
    self->deleteLater();

    auto answer = self->returnMessage();
    auto allValues = answer.childValues();
    QString sortCaps;

    for (KDSoapValue oneValue : allValues) {
        if (oneValue.name() == QLatin1String("SortCaps")) {
            sortCaps = oneValue.value().toString();
        }
    }

    qCDebug(orgKdeElisaUpnp()) << "SortCaps:" << sortCaps;

    //Q_EMIT getSortCapabilitiesFinished(sortCaps, !self->returnMessage().isFault());
}

void UpnpControlContentDirectory::finishedGetSystemUpdateIDCall(KDSoapPendingCallWatcher *self)
{
    self->deleteLater();

    auto answer = self->returnMessage();
    auto allValues = answer.childValues();

    for (KDSoapValue oneValue : allValues) {
        if (oneValue.name() == QLatin1String("Id")) {
            d->mSystemUpdateID = oneValue.value().toInt();
        }
    }

    qCDebug(orgKdeElisaUpnp()) << "Id:" << d->mSystemUpdateID;

    //Q_EMIT getSystemUpdateIDFinished(d->mSystemUpdateID, !self->returnMessage().isFault());
}

void UpnpControlContentDirectory::finishedSearchCall(KDSoapPendingCallWatcher *self)
{
    qCDebug(orgKdeElisaUpnp()) << "UpnpControlContentDirectory::finishedSearchCall";
    self->deleteLater();

    auto answer = self->returnMessage();
    auto allValues = answer.childValues();
    QString result;
    int numberReturned = 0;
    int totalMatches = 0;

    for (KDSoapValue oneValue : allValues) {
        if (oneValue.name() == QLatin1String("Result")) {
            result = oneValue.value().toString();
        }
        if (oneValue.name() == QLatin1String("NumberReturned")) {
            numberReturned = oneValue.value().toInt();
        }
        if (oneValue.name() == QLatin1String("TotalMatches")) {
            totalMatches = oneValue.value().toInt();
        }
        if (oneValue.name() == QLatin1String("UpdateID")) {
            d->mSystemUpdateID = oneValue.value().toInt();
        }
    }

    //qCDebug(orgKdeElisaUpnp()) << "Result:" << result;
    qCDebug(orgKdeElisaUpnp()) << "NumberReturned:" << numberReturned;
    qCDebug(orgKdeElisaUpnp()) << "TotalMatches:" << totalMatches;
    qCDebug(orgKdeElisaUpnp()) << "UpdateID:" << d->mSystemUpdateID;

    //Q_EMIT searchFinished(result, numberReturned, totalMatches, d->mSystemUpdateID, !self->returnMessage().isFault());
}

void UpnpControlContentDirectory::finishedBrowseCall(KDSoapPendingCallWatcher *self)
{
    self->deleteLater();

    auto answer = self->returnMessage();
    auto allValues = answer.childValues();
    QString result;
    int numberReturned = 0;
    int totalMatches = 0;

    for (KDSoapValue oneValue : allValues) {
        if (oneValue.name() == QLatin1String("Result")) {
            result = oneValue.value().toString();
        }
        if (oneValue.name() == QLatin1String("NumberReturned")) {
            numberReturned = oneValue.value().toInt();
        }
        if (oneValue.name() == QLatin1String("TotalMatches")) {
            totalMatches = oneValue.value().toInt();
        }
        if (oneValue.name() == QLatin1String("UpdateID")) {
            d->mSystemUpdateID = oneValue.value().toInt();
        }
    }

    //qCDebug(orgKdeElisaUpnp()) << "Result:" << result;
    //qCDebug(orgKdeElisaUpnp()) << "NumberReturned:" << numberReturned;
    //qCDebug(orgKdeElisaUpnp()) << "TotalMatches:" << totalMatches;
    //qCDebug(orgKdeElisaUpnp()) << "UpdateID:" << d->mSystemUpdateID;

    //Q_EMIT browseFinished(result, numberReturned, totalMatches, d->mSystemUpdateID, !self->returnMessage().isFault());
}
#endif

void UpnpControlContentDirectory::parseServiceDescription(QIODevice *serviceDescriptionContent)
{
    UpnpControlAbstractService::parseServiceDescription(serviceDescriptionContent);

    const QList<QString> &allVariables(stateVariables());

    d->mHasTransferIDs = allVariables.contains(QStringLiteral("TransferIDs"));
    Q_EMIT hasTransferIDsChanged();

    //const QList<QString> &allActions(actions());
}

void UpnpControlContentDirectory::parseEventNotification(const QString &eventName, const QString &eventValue)
{
    if (eventName == QLatin1String("TransferIDs")) {
        d->mTransferIDs = eventValue;
        Q_EMIT transferIDsChanged(d->mTransferIDs);
    }
    if (eventName == QLatin1String("SystemUpdateID")) {
        d->mSystemUpdateID = eventValue.toInt();
        Q_EMIT systemUpdateIDChanged(d->mSystemUpdateID);
    }
}

#include "moc_upnpcontrolcontentdirectory.cpp"
