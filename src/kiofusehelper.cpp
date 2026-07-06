/*
    SPDX-FileCopyrightText: 2026 Ramil Nurmanov <ramil2004nur@gmail.com>

    SPDX-License-Identifier: LGPL-3.0-or-later
*/

#include "kiofusehelper.h"

#include "config-upnp-qt.h"

#if QtDBus_FOUND
#include <QDBusInterface>
#include <QDBusReply>
#endif

#include <QDebug>

QUrl KioFuseHelper::mostLocalUrl(const QUrl &source)
{
    if (source.isLocalFile() || source.scheme().isEmpty() || source.scheme() == QLatin1String("http") || source.scheme() == QLatin1String("https")) {
        return source;
    }

#if QtDBus_FOUND
    QDBusInterface kioFuse(QStringLiteral("org.kde.KIOFuse"), QStringLiteral("/org/kde/KIOFuse"), QStringLiteral("org.kde.KIOFuse.VFS"));
    if (!kioFuse.isValid()) {
        qDebug() << "KioFuseHelper::mostLocalUrl kio-fuse service unavailable" << kioFuse.lastError().message();
        return source;
    }

    kioFuse.setTimeout(8000);
    const QDBusReply<QString> reply = kioFuse.call(QStringLiteral("mountUrl"), source.toString());
    if (!reply.isValid()) {
        qDebug() << "KioFuseHelper::mostLocalUrl kio-fuse mount failed for" << source << reply.error().message();
        return source;
    }

    return QUrl::fromLocalFile(reply.value());
#else
    return source;
#endif
}
