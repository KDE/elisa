enable_testing()

configure_file(mediaplaylisttestconfig.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/mediaplaylisttestconfig.h @ONLY)

include_directories(${elisa_CURRENT_BINARY_DIR})
include_directories(${elisa_BINARY_DIR})
include_directories(${elisa_BINARY_DIR}/src)

set(databaseInterfaceTest_SOURCES
    databaseinterfacetest.cpp
)

ecm_add_test(${databaseInterfaceTest_SOURCES}
    TEST_NAME "databaseInterfaceTest"
    LINK_LIBRARIES
        Qt5::Test elisaLib)

target_include_directories(databaseInterfaceTest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(managemediaplayercontrolTest_SOURCES
    managemediaplayercontroltest.cpp
)

ecm_add_test(${managemediaplayercontrolTest_SOURCES}
    TEST_NAME "managemediaplayercontrolTest"
    LINK_LIBRARIES
        Qt5::Test elisaLib
)

set(manageheaderbarTest_SOURCES
    manageheaderbartest.cpp
)

ecm_add_test(${manageheaderbarTest_SOURCES}
    TEST_NAME "manageheaderbarTest"
    LINK_LIBRARIES
        Qt5::Test elisaLib
)

target_include_directories(manageheaderbarTest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(manageaudioplayerTest_SOURCES
    manageaudioplayertest.cpp
)

ecm_add_test(${manageaudioplayerTest_SOURCES}
    TEST_NAME "manageaudioplayerTest"
    LINK_LIBRARIES Qt5::Test elisaLib
)

target_include_directories(manageaudioplayerTest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(mediaplaylistTest_SOURCES
    modeltest.cpp
    mediaplaylisttest.cpp
)

ecm_add_test(${mediaplaylistTest_SOURCES}
    TEST_NAME "mediaplaylistTest"
    LINK_LIBRARIES
        Qt5::Test elisaLib
)

target_include_directories(mediaplaylistTest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(trackslistenertest_SOURCES
    trackslistenertest.cpp
)

ecm_add_test(${trackslistenertest_SOURCES}
    TEST_NAME "trackslistenertest"
    LINK_LIBRARIES
        Qt5::Test elisaLib KF5::FileMetaData
)

target_include_directories(trackslistenertest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(allalbumsmodeltest_SOURCES
    modeltest.cpp
    allalbumsmodeltest.cpp
)

ecm_add_test(${allalbumsmodeltest_SOURCES}
    TEST_NAME "allalbumsmodeltest"
    LINK_LIBRARIES Qt5::Test elisaLib
)

target_include_directories(allalbumsmodeltest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(albummodeltest_SOURCES
    modeltest.cpp
    albummodeltest.cpp
)

ecm_add_test(${albummodeltest_SOURCES}
    TEST_NAME "albummodeltest"
    LINK_LIBRARIES
        Qt5::Test elisaLib
)

target_include_directories(albummodeltest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(allartistsmodeltest_SOURCES
    modeltest.cpp
    allartistsmodeltest.cpp
)

ecm_add_test(${allartistsmodeltest_SOURCES}
    TEST_NAME "allartistsmodeltest"
    LINK_LIBRARIES
        Qt5::Test elisaLib
)

target_include_directories(allartistsmodeltest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(alltracksmodeltest_SOURCES
    modeltest.cpp
    alltracksmodeltest.cpp
)

ecm_add_test(${alltracksmodeltest_SOURCES}
    TEST_NAME "alltracksmodeltest"
    LINK_LIBRARIES
        Qt5::Test elisaLib
)

target_include_directories(alltracksmodeltest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(alltracksproxymodeltest_SOURCES
    modeltest.cpp
    alltracksproxymodeltest.cpp
)

ecm_add_test(${alltracksproxymodeltest_SOURCES}
    TEST_NAME "alltracksproxymodeltest"
    LINK_LIBRARIES
        Qt5::Test elisaLib
)

target_include_directories(alltracksproxymodeltest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(localfilelistingtest_SOURCES
    localfilelistingtest.cpp
)

ecm_add_test(${localfilelistingtest_SOURCES}
    TEST_NAME "localfilelistingtest"
    LINK_LIBRARIES
        Qt5::Test elisaLib
)

target_include_directories(localfilelistingtest PRIVATE ${CMAKE_SOURCE_DIR}/src)


if (KF5XmlGui_FOUND AND KF5KCMUtils_FOUND)
    set(elisaapplicationtest_SOURCES
        elisaapplicationtest.cpp
    )

    ecm_add_test(${elisaapplicationtest_SOURCES}
        TEST_NAME "elisaapplicationtest"
        LINK_LIBRARIES
            Qt5::Test elisaLib
    )

    target_include_directories(elisaapplicationtest PRIVATE ${CMAKE_SOURCE_DIR}/src)
endif()

if (Qt5Quick_FOUND AND Qt5Widgets_FOUND)
    set(elisaqmltests_SOURCES
        elisaqmltests.cpp
        qmltests/tst_GridBrowserDelegate.qml
        qmltests/tst_NavigationActionBar.qml
        qmltests/tst_PlayListEntry.qml
    )

    ecm_add_test(${elisaqmltests_SOURCES}
        TEST_NAME "elisaqmltests"
        LINK_LIBRARIES Qt5::Core Qt5::Widgets Qt5::Test Qt5::QuickTest
        GUI)

    target_compile_definitions(elisaqmltests PRIVATE QUICK_TEST_SOURCE_DIR="${CMAKE_SOURCE_DIR}/autotests/qmltests")

    target_include_directories(elisaqmltests PRIVATE ${CMAKE_SOURCE_DIR}/src)
endif()
