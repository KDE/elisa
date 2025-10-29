# SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
#
# SPDX-License-Identifier: LGPL-3.0-or-later

include_directories(${elisa_BINARY_DIR})

set(elisaLib_SOURCES
    colorschemepreviewimageprovider.cpp
    mediaplaylist.cpp
    mediaplaylistproxymodel.cpp
    progressindicator.cpp
    qmlforeigntypes.h
    databaseinterface.cpp
    datatypes.cpp
    musiclistenersmanager.cpp
    managemediaplayercontrol.cpp
    manageheaderbar.cpp
    manageaudioplayer.cpp
    trackslistener.cpp
    elisaapplication.cpp
    modeldataloader.cpp
    elisautils.cpp
    abstractfile/abstractfilelistener.cpp
    abstractfile/abstractfilelisting.cpp
    filescanner.cpp
    filewriter.cpp
    viewmanager.cpp
    powermanagementinterface.cpp
    file/filelistener.cpp
    file/localfilelisting.cpp
    models/datamodel.cpp
    models/abstractmediaproxymodel.cpp
    models/gridviewproxymodel.cpp
    models/trackmetadatamodel.cpp
    models/trackmetadataproxymodel.cpp
    models/editabletrackmetadatamodel.cpp
    models/trackcontextmetadatamodel.cpp
    models/viewsmodel.cpp
    models/viewsproxymodel.cpp
    models/lyricsmodel.cpp
    viewslistdata.cpp
    viewconfigurationdata.cpp
    localFileConfiguration/elisaconfigurationdialog.cpp
)

set(elisaLib_INCLUDEDIRS
    ${CMAKE_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/abstractfile
    ${CMAKE_CURRENT_SOURCE_DIR}/file
    ${CMAKE_CURRENT_SOURCE_DIR}/localFileConfiguration
    ${CMAKE_CURRENT_SOURCE_DIR}/models
)

ecm_qt_declare_logging_category(elisaLib_SOURCES
    HEADER "indexersManager.h"
    IDENTIFIER "orgKdeElisaIndexersManager"
    CATEGORY_NAME "org.kde.elisa.indexers.manager"
    DEFAULT_SEVERITY Info
    )

ecm_qt_declare_logging_category(elisaLib_SOURCES
    HEADER "databaseLogging.h"
    IDENTIFIER "orgKdeElisaDatabase"
    CATEGORY_NAME "org.kde.elisa.database"
    DEFAULT_SEVERITY Info
    )

ecm_qt_declare_logging_category(elisaLib_SOURCES
    HEADER "abstractfile/indexercommon.h"
    IDENTIFIER "orgKdeElisaIndexer"
    CATEGORY_NAME "org.kde.elisa.indexer"
    DEFAULT_SEVERITY Info
    )

ecm_qt_declare_logging_category(elisaLib_SOURCES
    HEADER "models/modelLogging.h"
    IDENTIFIER "orgKdeElisaModel"
    CATEGORY_NAME "org.kde.elisa.model"
    DEFAULT_SEVERITY Info
    )

ecm_qt_declare_logging_category(elisaLib_SOURCES
    HEADER "lyricsLogging.h"
    IDENTIFIER "orgKdeElisaLyrics"
    CATEGORY_NAME "org.kde.elisa.lyrics"
    DEFAULT_SEVERITY Info
    )

ecm_qt_declare_logging_category(elisaLib_SOURCES
    HEADER "playListLogging.h"
    IDENTIFIER "orgKdeElisaPlayList"
    CATEGORY_NAME "org.kde.elisa.playlist"
    DEFAULT_SEVERITY Info
    )

ecm_qt_declare_logging_category(elisaLib_SOURCES
    HEADER "viewsLogging.h"
    IDENTIFIER "orgKdeElisaViews"
    CATEGORY_NAME "org.kde.elisa.views"
    DEFAULT_SEVERITY Info
    )

ecm_qt_declare_logging_category(elisaLib_SOURCES
    HEADER "playerLogging.h"
    IDENTIFIER "orgKdeElisaPlayer"
    CATEGORY_NAME "org.kde.elisa.player"
    DEFAULT_SEVERITY Info
    )

ecm_qt_declare_logging_category(elisaLib_SOURCES
    HEADER "powerManagementLogging.h"
    IDENTIFIER "orgKdeElisaPowerManagement"
    CATEGORY_NAME "org.kde.elisa.powermanagement"
    DEFAULT_SEVERITY Info
    )

if (LIBVLC_FOUND)
    ecm_qt_declare_logging_category(elisaLib_SOURCES
        HEADER "vlcLogging.h"
        IDENTIFIER "orgKdeElisaPlayerVlc"
        CATEGORY_NAME "org.kde.elisa.player.vlc"
        DEFAULT_SEVERITY Info
        )

    set(elisaLib_SOURCES
        ${elisaLib_SOURCES}
        audiowrapper_libvlc.cpp
        )
else()
    ecm_qt_declare_logging_category(elisaLib_SOURCES
        HEADER "qtMultimediaLogging.h"
        IDENTIFIER "orgKdeElisaPlayerQtMultimedia"
        CATEGORY_NAME "org.kde.elisa.player.qtMultimedia"
        DEFAULT_SEVERITY Info
        )

    set(elisaLib_SOURCES
        ${elisaLib_SOURCES}
        audiowrapper_qtmultimedia.cpp
        )
endif()

if (WIN32)
    ecm_qt_declare_logging_category(elisaLib_SOURCES
        HEADER "windowsLogging.h"
        IDENTIFIER "orgKdeElisaWindows"
        CATEGORY_NAME "org.kde.elisa.windows"
        DEFAULT_SEVERITY Info
        )

    set(elisaLib_SOURCES
        ${elisaLib_SOURCES}
        windows/taskbarmanager.cpp
    )
    set(elisaLib_INCLUDEDIRS
        ${elisaLib_INCLUDEDIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/windows
    )
endif()

if (ANDROID)
    ecm_qt_declare_logging_category(elisaLib_SOURCES
        HEADER "android/androidcommon.h"
        IDENTIFIER "orgKdeElisaAndroid"
        CATEGORY_NAME "org.kde.elisa.android"
        DEFAULT_SEVERITY Info
        )

    set(elisaLib_SOURCES
        ${elisaLib_SOURCES}
        android/androidcoverimageprovider.cpp
        android/androidmusiclistener.cpp
        android/androidfilelisting.cpp
        android/androidplayer.cpp
        android/androidplayerjni.cpp
        )
    set(elisaLib_INCLUDEDIRS
        ${elisaLib_INCLUDEDIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/android
    )
endif()

if (KF6FileMetaData_FOUND)
    set(elisaLib_SOURCES
        ${elisaLib_SOURCES}
        embeddedcoverageimageprovider.cpp
    )
endif()

if (KF6KIO_FOUND)
    set(elisaLib_SOURCES
        ${elisaLib_SOURCES}
        models/filebrowsermodel.cpp
        models/filebrowserproxymodel.cpp
        )
endif()

if (Qt6DBus_FOUND)
    set(elisaLib_SOURCES
        ${elisaLib_SOURCES}
        mpris2/mpris2.cpp
        mpris2/mediaplayer2.cpp
        mpris2/mediaplayer2player.cpp
        )
    set(elisaLib_INCLUDEDIRS
        ${elisaLib_INCLUDEDIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/mpris2
    )
endif()

if (UPNPQT_FOUND)
    set(elisaLib_SOURCES
        ${elisaLib_SOURCES}
        upnp/upnpcontrolcontentdirectory.cpp
        upnp/upnpcontentdirectorymodel.cpp
        upnp/upnpcontrolconnectionmanager.cpp
        upnp/upnpcontrolmediaserver.cpp
        upnp/didlparser.cpp
        upnp/upnplistener.cpp
        upnp/upnpdiscoverallmusic.cpp
        )
    set(elisaLib_INCLUDEDIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/upnp
    )
endif()

kconfig_add_kcfg_files(elisaLib_SOURCES ../src/elisa_settings.kcfgc GENERATE_MOC )
set(elisaLib_SOURCES
    ${elisaLib_SOURCES}
    ../src/elisa_core.kcfg
    )

add_library(elisaLib ${elisaLib_SOURCES})
target_link_libraries(elisaLib
    LINK_PUBLIC
    Qt::Multimedia Qt::Qml KF6::ConfigGui
    LINK_PRIVATE
    Qt::Core Qt::Quick Qt::Sql Qt::Widgets Qt::Concurrent
    KF6::I18n KF6::CoreAddons
    KF6::ConfigCore
    KF6::ColorScheme
    KF6::Codecs)

if (KF6FileMetaData_FOUND)
    target_link_libraries(elisaLib
        LINK_PRIVATE
        KF6::FileMetaData
        )
endif()

if (KF6KIO_FOUND)
    target_link_libraries(elisaLib
        LINK_PUBLIC
        KF6::KIOCore KF6::KIOFileWidgets KF6::KIOWidgets
        )
endif()

if (KF6XmlGui_FOUND)
    target_link_libraries(elisaLib
        LINK_PUBLIC
        KF6::XmlGui
        )
endif()

if (Qt6DBus_FOUND)
    target_link_libraries(elisaLib
        LINK_PUBLIC
        Qt::DBus
        )

    if (KF6DBusAddons_FOUND)
        target_link_libraries(elisaLib
            LINK_PUBLIC
            KF6::DBusAddons
            )
    endif()
endif()

if (LIBVLC_FOUND)
    target_include_directories(elisaLib
        PRIVATE
        ${LIBVLC_INCLUDE_DIR}
        )

    target_link_libraries(elisaLib
        LINK_PRIVATE
        ${LIBVLC_LIBRARY}
        )
endif()

if (ANDROID)
    target_link_libraries(elisaLib PRIVATE Qt6::CorePrivate Qt6::Quick jnigraphics)
endif()

generate_export_header(elisaLib BASE_NAME ElisaLib EXPORT_FILE_NAME elisaLib_export.h)

set_target_properties(elisaLib PROPERTIES
    VERSION 0.1
    SOVERSION 0
    EXPORT_NAME ElisaLib
    )

if (NOT APPLE AND NOT WIN32)
    install(TARGETS elisaLib
        LIBRARY DESTINATION ${KDE_INSTALL_FULL_LIBDIR}/elisa NAMELINK_SKIP
        RUNTIME DESTINATION ${KDE_INSTALL_FULL_LIBDIR}/elisa
        BUNDLE DESTINATION ${KDE_INSTALL_FULL_LIBDIR}/elisa
        )
else()
    install(TARGETS elisaLib ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
endif()

target_include_directories(elisaLib PRIVATE ${elisaLib_INCLUDEDIRS})

set_source_files_properties(windows/PlatformIntegration.qml PROPERTIES
    QT_RESOURCE_ALIAS qml/+windows/PlatformIntegration.qml
)

set_source_files_properties(android/PlatformIntegration.qml PROPERTIES
    QT_RESOURCE_ALIAS qml/+android/PlatformIntegration.qml
)

set_source_files_properties(android/qtquickcontrols2.conf PROPERTIES
    QT_RESOURCE_ALIAS qml/+android/qtquickcontrols2.conf
)

set_source_files_properties(qml/Theme.qml
    PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_policy(SET QTP0001 NEW)
qt_add_qml_module(elisaLib
    URI org.kde.elisa
    QML_FILES
        android/PlatformIntegration.qml

        windows/PlatformIntegration.qml

        qml/AbstractBrowserDelegate.qml
        qml/AbstractDataView.qml
        qml/AccessibleSlider.qml
        qml/ApplicationMenu.qml
        qml/Theme.qml
        qml/BasicPlayListAlbumHeader.qml
        qml/ButtonWithToolTip.qml
        qml/ConfigurationDialog.qml
        qml/ContentView.qml
        qml/ContextView.qml
        qml/DataGridView.qml
        qml/DataTrackView.qml
        qml/DurationSlider.qml
        qml/ElisaMainWindow.qml
        qml/FlatButtonWithToolTip.qml
        qml/FooterBar.qml
        qml/GridBrowserDelegate.qml
        qml/HeaderBar.qml
        qml/ImageWithFallback.qml
        qml/LabelWithToolTip.qml
        qml/ListBrowserDelegate.qml
        qml/MediaPlayerControl.qml
        qml/MediaPlayListView.qml
        qml/MediaTrackMetadataDelegate.qml
        qml/MediaTrackMetadataForm.qml
        qml/MediaTrackMetadataView.qml
        qml/NativeGlobalMenu.qml
        qml/NativeGlobalMenuPlaylistModeItem.qml
        qml/NativeGlobalMenuShuffleModeItem.qml
        qml/NativeMenuItemFromAction.qml
        qml/NativeTrayMenu.qml
        qml/NavigationActionBar.qml
        qml/PlatformIntegration.qml
        qml/PlayListEntry.qml
        qml/PlaylistModeItem.qml
        qml/RatingStar.qml
        qml/SettingsForm.qml
        qml/ShuffleModeItem.qml
        qml/SimplePlayListView.qml
        qml/SortMenu.qml
        qml/TrackBrowserDelegate.qml
        qml/TrackImportNotification.qml
        qml/TracksDiscHeader.qml
        qml/ViewSelector.qml
        qml/VolumeSlider.qml

        qml/mobile/MobileFooterBar.qml
        qml/mobile/MobileMediaTrackMetadataView.qml
        qml/mobile/MobileMinimizedPlayerControl.qml
        qml/mobile/MobilePlayListDelegate.qml
        qml/mobile/MobileSettingsPage.qml
        qml/mobile/MobileSidebar.qml
        qml/mobile/MobileTrackPlayer.qml
        qml/mobile/MobileVolumeButton.qml

        qml/shared/BasePlayerControl.qml
        qml/shared/BasePlayListDelegate.qml
    RESOURCES
        background.png
        qtquickcontrols2.conf
    )

if (Qt6Quick_FOUND AND Qt6Widgets_FOUND)

    set(elisa_SOURCES
        main.cpp
    )

    set(elisa_ICONS_PNG
        ../icons/128-apps-elisa.png
        ../icons/64-apps-elisa.png
        ../icons/48-apps-elisa.png
        ../icons/32-apps-elisa.png
        ../icons/22-apps-elisa.png
        ../icons/16-apps-elisa.png
        )

    # add icons to application sources, to have them bundled
    ecm_add_app_icon(elisa_SOURCES ICONS ${elisa_ICONS_PNG})

    add_executable(elisa ${elisa_SOURCES})

    target_include_directories(elisa PRIVATE ${KDSoap_INCLUDE_DIRS})

    target_link_libraries(elisa
        LINK_PRIVATE
        elisaLib
        Qt::Widgets Qt::QuickControls2
        KF6::I18n KF6::CoreAddons KF6::ConfigCore KF6::ConfigGui
        KF6::IconThemes
        )

    if (ANDROID)
        target_link_libraries(elisa
            PRIVATE
            Qt::Svg Qt::Sql Qt::Concurrent KF6::Kirigami Qt6::CorePrivate
        )
    endif()

    if (KF6Crash_FOUND)
        target_link_libraries(elisa
            LINK_PRIVATE
            KF6::Crash
            )
    endif()

    if (NOT APPLE AND NOT WIN32)
        set_target_properties(elisa PROPERTIES
            INSTALL_RPATH "${KDE_INSTALL_FULL_LIBDIR}/elisa;${CMAKE_INSTALL_RPATH}"
            )
    endif()

    install(TARGETS elisa ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
endif()

set(elisaImport_SOURCES
    elisaimport.cpp
    elisaimportapplication.cpp
    )

add_executable(elisaImport ${elisaImport_SOURCES})

target_link_libraries(elisaImport
    LINK_PRIVATE
    KF6::ConfigCore KF6::ConfigGui
    elisaLib
    )

if (KF6FileMetaData_FOUND)
    target_link_libraries(elisaImport
        LINK_PRIVATE
        KF6::FileMetaData
        )
endif()

set(QML_IMPORT_PATH ${CMAKE_BINARY_DIR}/bin CACHE INTERNAL "qml import path" FORCE)

if (ANDROID)
    kirigami_package_breeze_icons(ICONS
        elisa
        open-menu-symbolic
        system-search
        overflow-menu
        draw-arrow-down
        draw-arrow-up
        media-playback-pause
        media-playback-start
        dialog-close
        arrow-down
        view-list-details-symbolic
        view-list-icons-symbolic
        view-media-playlist
        view-media-artist
        view-right-close
        view-refresh
        view-media-track
        view-media-album-cover
        view-media-lyrics
        view-media-playcount
        view-media-genre
        dialog-ok-apply
        dialog-cancel
        help-about
        list-add
        view-more-symbolic
        edit-redo
        media-track-show-active
        document-save
        document-open
        document-edit
        document-open-folder
        edit-clear-all
        edit-clear
        media-skip-forward
        media-skip-backward
        media-playlist-append-next
        media-playlist-shuffle
        media-playlist-no-shuffle-symbolic
        media-random-albums-amarok
        media-playlist-play
        media-repeat-none
        media-repeat-single
        media-repeat-all
        media-default-album
        audio-volume-muted
        audio-volume-high
        delete
        go-next
        go-previous
        search
        error
        documentinfo
        folder
        radio
        non-starred
        starred
        media-playback-playing
        media-playback-paused
        settings-configure
        view-sort
    )
    ecm_add_android_apk(elisa ANDROID_DIR ${CMAKE_SOURCE_DIR}/android)
endif()
