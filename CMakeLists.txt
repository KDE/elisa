# SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
#
# SPDX-License-Identifier: LGPL-3.0-or-later

cmake_minimum_required(VERSION 3.8)

# KDE Applications version, managed by release script.
set(RELEASE_SERVICE_VERSION_MAJOR "22")
set(RELEASE_SERVICE_VERSION_MINOR "11")
set(RELEASE_SERVICE_VERSION_MICRO "80")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(elisa
    VERSION ${RELEASE_SERVICE_VERSION}
    LANGUAGES CXX)

set(REQUIRED_KF5_VERSION "5.98.0")
find_package(ECM ${REQUIRED_KF5_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMInstallIcons)
include(FeatureSummary)
include(ECMAddAppIcon)
include(ECMAddTests)
include(ECMQtDeclareLoggingCategory)
include(ECMGenerateQmlTypes)

set(REQUIRED_QT_VERSION "5.15.0")
find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} CONFIG REQUIRED Core Network Qml Quick Test Sql Multimedia Svg Gui Widgets QuickTest Concurrent QuickControls2)

find_package(Qt${QT_MAJOR_VERSION}Core ${REQUIRED_QT_VERSION} CONFIG REQUIRED Private)



if (NOT WIN32 AND NOT ANDROID)
    find_package(Qt${QT_MAJOR_VERSION}DBus ${REQUIRED_QT_VERSION} CONFIG QUIET)
    set_package_properties(Qt${QT_MAJOR_VERSION}DBus PROPERTIES
        DESCRIPTION "Qt${QT_MAJOR_VERSION} DBus is needed to provide MPris2 interface to allow remote control by the desktop workspace."
        TYPE OPTIONAL)
endif()

if (Qt${QT_MAJOR_VERSION}DBus_FOUND)
set(QtDBus_FOUND 1)
endif()

find_package(Qt${QT_MAJOR_VERSION}QuickWidgets ${REQUIRED_QT_VERSION} CONFIG QUIET)
set_package_properties(Qt${QT_MAJOR_VERSION}QuickWidgets PROPERTIES
    DESCRIPTION "Qt${QT_MAJOR_VERSION} Quick Widgets is needed at runtime to provide the interface."
    TYPE RUNTIME)

find_package(Qt${QT_MAJOR_VERSION}QuickControls2 ${REQUIRED_QT_VERSION} CONFIG QUIET)
set_package_properties(Qt${QT_MAJOR_VERSION}QuickControls2 PROPERTIES
    DESCRIPTION "Qt${QT_MAJOR_VERSION} Quick Controls version 2 is needed at runtime to provide the interface."
    TYPE RUNTIME)

if (ANDROID)
    find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} CONFIG QUIET OPTIONAL_COMPONENTS AndroidExtras)
    set_package_properties(Qt5AndroidExtras PROPERTIES
        DESCRIPTION "Qt5 AndroidExtras is needed to provide the Android integration."
        TYPE REQUIRED)
    set(QtAndroidExtras_FOUND 1)
endif()

find_package(KF5Kirigami2 ${REQUIRED_KF5_VERSION} CONFIG QUIET)

set_package_properties(KF5Kirigami2 PROPERTIES
    DESCRIPTION "KF5 Kirigami 2 is needed to provide the mobile UI components."
    TYPE REQUIRED)

find_package(KF5I18n ${REQUIRED_KF5_VERSION} CONFIG QUIET)
set_package_properties(KF5I18n PROPERTIES
    DESCRIPTION "KF5 text internationalization library."
    TYPE REQUIRED)

find_package(KF5CoreAddons ${REQUIRED_KF5_VERSION} CONFIG QUIET)
set_package_properties(KF5CoreAddons PROPERTIES
    DESCRIPTION "Qt addon library with a collection of non-GUI utilities."
    TYPE REQUIRED)

find_package(KF5IconThemes ${REQUIRED_KF5_VERSION} CONFIG QUIET)
set_package_properties(KF5IconThemes PROPERTIES
    DESCRIPTION "Support for icon themes."
    TYPE REQUIRED)

if (NOT WIN32)
find_package(KF5Baloo ${REQUIRED_KF5_VERSION} CONFIG QUIET)
set_package_properties(KF5Baloo PROPERTIES
    DESCRIPTION "Baloo provides file searching and indexing."
    TYPE RECOMMENDED)
endif()

find_package(KF5FileMetaData ${REQUIRED_KF5_VERSION} CONFIG QUIET)
set_package_properties(KF5FileMetaData PROPERTIES
    DESCRIPTION "Provides a simple library for extracting metadata."
    TYPE RECOMMENDED)

find_package(KF5DocTools ${REQUIRED_KF5_VERSION} CONFIG QUIET)
set_package_properties(KF5DocTools PROPERTIES
    DESCRIPTION "Create documentation from DocBook library."
    TYPE OPTIONAL)

find_package(KF5XmlGui ${REQUIRED_KF5_VERSION} CONFIG QUIET)
set_package_properties(KF5XmlGui PROPERTIES
    DESCRIPTION "Framework for managing menu and toolbar actions."
    TYPE RECOMMENDED)

find_package(KF5Config ${REQUIRED_KF5_VERSION} CONFIG QUIET)
set_package_properties(KF5Config PROPERTIES
    DESCRIPTION "Persistent platform-independent application settings."
    TYPE REQUIRED)

find_package(KF5ConfigWidgets ${REQUIRED_KF5_VERSION} CONFIG QUIET)
set_package_properties(KF5ConfigWidgets PROPERTIES
    DESCRIPTION "Widgets for configuration dialogs."
    TYPE RECOMMENDED)

find_package(KF5Crash ${REQUIRED_KF5_VERSION} CONFIG QUIET)
set_package_properties(KF5Crash PROPERTIES
    DESCRIPTION "Graceful handling of application crashes."
    TYPE OPTIONAL)

if (NOT WIN32 AND NOT ANDROID)
    find_package(KF5DBusAddons ${REQUIRED_KF5_VERSION} CONFIG QUIET)
    set_package_properties(KF5DBusAddons PROPERTIES
        DESCRIPTION "Convenience classes for D-Bus."
        TYPE OPTIONAL)
endif()

find_package(KF5Package ${REQUIRED_KF5_VERSION} CONFIG QUIET)
set_package_properties(KF5Package PROPERTIES
    DESCRIPTION "KF5 package management library needed to get the configuration dialogs."
    TYPE RECOMMENDED)

if (NOT ANDROID)
    find_package(KF5KIO ${REQUIRED_KF5_VERSION} CONFIG QUIET)
    set_package_properties(KF5KIO PROPERTIES
        DESCRIPTION "File management libraries used for file browsing."
        TYPE REQUIRED)
endif()

find_package(UPNPQT CONFIG QUIET)
set_package_properties(UPNPQT PROPERTIES
    DESCRIPTION "UPNP layer build with Qt 5. UPnP support is currently broken. You should probably avoid this dependency."
    URL "https://gitlab.com/homeautomationqt/upnp-player-qt"
    TYPE OPTIONAL)

if (UPNPQT_FOUND)
    message(WARNING "UPnP support is experimental and may not work.")
endif()

find_package(LIBVLC QUIET)
set_package_properties(LIBVLC PROPERTIES
    DESCRIPTION "libvlc allows to play music in Elisa (otherwise it will use QtMultimedia)"
    URL "https://www.videolan.org/vlc/libvlc.html"
    TYPE RECOMMENDED)

include(FeatureSummary)
include(GenerateExportHeader)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMGenerateDBusServiceFile)
include(CMakePackageConfigHelpers)

configure_file(config-upnp-qt.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-upnp-qt.h)

ecm_setup_version(${RELEASE_SERVICE_VERSION}
    VARIABLE_PREFIX ELISA
    VERSION_HEADER elisa-version.h)

set(QML_IMPORT_PATH ${CMAKE_BINARY_DIR}/bin CACHE STRING "" FORCE)

add_subdirectory(src)
add_subdirectory(icons)
if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()
add_subdirectory(doc)

if (KF5DBusAddons_FOUND)
    ecm_generate_dbus_service_file(
        NAME org.kde.elisa
        EXECUTABLE ${KDE_INSTALL_FULL_BINDIR}/elisa
        DESTINATION ${KDE_INSTALL_DBUSSERVICEDIR}
    )
    set(ELISA_DBUSACTIVATABLE "DBusActivatable=true")
else()
    set(ELISA_DBUSACTIVATABLE)
endif()

configure_file(org.kde.elisa.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/org.kde.elisa.desktop @ONLY)
install(
    PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/org.kde.elisa.desktop
    DESTINATION ${KDE_INSTALL_APPDIR}
)

install(
    FILES org.kde.elisa.appdata.xml
    DESTINATION ${KDE_INSTALL_METAINFODIR}
)

ki18n_install(po)
if (KF5DocTools_FOUND)
    kdoctools_install(po)
endif()

install(FILES elisa.categories DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR})

if (ANDROID)
    file(COPY androidResources/icon.png androidResources/splash.xml androidResources/vendor_logo.png androidResources/splash_logo.png
        DESTINATION ${CMAKE_SOURCE_DIR}/data/res/drawable)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/version.gradle.in ${CMAKE_BINARY_DIR}/version.gradle)
endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

