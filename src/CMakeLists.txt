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

        MediaServer.qml

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
                baloo/localbalootrack.cpp
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

    if (CMAKE_SYSTEM_NAME STREQUAL Android)
        qt5_add_resources(elisa_SOURCES upnpControlMobile.qrc)
    else()
        qt5_add_resources(elisa_SOURCES upnpControl.qrc)
    endif()

    add_executable(elisa ${elisa_SOURCES})

    set_target_properties(elisa PROPERTIES LINK_FLAGS "-Wl,--no-undefined")

    target_include_directories(elisa PRIVATE ${KDSoap_INCLUDE_DIRS})

    target_link_libraries(elisa
        LINK_PRIVATE
            Qt5::Quick
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

    if (UPNPQT_FOUND)
        target_link_libraries(elisa
            LINK_PRIVATE
                UPNP::upnpQt
        )
    endif()
endif()

if (KF5Kirigami_FOUND)
    kirigami_package_breeze_icons(ICONS media-skip-backward media-skip-forward audio-x-generic folder go-next go-previous list-remove)
endif()
