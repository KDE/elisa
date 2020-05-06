/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "baloodetector.h"

#include "baloo/baloocommon.h"

#include <Baloo/IndexerConfig>

#include <QDBusConnection>
#include <QDBusConnectionInterface>

BalooDetector::BalooDetector(QObject *parent) : QObject(parent)
{
    connect(&mBalooWatcher, &QDBusServiceWatcher::serviceRegistered,
            this, &BalooDetector::serviceRegistered);
    connect(&mBalooWatcher, &QDBusServiceWatcher::serviceUnregistered,
            this, &BalooDetector::serviceUnregistered);
}

bool BalooDetector::balooAvailability() const
{
    return mBalooAvailability;
}

void BalooDetector::checkBalooAvailability()
{
    bool problemDetected = false;

    problemDetected = problemDetected || checkBalooConfiguration();

    mBalooWatcher.setWatchedServices({QStringLiteral("org.kde.baloo")});

    problemDetected = problemDetected || checkBalooServiceIsRunning();

    signalBalooAvailability(problemDetected);
}

void BalooDetector::serviceRegistered(const QString &service)
{
    Q_UNUSED(service);

    bool problemDetected = false;

    problemDetected = problemDetected || checkBalooConfiguration();
    problemDetected = problemDetected || checkBalooServiceIsRunning();

    signalBalooAvailability(problemDetected);
}

void BalooDetector::serviceUnregistered(const QString &service)
{
    Q_UNUSED(service);

    if (mBalooAvailability) {
        qCInfo(orgKdeElisaBaloo) << "Baloo service is no longer detected and seems not be running. Disabling Elisa support for using Baloo indexer.";

        mBalooAvailability = false;
        Q_EMIT balooAvailabilityChanged();
    }
}

bool BalooDetector::checkBalooConfiguration()
{
    Baloo::IndexerConfig balooConfiguration;

    auto problemDetected = !balooConfiguration.fileIndexingEnabled();

    if (problemDetected) {
        qCInfo(orgKdeElisaBaloo) << "Baloo indexer has been configured to be deactivated. Disabling Elisa support for using Baloo indexer.";
    }

    return problemDetected;
}

bool BalooDetector::checkBalooServiceIsRunning()
{
    auto problemDetected = !QDBusConnection::sessionBus().interface()->isServiceRegistered(QStringLiteral("org.kde.baloo"));

    if (problemDetected) {
        qCInfo(orgKdeElisaBaloo) << "Baloo service is not detected and seems not be running. Disabling Elisa support for using Baloo indexer.";
    }

    return problemDetected;
}

void BalooDetector::signalBalooAvailability(bool isNotAvailable)
{
    if (!isNotAvailable) {
        if (!mBalooAvailability || mBalooAvailabilityFirstChange) {
            mBalooAvailability = true;
            Q_EMIT balooAvailabilityChanged();
        }
    } else {
        if (mBalooAvailability || mBalooAvailabilityFirstChange) {
            mBalooAvailability = false;
            Q_EMIT balooAvailabilityChanged();
        }
    }

    mBalooAvailabilityFirstChange = false;
}


#include "moc_baloodetector.cpp"
