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

#include "powermanagementinterface.h"

#include "config-upnp-qt.h"

#include <KLocalizedString>

#if defined Qt5DBus_FOUND && Qt5DBus_FOUND
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCall>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QDBusUnixFileDescriptor>
#endif

#include <QString>
#include <QByteArray>

#include <QCoreApplication>

#include <QDebug>

class PowerManagementInterfacePrivate
{
public:

    bool mPreventSleep = false;

    bool mInhibitedSleep = false;

    uint mInhibitSleepCookie = 0;

    QDBusUnixFileDescriptor mInhibitSleepFileDescriptor;

};

PowerManagementInterface::PowerManagementInterface(QObject *parent) : QObject(parent), d(std::make_unique<PowerManagementInterfacePrivate>())
{
#if defined Qt5DBus_FOUND && Qt5DBus_FOUND
    auto sessionBus = QDBusConnection::sessionBus();

    sessionBus.connect(QStringLiteral("org.freedesktop.PowerManagement.Inhibit"),
                       QStringLiteral("/org/freedesktop/PowerManagement/Inhibit"),
                       QStringLiteral("org.freedesktop.PowerManagement.Inhibit"),
                       QStringLiteral("HasInhibitChanged"), this, SLOT(hostSleepInhibitChanged()));
#endif
}

PowerManagementInterface::~PowerManagementInterface() = default;

bool PowerManagementInterface::preventSleep() const
{
    return d->mPreventSleep;
}

bool PowerManagementInterface::sleepInhibited() const
{
    return d->mInhibitedSleep;
}

void PowerManagementInterface::setPreventSleep(bool value)
{
    if (d->mPreventSleep == value) {
        return;
    }

    if (value) {
        inhibitSleepPlasmaWorkspace();
        inhibitSleepGnomeWorkspace();
        d->mPreventSleep = true;
    } else {
        uninhibitSleepPlasmaWorkspace();
        uninhibitSleepGnomeWorkspace();
        d->mPreventSleep = false;
    }

    Q_EMIT preventSleepChanged();
}

void PowerManagementInterface::retryInhibitingSleep()
{
    if (d->mPreventSleep && !d->mInhibitedSleep) {
        inhibitSleepPlasmaWorkspace();
        inhibitSleepGnomeWorkspace();
    }
}

void PowerManagementInterface::hostSleepInhibitChanged()
{
}

void PowerManagementInterface::inhibitDBusCallFinishedPlasmaWorkspace(QDBusPendingCallWatcher *aWatcher)
{
#if defined Qt5DBus_FOUND && Qt5DBus_FOUND
    QDBusPendingReply<uint> reply = *aWatcher;
    if (reply.isError()) {
    } else {
        d->mInhibitSleepCookie = reply.argumentAt<0>();
        d->mInhibitedSleep = true;

        Q_EMIT sleepInhibitedChanged();
    }
    aWatcher->deleteLater();
#endif
}

void PowerManagementInterface::uninhibitDBusCallFinishedPlasmaWorkspace(QDBusPendingCallWatcher *aWatcher)
{
#if defined Qt5DBus_FOUND && Qt5DBus_FOUND
    QDBusPendingReply<> reply = *aWatcher;
    if (reply.isError()) {
        qDebug() << "PowerManagementInterface::uninhibitDBusCallFinished" << reply.error();
    } else {
        d->mInhibitedSleep = false;

        Q_EMIT sleepInhibitedChanged();
    }
    aWatcher->deleteLater();
#endif
}

void PowerManagementInterface::inhibitDBusCallFinishedGnomeWorkspace(QDBusPendingCallWatcher *aWatcher)
{
#if defined Qt5DBus_FOUND && Qt5DBus_FOUND
    QDBusPendingReply<uint> reply = *aWatcher;
    if (reply.isError()) {
        qDebug() << "PowerManagementInterface::inhibitDBusCallFinishedGnomeWorkspace" << reply.error();
    } else {
        d->mInhibitSleepCookie = reply.argumentAt<0>();
        d->mInhibitedSleep = true;

        Q_EMIT sleepInhibitedChanged();
    }
    aWatcher->deleteLater();
#endif
}

void PowerManagementInterface::uninhibitDBusCallFinishedGnomeWorkspace(QDBusPendingCallWatcher *aWatcher)
{
#if defined Qt5DBus_FOUND && Qt5DBus_FOUND
    QDBusPendingReply<> reply = *aWatcher;
    if (reply.isError()) {
        qDebug() << "PowerManagementInterface::uninhibitDBusCallFinished" << reply.error();
    } else {
        d->mInhibitedSleep = false;

        Q_EMIT sleepInhibitedChanged();
    }
    aWatcher->deleteLater();
#endif
}

void PowerManagementInterface::inhibitSleepPlasmaWorkspace()
{
#if defined Qt5DBus_FOUND && Qt5DBus_FOUND
    auto sessionBus = QDBusConnection::sessionBus();

    auto inhibitCall = QDBusMessage::createMethodCall(QStringLiteral("org.freedesktop.PowerManagement.Inhibit"),
                                                      QStringLiteral("/org/freedesktop/PowerManagement/Inhibit"),
                                                      QStringLiteral("org.freedesktop.PowerManagement.Inhibit"),
                                                      QStringLiteral("Inhibit"));

    inhibitCall.setArguments({{QCoreApplication::applicationName()}, {i18nc("explanation for sleep inhibit during play of music", "Playing Music")}});

    auto asyncReply = sessionBus.asyncCall(inhibitCall);

    auto replyWatcher = new QDBusPendingCallWatcher(asyncReply, this);

    QObject::connect(replyWatcher, &QDBusPendingCallWatcher::finished,
                     this, &PowerManagementInterface::inhibitDBusCallFinishedPlasmaWorkspace);
#endif
}

void PowerManagementInterface::uninhibitSleepPlasmaWorkspace()
{
#if defined Qt5DBus_FOUND && Qt5DBus_FOUND
    auto sessionBus = QDBusConnection::sessionBus();

    auto uninhibitCall = QDBusMessage::createMethodCall(QStringLiteral("org.freedesktop.PowerManagement.Inhibit"),
                                                      QStringLiteral("/org/freedesktop/PowerManagement/Inhibit"),
                                                      QStringLiteral("org.freedesktop.PowerManagement.Inhibit"),
                                                      QStringLiteral("UnInhibit"));

    uninhibitCall.setArguments({{d->mInhibitSleepCookie}});

    auto asyncReply = sessionBus.asyncCall(uninhibitCall);

    auto replyWatcher = new QDBusPendingCallWatcher(asyncReply, this);

    QObject::connect(replyWatcher, &QDBusPendingCallWatcher::finished,
                     this, &PowerManagementInterface::uninhibitDBusCallFinishedPlasmaWorkspace);
#endif
}

void PowerManagementInterface::inhibitSleepGnomeWorkspace()
{
#if defined Qt5DBus_FOUND && Qt5DBus_FOUND
    auto sessionBus = QDBusConnection::sessionBus();

    auto inhibitCall = QDBusMessage::createMethodCall(QStringLiteral("org.gnome.SessionManager"),
                                                      QStringLiteral("/org/gnome/SessionManager"),
                                                      QStringLiteral("org.gnome.SessionManager"),
                                                      QStringLiteral("Inhibit"));

    inhibitCall.setArguments({{QCoreApplication::applicationName()}, {uint(0)},
                              {i18nc("explanation for sleep inhibit during play of music", "Playing Music")}, {uint(8)}});

    auto asyncReply = sessionBus.asyncCall(inhibitCall);

    auto replyWatcher = new QDBusPendingCallWatcher(asyncReply, this);

    QObject::connect(replyWatcher, &QDBusPendingCallWatcher::finished,
                     this, &PowerManagementInterface::inhibitDBusCallFinishedGnomeWorkspace);
#endif
}

void PowerManagementInterface::uninhibitSleepGnomeWorkspace()
{
#if defined Qt5DBus_FOUND && Qt5DBus_FOUND
    auto sessionBus = QDBusConnection::sessionBus();

    auto uninhibitCall = QDBusMessage::createMethodCall(QStringLiteral("org.gnome.SessionManager"),
                                                        QStringLiteral("/org/gnome/SessionManager"),
                                                        QStringLiteral("org.gnome.SessionManager"),
                                                        QStringLiteral("UnInhibit"));

    uninhibitCall.setArguments({{d->mInhibitSleepCookie}});

    auto asyncReply = sessionBus.asyncCall(uninhibitCall);

    auto replyWatcher = new QDBusPendingCallWatcher(asyncReply, this);

    QObject::connect(replyWatcher, &QDBusPendingCallWatcher::finished,
                     this, &PowerManagementInterface::uninhibitDBusCallFinishedGnomeWorkspace);
#endif
}

void PowerManagementInterface::inhibitSleepWindowsWorkspace()
{
#if defined Q_OS_WIN
    SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED);
#endif
}

void PowerManagementInterface::uninhibitSleepWindowsWorkspace()
{
#if defined Q_OS_WIN
    SetThreadExecutionState(ES_CONTINUOUS);
#endif
}


#include "moc_powermanagementinterface.cpp"
