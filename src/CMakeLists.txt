include_directories(${elisa_BINARY_DIR})

if (Qt5Quick_FOUND AND Qt5Widgets_FOUND)
    set(elisa_SOURCES
        upnpControl.cpp
        mediaplaylist.cpp
        playlistcontroler.cpp
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
        AudioTrackDelegate.qml
        PlayListEntry.qml
        MediaAlbumDelegate.qml
        MediaArtistDelegate.qml
        MediaBrowser.qml
        DraggableItem.qml
        PassiveNotification.qml

        HeaderBar.qml
        NavigationActionBar.qml
        MediaPlayerControl.qml
        ContextView.qml

        MediaArtistAlbumView.qml
        MediaPlayListView.qml
        MediaAlbumView.qml
        MediaAllAlbumView.qml
        MediaAllArtistView.qml
        )

    if (KF5Baloo_FOUND)
        if (Qt5DBus_FOUND)
            set(elisa_SOURCES
                ${elisa_SOURCES}
                baloo/localbaloofilelisting.cpp
                baloo/baloolistener.cpp
                )
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

    if (KF5FileMetaData_FOUND)
        set(elisa_SOURCES
            ${elisa_SOURCES}
            abstractfile/abstractfilelistener.cpp
            abstractfile/abstractfilelisting.cpp
            file/filelistener.cpp
            file/localfilelisting.cpp
        )
    endif()

    qt5_add_resources(elisa_SOURCES upnpControl.qrc)

    add_executable(elisa ${elisa_SOURCES})

    set_target_properties(elisa PROPERTIES LINK_FLAGS "-Wl,--no-undefined")

    target_include_directories(elisa PRIVATE ${KDSoap_INCLUDE_DIRS})

    target_link_libraries(elisa
        LINK_PRIVATE
            Qt5::Quick
            Qt5::Gui
            Qt5::Widgets
            Qt5::Multimedia
            Qt5::Svg
            Qt5::Xml
            Qt5::Sql
            KF5::I18n
    )

    if (Qt5DBus_FOUND)
        target_link_libraries(elisa
            LINK_PRIVATE
                Qt5::DBus
        )
    endif()

    if (Qt5AndroidExtras_FOUND)
        target_link_libraries(elisa
            LINK_PRIVATE
                Qt5::AndroidExtras
        )
    endif()

    if (KF5FileMetaData_FOUND)
        target_link_libraries(elisa
            LINK_PRIVATE
                KF5::FileMetaData
        )
    endif()

    if (KF5CoreAddons_FOUND)
        target_link_libraries(elisa
            LINK_PRIVATE
                KF5::CoreAddons
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

    if (KF5ConfigWidgets_FOUND)
        target_link_libraries(elisa
            LINK_PRIVATE
                KF5::ConfigWidgets
        )
    endif()

    if (KF5Config_FOUND)
        target_link_libraries(elisa
            LINK_PRIVATE
                KF5::ConfigCore
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
        )
    endif()
endif()

install(TARGETS elisa ${INSTALL_TARGETS_DEFAULT_ARGS})
