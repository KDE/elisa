/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "filelistener.h"

#include "localfilelisting.h"
#include "databaseinterface.h"

#include <QThread>

class FileListenerPrivate
{
public:

    LocalFileListing mLocalFileIndexer;

};

FileListener::FileListener(QObject *parent) : AbstractFileListener(parent), d(std::make_unique<FileListenerPrivate>())
{
    setFileListing(&d->mLocalFileIndexer);
}

FileListener::~FileListener()
= default;

const LocalFileListing& FileListener::localFileIndexer() const
{
    return d->mLocalFileIndexer;
}


#include "moc_filelistener.cpp"
