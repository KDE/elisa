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

#ifndef TOPNOTIFICATIONMANAGER_H
#define TOPNOTIFICATIONMANAGER_H

#include "elisaLib_export.h"

#include "notificationitem.h"

#include <QObject>
#include <QAbstractListModel>

#include <memory>

class TopNotificationManagerPrivate;

class ELISALIB_EXPORT TopNotificationManager : public QAbstractListModel
{

    Q_OBJECT

    Q_PROPERTY(int countNotifications
               READ countNotifications
               NOTIFY countNotificationsChanged)

    Q_PROPERTY(QString notificationMessage
               READ notificationMessage
               NOTIFY notificationMessageChanged)

    Q_PROPERTY(QString notificationMainButtonText
               READ notificationMainButtonText
               NOTIFY notificationMainButtonTextChanged)

    Q_PROPERTY(QString notificationMainButtonIconName
               READ notificationMainButtonIconName
               NOTIFY notificationMainButtonIconNameChanged)

    Q_PROPERTY(QString notificationSecondaryButtonText
               READ notificationSecondaryButtonText
               NOTIFY notificationSecondaryButtonTextChanged)

    Q_PROPERTY(QString notificationSecondaryButtonIconName
               READ notificationSecondaryButtonIconName
               NOTIFY notificationSecondaryButtonIconNameChanged)

public:

    enum ColumnsRoles {
        MessageRole = Qt::UserRole + 1,
        MainButtonTextRole,
        MainButtonIconNameRole,
        SecondaryButtonTextRole,
        SecondaryButtonIconNameRole,
    };

    Q_ENUM(ColumnsRoles)

    explicit TopNotificationManager(QObject *parent = nullptr);

    ~TopNotificationManager();

    int countNotifications() const;

    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int,QByteArray> roleNames() const override;

    QString notificationMessage() const;

    QString notificationMainButtonText() const;

    QString notificationMainButtonIconName() const;

    QString notificationSecondaryButtonText() const;

    QString notificationSecondaryButtonIconName() const;

Q_SIGNALS:

    void countNotificationsChanged();

    void notificationMessageChanged();

    void notificationMainButtonTextChanged();

    void notificationMainButtonIconNameChanged();

    void notificationSecondaryButtonTextChanged();

    void notificationSecondaryButtonIconNameChanged();

public Q_SLOTS:

    void addNotification(NotificationItem notification);

    void closeNotification(int index);

    void closeNotificationById(const QString &notificationId);

    void triggerMainButton(int index) const;

    void triggerSecondaryButton(int index) const;

private:

    std::unique_ptr<TopNotificationManagerPrivate> d;

};



#endif // TOPNOTIFICATIONMANAGER_H
