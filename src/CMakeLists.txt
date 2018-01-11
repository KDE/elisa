include_directories(${elisa_BINARY_DIR})

if (Qt5Quick_FOUND AND Qt5Widgets_FOUND)
    set(elisa_SOURCES
        upnpControl.cpp
        mediaplaylist.cpp
        musicstatistics.cpp
        musicalbum.cpp
        musicaudiotrack.cpp
        musicartist.cpp
        progressindicator.cpp
        albummodel.cpp
        allalbumsmodel.cpp
        allartistsmodel.cpp
        databaseinterface.cpp
        musiclistenersmanager.cpp
        managemediaplayercontrol.cpp
        manageheaderbar.cpp
        manageaudioplayer.cpp
        albumfilterproxymodel.cpp
        trackslistener.cpp
        elisaapplication.cpp
        audiowrapper.cpp
        alltracksmodel.cpp
        notificationitem.cpp
        topnotificationmanager.cpp
        elisautils.cpp
        abstractfile/abstractfilelistener.cpp
        abstractfile/abstractfilelisting.cpp
        file/filelistener.cpp
        file/localfilelisting.cpp

        MediaServer.qml
        Theme.qml
        PlatformIntegration.qml
        LabelWithToolTip.qml

        windows/WindowsTheme.qml
        windows/PlatformIntegration.qml

        android/MediaServer.qml
        android/AndroidTheme.qml
        android/PlatformIntegration.qml

        RatingStar.qml
        PlayListEntry.qml
        MediaBrowser.qml
        DraggableItem.qml
        PassiveNotification.qml
        TopNotification.qml
        TopNotificationItem.qml
        TrackImportNotification.qml

        HeaderBar.qml
        NavigationActionBar.qml
        FilterBar.qml
        MediaPlayerControl.qml
        ContextView.qml

        MediaPlayListView.qml
        MediaAlbumView.qml
        MediaAllTracksView.qml
        MediaTrackDelegate.qml
        MediaAlbumTrackDelegate.qml
        GridBrowserView.qml
        GridBrowserDelegate.qml

        ViewSelector.qml
        )

    if (KF5Baloo_FOUND)
        if (Qt5DBus_FOUND)
            set(elisa_SOURCES
                ${elisa_SOURCES}
                baloo/localbaloofilelisting.cpp
                baloo/baloolistener.cpp
                )

            qt5_add_dbus_interface(elisa_SOURCES
                ${BALOO_DBUS_INTERFACES_DIR}/org.kde.baloo.fileindexer.xml
                baloo/fileindexer)

            qt5_add_dbus_interface(elisa_SOURCES
                ${BALOO_DBUS_INTERFACES_DIR}/org.kde.baloo.scheduler.xml
                baloo/scheduler)
        endif()
    endif()

    if (Qt5DBus_FOUND)
        set(elisa_SOURCES
            ${elisa_SOURCES}
            mpris2/mpris2.cpp
            mpris2/mediaplayer2.cpp
            mpris2/mediaplayer2player.cpp
        )
    endif()

    if (UPNPQT_FOUND)
        set(elisa_SOURCES
            ${elisa_SOURCES}
            upnp/upnpcontrolcontentdirectory.cpp
            upnp/upnpcontentdirectorymodel.cpp
            upnp/upnpcontrolconnectionmanager.cpp
            upnp/upnpcontrolmediaserver.cpp
            upnp/didlparser.cpp
            upnp/upnplistener.cpp
            upnp/upnpdiscoverallmusic.cpp
            )
    endif()

    kconfig_add_kcfg_files(elisa_SOURCES elisa_settings.kcfgc)
    set(elisa_SOURCES
        ${elisa_SOURCES}
        elisa_core.kcfg
    )

    qt5_add_resources(elisa_SOURCES upnpControl.qrc)

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

    set_target_properties(elisa PROPERTIES LINK_FLAGS "-Wl,--no-undefined")

    target_include_directories(elisa PRIVATE ${KDSoap_INCLUDE_DIRS})

    target_link_libraries(elisa
        LINK_PRIVATE
            Qt5::Core
            Qt5::Quick
            Qt5::Gui
            Qt5::Widgets
            Qt5::Multimedia
            Qt5::Svg
            Qt5::Sql
            KF5::I18n
            KF5::ConfigCore
            KF5::ConfigGui
            KF5::FileMetaData
            KF5::CoreAddons
    )

    if (Qt5DBus_FOUND)
        target_link_libraries(elisa
            LINK_PRIVATE
                Qt5::DBus
        )
    endif()

    if (KF5KCMUtils_FOUND)
        target_link_libraries(elisa
            LINK_PRIVATE
                KF5::KCMUtils
        )
    endif()

    if (Qt5AndroidExtras_FOUND)
        target_link_libraries(elisa
            LINK_PRIVATE
                Qt5::AndroidExtras
        )
    endif()

    if (KF5Baloo_FOUND)
        target_link_libraries(elisa
            LINK_PRIVATE
                KF5::Baloo
        )
    endif()

    if (KF5Declarative_FOUND)
        target_link_libraries(elisa
            LINK_PRIVATE
                KF5::Declarative
        )
    endif()

    if (KF5XmlGui_FOUND)
        target_link_libraries(elisa
            LINK_PRIVATE
                KF5::XmlGui
        )
    endif()

    if (KF5Crash_FOUND)
        target_link_libraries(elisa
            LINK_PRIVATE
                KF5::Crash
        )
    endif()

    if (UPNPQT_FOUND)
        target_link_libraries(elisa
            LINK_PRIVATE
                UPNP::upnpQt
                Qt5::Network
        )
    endif()

    if (KF5DBusAddons_FOUND)
        target_link_libraries(elisa
            LINK_PRIVATE
                KF5::DBusAddons
        )
    endif()
endif()

install(TARGETS elisa ${INSTALL_TARGETS_DEFAULT_ARGS})

if (KF5ConfigWidgets_FOUND)
    add_subdirectory(localFileConfiguration)
endif()
