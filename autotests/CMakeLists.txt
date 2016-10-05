set(playListTest_SOURCES
    ../src/mediaplaylist.cpp
    ../src/playlistcontroler.cpp
    ../src/databaseinterface.cpp
    ../src/musicalbum.cpp
    ../src/musicaudiotrack.cpp
    playlisttests.cpp
)

add_executable(playListTest ${playListTest_SOURCES})
target_link_libraries(playListTest Qt5::Test Qt5::Core Qt5::Sql)
target_include_directories(playListTest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(databaseInterfaceTest_SOURCES
    ../src/databaseinterface.cpp
    ../src/musicalbum.cpp
    ../src/musicaudiotrack.cpp
    databaseinterfacetest.cpp
)

add_executable(databaseInterfaceTest ${databaseInterfaceTest_SOURCES})
target_link_libraries(databaseInterfaceTest Qt5::Test Qt5::Core Qt5::Sql)
target_include_directories(databaseInterfaceTest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(playListControlerTest_SOURCES
    ../src/playlistcontroler.cpp
    ../src/mediaplaylist.cpp
    ../src/databaseinterface.cpp
    ../src/musicalbum.cpp
    ../src/musicaudiotrack.cpp
    playlistcontrolertest.cpp
)

add_executable(playListControlerTest ${playListControlerTest_SOURCES})
target_link_libraries(playListControlerTest Qt5::Test Qt5::Core Qt5::Sql)
target_include_directories(playListControlerTest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(managemediaplayercontrolTest_SOURCES
    ../src/managemediaplayercontrol.cpp
    ../src/mediaplaylist.cpp
    ../src/databaseinterface.cpp
    ../src/musicalbum.cpp
    ../src/musicaudiotrack.cpp
    managemediaplayercontroltest.cpp
)

add_executable(managemediaplayercontrolTest ${managemediaplayercontrolTest_SOURCES})
target_link_libraries(managemediaplayercontrolTest Qt5::Test Qt5::Core Qt5::Sql)
target_include_directories(managemediaplayercontrolTest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(manageheaderbarTest_SOURCES
    ../src/manageheaderbar.cpp
    ../src/mediaplaylist.cpp
    ../src/databaseinterface.cpp
    ../src/musicalbum.cpp
    ../src/musicaudiotrack.cpp
    manageheaderbartest.cpp
)

add_executable(manageheaderbarTest ${manageheaderbarTest_SOURCES})
target_link_libraries(manageheaderbarTest Qt5::Test Qt5::Core Qt5::Sql Qt5::Gui)
target_include_directories(manageheaderbarTest PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(manageaudioplayerTest_SOURCES
    ../src/manageaudioplayer.cpp
    manageaudioplayertest.cpp
)

add_executable(manageaudioplayerTest ${manageaudioplayerTest_SOURCES})
target_link_libraries(manageaudioplayerTest Qt5::Test Qt5::Core Qt5::Gui)
target_include_directories(manageaudioplayerTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
