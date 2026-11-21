/*
    SPDX-FileCopyrightText: 2026 Ian Monroe <imonroe@kde.org>

    SPDX-License-Identifier: LGPL-3.0-or-later
*/

#pragma once

#include "config-upnp-qt.h"

#if QtDBus_FOUND
#include <QDBusMessage>
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QString>
#include <QThread>

class MockKioFuse : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.KIOFuse.VFS")

public:
    explicit MockKioFuse(QString connectionName, QObject *parent = nullptr);
    ~MockKioFuse() override;

    void setMountMapping(const QString &remoteUrl, const QString &localPath);

    bool registerService();
    void unregisterService();

public Q_SLOTS:
    QString mountUrl(const QString &remoteUrl, const QDBusMessage &message);

private:
    QString m_connectionName;
    QMutex m_mutex;
    QMap<QString, QString> m_mountMappings;
};

class ScopedMockKioFuse
{
public:
    bool isRegistered = false;

    ScopedMockKioFuse();
    ~ScopedMockKioFuse();

    Q_DISABLE_COPY_MOVE(ScopedMockKioFuse)

    void setMountMapping(const QString &remoteUrl, const QString &localPath);

private:
    QThread m_thread;
    MockKioFuse *m_mock = nullptr;
};
#endif
