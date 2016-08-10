include_directories(${elisa_BINARY_DIR})

if (Qt5Quick_FOUND AND Qt5Widgets_FOUND)
    set(elisa_SOURCES
        upnpControl.cpp
        mediaplaylist.cpp
        playlistcontroler.cpp
        musicstatistics.cpp
        musicalbum.cpp
        musicaudiotrack.cpp
        albumfilterproxymodel.cpp
        progressindicator.cpp
        abstractalbummodel.cpp
        databaseinterface.cpp

        MediaServer.qml
        MediaServerListing.qml
        MediaAlbumView.qml
        RatingStar.qml
        AudioTrackDelegate.qml
        MediaPlayerControl.qml
        MediaServerEntry.qml
        MediaContentDirectory.qml
        MediaPlayListView.qml
        HeaderBar.qml
        MediaServerMobile.qml
        AbstractAudioTrackDelegateMobile.qml
        AudioTrackDelegateMobile.qml
        AlbumTrackDelegateMobile.qml
        MediaServerMobile.qml
        MediaServerListingMobile.qml
        MediaAlbumViewMobile.qml
        MediaPlayerControlMobile.qml
        MediaServerEntryMobile.qml
        MediaContentDirectoryMobile.qml
        MediaServerPageMobile.qml
        MediaPlayPageMobile.qml
        HeaderBarMobile.qml
        ContextView.qml
        DraggableItem.qml
        PassiveNotification.qml
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
            mpris2/mediaplayer2tracklist.cpp
        )
    endif()

    if (UPNPQT_FOUND)
        set(elisa_SOURCES
            ${elisa_SOURCES}
            upnpcontrolcontentdirectory.cpp
            upnpcontentdirectorymodel.cpp
            upnpdevicemodel.cpp
            upnpcontrolconnectionmanager.cpp
            upnpcontrolmediaserver.cpp
            didlparser.cpp
            upnplistener.cpp
            upnpdiscoverallmusic.cpp
            )
    endif()

    if (${KF5Declarative_FOUND})
        qt5_add_resources(elisa_SOURCES upnpControl.qrc)
    else()
        qt5_add_resources(elisa_SOURCES upnpControlMobile.qrc)
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
    )

    if (Qt5DBus_FOUND)
        target_link_libraries(elisa
            LINK_PRIVATE
                Qt5::DBus
        )
    endif()

    if (KF5I18n_FOUND)
        target_link_libraries(elisa
            LINK_PRIVATE
                KF5::I18n
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

    if (KF5Service_FOUND)
        target_link_libraries(elisa
            LINK_PRIVATE
                KF5::Service
        )
    endif()

    if (UPNPQT_FOUND)
        target_link_libraries(elisa
            LINK_PRIVATE
                UPNP::upnpQt
        )
    endif()
endif()

set(playListTest_SOURCES
    mediaplaylist.cpp
    playlistcontroler.cpp
    playlisttests.cpp
)

add_executable(playListTest ${playListTest_SOURCES})
target_link_libraries(playListTest Qt5::Test Qt5::Core)
