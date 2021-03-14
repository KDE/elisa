/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "androidmusiclistener.h"

#include "androidfilelisting.h"

#include "android/androidcommon.h"

class AndroidMusicListenerPrivate
{
public:

    AndroidFileListing mAndroidFileIndexer;

};

AndroidMusicListener::AndroidMusicListener(QObject *parent)
    : AbstractFileListener(parent), d(std::make_unique<AndroidMusicListenerPrivate>())
{
    setFileListing(&d->mAndroidFileIndexer);
}

AndroidMusicListener::~AndroidMusicListener()
= default;

#include "moc_androidmusiclistener.cpp"
