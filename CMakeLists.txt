cmake_minimum_required(VERSION 3.3)

project(elisa)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)

find_package(ECM 5.15.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings)
include(KDECMakeSettings)

set(REQUIRED_QT_VERSION "5.4.0")
find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Core Network Xml Qml WebSockets Sql)
find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG QUIET OPTIONAL_COMPONENTS Quick Widgets Multimedia Svg Test DBus)

find_package(KDSoap CONFIG REQUIRED)
find_package(KF5Declarative CONFIG QUIET)
find_package(KF5I18n CONFIG QUIET)
find_package(KF5CoreAddons CONFIG QUIET)
find_package(KF5Baloo CONFIG QUIET)
find_package(KF5FileMetaData CONFIG QUIET)
find_package(UPNPQT CONFIG REQUIRED)

include(FeatureSummary)
include(GenerateExportHeader)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(CMakePackageConfigHelpers)

if (CMAKE_SYSTEM_NAME STREQUAL Android)
    set(QT_QMAKE_EXECUTABLE "$ENV{Qt5_android}/bin/qmake")
    # workaround until this fix is in released ECM
    if(ECM_VERSION VERSION_LESS "5.15.0")
        add_definitions(-DANDROID)
    endif()
endif()

configure_file(config-upnp-qt.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-upnp-qt.h )

add_subdirectory(src)
#add_subdirectory(tests)
#add_subdirectory(autotests)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

