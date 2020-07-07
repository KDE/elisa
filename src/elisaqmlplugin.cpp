/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "elisaqmlplugin.h"

#include "config-upnp-qt.h"

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

#if defined KF5DBusAddons_FOUND && KF5DBusAddons_FOUND
#include <KDBusService>
#endif

#include "elisautils.h"
#include "elisaapplication.h"
#include "progressindicator.h"
#include "mediaplaylist.h"
#include "mediaplaylistproxymodel.h"
#include "managemediaplayercontrol.h"
#include "manageheaderbar.h"
#include "manageaudioplayer.h"
#include "musiclistenersmanager.h"
#include "trackslistener.h"
#include "viewmanager.h"
#include "viewslistdata.h"
#include "viewconfigurationdata.h"
#include "databaseinterface.h"
#include "datatypes.h"
#include "models/datamodel.h"
#include "models/trackmetadatamodel.h"
#include "models/trackcontextmetadatamodel.h"
#include "models/editabletrackmetadatamodel.h"
#include "models/viewsmodel.h"
#include "models/viewsproxymodel.h"
#include "models/gridviewproxymodel.h"
#include "localFileConfiguration/elisaconfigurationdialog.h"

#if defined KF5FileMetaData_FOUND && KF5FileMetaData_FOUND
#include "embeddedcoverageimageprovider.h"
#endif

#if defined KF5KIO_FOUND && KF5KIO_FOUND
#include "models/filebrowsermodel.h"
#include "models/filebrowserproxymodel.h"
#endif

#include "audiowrapper.h"

#if defined Qt5DBus_FOUND && Qt5DBus_FOUND
#include "mpris2/mpris2.h"
#include "mpris2/mediaplayer2player.h"
#endif


#include <QAction>
#include <QStandardPaths>
#include <QAbstractProxyModel>
#include <QAbstractItemModel>

#include <QQmlExtensionPlugin>
#include <QQmlEngine>

ElisaQmlTestPlugin::ElisaQmlTestPlugin(QObject *aParent)
    : QQmlExtensionPlugin(aParent)
{
}

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

    qmlRegisterType<MediaPlayList>(uri, 1, 0, "MediaPlayList");
    qmlRegisterType<MediaPlayListProxyModel>(uri, 1, 0, "MediaPlayListProxyModel");
    qmlRegisterType<ManageMediaPlayerControl>(uri, 1, 0, "ManageMediaPlayerControl");
    qmlRegisterType<ManageHeaderBar>(uri, 1, 0, "ManageHeaderBar");
    qmlRegisterType<ManageAudioPlayer>(uri, 1, 0, "ManageAudioPlayer");
    qmlRegisterType<ProgressIndicator>(uri, 1, 0, "ProgressIndicator");
    qmlRegisterType<MusicListenersManager>(uri, 1, 0, "MusicListenersManager");
    qmlRegisterType<ViewManager>(uri, 1, 0, "ViewManager");
    qmlRegisterType<ViewsListData>(uri, 1, 0, "ViewsListData");
    qmlRegisterType<ViewConfigurationData>(uri, 1, 0, "ViewConfigurationData");
    qmlRegisterType<DataModel>(uri, 1, 0, "DataModel");
    qmlRegisterType<TrackMetadataModel>(uri, 1, 0, "TrackMetadataModel");
    qmlRegisterType<TrackContextMetaDataModel>(uri, 1, 0, "TrackContextMetaDataModel");
    qmlRegisterType<EditableTrackMetadataModel>(uri, 1, 0, "EditableTrackMetadataModel");
    qmlRegisterType<ViewsModel>(uri, 1, 0, "ViewsModel");
    qmlRegisterType<ViewsProxyModel>(uri, 1, 0, "ViewsProxyModel");
    qmlRegisterType<ViewsListData>(uri, 1, 0, "ViewsListData");
    qmlRegisterType<GridViewProxyModel>(uri, 1, 0, "GridViewProxyModel");

#if defined KF5KIO_FOUND && KF5KIO_FOUND
    qmlRegisterType<FileBrowserModel>(uri, 1, 0, "FileBrowserModel");
    qmlRegisterType<FileBrowserProxyModel>(uri, 1, 0, "FileBrowserProxyModel");
#endif

    qmlRegisterType<AudioWrapper>(uri, 1, 0, "AudioWrapper");
    qmlRegisterUncreatableType<DatabaseInterface>(uri, 1, 0, "DatabaseInterface", QStringLiteral("Only created in c++"));
    qmlRegisterUncreatableType<QAbstractItemModel>(uri, 1, 0, "AbstractItemModel", QStringLiteral("Abstract Qt type"));
    qmlRegisterUncreatableType<QAbstractProxyModel>(uri, 1, 0, "AbstractProxyModel", QStringLiteral("Abstract Qt type"));

#if defined Qt5DBus_FOUND && Qt5DBus_FOUND
    qmlRegisterType<Mpris2>(uri, 1, 0, "Mpris2");
    qRegisterMetaType<MediaPlayer2Player*>();
#endif

    qRegisterMetaType<AbstractMediaProxyModel*>();
    qRegisterMetaType<QHash<QString,QUrl>>("QHash<QString,QUrl>");
    qRegisterMetaType<QHash<QUrl,QDateTime>>("QHash<QUrl,QDateTime>");
    qRegisterMetaType<QVector<qulonglong>>("QVector<qulonglong>");
    qRegisterMetaType<QHash<qulonglong,int>>("QHash<qulonglong,int>");
    qRegisterMetaType<DataTypes::ListTrackDataType>("DataTypes::ListTrackDataType");
    qRegisterMetaType<DataTypes::ListRadioDataType>("DataTypes::ListRadioDataType");
    qRegisterMetaType<DataTypes::ListAlbumDataType>("DataTypes::ListAlbumDataType");
    qRegisterMetaType<DataTypes::ListArtistDataType>("DataTypes::ListArtistDataType");
    qRegisterMetaType<DataTypes::ListGenreDataType>("DataTypes::ListGenreDataType");
    qRegisterMetaType<ModelDataLoader::ListTrackDataType>("ModelDataLoader::ListTrackDataType");
    qRegisterMetaType<ModelDataLoader::ListRadioDataType>("ModelDataLoader::ListRadioDataType");
    qRegisterMetaType<ModelDataLoader::ListAlbumDataType>("ModelDataLoader::ListAlbumDataType");
    qRegisterMetaType<ModelDataLoader::ListArtistDataType>("ModelDataLoader::ListArtistDataType");
    qRegisterMetaType<ModelDataLoader::ListGenreDataType>("ModelDataLoader::ListGenreDataType");
    qRegisterMetaType<ModelDataLoader::AlbumDataType>("ModelDataLoader::AlbumDataType");
    qRegisterMetaType<TracksListener::ListTrackDataType>("TracksListener::ListTrackDataType");
    qRegisterMetaType<QMap<QString, int>>();
    qRegisterMetaType<QAction*>();
    qRegisterMetaType<QMap<QString,int>>("QMap<QString,int>");
    qRegisterMetaType<ElisaUtils::PlayListEnqueueMode>("ElisaUtils::PlayListEnqueueMode");
    qRegisterMetaType<ElisaUtils::PlayListEnqueueTriggerPlay>("ElisaUtils::PlayListEnqueueTriggerPlay");
    qRegisterMetaType<ElisaUtils::PlayListEntryType>("ElisaUtils::PlayListEntryType");
    qRegisterMetaType<DataTypes::EntryData>("DataTypes::EntryData");
    qRegisterMetaType<DataTypes::EntryDataList>("DataTypes::EntryDataList");
    qRegisterMetaType<ElisaUtils::FilterType>("ElisaUtils::FilterType");
    qRegisterMetaType<DataTypes::TrackDataType>("DataTypes::TrackDataType");
    qRegisterMetaType<DataTypes::AlbumDataType>("DataTypes::AlbumDataType");
    qRegisterMetaType<DataTypes::ArtistDataType>("DataTypes::ArtistDataType");
    qRegisterMetaType<DataTypes::GenreDataType>("DataTypes::GenreDataType");
    qRegisterMetaType<DataTypes::ColumnsRoles>("DataTypes::ColumnsRoles");
    qRegisterMetaType<ModelDataLoader::TrackDataType>("ModelDataLoader::TrackDataType");
    qRegisterMetaType<TracksListener::TrackDataType>("TracksListener::TrackDataType");
    qRegisterMetaType<ViewManager::IsTreeModelType>("ViewManager::IsTreeModelType");
    qRegisterMetaType<DataTypes::DataType>("DataTypes::DataType");

    qmlRegisterSingletonType<ElisaConfigurationDialog>(uri, 1, 0, "ElisaConfigurationDialog",
                                                       [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject* {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        return new ElisaConfigurationDialog;
    });

    qmlRegisterSingletonType<ElisaApplication>(uri, 1, 0, "ElisaApplication",
                                                       [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject* {
        Q_UNUSED(scriptEngine)

        auto newApplication = std::make_unique<ElisaApplication>();

#if defined KF5DBusAddons_FOUND && KF5DBusAddons_FOUND
        auto *elisaService = new KDBusService(KDBusService::Unique, newApplication.get());
#endif

#if defined KF5DBusAddons_FOUND && KF5DBusAddons_FOUND
        QObject::connect(elisaService, &KDBusService::activateActionRequested, newApplication.get(), &ElisaApplication::activateActionRequested);
        QObject::connect(elisaService, &KDBusService::activateRequested, newApplication.get(), &ElisaApplication::activateRequested);
        QObject::connect(elisaService, &KDBusService::openRequested, newApplication.get(), &ElisaApplication::openRequested);
#endif

        newApplication->setQmlEngine(engine);

        return newApplication.release();
    });

    qmlRegisterUncreatableMetaObject(ElisaUtils::staticMetaObject, uri, 1, 0, "ElisaUtils", QStringLiteral("Namespace ElisaUtils"));
}
