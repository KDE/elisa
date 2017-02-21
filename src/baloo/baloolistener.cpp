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

#include <QThread>

class BalooListenerPrivate
{
public:

    QThread mBalooQueryThread;

    LocalBalooFileListing mFileListing;

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
    return nullptr;
}

void BalooListener::setDatabaseInterface(DatabaseInterface *model)
{
    if (model) {
        connect(this, &BalooListener::databaseReady, &d->mFileListing, &LocalBalooFileListing::databaseIsReady);
        connect(&d->mFileListing, &LocalBalooFileListing::initialTracksListRequired, this, &BalooListener::initialTracksListRequired);
        connect(this, &BalooListener::initialTracksList, &d->mFileListing, &LocalBalooFileListing::initialTracksList);
        connect(&d->mFileListing, &LocalBalooFileListing::tracksList, model, &DatabaseInterface::insertTracksList);

        QMetaObject::invokeMethod(&d->mFileListing, "init", Qt::QueuedConnection);
    }

    Q_EMIT databaseInterfaceChanged();
}

void BalooListener::applicationAboutToQuit()
{
    d->mBalooQueryThread.exit();
    d->mBalooQueryThread.wait();
}


#include "moc_baloolistener.cpp"
