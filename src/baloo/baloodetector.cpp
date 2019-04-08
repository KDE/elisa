/*
 * Copyright 2016-2019 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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

void BalooDetector::signalBalooAvailability(bool isAvailable)
{
    if (!isAvailable) {
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
