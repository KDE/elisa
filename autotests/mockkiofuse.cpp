/*
    SPDX-FileCopyrightText: 2026 Ian Monroe <imonroe@kde.org>

    SPDX-License-Identifier: LGPL-3.0-or-later
*/

#include "mockkiofuse.h"

#if QtDBus_FOUND
#include <QDBusConnection>

MockKioFuse::MockKioFuse(QObject *parent)
    : QObject(parent)
{
}

MockKioFuse::~MockKioFuse() = default;

bool MockKioFuse::registerService()
{
    if (!QDBusConnection::sessionBus().isConnected()) {
        return false;
    }
    if (!QDBusConnection::sessionBus().registerService(QStringLiteral("org.kde.KIOFuse"))) {
        return false;
    }
    return QDBusConnection::sessionBus().registerObject(QStringLiteral("/org/kde/KIOFuse"),
                                                        QStringLiteral("org.kde.KIOFuse.VFS"),
                                                        this,
                                                        QDBusConnection::ExportAllSlots);
}

void MockKioFuse::unregisterService()
{
    if (QDBusConnection::sessionBus().isConnected()) {
        QDBusConnection::sessionBus().unregisterObject(QStringLiteral("/org/kde/KIOFuse"));
        QDBusConnection::sessionBus().unregisterService(QStringLiteral("org.kde.KIOFuse"));
    }
}

QString MockKioFuse::mountUrl(const QString &remoteUrl, const QDBusMessage &message)
{
    if (mountMappings.contains(remoteUrl)) {
        return mountMappings.value(remoteUrl);
    }
    message.setDelayedReply(true);
    auto errorReply = message.createErrorReply(QStringLiteral("org.kde.KIOFuse.VFS.Error.CannotMount"), QStringLiteral("Mount failed"));
    QDBusConnection::sessionBus().send(errorReply);
    return {};
}

ScopedMockKioFuse::ScopedMockKioFuse()
{
    isRegistered = mock.registerService();
}

ScopedMockKioFuse::~ScopedMockKioFuse()
{
    if (isRegistered) {
        mock.unregisterService();
    }
}

#endif
