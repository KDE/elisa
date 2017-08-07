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

#include <QObject>

class NotificationItem : public QObject
{

    Q_OBJECT

    Q_PROPERTY(QString message
               READ message
               WRITE setMessage
               NOTIFY messageChanged)

    Q_PROPERTY(QString mainButtonText
               READ mainButtonText
               WRITE setMainButtonText
               NOTIFY mainButtonTextChanged)

    Q_PROPERTY(QString mainButtonIconName
               READ mainButtonIconName
               WRITE setMainButtonIconName
               NOTIFY mainButtonIconNameChanged)

    Q_PROPERTY(QString secondaryButtonText
               READ secondaryButtonText
               WRITE setSecondaryButtonText
               NOTIFY secondaryButtonTextChanged)

    Q_PROPERTY(QString secondaryButtonIconName
               READ secondaryButtonIconName
               WRITE setSecondaryButtonIconName
               NOTIFY secondaryButtonIconNameTextChanged)

public:

    explicit NotificationItem(QObject *parent = nullptr);

    QString message() const;

    QString mainButtonText() const;

    QString mainButtonIconName() const;

    QString secondaryButtonText() const;

    QString secondaryButtonIconName() const;

Q_SIGNALS:

    void messageChanged(QString message);

    void mainButtonTextChanged(QString mainButtonText);

    void mainButtonIconNameChanged(QString mainButtonIconName);

    void secondaryButtonTextChanged(QString secondaryButtonText);

    void secondaryButtonIconNameTextChanged(QString secondaryButtonIconName);

    void mainButtonTriggered();

    void secondaryButtonTriggered();

public Q_SLOTS:

    void setMessage(QString message);

    void setMainButtonText(QString mainButtonText);

    void setMainButtonIconName(QString mainButtonIconName);

    void setSecondaryButtonText(QString secondaryButtonText);

    void setSecondaryButtonIconName(QString secondaryButtonIconName);

    void triggerMainButton();

    void triggerSecondaryButton();

private:

    QString mMessage;

    QString mMainButtonText;

    QString mMainButtonIconName;

    QString mSecondaryButtonText;

    QString mSecondaryButtonIconName;
};

#endif // NOTIFICATIONITEM_H
