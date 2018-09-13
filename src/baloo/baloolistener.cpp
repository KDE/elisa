/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "baloolistener.h"

#include "localbaloofilelisting.h"
#include "databaseinterface.h"

#include <QThread>

class BalooListenerPrivate
{
public:

    LocalBalooFileListing mBalooFileIndexer;

};

BalooListener::BalooListener(QObject *parent) : AbstractFileListener(parent), d(new BalooListenerPrivate)
{
    setFileListing(&d->mBalooFileIndexer);
}

BalooListener::~BalooListener()
= default;


#include "moc_baloolistener.cpp"
