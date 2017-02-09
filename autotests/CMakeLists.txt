enable_testing()

include_directories(${elisa_BINARY_DIR})

set(playListTest_SOURCES
    ../src/mediaplaylist.cpp
    ../src/playlistcontroler.cpp
    ../src/databaseinterface.cpp
    ../src/musiclistenersmanager.cpp
    ../src/trackslistener.cpp
    ../src/musicartist.cpp
    ../src/musicalbum.cpp
    ../src/musicaudiotrack.cpp
    ../src/file/filelistener.cpp
    ../src/file/localfilelisting.cpp
    playlisttests.cpp
)

if (KF5Baloo_FOUND)
    if (Qt5DBus_FOUND)
        set(playListTest_SOURCES
            ${playListTest_SOURCES}
            ../src/baloo/baloolistener.cpp
            ../src/baloo/localbaloofilelisting.cpp
            )
    endif()
endif()

if (UPNPQT_FOUND)
    set(playListTest_SOURCES
        ${playListTest_SOURCES}
        ../src/upnp/upnpcontrolcontentdirectory.cpp
        ../src/upnp/upnpcontentdirectorymodel.cpp
        ../src/upnp/upnpcontrolconnectionmanager.cpp
        ../src/upnp/upnpcontrolmediaserver.cpp
        ../src/upnp/didlparser.cpp
        ../src/upnp/upnplistener.cpp
        ../src/upnp/upnpdiscoverallmusic.cpp
        )
endif()

add_executable(playListTest ${playListTest_SOURCES})
target_link_libraries(playListTest Qt5::Test Qt5::Core Qt5::Sql KF5::I18n)
if (KF5Baloo_FOUND)
    target_link_libraries(playListTest KF5::Baloo Qt5::DBus)
endif()
if (KF5FileMetaData_FOUND)
    target_link_libraries(playListTest KF5::FileMetaData)
endif()
if (UPNPQT_FOUND)
    target_link_libraries(playListTest Qt5::Xml UPNP::upnpQt)
endif()

target_include_directories(playListTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(playListTest playListTest)

set(databaseInterfaceTest_SOURCES
    ../src/databaseinterface.cpp
    ../src/musicartist.cpp
    ../src/musicalbum.cpp
    ../src/musicaudiotrack.cpp
    databaseinterfacetest.cpp
)

add_executable(databaseInterfaceTest ${databaseInterfaceTest_SOURCES})
target_link_libraries(databaseInterfaceTest Qt5::Test Qt5::Core Qt5::Sql KF5::I18n)
target_include_directories(databaseInterfaceTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(databaseInterfaceTest databaseInterfaceTest)

set(playListControlerTest_SOURCES
    ../src/playlistcontroler.cpp
    ../src/mediaplaylist.cpp
    ../src/databaseinterface.cpp
    ../src/musiclistenersmanager.cpp
    ../src/trackslistener.cpp
    ../src/musicartist.cpp
    ../src/musicalbum.cpp
    ../src/musicaudiotrack.cpp
    ../src/file/filelistener.cpp
    ../src/file/localfilelisting.cpp
    playlistcontrolertest.cpp
)

if (KF5Baloo_FOUND)
    if (Qt5DBus_FOUND)
        set(playListControlerTest_SOURCES
            ${playListControlerTest_SOURCES}
            ../src/baloo/baloolistener.cpp
            ../src/baloo/localbaloofilelisting.cpp
            )
    endif()
endif()

if (UPNPQT_FOUND)
    set(playListControlerTest_SOURCES
        ${playListControlerTest_SOURCES}
        ../src/upnp/upnpcontrolcontentdirectory.cpp
        ../src/upnp/upnpcontentdirectorymodel.cpp
        ../src/upnp/upnpcontrolconnectionmanager.cpp
        ../src/upnp/upnpcontrolmediaserver.cpp
        ../src/upnp/didlparser.cpp
        ../src/upnp/upnplistener.cpp
        ../src/upnp/upnpdiscoverallmusic.cpp
        )
endif()

add_executable(playListControlerTest ${playListControlerTest_SOURCES})

target_link_libraries(playListControlerTest Qt5::Test Qt5::Core Qt5::Sql KF5::I18n)
if (KF5Baloo_FOUND)
    target_link_libraries(playListControlerTest KF5::Baloo Qt5::DBus)
endif()
if (KF5FileMetaData_FOUND)
    target_link_libraries(playListControlerTest KF5::FileMetaData)
endif()
if (UPNPQT_FOUND)
    target_link_libraries(playListControlerTest Qt5::Xml UPNP::upnpQt)
endif()

target_include_directories(playListControlerTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(playListControlerTest playListControlerTest)

set(managemediaplayercontrolTest_SOURCES
    ../src/managemediaplayercontrol.cpp
    ../src/mediaplaylist.cpp
    ../src/databaseinterface.cpp
    ../src/musiclistenersmanager.cpp
    ../src/trackslistener.cpp
    ../src/musicartist.cpp
    ../src/musicalbum.cpp
    ../src/musicaudiotrack.cpp
    ../src/file/filelistener.cpp
    ../src/file/localfilelisting.cpp
    managemediaplayercontroltest.cpp
)

if (KF5Baloo_FOUND)
    if (Qt5DBus_FOUND)
        set(managemediaplayercontrolTest_SOURCES
            ${managemediaplayercontrolTest_SOURCES}
            ../src/baloo/baloolistener.cpp
            ../src/baloo/localbaloofilelisting.cpp
            )
    endif()
endif()

if (UPNPQT_FOUND)
    set(managemediaplayercontrolTest_SOURCES
        ${managemediaplayercontrolTest_SOURCES}
        ../src/upnp/upnpcontrolcontentdirectory.cpp
        ../src/upnp/upnpcontentdirectorymodel.cpp
        ../src/upnp/upnpcontrolconnectionmanager.cpp
        ../src/upnp/upnpcontrolmediaserver.cpp
        ../src/upnp/didlparser.cpp
        ../src/upnp/upnplistener.cpp
        ../src/upnp/upnpdiscoverallmusic.cpp
        )
endif()

add_executable(managemediaplayercontrolTest ${managemediaplayercontrolTest_SOURCES})

target_link_libraries(managemediaplayercontrolTest Qt5::Test Qt5::Core Qt5::Sql KF5::I18n)
if (KF5Baloo_FOUND)
    target_link_libraries(managemediaplayercontrolTest KF5::Baloo Qt5::DBus)
endif()
if (KF5FileMetaData_FOUND)
    target_link_libraries(managemediaplayercontrolTest KF5::FileMetaData)
endif()
if (UPNPQT_FOUND)
    target_link_libraries(managemediaplayercontrolTest Qt5::Xml UPNP::upnpQt)
endif()

target_include_directories(managemediaplayercontrolTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(managemediaplayercontrolTest managemediaplayercontrolTest)

set(manageheaderbarTest_SOURCES
    ../src/manageheaderbar.cpp
    ../src/mediaplaylist.cpp
    ../src/databaseinterface.cpp
    ../src/musiclistenersmanager.cpp
    ../src/trackslistener.cpp
    ../src/trackslistener.cpp
    ../src/musicartist.cpp
    ../src/musicalbum.cpp
    ../src/musicaudiotrack.cpp
    ../src/file/filelistener.cpp
    ../src/file/localfilelisting.cpp
    manageheaderbartest.cpp
)

if (KF5Baloo_FOUND)
    if (Qt5DBus_FOUND)
        set(manageheaderbarTest_SOURCES
            ${manageheaderbarTest_SOURCES}
            ../src/baloo/baloolistener.cpp
            ../src/baloo/localbaloofilelisting.cpp
            )
    endif()
endif()

if (UPNPQT_FOUND)
    set(manageheaderbarTest_SOURCES
        ${manageheaderbarTest_SOURCES}
        ../src/upnp/upnpcontrolcontentdirectory.cpp
        ../src/upnp/upnpcontentdirectorymodel.cpp
        ../src/upnp/upnpcontrolconnectionmanager.cpp
        ../src/upnp/upnpcontrolmediaserver.cpp
        ../src/upnp/didlparser.cpp
        ../src/upnp/upnplistener.cpp
        ../src/upnp/upnpdiscoverallmusic.cpp
        )
endif()

add_executable(manageheaderbarTest ${manageheaderbarTest_SOURCES})

target_link_libraries(manageheaderbarTest Qt5::Test Qt5::Core Qt5::Sql Qt5::Gui KF5::I18n)
if (KF5Baloo_FOUND)
    target_link_libraries(manageheaderbarTest KF5::Baloo Qt5::DBus)
endif()
if (KF5FileMetaData_FOUND)
    target_link_libraries(manageheaderbarTest KF5::FileMetaData)
endif()
if (UPNPQT_FOUND)
    target_link_libraries(manageheaderbarTest Qt5::Xml UPNP::upnpQt)
endif()

target_include_directories(manageheaderbarTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(manageheaderbarTest manageheaderbarTest)

set(manageaudioplayerTest_SOURCES
    ../src/manageaudioplayer.cpp
    manageaudioplayertest.cpp
)

add_executable(manageaudioplayerTest ${manageaudioplayerTest_SOURCES})
target_link_libraries(manageaudioplayerTest Qt5::Test Qt5::Core Qt5::Gui)
target_include_directories(manageaudioplayerTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(manageaudioplayerTest manageaudioplayerTest)

set(mediaplaylistTest_SOURCES
    ../src/mediaplaylist.cpp
    ../src/databaseinterface.cpp
    ../src/trackslistener.cpp
    ../src/musiclistenersmanager.cpp
    ../src/musicartist.cpp
    ../src/musicalbum.cpp
    ../src/musicaudiotrack.cpp
    ../src/file/filelistener.cpp
    ../src/file/localfilelisting.cpp
    mediaplaylisttest.cpp
)

if (KF5Baloo_FOUND)
    if (Qt5DBus_FOUND)
        set(mediaplaylistTest_SOURCES
            ${mediaplaylistTest_SOURCES}
            ../src/baloo/baloolistener.cpp
            ../src/baloo/localbaloofilelisting.cpp
            )
    endif()
endif()

if (UPNPQT_FOUND)
    set(mediaplaylistTest_SOURCES
        ${mediaplaylistTest_SOURCES}
        ../src/upnp/upnpcontrolcontentdirectory.cpp
        ../src/upnp/upnpcontentdirectorymodel.cpp
        ../src/upnp/upnpcontrolconnectionmanager.cpp
        ../src/upnp/upnpcontrolmediaserver.cpp
        ../src/upnp/didlparser.cpp
        ../src/upnp/upnplistener.cpp
        ../src/upnp/upnpdiscoverallmusic.cpp
        )
endif()

add_executable(mediaplaylistTest ${mediaplaylistTest_SOURCES})

target_link_libraries(mediaplaylistTest Qt5::Test Qt5::Core Qt5::Sql Qt5::Gui KF5::I18n)
if (KF5Baloo_FOUND)
    target_link_libraries(mediaplaylistTest KF5::Baloo Qt5::DBus)
endif()
if (KF5FileMetaData_FOUND)
    target_link_libraries(mediaplaylistTest KF5::FileMetaData)
endif()
if (UPNPQT_FOUND)
    target_link_libraries(mediaplaylistTest Qt5::Xml UPNP::upnpQt)
endif()

target_include_directories(mediaplaylistTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(mediaplaylistTest mediaplaylistTest)

set(allalbumsmodeltest_SOURCES
    ../src/databaseinterface.cpp
    ../src/musicartist.cpp
    ../src/musicalbum.cpp
    ../src/musicaudiotrack.cpp
    ../src/allalbumsmodel.cpp
    allalbumsmodeltest.cpp
)

add_executable(allalbumsmodeltest ${allalbumsmodeltest_SOURCES})
target_link_libraries(allalbumsmodeltest Qt5::Test Qt5::Core Qt5::Sql KF5::I18n)
target_include_directories(allalbumsmodeltest PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(allalbumsmodeltest allalbumsmodeltest)

set(albummodeltest_SOURCES
    ../src/databaseinterface.cpp
    ../src/musicartist.cpp
    ../src/musicalbum.cpp
    ../src/musicaudiotrack.cpp
    ../src/albummodel.cpp
    albummodeltest.cpp
)

add_executable(albummodeltest ${albummodeltest_SOURCES})
target_link_libraries(albummodeltest Qt5::Test Qt5::Core Qt5::Sql KF5::I18n)
target_include_directories(albummodeltest PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(albummodeltest albummodeltest)

set(allartistsmodeltest_SOURCES
    ../src/databaseinterface.cpp
    ../src/musicartist.cpp
    ../src/musicalbum.cpp
    ../src/musicaudiotrack.cpp
    ../src/allartistsmodel.cpp
    allartistsmodeltest.cpp
)

add_executable(allartistsmodeltest ${allartistsmodeltest_SOURCES})
target_link_libraries(allartistsmodeltest Qt5::Test Qt5::Core Qt5::Sql KF5::I18n)
target_include_directories(allartistsmodeltest PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(allartistsmodeltest allartistsmodeltest)
