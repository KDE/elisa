/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "config-upnp-qt.h"

#include "musiclistenersmanager.h"
#include "elisaimportapplication.h"
#include "elisa_settings.h"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QStandardPaths>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    qRegisterMetaType<QHash<QString,QUrl>>("QHash<QString,QUrl>");
    qRegisterMetaType<QVector<qulonglong>>("QVector<qulonglong>");
    qRegisterMetaType<QHash<qulonglong,int>>("QHash<qulonglong,int>");
    qRegisterMetaType<QMap<QString, int>>();
    qRegisterMetaType<QMap<QString,int>>("QMap<QString,int>");
    qRegisterMetaType<QHash<QUrl,QDateTime>>("QHash<QUrl,QDateTime>");
    qRegisterMetaType<DataTypes::ListTrackDataType>("DataTypes::ListTrackDataType");

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

    QObject::connect(&myMusicManager, &MusicListenersManager::indexerBusyChanged,
            &myApplication, &ElisaImportApplication::indexingChanged);

    return app.exec();
}

