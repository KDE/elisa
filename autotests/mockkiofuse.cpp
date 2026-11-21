/*
    SPDX-FileCopyrightText: 2026 Ian Monroe <imonroe@kde.org>

    SPDX-License-Identifier: LGPL-3.0-or-later
*/

#include "mockkiofuse.h"

#if QtDBus_FOUND
#include <QDBusConnection>
#include <QMetaObject>
#include <QMutexLocker>

#include <atomic>
#include <utility>

MockKioFuse::MockKioFuse(QString connectionName, QObject *parent)
    : QObject(parent)
    , m_connectionName(std::move(connectionName))
{
}

MockKioFuse::~MockKioFuse() = default;

void MockKioFuse::setMountMapping(const QString &remoteUrl, const QString &localPath)
{
    const QMutexLocker lock(&m_mutex);
    m_mountMappings[remoteUrl] = localPath;
}

bool MockKioFuse::registerService()
{
    {
        auto bus = QDBusConnection::connectToBus(QDBusConnection::SessionBus, m_connectionName);
        if (bus.isConnected() && bus.registerService(QStringLiteral("org.kde.KIOFuse"))) {
            if (bus.registerObject(QStringLiteral("/org/kde/KIOFuse"), QStringLiteral("org.kde.KIOFuse.VFS"), this, QDBusConnection::ExportAllSlots)) {
                return true;
            }
            bus.unregisterService(QStringLiteral("org.kde.KIOFuse"));
        }
    }
    QDBusConnection::disconnectFromBus(m_connectionName);
    return false;
}

void MockKioFuse::unregisterService()
{
    {
        auto bus = QDBusConnection(m_connectionName);
        if (bus.isConnected()) {
            bus.unregisterObject(QStringLiteral("/org/kde/KIOFuse"));
            bus.unregisterService(QStringLiteral("org.kde.KIOFuse"));
        }
    }
    QDBusConnection::disconnectFromBus(m_connectionName);
}

QString MockKioFuse::mountUrl(const QString &remoteUrl, const QDBusMessage &message)
{
    {
        const QMutexLocker lock(&m_mutex);
        if (m_mountMappings.contains(remoteUrl)) {
            return m_mountMappings.value(remoteUrl);
        }
    }
    message.setDelayedReply(true);
    auto errorReply = message.createErrorReply(QStringLiteral("org.kde.KIOFuse.VFS.Error.CannotMount"), QStringLiteral("Mount failed"));
    QDBusConnection(m_connectionName).send(errorReply);
    return {};
}

ScopedMockKioFuse::ScopedMockKioFuse()
{
    static std::atomic<int> connectionCounter{0};
    const QString connectionName = QStringLiteral("mock-kiofuse-%1").arg(++connectionCounter);

    m_mock = new MockKioFuse(connectionName);
    m_mock->moveToThread(&m_thread);
    m_thread.start();

    QMetaObject::invokeMethod(
        m_mock,
        [this]() {
            isRegistered = m_mock->registerService();
        },
        Qt::BlockingQueuedConnection);
}

ScopedMockKioFuse::~ScopedMockKioFuse()
{
    if (m_mock) {
        QMetaObject::invokeMethod(
            m_mock,
            [this]() {
                if (isRegistered) {
                    m_mock->unregisterService();
                }
                delete m_mock;
                m_mock = nullptr;
            },
            Qt::BlockingQueuedConnection);
    }
    m_thread.quit();
    m_thread.wait();
}

void ScopedMockKioFuse::setMountMapping(const QString &remoteUrl, const QString &localPath)
{
    if (m_mock) {
        m_mock->setMountMapping(remoteUrl, localPath);
    }
}

#endif
