/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "elisaqmltestplugin.h"

#include <QQmlExtensionPlugin>
#include <QQmlEngine>

#include "trackdatahelper.h"
#include "mediaplaylist.h"

void ElisaQmlTestPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}

void ElisaQmlTestPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<TrackDataHelper>(uri, 1, 0, "TrackDataHelper");
    qmlRegisterType<MediaPlayList>(uri, 1, 0, "MediaPlayList");
}
