/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef NOTIFICATIONITEM_H
#define NOTIFICATIONITEM_H

#include <QString>

#include <memory>

class NotificationItemPrivate;
class QObject;

class NotificationItem
{

public:

    NotificationItem();

    NotificationItem(const NotificationItem &other);

    NotificationItem(NotificationItem &&other);

    NotificationItem& operator=(const NotificationItem &other);

    NotificationItem& operator=(NotificationItem &&other);

    ~NotificationItem();

    const QString& notificationId() const;

    const QString& message() const;

    const QString& mainButtonText() const;

    const QString& mainButtonIconName() const;

    const QString& secondaryButtonText() const;

    const QString& secondaryButtonIconName() const;

    void setNotificationId(QString notificationId);

    void setMessage(QString message);

    void setMainButtonText(QString mainButtonText);

    void setMainButtonIconName(QString mainButtonIconName);

    void setSecondaryButtonText(QString secondaryButtonText);

    void setSecondaryButtonIconName(QString secondaryButtonIconName);

    void setTargetObject(QObject *target);

    void setMainButtonMethodName(QString methodName);

    void setSecondaryButtonMethodName(QString methodName);

    void triggerMainButton() const;

    void triggerSecondaryButton() const;

private:

    std::unique_ptr<NotificationItemPrivate> d;

};

#endif // NOTIFICATIONITEM_H
