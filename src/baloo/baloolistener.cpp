/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
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
