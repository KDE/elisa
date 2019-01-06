/*
 * Copyright 2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "elisaqmlplugin.h"

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

#include "elisautils.h"
#include "elisaapplication.h"
#include "progressindicator.h"
#include "mediaplaylist.h"
#include "managemediaplayercontrol.h"
#include "manageheaderbar.h"
#include "manageaudioplayer.h"
#include "musicaudiotrack.h"
#include "musiclistenersmanager.h"
#include "trackslistener.h"
#include "viewmanager.h"
#include "databaseinterface.h"
#include "models/datamodel.h"
#include "models/trackmetadatamodel.h"
#include "models/viewsmodel.h"
#include "models/allalbumsproxymodel.h"
#include "models/alltracksproxymodel.h"
#include "models/allartistsproxymodel.h"
#include "models/singlealbumproxymodel.h"

#if defined KF5FileMetaData_FOUND && KF5FileMetaData_FOUND
#include "embeddedcoverageimageprovider.h"
#endif

#if defined KF5KIO_FOUND && KF5KIO_FOUND
#include "models/filebrowsermodel.h"
#include "models/filebrowserproxymodel.h"
#endif

#include "audiowrapper.h"
#include "notificationitem.h"
#include "topnotificationmanager.h"
#include "elisautils.h"
#include "datatype.h"

#if defined Qt5DBus_FOUND && Qt5DBus_FOUND
#include "mpris2/mpris2.h"
#include "mpris2/mediaplayer2player.h"
#endif

#include <QSortFilterProxyModel>

#include <QAction>
#include <QStandardPaths>

#include <QQmlExtensionPlugin>
#include <QQmlEngine>
#include <QQmlContext>

void ElisaQmlTestPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
#if defined KF5FileMetaData_FOUND && KF5FileMetaData_FOUND
    engine->addImageProvider(QStringLiteral("cover"), new EmbeddedCoverageImageProvider);
#endif
}

void ElisaQmlTestPlugin::registerTypes(const char *uri)
{
#if defined UPNPQT_FOUND && UPNPQT_FOUND
    qmlRegisterType<UpnpSsdpEngine>(uri, 1, 0, "UpnpSsdpEngine");
    qmlRegisterType<UpnpDiscoverAllMusic>(uri, 1, 0, "UpnpDiscoverAllMusic");

    qmlRegisterType<UpnpAbstractDevice>(uri, 1, 0, "UpnpAbstractDevice");
    qmlRegisterType<UpnpAbstractService>(uri, 1, 0, "UpnpAbstractService");
    qmlRegisterType<UpnpControlAbstractDevice>(uri, 1, 0, "UpnpControlAbstractDevice");
    qmlRegisterType<UpnpControlAbstractService>(uri, 1, 0, "UpnpControlAbstractService");
    qmlRegisterType<UpnpControlConnectionManager>(uri, 1, 0, "UpnpControlConnectionManager");
    qmlRegisterType<UpnpControlMediaServer>(uri, 1, 0, "UpnpControlMediaServer");
    qmlRegisterType<UpnpContentDirectoryModel>(uri, 1, 0, "UpnpContentDirectoryModel");
    qmlRegisterType<DidlParser>(uri, 1, 0, "DidlParser");
    qmlRegisterType<UpnpControlContentDirectory>(uri, 1, 0, "UpnpControlContentDirectory");
    qmlRegisterType<UpnpDeviceDescription>(uri, 1, 0, "UpnpDeviceDescription");

    qRegisterMetaType<A_ARG_TYPE_InstanceID>();
    qRegisterMetaType<QPointer<UpnpAbstractDevice> >();
    qRegisterMetaType<UpnpControlConnectionManager*>();
    qRegisterMetaType<UpnpContentDirectoryModel*>();
    qRegisterMetaType<UpnpDeviceDescription*>();
#endif
    qRegisterMetaType<DataUtils::DataType>("DataUtils::DataType");

    qmlRegisterType<MediaPlayList>(uri, 1, 0, "MediaPlayList");
    qmlRegisterType<ManageMediaPlayerControl>(uri, 1, 0, "ManageMediaPlayerControl");
    qmlRegisterType<ManageHeaderBar>(uri, 1, 0, "ManageHeaderBar");
    qmlRegisterType<ManageAudioPlayer>(uri, 1, 0, "ManageAudioPlayer");
    qmlRegisterType<ProgressIndicator>(uri, 1, 0, "ProgressIndicator");
    qmlRegisterType<MusicListenersManager>(uri, 1, 0, "MusicListenersManager");
    qmlRegisterType<ViewManager>(uri, 1, 0, "ViewManager");
    qmlRegisterType<DataModel>(uri, 1, 0, "DataModel");
    qmlRegisterType<TrackMetadataModel>(uri, 1, 0, "TrackMetadataModel");
    qmlRegisterType<ViewsModel>(uri, 1, 0, "ViewsModel");
    qmlRegisterType<AllAlbumsProxyModel>(uri, 1, 0, "AllAlbumsProxyModel");
    qmlRegisterType<AllArtistsProxyModel>(uri, 1, 0, "AllArtistsProxyModel");
    qmlRegisterType<AllTracksProxyModel>(uri, 1, 0, "AllTracksProxyModel");
    qmlRegisterType<SingleAlbumProxyModel>(uri, 1, 0, "SingleAlbumProxyModel");

#if defined KF5KIO_FOUND && KF5KIO_FOUND
    qmlRegisterType<FileBrowserModel>(uri, 1, 0, "FileBrowserModel");
    qmlRegisterType<FileBrowserProxyModel>(uri, 1, 0, "FileBrowserProxyModel");
#endif

    qmlRegisterType<AudioWrapper>(uri, 1, 0, "AudioWrapper");
    qmlRegisterType<TopNotificationManager>(uri, 1, 0, "TopNotificationManager");
    qmlRegisterUncreatableMetaObject(DataUtils::staticMetaObject, uri, 1, 0, "DataUtils", QStringLiteral("Only enums"));
    qmlRegisterUncreatableType<DatabaseInterface>(uri, 1, 0, "DatabaseInterface", QStringLiteral("Only created in c++"));

#if defined Qt5DBus_FOUND && Qt5DBus_FOUND
    qmlRegisterType<Mpris2>(uri, 1, 0, "Mpris2");
    qRegisterMetaType<MediaPlayer2Player*>();
#endif

    qRegisterMetaType<AbstractMediaProxyModel*>();
    qRegisterMetaType<QHash<QString,QUrl>>("QHash<QString,QUrl>");
    qRegisterMetaType<QHash<QUrl,QDateTime>>("QHash<QUrl,QDateTime>");
    qRegisterMetaType<QList<MusicAudioTrack>>("QList<MusicAudioTrack>");
    qRegisterMetaType<QList<MusicAudioTrack>>("QVector<MusicAudioTrack>");
    qRegisterMetaType<QVector<qulonglong>>("QVector<qulonglong>");
    qRegisterMetaType<QHash<qulonglong,int>>("QHash<qulonglong,int>");
    qRegisterMetaType<DatabaseInterface::ListTrackDataType>("DatabaseInterface::ListTrackDataType");
    qRegisterMetaType<DatabaseInterface::ListAlbumDataType>("DatabaseInterface::ListAlbumDataType");
    qRegisterMetaType<DatabaseInterface::ListArtistDataType>("DatabaseInterface::ListArtistDataType");
    qRegisterMetaType<DatabaseInterface::ListGenreDataType>("DatabaseInterface::ListGenreDataType");
    qRegisterMetaType<ModelDataLoader::ListTrackDataType>("ModelDataLoader::ListTrackDataType");
    qRegisterMetaType<ModelDataLoader::ListAlbumDataType>("ModelDataLoader::ListAlbumDataType");
    qRegisterMetaType<ModelDataLoader::ListArtistDataType>("ModelDataLoader::ListArtistDataType");
    qRegisterMetaType<ModelDataLoader::ListGenreDataType>("ModelDataLoader::ListGenreDataType");
    qRegisterMetaType<TracksListener::ListTrackDataType>("TracksListener::ListTrackDataType");
    qRegisterMetaType<QMap<QString, int>>();
    qRegisterMetaType<QAction*>();
    qRegisterMetaType<NotificationItem>("NotificationItem");
    qRegisterMetaType<QMap<QString,int>>("QMap<QString,int>");
    qRegisterMetaType<ElisaUtils::PlayListEnqueueMode>("PlayListEnqueueMode");
    qRegisterMetaType<ElisaUtils::PlayListEnqueueTriggerPlay>("PlayListEnqueueTriggerPlay");
    qRegisterMetaType<ElisaUtils::PlayListEntryType>("PlayListEntryType");
    qRegisterMetaType<ElisaUtils::EntryData>("EntryData");
    qRegisterMetaType<ElisaUtils::EntryDataList>("EntryDataList");
    qRegisterMetaType<DatabaseInterface::TrackDataType>("DatabaseInterface::TrackDataType");
    qRegisterMetaType<DatabaseInterface::AlbumDataType>("DatabaseInterface::AlbumDataType");
    qRegisterMetaType<DatabaseInterface::ArtistDataType>("DatabaseInterface::ArtistDataType");
    qRegisterMetaType<DatabaseInterface::GenreDataType>("DatabaseInterface::GenreDataType");
    qRegisterMetaType<ModelDataLoader::TrackDataType>("ModelDataLoader::TrackDataType");
    qRegisterMetaType<TracksListener::TrackDataType>("TracksListener::TrackDataType");

    qRegisterMetaTypeStreamOperators<ManageMediaPlayerControl::PlayerState>("PlayListControler::PlayerState");

    qmlRegisterUncreatableType<ElisaApplication>(uri, 1, 0, "ElisaApplication", QStringLiteral("only one and done in c++"));
    qmlRegisterUncreatableMetaObject(ElisaUtils::staticMetaObject, uri, 1, 0, "ElisaUtils", QStringLiteral("Namespace ElisaUtils"));
}
