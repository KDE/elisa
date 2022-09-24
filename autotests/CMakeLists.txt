# SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
#
# SPDX-License-Identifier: LGPL-3.0-or-later

enable_testing()

configure_file(mediaplaylisttestconfig.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/mediaplaylisttestconfig.h @ONLY)

include_directories(${elisa_CURRENT_BINARY_DIR})
include_directories(${elisa_BINARY_DIR})
include_directories(${elisa_BINARY_DIR}/src)

set(viewManagerTest_SOURCES
    viewmanagertest.cpp
    ../src/elisautils.cpp
)

ecm_add_test(${viewManagerTest_SOURCES}
    TEST_NAME "viewManagerTest"
    LINK_LIBRARIES
        Qt::Test elisaLib)

target_include_directories(viewManagerTest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(databaseInterfaceTest_SOURCES
    databaseinterfacetest.cpp
    databasetestdata.h
)

ecm_add_test(${databaseInterfaceTest_SOURCES}
    TEST_NAME "databaseInterfaceTest"
    LINK_LIBRARIES
        Qt::Test elisaLib)

target_include_directories(databaseInterfaceTest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(managemediaplayercontrolTest_SOURCES
    managemediaplayercontroltest.cpp
    ../src/elisautils.cpp
)

ecm_add_test(${managemediaplayercontrolTest_SOURCES}
    TEST_NAME "managemediaplayercontrolTest"
    LINK_LIBRARIES
        Qt::Test elisaLib
)

set(manageheaderbarTest_SOURCES
    manageheaderbartest.cpp
    ../src/elisautils.cpp
)

ecm_add_test(${manageheaderbarTest_SOURCES}
    TEST_NAME "manageheaderbarTest"
    LINK_LIBRARIES
        Qt::Test elisaLib
)

target_include_directories(manageheaderbarTest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(manageaudioplayerTest_SOURCES
    manageaudioplayertest.cpp
)

ecm_add_test(${manageaudioplayerTest_SOURCES}
    TEST_NAME "manageaudioplayerTest"
    LINK_LIBRARIES Qt::Test elisaLib
)

target_include_directories(manageaudioplayerTest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(mediaplaylistTest_SOURCES
    mediaplaylisttest.cpp
    ../src/elisautils.cpp
)

ecm_add_test(${mediaplaylistTest_SOURCES}
    TEST_NAME "mediaplaylistTest"
    LINK_LIBRARIES
        Qt::Test elisaLib Qt::CorePrivate
)

target_include_directories(mediaplaylistTest PRIVATE ${CMAKE_SOURCE_DIR}/src)

ecm_qt_declare_logging_category(mediaplaylistproxymodelTest_SOURCES HEADER elisa_debug.h IDENTIFIER lcModelTest CATEGORY_NAME qt.modeltest)

set(mediaplaylistproxymodelTest_SOURCES
    mediaplaylistproxymodeltest.cpp
    ../src/elisautils.cpp
)

ecm_add_test(${mediaplaylistproxymodelTest_SOURCES}
    TEST_NAME "mediaplaylistproxymodelTest"
    LINK_LIBRARIES
        Qt::Test elisaLib Qt::CorePrivate
)

target_include_directories(mediaplaylistproxymodelTest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(trackslistenertest_SOURCES
    trackslistenertest.cpp
    ../src/elisautils.cpp
)

ecm_add_test(${trackslistenertest_SOURCES}
    TEST_NAME "trackslistenertest"
    LINK_LIBRARIES
        Qt::Test elisaLib
)

target_include_directories(trackslistenertest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(datamodeltest_SOURCES
    datamodeltest.cpp
)

ecm_add_test(${datamodeltest_SOURCES}
    TEST_NAME "datamodeltest"
    LINK_LIBRARIES
        Qt::Test elisaLib Qt::CorePrivate
)

target_include_directories(datamodeltest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(viewsmodeltest_SOURCES
    viewsmodeltest.cpp
)

ecm_add_test(${viewsmodeltest_SOURCES}
    TEST_NAME "viewsmodeltest"
    LINK_LIBRARIES
        Qt::Test elisaLib Qt::CorePrivate
)

target_include_directories(viewsmodeltest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(trackmetadatamodeltest_SOURCES
    trackmetadatamodeltest.cpp
)

ecm_add_test(${trackmetadatamodeltest_SOURCES}
    TEST_NAME "trackmetadatamodeltest"
    LINK_LIBRARIES
        Qt::Test elisaLib Qt::CorePrivate
)

target_include_directories(trackmetadatamodeltest PRIVATE ${CMAKE_SOURCE_DIR}/src)

if (KF5FileMetaData_FOUND)
    set(localfilelistingtest_SOURCES
        localfilelistingtest.cpp
    )

    ecm_add_test(${localfilelistingtest_SOURCES}
        TEST_NAME "localfilelistingtest"
        LINK_LIBRARIES
            Qt::Test elisaLib
    )

    target_include_directories(localfilelistingtest PRIVATE ${CMAKE_SOURCE_DIR}/src)
endif()

if (KF5XmlGui_FOUND AND KF5KCMUtils_FOUND)
    set(elisaapplicationtest_SOURCES
        elisaapplicationtest.cpp
        ../src/elisautils.cpp
    )

    ecm_add_test(${elisaapplicationtest_SOURCES}
        TEST_NAME "elisaapplicationtest"
        LINK_LIBRARIES
            Qt::Test elisaLib
    )

    if (KF5Declarative_FOUND)
        target_link_libraries(elisaapplicationtest
            KF5::Declarative
            )
    endif()

    target_include_directories(elisaapplicationtest PRIVATE ${CMAKE_SOURCE_DIR}/src)
endif()

if (Qt5Quick_FOUND AND Qt5Widgets_FOUND AND NOT WIN32)
    set(elisaqmltests_SOURCES
        elisaqmltests.cpp
        qmltests/tst_GridBrowserDelegate.qml
        qmltests/tst_NavigationActionBar.qml
        qmltests/tst_PlayListEntry.qml
    )

    ecm_add_test(${elisaqmltests_SOURCES}
        TEST_NAME "elisaqmltests"
        LINK_LIBRARIES Qt::Core Qt::Widgets Qt::Test Qt::QuickTest
        GUI)

    target_compile_definitions(elisaqmltests PRIVATE QUICK_TEST_SOURCE_DIR="${CMAKE_SOURCE_DIR}/autotests/qmltests")

    target_include_directories(elisaqmltests PRIVATE ${CMAKE_SOURCE_DIR}/src)
endif()

set(filescannerTest_SOURCES
    filescannertest.cpp
)

ecm_add_test(${filescannerTest_SOURCES}
    TEST_NAME "filescannerTest"
    LINK_LIBRARIES Qt::Test elisaLib
)

set(filewriterTest_SOURCES
    filewritertest.cpp
)

ecm_add_test(${filewriterTest_SOURCES}
    TEST_NAME "filewriterTest"
    LINK_LIBRARIES Qt::Test elisaLib
)
