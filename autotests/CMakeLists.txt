enable_testing()

include_directories(${elisa_CURRENT_BINARY_DIR})
include_directories(${elisa_BINARY_DIR})
include_directories(${elisa_BINARY_DIR}/src)

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
    ../src/elisaapplication.cpp
    ../src/notificationitem.cpp
    ../src/trackslistener.cpp
    ../src/musicartist.cpp
    ../src/musicalbum.cpp
    ../src/musicaudiotrack.cpp
    playlistcontrolertest.cpp
)

if (KF5Baloo_FOUND)
    if (Qt5DBus_FOUND)
        qt5_add_dbus_interface(playListControlerTest_SOURCES
            ${BALOO_DBUS_INTERFACES_DIR}/org.kde.baloo.fileindexer.xml
            baloo/fileindexer)

        qt5_add_dbus_interface(playListControlerTest_SOURCES
            ${BALOO_DBUS_INTERFACES_DIR}/org.kde.baloo.scheduler.xml
            baloo/scheduler)

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

if (KF5FileMetaData_FOUND)
    set(playListControlerTest_SOURCES
        ${playListControlerTest_SOURCES}
        ../src/file/filelistener.cpp
        ../src/file/localfilelisting.cpp
        ../src/abstractfile/abstractfilelistener.cpp
        ../src/abstractfile/abstractfilelisting.cpp
    )
endif()

if (KF5Config_FOUND)
    kconfig_add_kcfg_files(playListControlerTest_SOURCES ../src/elisa_settings.kcfgc )
    set(playListControlerTest_SOURCES
        ${playListControlerTest_SOURCES}
        ../src/elisa_core.kcfg
    )
endif()

add_executable(playListControlerTest ${playListControlerTest_SOURCES})

target_link_libraries(playListControlerTest Qt5::Test Qt5::Core Qt5::Sql Qt5::Widgets KF5::I18n Qt5::Quick Qt5::Multimedia)
if (KF5Baloo_FOUND)
    target_link_libraries(playListControlerTest KF5::Baloo Qt5::DBus)
endif()
if (KF5Config_FOUND)
    target_link_libraries(playListControlerTest KF5::ConfigCore KF5::ConfigGui)
endif()
if (KF5KCMUtils_FOUND)
    target_link_libraries(playListControlerTest KF5::KCMUtils)
endif()
if (KF5FileMetaData_FOUND)
    target_link_libraries(playListControlerTest KF5::FileMetaData)
endif()
if (UPNPQT_FOUND)
    target_link_libraries(playListControlerTest Qt5::Xml UPNP::upnpQt Qt5::Network)
endif()
if (KF5XmlGui_FOUND)
    target_link_libraries(playListControlerTest KF5::XmlGui)
endif()

target_include_directories(playListControlerTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(playListControlerTest playListControlerTest)

set(managemediaplayercontrolTest_SOURCES
    ../src/managemediaplayercontrol.cpp
    ../src/mediaplaylist.cpp
    ../src/databaseinterface.cpp
    ../src/musiclistenersmanager.cpp
    ../src/elisaapplication.cpp
    ../src/notificationitem.cpp
    ../src/trackslistener.cpp
    ../src/musicartist.cpp
    ../src/musicalbum.cpp
    ../src/musicaudiotrack.cpp
    managemediaplayercontroltest.cpp
)

if (KF5Baloo_FOUND)
    if (Qt5DBus_FOUND)
        qt5_add_dbus_interface(managemediaplayercontrolTest_SOURCES
            ${BALOO_DBUS_INTERFACES_DIR}/org.kde.baloo.fileindexer.xml
            baloo/fileindexer)

        qt5_add_dbus_interface(managemediaplayercontrolTest_SOURCES
            ${BALOO_DBUS_INTERFACES_DIR}/org.kde.baloo.scheduler.xml
            baloo/scheduler)

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

if (KF5FileMetaData_FOUND)
    set(managemediaplayercontrolTest_SOURCES
        ${managemediaplayercontrolTest_SOURCES}
        ../src/file/filelistener.cpp
        ../src/file/localfilelisting.cpp
        ../src/abstractfile/abstractfilelistener.cpp
        ../src/abstractfile/abstractfilelisting.cpp
    )
endif()

if (KF5Config_FOUND)
    kconfig_add_kcfg_files(managemediaplayercontrolTest_SOURCES ../src/elisa_settings.kcfgc )
    set(managemediaplayercontrolTest_SOURCES
        ${managemediaplayercontrolTest_SOURCES}
        ../src/elisa_core.kcfg
    )
endif()

add_executable(managemediaplayercontrolTest ${managemediaplayercontrolTest_SOURCES})

target_link_libraries(managemediaplayercontrolTest Qt5::Test Qt5::Core Qt5::Sql Qt5::Widgets KF5::I18n Qt5::Quick Qt5::Multimedia)
if (KF5Baloo_FOUND)
    target_link_libraries(managemediaplayercontrolTest KF5::Baloo Qt5::DBus)
endif()
if (KF5Config_FOUND)
    target_link_libraries(managemediaplayercontrolTest KF5::ConfigCore KF5::ConfigGui)
endif()
if (KF5KCMUtils_FOUND)
    target_link_libraries(managemediaplayercontrolTest KF5::KCMUtils)
endif()
if (KF5FileMetaData_FOUND)
    target_link_libraries(managemediaplayercontrolTest KF5::FileMetaData)
endif()
if (UPNPQT_FOUND)
    target_link_libraries(managemediaplayercontrolTest Qt5::Xml UPNP::upnpQt Qt5::Network)
endif()
if (KF5XmlGui_FOUND)
    target_link_libraries(managemediaplayercontrolTest KF5::XmlGui)
endif()

target_include_directories(managemediaplayercontrolTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(managemediaplayercontrolTest managemediaplayercontrolTest)

set(manageheaderbarTest_SOURCES
    ../src/manageheaderbar.cpp
    ../src/mediaplaylist.cpp
    ../src/databaseinterface.cpp
    ../src/musiclistenersmanager.cpp
    ../src/elisaapplication.cpp
    ../src/notificationitem.cpp
    ../src/trackslistener.cpp
    ../src/trackslistener.cpp
    ../src/musicartist.cpp
    ../src/musicalbum.cpp
    ../src/musicaudiotrack.cpp
    manageheaderbartest.cpp
)

if (KF5Baloo_FOUND)
    if (Qt5DBus_FOUND)
        qt5_add_dbus_interface(manageheaderbarTest_SOURCES
            ${BALOO_DBUS_INTERFACES_DIR}/org.kde.baloo.fileindexer.xml
            baloo/fileindexer)

        qt5_add_dbus_interface(manageheaderbarTest_SOURCES
            ${BALOO_DBUS_INTERFACES_DIR}/org.kde.baloo.scheduler.xml
            baloo/scheduler)

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

if (KF5FileMetaData_FOUND)
    set(manageheaderbarTest_SOURCES
        ${manageheaderbarTest_SOURCES}
        ../src/file/filelistener.cpp
        ../src/file/localfilelisting.cpp
        ../src/abstractfile/abstractfilelistener.cpp
        ../src/abstractfile/abstractfilelisting.cpp
    )
endif()

if (KF5Config_FOUND)
    kconfig_add_kcfg_files(manageheaderbarTest_SOURCES ../src/elisa_settings.kcfgc )
    set(manageheaderbarTest_SOURCES
        ${manageheaderbarTest_SOURCES}
        ../src/elisa_core.kcfg
    )
endif()

add_executable(manageheaderbarTest ${manageheaderbarTest_SOURCES})

target_link_libraries(manageheaderbarTest Qt5::Test Qt5::Core Qt5::Sql Qt5::Gui Qt5::Widgets KF5::I18n Qt5::Quick Qt5::Multimedia)
if (KF5Baloo_FOUND)
    target_link_libraries(manageheaderbarTest KF5::Baloo Qt5::DBus)
endif()
if (KF5Config_FOUND)
    target_link_libraries(manageheaderbarTest KF5::ConfigCore KF5::ConfigGui)
endif()
if (KF5KCMUtils_FOUND)
    target_link_libraries(manageheaderbarTest KF5::KCMUtils)
endif()
if (KF5FileMetaData_FOUND)
    target_link_libraries(manageheaderbarTest KF5::FileMetaData)
endif()
if (UPNPQT_FOUND)
    target_link_libraries(manageheaderbarTest Qt5::Xml UPNP::upnpQt Qt5::Network)
endif()
if (KF5XmlGui_FOUND)
    target_link_libraries(manageheaderbarTest KF5::XmlGui)
endif()

target_include_directories(manageheaderbarTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(manageheaderbarTest manageheaderbarTest)

set(manageaudioplayerTest_SOURCES
    ../src/manageaudioplayer.cpp
    manageaudioplayertest.cpp
)

add_executable(manageaudioplayerTest ${manageaudioplayerTest_SOURCES})
target_link_libraries(manageaudioplayerTest Qt5::Test Qt5::Core Qt5::Gui Qt5::Multimedia)
target_include_directories(manageaudioplayerTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(manageaudioplayerTest manageaudioplayerTest)

set(mediaplaylistTest_SOURCES
    ../src/mediaplaylist.cpp
    ../src/databaseinterface.cpp
    ../src/trackslistener.cpp
    ../src/musiclistenersmanager.cpp
    ../src/elisaapplication.cpp
    ../src/notificationitem.cpp
    ../src/musicartist.cpp
    ../src/musicalbum.cpp
    ../src/musicaudiotrack.cpp
    mediaplaylisttest.cpp
)

if (KF5Baloo_FOUND)
    if (Qt5DBus_FOUND)
        qt5_add_dbus_interface(mediaplaylistTest_SOURCES
            ${BALOO_DBUS_INTERFACES_DIR}/org.kde.baloo.fileindexer.xml
            baloo/fileindexer)

        qt5_add_dbus_interface(mediaplaylistTest_SOURCES
            ${BALOO_DBUS_INTERFACES_DIR}/org.kde.baloo.scheduler.xml
            baloo/scheduler)

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

if (KF5FileMetaData_FOUND)
    set(mediaplaylistTest_SOURCES
        ${mediaplaylistTest_SOURCES}
        ../src/file/filelistener.cpp
        ../src/file/localfilelisting.cpp
        ../src/abstractfile/abstractfilelistener.cpp
        ../src/abstractfile/abstractfilelisting.cpp
    )
endif()

if (KF5Config_FOUND)
    kconfig_add_kcfg_files(mediaplaylistTest_SOURCES ../src/elisa_settings.kcfgc )
    set(mediaplaylistTest_SOURCES
        ${mediaplaylistTest_SOURCES}
        ../src/elisa_core.kcfg
    )
endif()

add_executable(mediaplaylistTest ${mediaplaylistTest_SOURCES})

target_link_libraries(mediaplaylistTest Qt5::Test Qt5::Core Qt5::Sql Qt5::Gui Qt5::Widgets KF5::I18n Qt5::Quick Qt5::Multimedia)
if (KF5Baloo_FOUND)
    target_link_libraries(mediaplaylistTest KF5::Baloo Qt5::DBus)
endif()
if (KF5KCMUtils_FOUND)
    target_link_libraries(mediaplaylistTest KF5::KCMUtils)
endif()
if (KF5FileMetaData_FOUND)
    target_link_libraries(mediaplaylistTest KF5::FileMetaData)
endif()
if (UPNPQT_FOUND)
    target_link_libraries(mediaplaylistTest Qt5::Xml UPNP::upnpQt Qt5::Network)
endif()
if (KF5XmlGui_FOUND)
    target_link_libraries(mediaplaylistTest KF5::XmlGui)
endif()
if (KF5Config_FOUND)
    target_link_libraries(mediaplaylistTest KF5::ConfigCore KF5::ConfigGui)
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

set(alltracksmodeltest_SOURCES
    ../src/databaseinterface.cpp
    ../src/musicartist.cpp
    ../src/musicalbum.cpp
    ../src/musicaudiotrack.cpp
    ../src/alltracksmodel.cpp
    alltracksmodeltest.cpp
)

add_executable(alltracksmodeltest ${alltracksmodeltest_SOURCES})
target_link_libraries(alltracksmodeltest Qt5::Test Qt5::Core Qt5::Sql KF5::I18n)
target_include_directories(alltracksmodeltest PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(alltracksmodeltest alltracksmodeltest)

if (KF5FileMetaData_FOUND)
    set(localfilelistingtest_SOURCES
        ../src/file/localfilelisting.cpp
        ../src/abstractfile/abstractfilelisting.cpp
        ../src/musicaudiotrack.cpp
        ../src/notificationitem.cpp
        localfilelistingtest.cpp
    )

    kconfig_add_kcfg_files(localfilelistingtest_SOURCES ../src/elisa_settings.kcfgc )
    set(localfilelistingtest_SOURCES
        ${localfilelistingtest_SOURCES}
        ../src/elisa_core.kcfg
    )

    add_executable(localfilelistingtest ${localfilelistingtest_SOURCES})
    target_link_libraries(localfilelistingtest Qt5::Test Qt5::Core Qt5::Sql KF5::I18n KF5::ConfigCore KF5::ConfigGui KF5::FileMetaData)

    target_include_directories(localfilelistingtest PRIVATE ${CMAKE_SOURCE_DIR}/src)
    add_test(localfilelistingtest localfilelistingtest)

endif()
