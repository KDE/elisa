/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "filelistener.h"

#include "localfilelisting.h"
#include "databaseinterface.h"

#include <QThread>

class FileListenerPrivate
{
public:

};

FileListener::FileListener(QObject *parent) : AbstractFileListener(new LocalFileListing, parent), d(new FileListenerPrivate)
{
}

FileListener::~FileListener()
{
    delete d;
}


#include "moc_filelistener.cpp"
