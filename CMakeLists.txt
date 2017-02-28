cmake_minimum_required(VERSION 3.5)

project(elisa)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)

set(REQUIRED_QT_VERSION "5.6.0")
find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Core Network Xml Qml Sql Multimedia Svg Gui)
find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG QUIET OPTIONAL_COMPONENTS Quick Widgets Test DBus WebSockets)

find_package(ECM 5.24.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings)
include(KDECMakeSettings)
include(FeatureSummary)

find_package(KF5I18n CONFIG REQUIRED QUIET)

find_package(KF5Declarative CONFIG QUIET)
find_package(KF5CoreAddons CONFIG QUIET)
find_package(KF5Baloo CONFIG QUIET)
find_package(KF5FileMetaData CONFIG QUIET)
find_package(KF5DocTools CONFIG QUIET)
find_package(KF5XmlGui CONFIG QUIET)
find_package(KF5ConfigWidgets CONFIG QUIET)
find_package(KF5Config CONFIG QUIET)
find_package(KF5Crash CONFIG QUIET)

find_package(KDSoap CONFIG QUIET)
set_package_properties(KDSoap PROPERTIES
    DESCRIPTION "Tool for creating client applications for web services."
    URL "https://www.kdab.com/development-resources/qt-tools/kd-soap/"
    TYPE OPTIONAL)

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

