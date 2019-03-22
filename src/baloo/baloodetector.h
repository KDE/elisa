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

#ifndef BALOODETECTOR_H
#define BALOODETECTOR_H

#include <QObject>
#include <QDBusServiceWatcher>

class BalooDetector : public QObject
{

    Q_OBJECT

    Q_PROPERTY(bool balooAvailability
               READ balooAvailability
               NOTIFY balooAvailabilityChanged)

public:

    explicit BalooDetector(QObject *parent = nullptr);

    bool balooAvailability() const;

Q_SIGNALS:

    void balooAvailabilityChanged();

public Q_SLOTS:

    void checkBalooAvailability();

private Q_SLOTS:

    void serviceRegistered(const QString &service);

    void serviceUnregistered(const QString &service);

private:

    bool checkBalooConfiguration();

    bool checkBalooServiceIsRunning();

    void signalBalooAvailability(bool isAvailable);

    QDBusServiceWatcher mBalooWatcher;

    bool mBalooAvailability = false;

};

#endif // BALOODETECTOR_H
