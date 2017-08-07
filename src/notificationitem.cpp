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

#include "notificationitem.h"

NotificationItem::NotificationItem(QObject *parent) : QObject(parent)
{
}

QString NotificationItem::message() const
{
    return mMessage;
}

QString NotificationItem::mainButtonText() const
{
    return mMainButtonText;
}

QString NotificationItem::secondaryButtonText() const
{
    return mSecondaryButtonText;
}

QString NotificationItem::secondaryButtonIconName() const
{
    return mSecondaryButtonIconName;
}

QString NotificationItem::mainButtonIconName() const
{
    return mMainButtonIconName;
}

void NotificationItem::setMessage(QString message)
{
    if (mMessage == message) {
        return;
    }

    mMessage = message;
    Q_EMIT messageChanged(mMessage);
}

void NotificationItem::setMainButtonText(QString mainButtonText)
{
    if (mMainButtonText == mainButtonText) {
        return;
    }

    mMainButtonText = mainButtonText;
    Q_EMIT mainButtonTextChanged(mMainButtonText);
}

void NotificationItem::setSecondaryButtonText(QString secondaryButtonText)
{
    if (mSecondaryButtonText == secondaryButtonText) {
        return;
    }

    mSecondaryButtonText = secondaryButtonText;
    Q_EMIT secondaryButtonTextChanged(mSecondaryButtonText);
}

void NotificationItem::setSecondaryButtonIconName(QString secondaryButtonIconName)
{
    if (mSecondaryButtonIconName == secondaryButtonIconName) {
        return;
    }

    mSecondaryButtonIconName = secondaryButtonIconName;
    Q_EMIT secondaryButtonIconNameTextChanged(mSecondaryButtonIconName);
}

void NotificationItem::triggerMainButton()
{
    Q_EMIT mainButtonTriggered();
}

void NotificationItem::triggerSecondaryButton()
{
    Q_EMIT secondaryButtonTriggered();
}

void NotificationItem::setMainButtonIconName(QString mainButtonIconName)
{
    if (mMainButtonIconName == mainButtonIconName) {
        return;
    }

    mMainButtonIconName = mainButtonIconName;
    Q_EMIT mainButtonIconNameChanged(mMainButtonIconName);
}


#include "moc_notificationitem.cpp"
