/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "baloolistener.h"

#include "localbaloofilelisting.h"
#include "databaseinterface.h"

#include <QtCore/QThread>

class BalooListenerPrivate
{
public:

    QThread mBalooQueryThread;

    LocalBalooFileListing mFileListing;

    DatabaseInterface* mDatabaseInterface = nullptr;

};

BalooListener::BalooListener(QObject *parent) : QObject(parent), d(new BalooListenerPrivate)
{
    d->mBalooQueryThread.start();
    d->mFileListing.moveToThread(&d->mBalooQueryThread);
}

BalooListener::~BalooListener()
{
    delete d;
}

DatabaseInterface *BalooListener::databaseInterface() const
{
    return d->mDatabaseInterface;
}

void BalooListener::setDatabaseInterface(DatabaseInterface *model)
{
    if (d->mDatabaseInterface == model) {
        return;
    }

    if (d->mDatabaseInterface) {
        disconnect(d->mDatabaseInterface);
    }

    d->mDatabaseInterface = model;

    if (d->mDatabaseInterface) {
        connect(this, &BalooListener::refreshContent, &d->mFileListing, &LocalBalooFileListing::refreshContent, Qt::QueuedConnection);
        connect(&d->mFileListing, &LocalBalooFileListing::tracksList, d->mDatabaseInterface, &DatabaseInterface::insertTracksList);

        QMetaObject::invokeMethod(&d->mFileListing, "init", Qt::QueuedConnection);

        Q_EMIT refreshContent();
    }

    emit databaseInterfaceChanged();
}
