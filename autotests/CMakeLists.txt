enable_testing()

configure_file(mediaplaylisttestconfig.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/mediaplaylisttestconfig.h @ONLY)

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

ecm_add_test(${databaseInterfaceTest_SOURCES}
    TEST_NAME "databaseInterfaceTest"
    LINK_LIBRARIES Qt5::Test Qt5::Core Qt5::Sql KF5::I18n)

target_include_directories(databaseInterfaceTest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(managemediaplayercontrolTest_SOURCES
    ../src/managemediaplayercontrol.cpp
    ../src/mediaplaylist.cpp
    ../src/databaseinterface.cpp
    ../src/musiclistenersmanager.cpp
    ../src/elisaapplication.cpp
    ../src/manageaudioplayer.cpp
    ../src/models/allalbumsmodel.cpp
    ../src/models/allartistsmodel.cpp
    ../src/models/alltracksmodel.cpp
    ../src/models/albummodel.cpp
    ../src/models/abstractmediaproxymodel.cpp
    ../src/models/allalbumsproxymodel.cpp
    ../src/models/allartistsproxymodel.cpp
    ../src/models/alltracksproxymodel.cpp
    ../src/models/singleartistproxymodel.cpp
    ../src/models/singlealbumproxymodel.cpp
    ../src/notificationitem.cpp
    ../src/trackslistener.cpp
    ../src/musicartist.cpp
    ../src/musicalbum.cpp
    ../src/musicaudiotrack.cpp
    ../src/elisautils.cpp
    ../src/file/filelistener.cpp
    ../src/file/localfilelisting.cpp
    ../src/abstractfile/abstractfilelistener.cpp
    ../src/abstractfile/abstractfilelisting.cpp
    ../src/audiowrapper.cpp
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

kconfig_add_kcfg_files(managemediaplayercontrolTest_SOURCES ../src/elisa_settings.kcfgc )
set(managemediaplayercontrolTest_SOURCES
    ${managemediaplayercontrolTest_SOURCES}
    ../src/elisa_core.kcfg
)

ecm_add_test(${managemediaplayercontrolTest_SOURCES}
    TEST_NAME "managemediaplayercontrolTest"
    LINK_LIBRARIES
        Qt5::Test Qt5::Core Qt5::Sql Qt5::Widgets KF5::I18n Qt5::Quick Qt5::Multimedia
        Qt5::Concurrent KF5::ConfigCore KF5::ConfigGui KF5::FileMetaData
)

if (KF5Baloo_FOUND)
    target_link_libraries(managemediaplayercontrolTest KF5::Baloo Qt5::DBus)
endif()
if (KF5KCMUtils_FOUND)
    target_link_libraries(managemediaplayercontrolTest KF5::KCMUtils)
endif()
if (UPNPQT_FOUND)
    target_link_libraries(managemediaplayercontrolTest Qt5::Xml UPNP::upnpQt Qt5::Network)
endif()
if (KF5XmlGui_FOUND)
    target_link_libraries(managemediaplayercontrolTest KF5::XmlGui)
endif()

target_include_directories(managemediaplayercontrolTest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(manageheaderbarTest_SOURCES
    ../src/manageheaderbar.cpp
    ../src/mediaplaylist.cpp
    ../src/databaseinterface.cpp
    ../src/musiclistenersmanager.cpp
    ../src/elisaapplication.cpp
    ../src/manageaudioplayer.cpp
    ../src/models/allalbumsmodel.cpp
    ../src/models/allartistsmodel.cpp
    ../src/models/alltracksmodel.cpp
    ../src/models/albummodel.cpp
    ../src/models/abstractmediaproxymodel.cpp
    ../src/models/allalbumsproxymodel.cpp
    ../src/models/allartistsproxymodel.cpp
    ../src/models/alltracksproxymodel.cpp
    ../src/models/singleartistproxymodel.cpp
    ../src/models/singlealbumproxymodel.cpp
    ../src/notificationitem.cpp
    ../src/trackslistener.cpp
    ../src/trackslistener.cpp
    ../src/musicartist.cpp
    ../src/musicalbum.cpp
    ../src/musicaudiotrack.cpp
    ../src/elisautils.cpp
    ../src/file/filelistener.cpp
    ../src/file/localfilelisting.cpp
    ../src/abstractfile/abstractfilelistener.cpp
    ../src/abstractfile/abstractfilelisting.cpp
    ../src/audiowrapper.cpp
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

kconfig_add_kcfg_files(manageheaderbarTest_SOURCES ../src/elisa_settings.kcfgc )
set(manageheaderbarTest_SOURCES
    ${manageheaderbarTest_SOURCES}
    ../src/elisa_core.kcfg
)

ecm_add_test(${manageheaderbarTest_SOURCES}
    TEST_NAME "manageheaderbarTest"
    LINK_LIBRARIES
        Qt5::Test Qt5::Core Qt5::Sql Qt5::Gui Qt5::Widgets KF5::I18n Qt5::Quick Qt5::Multimedia
        Qt5::Concurrent KF5::ConfigCore KF5::ConfigGui KF5::FileMetaData
)

if (KF5Baloo_FOUND)
    target_link_libraries(manageheaderbarTest KF5::Baloo Qt5::DBus)
endif()
if (KF5KCMUtils_FOUND)
    target_link_libraries(manageheaderbarTest KF5::KCMUtils)
endif()
if (UPNPQT_FOUND)
    target_link_libraries(manageheaderbarTest Qt5::Xml UPNP::upnpQt Qt5::Network)
endif()
if (KF5XmlGui_FOUND)
    target_link_libraries(manageheaderbarTest KF5::XmlGui)
endif()

target_include_directories(manageheaderbarTest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(manageaudioplayerTest_SOURCES
    ../src/manageaudioplayer.cpp
    manageaudioplayertest.cpp
)

ecm_add_test(${manageaudioplayerTest_SOURCES}
    TEST_NAME "manageaudioplayerTest"
    LINK_LIBRARIES Qt5::Test Qt5::Core Qt5::Gui Qt5::Multimedia KF5::FileMetaData)

target_include_directories(manageaudioplayerTest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(mediaplaylistTest_SOURCES
    ../src/mediaplaylist.cpp
    ../src/databaseinterface.cpp
    ../src/trackslistener.cpp
    ../src/musiclistenersmanager.cpp
    ../src/models/allalbumsmodel.cpp
    ../src/models/allartistsmodel.cpp
    ../src/models/alltracksmodel.cpp
    ../src/models/albummodel.cpp
    ../src/models/abstractmediaproxymodel.cpp
    ../src/models/allalbumsproxymodel.cpp
    ../src/models/allartistsproxymodel.cpp
    ../src/models/alltracksproxymodel.cpp
    ../src/models/singleartistproxymodel.cpp
    ../src/models/singlealbumproxymodel.cpp
    ../src/manageaudioplayer.cpp
    ../src/elisaapplication.cpp
    ../src/notificationitem.cpp
    ../src/musicartist.cpp
    ../src/musicalbum.cpp
    ../src/musicaudiotrack.cpp
    ../src/elisautils.cpp
    ../src/file/filelistener.cpp
    ../src/file/localfilelisting.cpp
    ../src/abstractfile/abstractfilelistener.cpp
    ../src/abstractfile/abstractfilelisting.cpp
    ../src/audiowrapper.cpp
    modeltest.cpp
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

kconfig_add_kcfg_files(mediaplaylistTest_SOURCES ../src/elisa_settings.kcfgc )
set(mediaplaylistTest_SOURCES
    ${mediaplaylistTest_SOURCES}
    ../src/elisa_core.kcfg
)

ecm_add_test(${mediaplaylistTest_SOURCES}
    TEST_NAME "mediaplaylistTest"
    LINK_LIBRARIES
        Qt5::Test Qt5::Core Qt5::Sql Qt5::Gui Qt5::Widgets KF5::I18n Qt5::Quick Qt5::Multimedia
        Qt5::Concurrent KF5::ConfigCore KF5::ConfigGui KF5::FileMetaData
)

if (KF5Baloo_FOUND)
    target_link_libraries(mediaplaylistTest KF5::Baloo Qt5::DBus)
endif()
if (KF5KCMUtils_FOUND)
    target_link_libraries(mediaplaylistTest KF5::KCMUtils)
endif()
if (UPNPQT_FOUND)
    target_link_libraries(mediaplaylistTest Qt5::Xml UPNP::upnpQt Qt5::Network)
endif()
if (KF5XmlGui_FOUND)
    target_link_libraries(mediaplaylistTest KF5::XmlGui)
endif()

target_include_directories(mediaplaylistTest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(trackslistenertest_SOURCES
    ../src/mediaplaylist.cpp
    ../src/databaseinterface.cpp
    ../src/trackslistener.cpp
    ../src/musiclistenersmanager.cpp
    ../src/elisaapplication.cpp
    ../src/manageaudioplayer.cpp
    ../src/models/allalbumsmodel.cpp
    ../src/models/allartistsmodel.cpp
    ../src/models/alltracksmodel.cpp
    ../src/models/albummodel.cpp
    ../src/models/abstractmediaproxymodel.cpp
    ../src/models/allalbumsproxymodel.cpp
    ../src/models/allartistsproxymodel.cpp
    ../src/models/alltracksproxymodel.cpp
    ../src/models/singleartistproxymodel.cpp
    ../src/models/singlealbumproxymodel.cpp
    ../src/notificationitem.cpp
    ../src/musicartist.cpp
    ../src/musicalbum.cpp
    ../src/musicaudiotrack.cpp
    ../src/elisautils.cpp
    ../src/file/filelistener.cpp
    ../src/file/localfilelisting.cpp
    ../src/abstractfile/abstractfilelistener.cpp
    ../src/abstractfile/abstractfilelisting.cpp
    ../src/audiowrapper.cpp
    trackslistenertest.cpp
)

if (KF5Baloo_FOUND)
    if (Qt5DBus_FOUND)
        qt5_add_dbus_interface(trackslistenertest_SOURCES
            ${BALOO_DBUS_INTERFACES_DIR}/org.kde.baloo.fileindexer.xml
            baloo/fileindexer)

        qt5_add_dbus_interface(trackslistenertest_SOURCES
            ${BALOO_DBUS_INTERFACES_DIR}/org.kde.baloo.scheduler.xml
            baloo/scheduler)

        set(trackslistenertest_SOURCES
            ${trackslistenertest_SOURCES}
            ../src/baloo/baloolistener.cpp
            ../src/baloo/localbaloofilelisting.cpp
            )
    endif()
endif()

if (UPNPQT_FOUND)
    set(trackslistenertest_SOURCES
        ${trackslistenertest_SOURCES}
        ../src/upnp/upnpcontrolcontentdirectory.cpp
        ../src/upnp/upnpcontentdirectorymodel.cpp
        ../src/upnp/upnpcontrolconnectionmanager.cpp
        ../src/upnp/upnpcontrolmediaserver.cpp
        ../src/upnp/didlparser.cpp
        ../src/upnp/upnplistener.cpp
        ../src/upnp/upnpdiscoverallmusic.cpp
        )
endif()

kconfig_add_kcfg_files(trackslistenertest_SOURCES ../src/elisa_settings.kcfgc )
set(trackslistenertest_SOURCES
    ${trackslistenertest_SOURCES}
    ../src/elisa_core.kcfg
)

ecm_add_test(${trackslistenertest_SOURCES}
    TEST_NAME "trackslistenertest"
    LINK_LIBRARIES
        Qt5::Test Qt5::Core Qt5::Sql Qt5::Gui Qt5::Widgets KF5::I18n Qt5::Quick Qt5::Multimedia
        Qt5::Concurrent KF5::ConfigCore KF5::ConfigGui KF5::FileMetaData
)

if (KF5Baloo_FOUND)
    target_link_libraries(trackslistenertest KF5::Baloo Qt5::DBus)
endif()
if (KF5KCMUtils_FOUND)
    target_link_libraries(trackslistenertest KF5::KCMUtils)
endif()
if (UPNPQT_FOUND)
    target_link_libraries(trackslistenertest Qt5::Xml UPNP::upnpQt Qt5::Network)
endif()
if (KF5XmlGui_FOUND)
    target_link_libraries(trackslistenertest KF5::XmlGui)
endif()

target_include_directories(trackslistenertest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(allalbumsmodeltest_SOURCES
    ../src/databaseinterface.cpp
    ../src/musicartist.cpp
    ../src/musicalbum.cpp
    ../src/musicaudiotrack.cpp
    ../src/models/allalbumsmodel.cpp
    modeltest.cpp
    allalbumsmodeltest.cpp
)

ecm_add_test(${allalbumsmodeltest_SOURCES}
    TEST_NAME "allalbumsmodeltest"
    LINK_LIBRARIES Qt5::Test Qt5::Core Qt5::Sql Qt5::Concurrent KF5::I18n)

target_include_directories(allalbumsmodeltest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(albummodeltest_SOURCES
    ../src/databaseinterface.cpp
    ../src/musicartist.cpp
    ../src/musicalbum.cpp
    ../src/musicaudiotrack.cpp
    ../src/models/albummodel.cpp
    modeltest.cpp
    albummodeltest.cpp
)

ecm_add_test(${albummodeltest_SOURCES}
    TEST_NAME "albummodeltest"
    LINK_LIBRARIES Qt5::Test Qt5::Core Qt5::Sql Qt5::Concurrent KF5::I18n)

target_include_directories(albummodeltest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(allartistsmodeltest_SOURCES
    ../src/databaseinterface.cpp
    ../src/musicartist.cpp
    ../src/musicalbum.cpp
    ../src/musicaudiotrack.cpp
    ../src/models/allartistsmodel.cpp
    modeltest.cpp
    allartistsmodeltest.cpp
)

ecm_add_test(${allartistsmodeltest_SOURCES}
    TEST_NAME "allartistsmodeltest"
    LINK_LIBRARIES Qt5::Test Qt5::Core Qt5::Sql KF5::I18n)

target_include_directories(allartistsmodeltest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(alltracksmodeltest_SOURCES
    ../src/databaseinterface.cpp
    ../src/musicartist.cpp
    ../src/musicalbum.cpp
    ../src/musicaudiotrack.cpp
    ../src/models/alltracksmodel.cpp
    modeltest.cpp
    alltracksmodeltest.cpp
)

ecm_add_test(${alltracksmodeltest_SOURCES}
    TEST_NAME "alltracksmodeltest"
    LINK_LIBRARIES Qt5::Test Qt5::Core Qt5::Sql KF5::I18n)

target_include_directories(alltracksmodeltest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(localfilelistingtest_SOURCES
    ../src/file/localfilelisting.cpp
    ../src/abstractfile/abstractfilelisting.cpp
    ../src/musicaudiotrack.cpp
    ../src/notificationitem.cpp
    ../src/elisautils.cpp
    localfilelistingtest.cpp
)

kconfig_add_kcfg_files(localfilelistingtest_SOURCES ../src/elisa_settings.kcfgc )
set(localfilelistingtest_SOURCES
    ${localfilelistingtest_SOURCES}
    ../src/elisa_core.kcfg
)

ecm_add_test(${localfilelistingtest_SOURCES}
    TEST_NAME "localfilelistingtest"
    LINK_LIBRARIES Qt5::Test Qt5::Core Qt5::Sql KF5::I18n KF5::FileMetaData KF5::ConfigCore KF5::ConfigGui)

target_include_directories(localfilelistingtest PRIVATE ${CMAKE_SOURCE_DIR}/src)


if (KF5XmlGui_FOUND AND KF5KCMUtils_FOUND)
    set(elisaapplicationtest_SOURCES
        ../src/elisaapplication.cpp
        ../src/musiclistenersmanager.cpp
        ../src/databaseinterface.cpp
        ../src/notificationitem.cpp
        ../src/trackslistener.cpp
        ../src/mediaplaylist.cpp
        ../src/audiowrapper.cpp
        ../src/musicartist.cpp
        ../src/musicalbum.cpp
        ../src/musicaudiotrack.cpp
        ../src/elisautils.cpp
        ../src/manageaudioplayer.cpp
        ../src/models/allalbumsmodel.cpp
        ../src/models/allartistsmodel.cpp
        ../src/models/alltracksmodel.cpp
        ../src/models/albummodel.cpp
        ../src/models/abstractmediaproxymodel.cpp
        ../src/models/allalbumsproxymodel.cpp
        ../src/models/allartistsproxymodel.cpp
        ../src/models/alltracksproxymodel.cpp
        ../src/models/singleartistproxymodel.cpp
        ../src/models/singlealbumproxymodel.cpp
        ../src/file/filelistener.cpp
        ../src/file/localfilelisting.cpp
        ../src/abstractfile/abstractfilelistener.cpp
        ../src/abstractfile/abstractfilelisting.cpp
        elisaapplicationtest.cpp
    )

    if (KF5Baloo_FOUND)
        if (Qt5DBus_FOUND)
            qt5_add_dbus_interface(elisaapplicationtest_SOURCES
                ${BALOO_DBUS_INTERFACES_DIR}/org.kde.baloo.fileindexer.xml
                baloo/fileindexer)

            qt5_add_dbus_interface(elisaapplicationtest_SOURCES
                ${BALOO_DBUS_INTERFACES_DIR}/org.kde.baloo.scheduler.xml
                baloo/scheduler)

            set(elisaapplicationtest_SOURCES
                ${elisaapplicationtest_SOURCES}
                ../src/baloo/baloolistener.cpp
                ../src/baloo/localbaloofilelisting.cpp
                )
        endif()
    endif()

    kconfig_add_kcfg_files(elisaapplicationtest_SOURCES ../src/elisa_settings.kcfgc )
    set(elisaapplicationtest_SOURCES
        ${elisaapplicationtest_SOURCES}
        ../src/elisa_core.kcfg
    )

    ecm_add_test(${elisaapplicationtest_SOURCES}
        TEST_NAME "elisaapplicationtest"
        LINK_LIBRARIES Qt5::Test Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::Sql Qt5::Concurrent
            KF5::FileMetaData KF5::I18n KF5::ConfigCore KF5::ConfigGui KF5::ConfigWidgets KF5::XmlGui KF5::KCMUtils)

    if (KF5Baloo_FOUND)
        target_link_libraries(elisaapplicationtest KF5::Baloo Qt5::DBus)
    endif()

    target_include_directories(elisaapplicationtest PRIVATE ${CMAKE_SOURCE_DIR}/src)
endif()

if (Qt5Quick_FOUND AND Qt5Widgets_FOUND)
    set(elisaqmltests_SOURCES
        elisaqmltests.cpp
        qmltests/tst_GridBrowserDelegate.qml
        qmltests/tst_NavigationActionBar.qml
    )

    ecm_add_test(${elisaqmltests_SOURCES}
        TEST_NAME "elisaqmltests"
        LINK_LIBRARIES Qt5::Core Qt5::Widgets Qt5::Test Qt5::QuickTest
        GUI)

    target_compile_definitions(elisaqmltests PRIVATE QUICK_TEST_SOURCE_DIR="${CMAKE_SOURCE_DIR}/autotests/qmltests")

    target_include_directories(elisaqmltests PRIVATE ${CMAKE_SOURCE_DIR}/src)
endif()
