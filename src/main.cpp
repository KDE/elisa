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
#include "notificationitem.h"
#include "topnotificationmanager.h"
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

#include <KCoreAddons/KAboutData>

#if defined KF5Crash_FOUND && KF5Crash_FOUND
#include <KCrash>
#endif

#include <QSortFilterProxyModel>

#include <QIcon>
#include <QAction>

#include <QApplication>
#include <QCommandLineParser>
#include <QtGlobal>
#include <QStandardPaths>

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
    qmlRegisterType<UpnpSsdpEngine>("org.kde.elisa", 1, 0, "UpnpSsdpEngine");
    qmlRegisterType<UpnpDiscoverAllMusic>("org.kde.elisa", 1, 0, "UpnpDiscoverAllMusic");

    qmlRegisterType<UpnpAbstractDevice>("org.kde.elisa", 1, 0, "UpnpAbstractDevice");
    qmlRegisterType<UpnpAbstractService>("org.kde.elisa", 1, 0, "UpnpAbstractService");
    qmlRegisterType<UpnpControlAbstractDevice>("org.kde.elisa", 1, 0, "UpnpControlAbstractDevice");
    qmlRegisterType<UpnpControlAbstractService>("org.kde.elisa", 1, 0, "UpnpControlAbstractService");
    qmlRegisterType<UpnpControlConnectionManager>("org.kde.elisa", 1, 0, "UpnpControlConnectionManager");
    qmlRegisterType<UpnpControlMediaServer>("org.kde.elisa", 1, 0, "UpnpControlMediaServer");
    qmlRegisterType<UpnpContentDirectoryModel>("org.kde.elisa", 1, 0, "UpnpContentDirectoryModel");
    qmlRegisterType<DidlParser>("org.kde.elisa", 1, 0, "DidlParser");
    qmlRegisterType<UpnpControlContentDirectory>("org.kde.elisa", 1, 0, "UpnpControlContentDirectory");
    qmlRegisterType<UpnpDeviceDescription>("org.kde.elisa", 1, 0, "UpnpDeviceDescription");

    qRegisterMetaType<A_ARG_TYPE_InstanceID>();
    qRegisterMetaType<QPointer<UpnpAbstractDevice> >();
    qRegisterMetaType<UpnpControlConnectionManager*>();
    qRegisterMetaType<UpnpContentDirectoryModel*>();
    qRegisterMetaType<UpnpDeviceDescription*>();
#endif

    qmlRegisterType<MediaPlayList>("org.kde.elisa", 1, 0, "MediaPlayList");
    qmlRegisterType<ManageMediaPlayerControl>("org.kde.elisa", 1, 0, "ManageMediaPlayerControl");
    qmlRegisterType<ManageHeaderBar>("org.kde.elisa", 1, 0, "ManageHeaderBar");
    qmlRegisterType<ManageAudioPlayer>("org.kde.elisa", 1, 0, "ManageAudioPlayer");
    qmlRegisterType<MusicStatistics>("org.kde.elisa", 1, 0, "MusicStatistics");
    qmlRegisterType<ProgressIndicator>("org.kde.elisa", 1, 0, "ProgressIndicator");
    qmlRegisterType<AllAlbumsModel>("org.kde.elisa", 1, 0, "AllAlbumsModel");
    qmlRegisterType<AllArtistsModel>("org.kde.elisa", 1, 0, "AllArtistsModel");
    qmlRegisterType<AlbumModel>("org.kde.elisa", 1, 0, "AlbumModel");
    qmlRegisterType<AllTracksModel>("org.kde.elisa", 1, 0, "AllTracksModel");
    qmlRegisterType<MusicListenersManager>("org.kde.elisa", 1, 0, "MusicListenersManager");
    qmlRegisterType<QSortFilterProxyModel>("org.kde.elisa", 1, 0, "SortFilterProxyModel");
    qmlRegisterType<AlbumFilterProxyModel>("org.kde.elisa", 1, 0, "AlbumFilterProxyModel");
    qmlRegisterType<AudioWrapper>("org.kde.elisa", 1, 0, "AudioWrapper");
    qmlRegisterType<TopNotificationManager>("org.kde.elisa", 1, 0, "TopNotificationManager");

#if defined Qt5DBus_FOUND && Qt5DBus_FOUND
    qmlRegisterType<Mpris2>("org.kde.elisa", 1, 0, "Mpris2");
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
    qRegisterMetaType<QMap<QString, int>>();
    qRegisterMetaType<QAction*>();
    qRegisterMetaType<NotificationItem>("NotificationItem");
    qRegisterMetaType<QMap<QString,int>>("QMap<QString,int>");
    qmlRegisterUncreatableType<ElisaApplication>("org.kde.elisa", 1, 0, "ElisaApplication", QStringLiteral("only one and done in c++"));

    qRegisterMetaTypeStreamOperators<ManageMediaPlayerControl::PlayerState>("PlayListControler::PlayerState");

    KAboutData aboutData( QStringLiteral("elisa"),
                          i18n("Elisa"),
                          QStringLiteral("0.0.81"),
                          i18n("A Simple Music Player written with KDE Frameworks"),
                          KAboutLicense::LGPL_V3,
                          i18n("(c) 2015-2017, Matthieu Gallien &lt;mgallien@mgallien.fr&gt;"));

    aboutData.addAuthor(QStringLiteral("Matthieu Gallien"),i18n("Creator"), QStringLiteral("mgallien@mgallien.fr"));
    aboutData.addAuthor(QStringLiteral("Alexander Stippich"), i18n("Author"), QStringLiteral("a.stippich@gmx.net"));

    aboutData.addCredit(QStringLiteral("Andrew Lake"), i18n("Concept and design work"), QStringLiteral("jamboarder@gmail.com"));
    aboutData.addCredit(QStringLiteral("Luigi Toscano"), i18n("Localization support"), QStringLiteral("luigi.toscano@tiscali.it"));
    aboutData.addCredit(QStringLiteral("Safa Alfulaij"), i18n("Right to left support in interface"), QStringLiteral("safa1996alfulaij@gmail.com"));
    aboutData.addCredit(QStringLiteral("Diego Gangl"), i18n("Various improvements to the interface"), QStringLiteral("diego@sinestesia.co"));

    KAboutData::setApplicationData(aboutData);

#if defined KF5DBusAddons_FOUND && KF5DBusAddons_FOUND
    KDBusService elisaService(KDBusService::Unique);
#endif

    KLocalizedString::setApplicationDomain("elisa");
    ElisaApplication myApp;

#if defined KF5DBusAddons_FOUND && KF5DBusAddons_FOUND
    QObject::connect(&elisaService, &KDBusService::activateActionRequested, &myApp, &ElisaApplication::activateActionRequested);
    QObject::connect(&elisaService, &KDBusService::activateRequested, &myApp, &ElisaApplication::activateRequested);
    QObject::connect(&elisaService, &KDBusService::openRequested, &myApp, &ElisaApplication::openRequested);
#endif

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    myApp.setArguments(parser.positionalArguments());

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

    MusicListenersManager myMusicManager;
    myMusicManager.setElisaApplication(&myApp);

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
    engine.rootContext()->setContextProperty(QStringLiteral("allListeners"), &myMusicManager);

    engine.load(QUrl(QStringLiteral("qrc:/qml/ElisaMainWindow.qml")));

    return app.exec();
}
