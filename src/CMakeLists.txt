include_directories(${elisa_BINARY_DIR})

if (Qt5Quick_FOUND AND Qt5Widgets_FOUND)
    set(upnpControl_SOURCES
        upnpControl.cpp
        mediaplaylist.cpp
        playlistcontroler.cpp
        viewpagesmodel.cpp
        remoteserverentry.cpp
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
        AudioTrackDelegateMobile.qml
        MediaServerMobile.qml
        MediaServerListingMobile.qml
        MediaAlbumViewMobile.qml
        MediaPlayerControlMobile.qml
        MediaServerEntryMobile.qml
        MediaContentDirectoryMobile.qml
        MediaServerPageMobile.qml
        MediaPlayPageMobile.qml
        upnpalbummodel.cpp
    )

    qt5_add_resources(upnpControl_SOURCES upnpControl.qrc)

    add_executable(upnpControl ${upnpControl_SOURCES})

    target_include_directories(upnpControl PRIVATE ${KDSoap_INCLUDE_DIRS})

    target_link_libraries(upnpControl
        LINK_PRIVATE
            Qt5::Quick
            Qt5::Widgets
            Qt5::Multimedia
            Qt5::Svg
            Qt5::Xml
            UPNP::upnpQt
    )

    if (${KF5Declarative_FOUND})
    target_link_libraries(upnpControl
        LINK_PRIVATE
            KF5::Declarative
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
