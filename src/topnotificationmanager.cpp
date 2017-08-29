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

#include "topnotificationmanager.h"
#include "notificationitem.h"

class TopNotificationManagerPrivate
{
public:

    QList<NotificationItem> mNotifications;

};

TopNotificationManager::TopNotificationManager(QObject *parent) : QAbstractListModel(parent), d(std::make_unique<TopNotificationManagerPrivate>())
{
}

TopNotificationManager::~TopNotificationManager()
=default;

int TopNotificationManager::countNotifications() const
{
    return d->mNotifications.count();
}

QModelIndex TopNotificationManager::parent(const QModelIndex &index) const
{
    Q_UNUSED(index);

    return {};
}

void TopNotificationManager::addNotification(NotificationItem notification)
{
    beginInsertRows({}, d->mNotifications.count(), d->mNotifications.count());
    d->mNotifications.push_back(notification);
    endInsertRows();

    Q_EMIT countNotificationsChanged();
    Q_EMIT notificationMessageChanged();
    Q_EMIT notificationMainButtonTextChanged();
    Q_EMIT notificationMainButtonIconNameChanged();
    Q_EMIT notificationSecondaryButtonTextChanged();
    Q_EMIT notificationSecondaryButtonIconNameChanged();
}

void TopNotificationManager::closeNotification(int index)
{
    if (index < 0 || index >= d->mNotifications.count()) {
        return;
    }

    beginRemoveRows({}, index, index);
    d->mNotifications.removeAt(index);
    endRemoveRows();

    Q_EMIT countNotificationsChanged();
    Q_EMIT notificationMessageChanged();
    Q_EMIT notificationMainButtonTextChanged();
    Q_EMIT notificationMainButtonIconNameChanged();
    Q_EMIT notificationSecondaryButtonTextChanged();
    Q_EMIT notificationSecondaryButtonIconNameChanged();
}

void TopNotificationManager::closeNotificationById(const QString &notificationId)
{
    for (int i = 0; i < d->mNotifications.size(); ) {
        if (d->mNotifications.at(i).notificationId() == notificationId) {
            closeNotification(i);
        } else {
            ++i;
        }
    }
}

void TopNotificationManager::triggerMainButton(int index) const
{
    if (index < 0 || index >= d->mNotifications.count()) {
        return;
    }

    d->mNotifications.at(index).triggerMainButton();
}

void TopNotificationManager::triggerSecondaryButton(int index) const
{
    if (index < 0 || index >= d->mNotifications.count()) {
        return;
    }

    d->mNotifications.at(index).triggerSecondaryButton();
}

int TopNotificationManager::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return d->mNotifications.count();
}

QVariant TopNotificationManager::data(const QModelIndex &index, int role) const
{
    auto result = QVariant();

    if (!index.isValid()) {
        return result;
    }

    if (index.column() != 0) {
        return result;
    }

    if (index.row() < 0 || index.row() >= d->mNotifications.count()) {
        return result;
    }

    if (role < ColumnsRoles::MessageRole || role > ColumnsRoles::SecondaryButtonIconNameRole) {
        return result;
    }

    auto realRole = static_cast<ColumnsRoles>(role);

    switch(realRole)
    {
    case ColumnsRoles::MessageRole:
        result = d->mNotifications.at(index.row()).message();
        break;
    case ColumnsRoles::MainButtonTextRole:
        result = d->mNotifications.at(index.row()).mainButtonText();
        break;
    case ColumnsRoles::MainButtonIconNameRole:
        result = d->mNotifications.at(index.row()).mainButtonIconName();
        break;
    case ColumnsRoles::SecondaryButtonTextRole:
        result = d->mNotifications.at(index.row()).secondaryButtonText();
        break;
    case ColumnsRoles::SecondaryButtonIconNameRole:
        result = d->mNotifications.at(index.row()).secondaryButtonIconName();
        break;
    }

    return result;
}

QHash<int, QByteArray> TopNotificationManager::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[static_cast<int>(ColumnsRoles::MessageRole)] = "message";
    roles[static_cast<int>(ColumnsRoles::MainButtonTextRole)] = "mainButtonText";
    roles[static_cast<int>(ColumnsRoles::MainButtonIconNameRole)] = "mainButtonIconName";
    roles[static_cast<int>(ColumnsRoles::SecondaryButtonTextRole)] = "secondaryButtonText";
    roles[static_cast<int>(ColumnsRoles::SecondaryButtonIconNameRole)] = "secondaryButtonIconName";

    return roles;
}

QString TopNotificationManager::notificationMessage() const
{
    return data(index(0, 0, {}), TopNotificationManager::MessageRole).toString();
}

QString TopNotificationManager::notificationMainButtonText() const
{
    return data(index(0, 0, {}), TopNotificationManager::MainButtonTextRole).toString();
}

QString TopNotificationManager::notificationMainButtonIconName() const
{
    return data(index(0, 0, {}), TopNotificationManager::MainButtonIconNameRole).toString();
}

QString TopNotificationManager::notificationSecondaryButtonText() const
{
    return data(index(0, 0, {}), TopNotificationManager::SecondaryButtonTextRole).toString();
}

QString TopNotificationManager::notificationSecondaryButtonIconName() const
{
    return data(index(0, 0, {}), TopNotificationManager::SecondaryButtonIconNameRole).toString();
}


//#include "moc_topnotificationmanager.cpp"
