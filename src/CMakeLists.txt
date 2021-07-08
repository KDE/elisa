# SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
#
# SPDX-License-Identifier: LGPL-3.0-or-later

include_directories(${elisa_BINARY_DIR})

set(elisaLib_SOURCES
    mediaplaylist.cpp
    mediaplaylistproxymodel.cpp
    progressindicator.cpp
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
    models/editabletrackmetadatamodel.cpp
    models/trackcontextmetadatamodel.cpp
    models/viewsmodel.cpp
    models/viewsproxymodel.cpp
    viewslistdata.cpp
    viewconfigurationdata.cpp
    localFileConfiguration/elisaconfigurationdialog.cpp
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

if (ANDROID)
    ecm_qt_declare_logging_category(elisaLib_SOURCES
        HEADER "android/androidcommon.h"
        IDENTIFIER "orgKdeElisaAndroid"
        CATEGORY_NAME "org.kde.elisa.android"
        DEFAULT_SEVERITY Info
        )

    set(elisaLib_SOURCES
        ${elisaLib_SOURCES}
        android/androidmusiclistener.cpp
        android/androidfilelisting.cpp
        )
endif()

if (KF5KIO_FOUND)
    set(elisaLib_SOURCES
        ${elisaLib_SOURCES}
        models/filebrowsermodel.cpp
        models/filebrowserproxymodel.cpp
        )
endif()

if (KF5Baloo_FOUND)
    if (Qt5DBus_FOUND)
        ecm_qt_declare_logging_category(elisaLib_SOURCES
            HEADER "baloo/baloocommon.h"
            IDENTIFIER "orgKdeElisaBaloo"
            CATEGORY_NAME "org.kde.elisa.baloo"
            DEFAULT_SEVERITY Info
            )

        set(elisaLib_SOURCES
            ${elisaLib_SOURCES}
            baloo/localbaloofilelisting.cpp
            baloo/baloolistener.cpp
            baloo/baloodetector.cpp
            )

        qt_add_dbus_interface(elisaLib_SOURCES
            ${BALOO_DBUS_INTERFACES_DIR}/org.kde.baloo.main.xml
            baloo/main)

        qt_add_dbus_interface(elisaLib_SOURCES
            ${BALOO_DBUS_INTERFACES_DIR}/org.kde.baloo.fileindexer.xml
            baloo/fileindexer)

        qt_add_dbus_interface(elisaLib_SOURCES
            ${BALOO_DBUS_INTERFACES_DIR}/org.kde.baloo.scheduler.xml
            baloo/scheduler)

        qt_add_dbus_adaptor(elisaLib_SOURCES
            ${BALOO_DBUS_INTERFACES_DIR}/org.kde.BalooWatcherApplication.xml
            baloo/localbaloofilelisting.h LocalBalooFileListing)
    endif()
endif()

if (Qt5DBus_FOUND)
    set(elisaLib_SOURCES
        ${elisaLib_SOURCES}
        mpris2/mpris2.cpp
        mpris2/mediaplayer2.cpp
        mpris2/mediaplayer2player.cpp
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
endif()

if (KF5Baloo_FOUND)
    if (Qt5DBus_FOUND)
        qt_add_dbus_interface(elisaLib_SOURCES
            ${BALOO_DBUS_INTERFACES_DIR}/org.kde.baloo.fileindexer.xml
            baloo/fileindexer)

        qt_add_dbus_interface(elisaLib_SOURCES
            ${BALOO_DBUS_INTERFACES_DIR}/org.kde.baloo.scheduler.xml
            baloo/scheduler)

        set(elisaLib_SOURCES
            ${elisaLib_SOURCES}
            ../src/baloo/baloolistener.cpp
            ../src/baloo/localbaloofilelisting.cpp
            )
    endif()
endif()

kconfig_add_kcfg_files(elisaLib_SOURCES ../src/elisa_settings.kcfgc GENERATE_MOC )
set(elisaLib_SOURCES
    ${elisaLib_SOURCES}
    ../src/elisa_core.kcfg
    )

add_library(elisaLib ${elisaLib_SOURCES})
target_link_libraries(elisaLib
    LINK_PUBLIC
    Qt::Multimedia
    LINK_PRIVATE
    Qt::Core Qt::Sql Qt::Widgets Qt::Concurrent Qt::Qml
    KF5::I18n KF5::CoreAddons
    KF5::ConfigCore KF5::ConfigGui)

if (KF5FileMetaData_FOUND)
    target_link_libraries(elisaLib
        LINK_PRIVATE
        KF5::FileMetaData
        )
endif()

if (KF5KIO_FOUND)
    target_link_libraries(elisaLib
        LINK_PUBLIC
        KF5::KIOCore KF5::KIOFileWidgets KF5::KIOWidgets
        )
endif()

if (KF5XmlGui_FOUND)
    target_link_libraries(elisaLib
        LINK_PUBLIC
        KF5::XmlGui
        )
endif()

if (KF5ConfigWidgets_FOUND)
    target_link_libraries(elisaLib
        LINK_PUBLIC
        KF5::ConfigWidgets
        )
endif()

if (KF5Baloo_FOUND)
    if (Qt5DBus_FOUND)
        target_link_libraries(elisaLib
            LINK_PUBLIC
            KF5::Baloo
            )
    endif()
endif()

if (Qt5DBus_FOUND)
    target_link_libraries(elisaLib
        LINK_PUBLIC
        Qt::DBus
        )

    if (KF5DBusAddons_FOUND)
        target_link_libraries(elisaLib
            LINK_PUBLIC
            KF5::DBusAddons
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
    target_link_libraries(elisaLib
        LINK_PUBLIC
        Qt::AndroidExtras
        )
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
    install(TARGETS elisaLib ${INSTALL_TARGETS_DEFAULT_ARGS})
endif()

set(elisaqmlplugin_SOURCES
    elisaqmlplugin.cpp
    elisautils.cpp
)

if (KF5FileMetaData_FOUND)
    set(elisaqmlplugin_SOURCES
        ${elisaqmlplugin_SOURCES}
        embeddedcoverageimageprovider.cpp
    )
endif()

add_library(elisaqmlplugin SHARED ${elisaqmlplugin_SOURCES})
target_link_libraries(elisaqmlplugin
    LINK_PRIVATE
    Qt::Quick Qt::Widgets
    KF5::ConfigCore KF5::ConfigGui
    elisaLib
    )

if (KF5FileMetaData_FOUND)
    target_link_libraries(elisaqmlplugin
        LINK_PRIVATE
        KF5::FileMetaData
        )
endif()

set_target_properties(elisaqmlplugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/org/kde/elisa
    )

if (NOT APPLE AND NOT WIN32)
    set_target_properties(elisaqmlplugin PROPERTIES
        INSTALL_RPATH "${KDE_INSTALL_FULL_LIBDIR}/elisa;${CMAKE_INSTALL_RPATH}"
        )
endif()

install(TARGETS elisaqmlplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/elisa/)

install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/elisa)

add_custom_target(copy)
add_custom_target(copy2)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/org/kde/elisa)

add_custom_command(TARGET copy PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E
    copy ${CMAKE_CURRENT_SOURCE_DIR}/qmldir ${CMAKE_BINARY_DIR}/bin/org/kde/elisa/)

add_custom_command(TARGET copy2 PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E
    copy ${CMAKE_CURRENT_SOURCE_DIR}/plugins.qmltypes ${CMAKE_BINARY_DIR}/bin/org/kde/elisa/)

add_dependencies(elisaqmlplugin copy copy2)

ecm_generate_qmltypes(org.kde.elisa 1.0
                      DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/elisa)

if (Qt5Quick_FOUND AND Qt5Widgets_FOUND)
    set(elisa_SOURCES
        main.cpp
        elisaarguments.cpp
        elisaarguments.h

        windows/WindowsTheme.qml
        windows/PlatformIntegration.qml

        android/PlatformIntegration.qml

        qml/ElisaMainWindow.qml
        qml/ApplicationMenu.qml
        qml/NativeGlobalMenu.qml
        qml/NativeTrayMenu.qml
        qml/NativeMenuItemFromAction.qml
        qml/BaseTheme.qml
        qml/Theme.qml
        qml/PlatformIntegration.qml
        qml/LabelWithToolTip.qml

        qml/RatingStar.qml
        qml/DraggableItem.qml
        qml/TrackImportNotification.qml
        qml/ImageWithFallback.qml

        qml/HeaderBar.qml
        qml/NavigationActionBar.qml
        qml/SortMenu.qml
        qml/MediaPlayerControl.qml
        qml/ContextView.qml
        qml/ContentView.qml
        qml/ViewSelector.qml
        qml/ViewSelectorDelegate.qml
        qml/DataGridView.qml
        qml/DataListView.qml
        qml/DurationSlider.qml
        qml/VolumeSlider.qml

        qml/MediaPlayListView.qml
        qml/PlayListEntry.qml
        qml/SimplePlayListView.qml
        qml/BasicPlayListAlbumHeader.qml

        qml/MediaTrackMetadataDelegate.qml
        qml/MediaTrackMetadataForm.qml
        qml/TracksDiscHeader.qml
        qml/MediaTrackMetadataView.qml
        qml/GridBrowserView.qml
        qml/GridBrowserDelegate.qml
        qml/ListBrowserView.qml
        qml/ListBrowserDelegate.qml
        qml/FlatButtonWithToolTip.qml
        qml/HeaderFooterToolbar.qml

        qml/SettingsForm.qml
        qml/ElisaConfigurationDialog.qml

        qml/mobile/MobileContextMenuEntry.qml
        qml/mobile/MobileContextMenuSheet.qml
        qml/mobile/MobileFooterBar.qml
        qml/mobile/MobileMediaTrackMetadataView.qml
        qml/mobile/MobileMinimizedPlayerControl.qml
        qml/mobile/MobilePlayListDelegate.qml
        qml/mobile/MobileSettingsPage.qml
        qml/mobile/MobileSidebar.qml
        qml/mobile/MobileTrackPlayer.qml
        qml/mobile/MobileVolumeButton.qml
    )

    qt_add_resources(elisa_SOURCES resources.qrc)
    set_property(SOURCE qrc_resources.cpp PROPERTY SKIP_AUTOMOC ON)

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
        KF5::I18n KF5::CoreAddons KF5::ConfigCore KF5::ConfigGui
        KF5::IconThemes
        )

    if (ANDROID)
        target_link_libraries(elisa
            PRIVATE
            Qt::AndroidExtras Qt::Svg Qt::Sql Qt::Concurrent KF5::Kirigami2
        )
    endif()

    if (KF5Crash_FOUND)
        target_link_libraries(elisa
            LINK_PRIVATE
            KF5::Crash
            )
    endif()

    if (KF5Declarative_FOUND)
        target_link_libraries(elisa
            LINK_PRIVATE
            KF5::Declarative
            KF5::QuickAddons
            )
    endif()

    if (NOT APPLE AND NOT WIN32)
        set_target_properties(elisa PROPERTIES
            INSTALL_RPATH "${KDE_INSTALL_FULL_LIBDIR}/elisa;${CMAKE_INSTALL_RPATH}"
            )
    endif()

    install(TARGETS elisa ${INSTALL_TARGETS_DEFAULT_ARGS})
endif()

set(elisaImport_SOURCES
    elisaimport.cpp
    elisaimportapplication.cpp
    )

add_executable(elisaImport ${elisaImport_SOURCES})

target_link_libraries(elisaImport
    LINK_PRIVATE
    KF5::ConfigCore KF5::ConfigGui
    elisaLib
    )

if (KF5FileMetaData_FOUND)
    target_link_libraries(elisaImport
        LINK_PRIVATE
        KF5::FileMetaData
        )
endif()

set(QML_IMPORT_PATH ${CMAKE_BINARY_DIR}/bin CACHE INTERNAL "qml import path" FORCE)

if (ANDROID)
    kirigami_package_breeze_icons(ICONS
        elisa
        application-menu
        system-search
        overflow-menu
        draw-arrow-down
        draw-arrow-up
        media-playback-pause
        media-playback-start
        dialog-close
        arrow-down
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
        media-playlist-shuffle
        media-playlist-normal
        media-playlist-play
        media-repeat-none
        media-repeat-single
        media-repeat-all
        media-optical-audio
        player-volume-muted
        player-volume
        delete
        go-next
        go-previous
        search
        error
        documentinfo
        folder
        radio
        rating-unrated
        rating
        media-playback-playing
        media-playback-paused
        settings-configure
    )
endif()
