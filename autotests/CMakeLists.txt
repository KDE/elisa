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

set(databaseInterfaceTest_SOURCES
    databaseinterfacetest.cpp
    databasetestdata.h
)

ecm_add_test(${databaseInterfaceTest_SOURCES}
    TEST_NAME "databaseInterfaceTest"
    LINK_LIBRARIES
        Qt::Test elisaLib Qt::Sql)

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

set(manageaudioplayerTest_SOURCES
    manageaudioplayertest.cpp
)

ecm_add_test(${manageaudioplayerTest_SOURCES}
    TEST_NAME "manageaudioplayerTest"
    LINK_LIBRARIES Qt::Test elisaLib
)

set(mediaplaylistTest_SOURCES
    mediaplaylisttest.cpp
    ../src/elisautils.cpp
)

ecm_add_test(${mediaplaylistTest_SOURCES}
    TEST_NAME "mediaplaylistTest"
    LINK_LIBRARIES
        Qt::Test elisaLib
)

ecm_qt_declare_logging_category(mediaplaylistproxymodelTest_SOURCES HEADER elisa_debug.h IDENTIFIER lcModelTest CATEGORY_NAME qt.modeltest)

set(mediaplaylistproxymodelTest_SOURCES
    mediaplaylistproxymodeltest.cpp
    ../src/elisautils.cpp
)

ecm_add_test(${mediaplaylistproxymodelTest_SOURCES}
    TEST_NAME "mediaplaylistproxymodelTest"
    LINK_LIBRARIES
        Qt::Test elisaLib
)

set(trackslistenertest_SOURCES
    trackslistenertest.cpp
    ../src/elisautils.cpp
)

ecm_add_test(${trackslistenertest_SOURCES}
    TEST_NAME "trackslistenertest"
    LINK_LIBRARIES
        Qt::Test elisaLib
)

set(datamodeltest_SOURCES
    datamodeltest.cpp
)

ecm_add_test(${datamodeltest_SOURCES}
    TEST_NAME "datamodeltest"
    LINK_LIBRARIES
        Qt::Test elisaLib
)

set(viewsmodeltest_SOURCES
    viewsmodeltest.cpp
)

ecm_add_test(${viewsmodeltest_SOURCES}
    TEST_NAME "viewsmodeltest"
    LINK_LIBRARIES
        Qt::Test elisaLib
)

set(trackmetadatamodeltest_SOURCES
    trackmetadatamodeltest.cpp
)

ecm_add_test(${trackmetadatamodeltest_SOURCES}
    TEST_NAME "trackmetadatamodeltest"
    LINK_LIBRARIES
        Qt::Test elisaLib
)

if (KF6FileMetaData_FOUND)
    set(localfilelistingtest_SOURCES
        localfilelistingtest.cpp
    )

    ecm_add_test(${localfilelistingtest_SOURCES}
        TEST_NAME "localfilelistingtest"
        LINK_LIBRARIES
            Qt::Test elisaLib
    )

endif()

if (KF6XmlGui_FOUND AND KF6KCMUtils_FOUND)
    set(elisaapplicationtest_SOURCES
        elisaapplicationtest.cpp
        ../src/elisautils.cpp
    )

    ecm_add_test(${elisaapplicationtest_SOURCES}
        TEST_NAME "elisaapplicationtest"
        LINK_LIBRARIES
            Qt::Test elisaLib
    )
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

if (KF6KIO_FOUND)
    set(filebrowserproxymodelTest_SOURCES
        filebrowserproxymodeltest.cpp
    )

    ecm_add_test(${filebrowserproxymodelTest_SOURCES}
        TEST_NAME "filebrowserproxymodelTest"
        LINK_LIBRARIES Qt::Test elisaLib
    )
endif()

ecm_add_test(lyricsmodeltest.cpp
    TEST_NAME "lyricsModelTest"
    LINK_LIBRARIES Qt::Test elisaLib
)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    ecm_add_test(audiowrappertest.cpp
        TEST_NAME "audioWrapperTest"
        LINK_LIBRARIES Qt::Test elisaLib KF6::I18n
    )

    option(ELISA_CI_PIPEWIRE "Start a headless PipeWire session for audio tests" OFF)
    if (ELISA_CI_PIPEWIRE)
        find_program(PIPEWIRE_EXECUTABLE pipewire)
        find_program(WIREPLUMBER_EXECUTABLE wireplumber)
        if (PIPEWIRE_EXECUTABLE AND WIREPLUMBER_EXECUTABLE)
            add_test(NAME elisaPipewireSetup
                COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/run-with-pipewire.sh start
            )
            set_tests_properties(elisaPipewireSetup PROPERTIES
                FIXTURES_SETUP "elisa_pipewire"
            )

            add_test(NAME elisaPipewireCleanup
                COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/run-with-pipewire.sh stop
            )
            set_tests_properties(elisaPipewireCleanup PROPERTIES
                FIXTURES_CLEANUP "elisa_pipewire"
            )

            set_tests_properties(audioWrapperTest PROPERTIES
                FIXTURES_REQUIRED "elisa_pipewire"
            )
        endif()
    endif()
endif()
