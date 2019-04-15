/*
 * Copyright 2019 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef POWERMANAGEMENTINTERFACE_H
#define POWERMANAGEMENTINTERFACE_H

#include <QObject>

#include <memory>

class QDBusPendingCallWatcher;
class PowerManagementInterfacePrivate;

class PowerManagementInterface : public QObject
{

    Q_OBJECT

    Q_PROPERTY(bool preventSleep
               READ preventSleep
               WRITE setPreventSleep
               NOTIFY preventSleepChanged)

    Q_PROPERTY(bool sleepInhibited
               READ sleepInhibited
               NOTIFY sleepInhibitedChanged)

public:

    explicit PowerManagementInterface(QObject *parent = nullptr);

    ~PowerManagementInterface();

    bool preventSleep() const;

    bool sleepInhibited() const;

Q_SIGNALS:

    void preventSleepChanged();

    void sleepInhibitedChanged();

public Q_SLOTS:

    void setPreventSleep(bool value);

    void retryInhibitingSleep();

private Q_SLOTS:

    void hostSleepInhibitChanged();

    void inhibitDBusCallFinishedPlasmaWorkspace(QDBusPendingCallWatcher *aWatcher);

    void uninhibitDBusCallFinishedPlasmaWorkspace(QDBusPendingCallWatcher *aWatcher);

    void inhibitDBusCallFinishedGnomeWorkspace(QDBusPendingCallWatcher *aWatcher);

    void uninhibitDBusCallFinishedGnomeWorkspace(QDBusPendingCallWatcher *aWatcher);

private:

    void inhibitSleepPlasmaWorkspace();

    void uninhibitSleepPlasmaWorkspace();

    void inhibitSleepGnomeWorkspace();

    void uninhibitSleepGnomeWorkspace();

    void inhibitSleepWindowsWorkspace();

    void uninhibitSleepWindowsWorkspace();

    std::unique_ptr<PowerManagementInterfacePrivate> d;

};

#endif // POWERMANAGEMENTINTERFACE_H
