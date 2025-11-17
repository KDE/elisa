# SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
#
# SPDX-License-Identifier: LGPL-3.0-or-later

cmake_minimum_required(VERSION 3.16)

# KDE Applications version, managed by release script.
set(RELEASE_SERVICE_VERSION_MAJOR "26")
set(RELEASE_SERVICE_VERSION_MINOR "03")
set(RELEASE_SERVICE_VERSION_MICRO "70")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(elisa
    VERSION ${RELEASE_SERVICE_VERSION}
    LANGUAGES CXX)

set(REQUIRED_KF_VERSION "6.9")
find_package(ECM ${REQUIRED_KF_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEClangFormat)
include(KDEGitCommitHooks)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMInstallIcons)
include(FeatureSummary)
include(ECMAddAppIcon)
include(ECMAddTests)
include(ECMQtDeclareLoggingCategory)
include(ECMGenerateQmlTypes)
include(ECMAddAndroidApk)
include(ECMFindQmlModule)

set(REQUIRED_QT_VERSION "6.6.0")
find_package(Qt6 ${REQUIRED_QT_VERSION} CONFIG REQUIRED COMPONENTS
    Concurrent
    Core
    Gui
    Multimedia
    Network
    Qml
    Quick
    QuickControls2
    QuickTest
    Sql
    Svg
    Test
    Widgets
)

find_package(Qt6Core ${REQUIRED_QT_VERSION} CONFIG REQUIRED Private)



if (NOT WIN32 AND NOT ANDROID)
    find_package(Qt6DBus ${REQUIRED_QT_VERSION} CONFIG QUIET)
    set_package_properties(Qt6DBus PROPERTIES
        DESCRIPTION "Qt6 DBus is needed to provide MPris2 interface to allow remote control by the desktop workspace."
        TYPE OPTIONAL)
endif()

if (Qt6DBus_FOUND)
set(QtDBus_FOUND 1)
endif()

find_package(KF6I18n ${REQUIRED_KF_VERSION} CONFIG QUIET)
set_package_properties(KF6I18n PROPERTIES
    DESCRIPTION "KF text internationalization library."
    TYPE REQUIRED)

find_package(KF6CoreAddons ${REQUIRED_KF_VERSION} CONFIG QUIET)
set_package_properties(KF6CoreAddons PROPERTIES
    DESCRIPTION "Qt addon library with a collection of non-GUI utilities."
    TYPE REQUIRED)

find_package(KF6IconThemes ${REQUIRED_KF_VERSION} CONFIG QUIET)
set_package_properties(KF6IconThemes PROPERTIES
    DESCRIPTION "Support for icon themes."
    TYPE REQUIRED)

find_package(KF6FileMetaData ${REQUIRED_KF_VERSION} CONFIG QUIET)
set_package_properties(KF6FileMetaData PROPERTIES
    DESCRIPTION "Provides a simple library for extracting metadata."
    TYPE RECOMMENDED)

find_package(KF6XmlGui ${REQUIRED_KF_VERSION} CONFIG QUIET)
set_package_properties(KF6XmlGui PROPERTIES
    DESCRIPTION "Framework for managing menu and toolbar actions."
    TYPE RECOMMENDED)

find_package(KF6Config ${REQUIRED_KF_VERSION} CONFIG QUIET)
set_package_properties(KF6Config PROPERTIES
    DESCRIPTION "Persistent platform-independent application settings."
    TYPE REQUIRED)

find_package(KF6ColorScheme ${REQUIRED_KF_VERSION} CONFIG QUIET)
set_package_properties(KF6ColorScheme PROPERTIES
    DESCRIPTION "Allows changing the app's color scheme."
    TYPE REQUIRED)

find_package(KF6Codecs ${REQUIRED_KF_VERSION} CONFIG QUIET)
set_package_properties(KF6Codecs PROPERTIES
    DESCRIPTION "Allows probing lyrics text codec."
    TYPE REQUIRED)

find_package(KF6Crash ${REQUIRED_KF_VERSION} CONFIG QUIET)
set_package_properties(KF6Crash PROPERTIES
    DESCRIPTION "Graceful handling of application crashes."
    TYPE OPTIONAL)

if (NOT WIN32 AND NOT ANDROID)
    find_package(KF6DBusAddons ${REQUIRED_KF_VERSION} CONFIG QUIET)
    set_package_properties(KF6DBusAddons PROPERTIES
        DESCRIPTION "Convenience classes for D-Bus."
        TYPE OPTIONAL)
endif()

if (ANDROID)
    find_package(Qt6 ${REQUIRED_QT_VERSION} CONFIG REQUIRED COMPONENTS CorePrivate)
    find_package(KF6Kirigami ${REQUIRED_KF_VERSION} CONFIG QUIET)
    set_package_properties(KF6Kirigami PROPERTIES
        DESCRIPTION "Used to package icons on Android."
        TYPE REQUIRED)
else()
    find_package(KF6KIO ${REQUIRED_KF_VERSION} CONFIG QUIET)
    set_package_properties(KF6KIO PROPERTIES
        DESCRIPTION "File management libraries used for file browsing."
        TYPE REQUIRED)
    find_package(KF6QQC2DesktopStyle ${REQUIRED_KF_VERSION} CONFIG QUIET)
    set_package_properties(KF6QQC2DesktopStyle PROPERTIES
        DESCRIPTION "KDE QtQuick style for desktop"
        TYPE RUNTIME)
endif()

find_package(UPNPQT CONFIG QUIET)
set_package_properties(UPNPQT PROPERTIES
    DESCRIPTION "UPNP layer build with Qt. UPnP support is currently broken. You should probably avoid this dependency."
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

ecm_find_qmlmodule(org.kde.kirigami 2.20)
ecm_find_qmlmodule(org.kde.kirigamiaddons.formcard 1.0)

include(FeatureSummary)
include(GenerateExportHeader)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMGenerateDBusServiceFile)
include(CMakePackageConfigHelpers)
include(ECMDeprecationSettings)

set(KFXmlGui_FOUND ${KF6XmlGui_FOUND})
set(KFCrash_FOUND ${KF6Crash_FOUND})
set(KFDBusAddons_FOUND ${KF6DBusAddons_FOUND})
set(KFKIO_FOUND ${KF6KIO_FOUND})
set(KFFileMetaData_FOUND ${KF6FileMetaData_FOUND})

configure_file(config-upnp-qt.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-upnp-qt.h)

ecm_setup_version(${RELEASE_SERVICE_VERSION}
    VARIABLE_PREFIX ELISA
    VERSION_HEADER elisa-version.h)

set(QML_IMPORT_PATH ${CMAKE_BINARY_DIR}/bin CACHE STRING "" FORCE)

ecm_set_disabled_deprecation_versions(
    QT 6.1.0
    KF 6.4
)


add_subdirectory(src)
add_subdirectory(icons)
if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()

if (KF6DBusAddons_FOUND)
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

install(FILES elisa.categories DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR})

if (ANDROID)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/android/version.gradle.in ${CMAKE_BINARY_DIR}/version.gradle)
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

# add clang-format target for all our real source files
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
