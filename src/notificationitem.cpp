/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "notificationitem.h"

#include <QObject>

class NotificationItemPrivate {
public:

    QString mNotificationId;

    QString mMessage;

    QString mMainButtonText;

    QString mMainButtonIconName;

    QString mSecondaryButtonText;

    QString mSecondaryButtonIconName;

    QObject *mTarget = nullptr;

    QString mMainButtonMethodName;

    QString mSecondaryButtonMethodName;

};

NotificationItem::NotificationItem() : d(std::make_unique<NotificationItemPrivate>())
{
}

NotificationItem::NotificationItem(const NotificationItem &other) : d(std::make_unique<NotificationItemPrivate>(*other.d))
{
}

NotificationItem::NotificationItem(NotificationItem &&other)
{
    other.d.swap(d);
}

NotificationItem& NotificationItem::operator=(const NotificationItem &other)
{
    if (&other != this) {
        *d = *(other.d);
    }

    return *this;
}

NotificationItem::~NotificationItem()
= default;

const QString& NotificationItem::notificationId() const
{
    return d->mNotificationId;
}

const QString& NotificationItem::message() const
{
    return d->mMessage;
}

const QString& NotificationItem::mainButtonText() const
{
    return d->mMainButtonText;
}

const QString& NotificationItem::secondaryButtonText() const
{
    return d->mSecondaryButtonText;
}

const QString& NotificationItem::secondaryButtonIconName() const
{
    return d->mSecondaryButtonIconName;
}

void NotificationItem::setNotificationId(QString notificationId)
{
    if (d->mNotificationId == notificationId) {
        return;
    }

    d->mNotificationId = std::move(notificationId);
}

const QString& NotificationItem::mainButtonIconName() const
{
    return d->mMainButtonIconName;
}

void NotificationItem::setMessage(QString message)
{
    if (d->mMessage == message) {
        return;
    }

    d->mMessage = std::move(message);
}

void NotificationItem::setMainButtonText(QString mainButtonText)
{
    if (d->mMainButtonText == mainButtonText) {
        return;
    }

    d->mMainButtonText = std::move(mainButtonText);
}

void NotificationItem::setSecondaryButtonText(QString secondaryButtonText)
{
    if (d->mSecondaryButtonText == secondaryButtonText) {
        return;
    }

    d->mSecondaryButtonText = std::move(secondaryButtonText);
}

void NotificationItem::setSecondaryButtonIconName(QString secondaryButtonIconName)
{
    if (d->mSecondaryButtonIconName == secondaryButtonIconName) {
        return;
    }

    d->mSecondaryButtonIconName = std::move(secondaryButtonIconName);
}

void NotificationItem::setTargetObject(QObject *target)
{
    d->mTarget = target;
}

void NotificationItem::setMainButtonMethodName(QString methodName)
{
    d->mMainButtonMethodName = std::move(methodName);
}

void NotificationItem::setSecondaryButtonMethodName(QString methodName)
{
    d->mSecondaryButtonMethodName = std::move(methodName);
}

void NotificationItem::triggerMainButton() const
{
    if (!d->mTarget) {
        return;
    }

    QMetaObject::invokeMethod(d->mTarget, d->mMainButtonMethodName.toLatin1().data(), Qt::QueuedConnection);
}

void NotificationItem::triggerSecondaryButton() const
{
    if (!d->mTarget) {
        return;
    }

    QMetaObject::invokeMethod(d->mTarget, d->mSecondaryButtonMethodName.toLatin1().data(), Qt::QueuedConnection);
}

void NotificationItem::setMainButtonIconName(QString mainButtonIconName)
{
    if (d->mMainButtonIconName == mainButtonIconName) {
        return;
    }

    d->mMainButtonIconName = std::move(mainButtonIconName);
}
