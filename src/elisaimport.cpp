/*
 * Copyright 2015-2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "config-upnp-qt.h"

#include "musiclistenersmanager.h"
#include "elisaimportapplication.h"
#include "elisa_settings.h"
#include "musicaudiotrack.h"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QtGlobal>
#include <QStandardPaths>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    qRegisterMetaType<QHash<QString,QUrl>>("QHash<QString,QUrl>");
    qRegisterMetaType<QList<MusicAudioTrack>>("QList<MusicAudioTrack>");
    qRegisterMetaType<QList<MusicAudioTrack>>("QVector<MusicAudioTrack>");
    qRegisterMetaType<QVector<qulonglong>>("QVector<qulonglong>");
    qRegisterMetaType<QHash<qulonglong,int>>("QHash<qulonglong,int>");
    qRegisterMetaType<QMap<QString, int>>();
    qRegisterMetaType<NotificationItem>("NotificationItem");
    qRegisterMetaType<QMap<QString,int>>("QMap<QString,int>");

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(app);

    auto configurationFileName = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    configurationFileName += QStringLiteral("/elisarc");
    Elisa::ElisaConfiguration::instance(configurationFileName);
    Elisa::ElisaConfiguration::self()->load();
    Elisa::ElisaConfiguration::self()->save();

    MusicListenersManager myMusicManager;
    ElisaImportApplication myApplication;

    QObject::connect(&myMusicManager, &MusicListenersManager::indexingRunningChanged,
            &myApplication, &ElisaImportApplication::indexingChanged);

    return app.exec();
}

