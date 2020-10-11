/*
   SPDX-FileCopyrightText: 2019 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
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

    ~PowerManagementInterface() override;

    [[nodiscard]] bool preventSleep() const;

    [[nodiscard]] bool sleepInhibited() const;

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
