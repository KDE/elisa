cmake_minimum_required(VERSION 3.5)

project(elisa)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)

set(REQUIRED_QT_VERSION "5.7.0")
find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Core Network Xml Qml Sql Multimedia Svg Gui)
find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG QUIET OPTIONAL_COMPONENTS Quick Widgets Test DBus WebSockets AndroidExtras)

set(REQUIRED_KF5_VERSION "5.31.0")
find_package(ECM ${REQUIRED_KF5_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings)
include(KDECMakeSettings)
include(FeatureSummary)

find_package(KF5I18n ${REQUIRED_KF5_VERSION} CONFIG REQUIRED QUIET)

find_package(KF5Declarative ${REQUIRED_KF5_VERSION} CONFIG QUIET)
find_package(KF5CoreAddons ${REQUIRED_KF5_VERSION} CONFIG QUIET)
find_package(KF5Baloo ${REQUIRED_KF5_VERSION} CONFIG QUIET)
find_package(KF5FileMetaData ${REQUIRED_KF5_VERSION} CONFIG QUIET)
find_package(KF5DocTools ${REQUIRED_KF5_VERSION} CONFIG QUIET)
find_package(KF5XmlGui ${REQUIRED_KF5_VERSION} CONFIG QUIET)
find_package(KF5ConfigWidgets ${REQUIRED_KF5_VERSION} CONFIG QUIET)
find_package(KF5Config ${REQUIRED_KF5_VERSION} CONFIG QUIET)
find_package(KF5Crash ${REQUIRED_KF5_VERSION} CONFIG QUIET)
find_package(KF5DBusAddons ${REQUIRED_KF5_VERSION} CONFIG QUIET)

find_package(UPNPQT CONFIG QUIET)
set_package_properties(UPNPQT PROPERTIES
    DESCRIPTION "UPNP layer build with Qt 5."
    URL "https://gitlab.com/homeautomationqt/upnp-player-qt"
    TYPE OPTIONAL)

include(FeatureSummary)
include(GenerateExportHeader)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(CMakePackageConfigHelpers)

if (CMAKE_SYSTEM_NAME STREQUAL Android)
    set(QT_QMAKE_EXECUTABLE "$ENV{Qt5_android}/bin/qmake")
    # workaround until this fix is in released ECM
    if(ECM_VERSION VERSION_LESS "5.24.0")
        add_definitions(-DANDROID)
    endif()
endif()

configure_file(config-upnp-qt.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-upnp-qt.h )

add_subdirectory(src)
add_subdirectory(autotests)
add_subdirectory(doc)

install(
    PROGRAMS org.kde.elisa.desktop
    DESTINATION ${XDG_APPS_INSTALL_DIR}
)

install(
    FILES org.kde.elisa.appdata.xml
    DESTINATION ${KDE_INSTALL_METAINFODIR}
)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

