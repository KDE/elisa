/*
 * Copyright 2015-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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
#include "upnp/upnpcontrolconnectionmanager.h"
#include "upnp/upnpcontrolmediaserver.h"
#include "upnp/upnpcontrolcontentdirectory.h"
#include "upnp/upnpcontentdirectorymodel.h"
#include "upnpdevicedescription.h"
#include "upnp/didlparser.h"
#include "upnp/upnpdiscoverallmusic.h"

#include "upnpssdpengine.h"
#include "upnpabstractservice.h"
#include "upnpcontrolabstractdevice.h"
#include "upnpcontrolabstractservice.h"
#include "upnpbasictypes.h"
#endif

#include "progressindicator.h"
#include "mediaplaylist.h"
#include "playlistcontroler.h"
#include "managemediaplayercontrol.h"
#include "manageheaderbar.h"
#include "manageaudioplayer.h"
#include "musicstatistics.h"
#include "allalbumsmodel.h"
#include "albummodel.h"
#include "allartistsmodel.h"
#include "musicaudiotrack.h"
#include "musiclistenersmanager.h"
#include "albumfilterproxymodel.h"
#include "elisaapplication.h"
#include "audiowrapper.h"
#include "alltracksmodel.h"
#include "elisa_settings.h"

#if defined Qt5DBus_FOUND && Qt5DBus_FOUND
#include "mpris2/mpris2.h"
#include "mpris2/mediaplayer2player.h"
#endif

#if defined KF5Declarative_FOUND && KF5Declarative_FOUND
#include <KDeclarative/KDeclarative>
#endif

#include <KI18n/KLocalizedString>
#include <KI18n/KLocalizedContext>

#if defined KF5CoreAddons_FOUND && KF5CoreAddons_FOUND
#include <KCoreAddons/KAboutData>
#endif

#if defined KF5Crash_FOUND && KF5Crash_FOUND
#include <KCrash>
#endif

#include <QSortFilterProxyModel>

#include <QIcon>
#include <QAction>

#include <QApplication>
#include <QCommandLineParser>
#include <QtGlobal>

#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlFileSelector>
#include <QQmlDebuggingEnabler>
#include <QQmlContext>

#if defined Qt5AndroidExtras_FOUND && Qt5AndroidExtras_FOUND
#include <QAndroidJniObject>
#include <QtAndroid>
#endif

#if defined KF5DBusAddons_FOUND && KF5DBusAddons_FOUND
#include <KDBusService>
#endif

#if defined Q_OS_ANDROID
int __attribute__((visibility("default"))) main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    qputenv("QT_GSTREAMER_USE_PLAYBIN_VOLUME", "true");

    QApplication app(argc, argv);

#if defined KF5Crash_FOUND && KF5Crash_FOUND
    KCrash::initialize();
#endif

    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("new-audio-alarm")));

    KLocalizedString::setApplicationDomain("elisa");

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

    qmlRegisterType<MediaPlayList>("org.mgallien.QmlExtension", 1, 0, "MediaPlayList");
    qmlRegisterType<PlayListControler>("org.mgallien.QmlExtension", 1, 0, "PlayListControler");
    qmlRegisterType<ManageMediaPlayerControl>("org.mgallien.QmlExtension", 1, 0, "ManageMediaPlayerControl");
    qmlRegisterType<ManageHeaderBar>("org.mgallien.QmlExtension", 1, 0, "ManageHeaderBar");
    qmlRegisterType<ManageAudioPlayer>("org.mgallien.QmlExtension", 1, 0, "ManageAudioPlayer");
    qmlRegisterType<MusicStatistics>("org.mgallien.QmlExtension", 1, 0, "MusicStatistics");
    qmlRegisterType<ProgressIndicator>("org.mgallien.QmlExtension", 1, 0, "ProgressIndicator");
    qmlRegisterType<AllAlbumsModel>("org.mgallien.QmlExtension", 1, 0, "AllAlbumsModel");
    qmlRegisterType<AllArtistsModel>("org.mgallien.QmlExtension", 1, 0, "AllArtistsModel");
    qmlRegisterType<AlbumModel>("org.mgallien.QmlExtension", 1, 0, "AlbumModel");
    qmlRegisterType<AllTracksModel>("org.mgallien.QmlExtension", 1, 0, "AllTracksModel");
    qmlRegisterType<MusicListenersManager>("org.mgallien.QmlExtension", 1, 0, "MusicListenersManager");
    qmlRegisterType<QSortFilterProxyModel>("org.mgallien.QmlExtension", 1, 0, "SortFilterProxyModel");
    qmlRegisterType<AlbumFilterProxyModel>("org.mgallien.QmlExtension", 1, 0, "AlbumFilterProxyModel");
    qmlRegisterType<AudioWrapper>("org.mgallien.QmlExtension", 1, 0, "AudioWrapper");

#if defined Qt5DBus_FOUND && Qt5DBus_FOUND
    qmlRegisterType<Mpris2>("org.mgallien.QmlExtension", 1, 0, "Mpris2");
    qRegisterMetaType<MediaPlayer2Player*>();
#endif

    qRegisterMetaType<QAbstractItemModel*>();
    qRegisterMetaType<QHash<QString,QUrl>>("QHash<QString,QUrl>");
    qRegisterMetaType<QList<MusicAudioTrack>>("QList<MusicAudioTrack>");
    qRegisterMetaType<QList<MusicAudioTrack>>("QVector<MusicAudioTrack>");
    qRegisterMetaType<QVector<qulonglong>>("QVector<qulonglong>");
    qRegisterMetaType<QHash<qulonglong,int>>("QHash<qulonglong,int>");
    qRegisterMetaType<MusicAlbum>("MusicAlbum");
    qRegisterMetaType<MusicArtist>("MusicArtist");
    qRegisterMetaType<QAction*>();
    qmlRegisterUncreatableType<ElisaApplication>("org.mgallien.QmlExtension", 1, 0, "ElisaApplication", QStringLiteral("only one and done in c++"));

    qRegisterMetaTypeStreamOperators<ManageMediaPlayerControl::PlayerState>("PlayListControler::PlayerState");

#if defined KF5CoreAddons_FOUND && KF5CoreAddons_FOUND
    KAboutData aboutData( QStringLiteral("elisa"),
                          i18n("Elisa"),
                          QStringLiteral("0.1"),
                          i18n("A Simple Music Player written with KDE Frameworks"),
                          KAboutLicense::LGPL_V3,
                          i18n("(c) 2015-2017, Matthieu Gallien <mgallien@mgallien.fr>"));

    aboutData.addAuthor(i18n("Matthieu Gallien"),i18n("Author"), QStringLiteral("mgallien@mgallien.fr"));
    KAboutData::setApplicationData(aboutData);
#else
    QGuiApplication::setApplicationName(QStringLiteral("elisa"));
    QGuiApplication::setOrganizationDomain(QStringLiteral("kde.org"));
    QGuiApplication::setOrganizationName(QStringLiteral("KDE"));
#endif

#if defined KF5DBusAddons_FOUND && KF5DBusAddons_FOUND
    KDBusService elisaService(KDBusService::Unique);
#endif

    KLocalizedString::setApplicationDomain("elisa");
    ElisaApplication myApp;

#if defined KF5CoreAddons_FOUND && KF5CoreAddons_FOUND
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    aboutData.setupCommandLine(&parser);

    parser.process(app);
    aboutData.processCommandLine(&parser);
#endif

#if defined Qt5AndroidExtras_FOUND && Qt5AndroidExtras_FOUND
    qDebug() << QCoreApplication::arguments();

    QAndroidJniObject::callStaticMethod<void>("com/kde/elisa/ElisaService",
                                              "startMyService",
                                              "(Landroid/content/Context;)V",
                                              QtAndroid::androidContext().object());
#endif

    auto configurationFileName = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    configurationFileName += QStringLiteral("/elisarc");
    Elisa::ElisaConfiguration::instance(configurationFileName);
    Elisa::ElisaConfiguration::self()->load();
    Elisa::ElisaConfiguration::self()->save();

    QQmlApplicationEngine engine;
    engine.addImportPath(QStringLiteral("qrc:/imports"));
    QQmlFileSelector selector(&engine);

#if defined KF5Declarative_FOUND && KF5Declarative_FOUND
    KDeclarative::KDeclarative decl;
    decl.setDeclarativeEngine(&engine);
    decl.setupBindings();
#endif

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.rootContext()->setContextProperty(QStringLiteral("elisa"), &myApp);

    engine.load(QUrl(QStringLiteral("qrc:/MediaServer.qml")));

    return app.exec();
}
