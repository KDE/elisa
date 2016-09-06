/*
 * Copyright 2015 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "config-upnp-qt.h"

//#define QT_QML_DEBUG

#if defined UPNPQT_FOUND && UPNPQT_FOUND
#include "upnpssdpengine.h"
#include "upnpabstractservice.h"
#include "upnpcontrolabstractdevice.h"
#include "upnpcontrolabstractservice.h"
#include "upnpbasictypes.h"
#include "upnp/upnpcontrolconnectionmanager.h"
#include "upnp/upnpcontrolmediaserver.h"
#include "upnp/upnpcontrolcontentdirectory.h"
#include "upnp/upnpcontentdirectorymodel.h"
#include "upnpdevicedescription.h"
#include "upnp/didlparser.h"
#include "upnp/upnpdiscoverallmusic.h"
#endif

#include "progressindicator.h"
#include "mediaplaylist.h"
#include "playlistcontroler.h"
#include "musicstatistics.h"
#include "albumfilterproxymodel.h"
#include "databaseinterface.h"
#include "allalbumsmodel.h"
#include "albummodel.h"
#include "musicaudiotrack.h"
#include "musiclistenersmanager.h"

#if defined Qt5DBus_FOUND && Qt5DBus_FOUND
#include "mpris2/mpris2.h"
#include "mpris2/mediaplayer2player.h"
#include "mpris2/mediaplayer2tracklist.h"
#endif

#if defined KF5Declarative_FOUND && KF5Declarative_FOUND
#include <KDeclarative/KDeclarative>
#endif

#if defined KF5I18n_FOUND && KF5I18n_FOUND
#include <KI18n/KLocalizedString>
#endif

#if defined KF5CoreAddons_FOUND && KF5CoreAddons_FOUND
#include <KCoreAddons/KAboutData>
#endif

#include <QtGui/QIcon>

#include <QtWidgets/QApplication>

#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlFileSelector>
#include <QtQml/QQmlDebuggingEnabler>
#include <QtQml>


int __attribute__((visibility("default"))) main(int argc, char *argv[])
{
    QApplication app(argc, argv);

#if defined UPNPQT_FOUND && UPNPQT_FOUND
    qmlRegisterType<UpnpSsdpEngine>("org.mgallien.QmlExtension", 1, 0, "UpnpSsdpEngine");
    qmlRegisterType<UpnpDiscoverAllMusic>("org.mgallien.QmlExtension", 1, 0, "UpnpDiscoverAllMusic");

    qmlRegisterType<UpnpAbstractDevice>("org.mgallien.QmlExtension", 1, 0, "UpnpAbstractDevice");
    qmlRegisterType<UpnpAbstractService>("org.mgallien.QmlExtension", 1, 0, "UpnpAbstractService");
    qmlRegisterType<UpnpControlAbstractDevice>("org.mgallien.QmlExtension", 1, 0, "UpnpControlAbstractDevice");
    qmlRegisterType<UpnpControlAbstractService>("org.mgallien.QmlExtension", 1, 0, "UpnpControlAbstractService");
    qmlRegisterType<UpnpControlConnectionManager>("org.mgallien.QmlExtension", 1, 0, "UpnpControlConnectionManager");
    qmlRegisterType<UpnpControlMediaServer>("org.mgallien.QmlExtension", 1, 0, "UpnpControlMediaServer");
    qmlRegisterType<UpnpContentDirectoryModel>("org.mgallien.QmlExtension", 1, 0, "UpnpContentDirectoryModel");
    qmlRegisterType<DidlParser>("org.mgallien.QmlExtension", 1, 0, "DidlParser");
    qmlRegisterType<UpnpControlContentDirectory>("org.mgallien.QmlExtension", 1, 0, "UpnpControlContentDirectory");
    qmlRegisterType<UpnpDeviceDescription>("org.mgallien.QmlExtension", 1, 0, "UpnpDeviceDescription");

    qRegisterMetaType<A_ARG_TYPE_InstanceID>();
    qRegisterMetaType<QPointer<UpnpAbstractDevice> >();
    qRegisterMetaType<UpnpControlConnectionManager*>();
    qRegisterMetaType<UpnpContentDirectoryModel*>();
    qRegisterMetaType<UpnpDeviceDescription*>();
#endif

    qmlRegisterType<AlbumFilterProxyModel>("org.mgallien.QmlExtension", 1, 0, "AlbumFilterProxyModel");
    qmlRegisterType<MediaPlayList>("org.mgallien.QmlExtension", 1, 0, "MediaPlayList");
    qmlRegisterType<PlayListControler>("org.mgallien.QmlExtension", 1, 0, "PlayListControler");
    qmlRegisterType<MusicStatistics>("org.mgallien.QmlExtension", 1, 0, "MusicStatistics");
    qmlRegisterType<ProgressIndicator>("org.mgallien.QmlExtension", 1, 0, "ProgressIndicator");
    qmlRegisterType<DatabaseInterface>("org.mgallien.QmlExtension", 1, 0, "DatabaseInterface");
    qmlRegisterType<AllAlbumsModel>("org.mgallien.QmlExtension", 1, 0, "AllAlbumsModel");
    qmlRegisterType<AlbumModel>("org.mgallien.QmlExtension", 1, 0, "AlbumModel");
    qmlRegisterType<MusicListenersManager>("org.mgallien.QmlExtension", 1, 0, "MusicListenersManager");

#if defined Qt5DBus_FOUND && Qt5DBus_FOUND
    qmlRegisterType<Mpris2>("org.mgallien.QmlExtension", 1, 0, "Mpris2");
    qRegisterMetaType<MediaPlayer2Player*>();
    qRegisterMetaType<MediaPlayer2Tracklist*>();
#endif

    qRegisterMetaType<QAbstractItemModel*>();
    qRegisterMetaType<QHash<QString,QUrl>>("QHash<QString,QUrl>");
    qRegisterMetaType<QHash<QString,QVector<MusicAudioTrack>>>("QHash<QString,QVector<MusicAudioTrack>>");
    qRegisterMetaType<QVector<qulonglong>>("QVector<qulonglong>");
    qRegisterMetaType<QHash<qulonglong,int>>("QHash<qulonglong,int>");

#if defined KF5I18n_FOUND && KF5I18n_FOUND
    KLocalizedString::setApplicationDomain("elisa");
#endif

    //QQmlDebuggingEnabler enabler;

#if defined KF5CoreAddons_FOUND && KF5CoreAddons_FOUND
    KAboutData aboutData( QStringLiteral("elisa"),
                          i18n("Elisa"),
                          QStringLiteral("0.1"),
                          i18n("A Simple Music Player written with KDE Frameworks"),
                          KAboutLicense::LGPL_V3,
                          i18n("(c) 2015-2016, Matthieu Gallien <mgallien@mgallien.fr>"));

    aboutData.addAuthor(i18n("Matthieu Gallien"),i18n("Author"), QStringLiteral("mgallien@mgallien.fr"));
    KAboutData::setApplicationData(aboutData);

    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("new-audio-alarm")));

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);
#endif

    QQmlApplicationEngine engine;
    engine.addImportPath(QStringLiteral("qrc:/imports"));
    QQmlFileSelector selector(&engine);

#if defined KF5Declarative_FOUND && KF5Declarative_FOUND
    KDeclarative::KDeclarative decl;
    decl.setDeclarativeEngine(&engine);
    decl.setTranslationDomain(QStringLiteral("elisa"));
    decl.setupBindings();
#endif

    engine.load(QUrl(QStringLiteral("qrc:/MediaServer.qml")));

    return app.exec();
}
