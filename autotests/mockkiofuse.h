/*
    SPDX-FileCopyrightText: 2026 Ian Monroe <imonroe@kde.org>

    SPDX-License-Identifier: LGPL-3.0-or-later
*/

#pragma once

#include "config-upnp-qt.h"

#if QtDBus_FOUND
#include <QDBusMessage>
#include <QMap>
#include <QObject>
#include <QString>

class MockKioFuse : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.KIOFuse.VFS")

public:
    explicit MockKioFuse(QObject *parent = nullptr);
    ~MockKioFuse() override;

    QMap<QString, QString> mountMappings;

    bool registerService();
    void unregisterService();

public Q_SLOTS:
    QString mountUrl(const QString &remoteUrl, const QDBusMessage &message);
};

class ScopedMockKioFuse
{
public:
    MockKioFuse mock;
    bool isRegistered = false;

    ScopedMockKioFuse();
    ~ScopedMockKioFuse();
};
#endif
