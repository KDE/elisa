# CMake module to search for LIBVLC (VLC library)
#
# SPDX-FileCopyrightText: 2010 (c) Rohit Yadav <rohityadav89@gmail.com>
# SPDX-FileCopyrightText: 2011 (c) Harald Sitter <sitter@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
# If it's found it sets LIBVLC_FOUND to TRUE
# and following variables are set:
#    LIBVLC_INCLUDE_DIR
#    LIBVLC_LIBRARY
#    LIBVLC_VERSION

set(LIBVLC_INCLUDE_DIR LIBVLC_INCLUDE_DIR-NOTFOUND)
set(LIBVLC_LIBRARY LIBVLC_LIBRARY-NOTFOUND)
set(LIBVLCCORE_LIBRARY LIBVLCCORE_LIBRARY-NOTFOUND)

if(NOT LIBVLC_MIN_VERSION)
    set(LIBVLC_MIN_VERSION "0.0")
endif(NOT LIBVLC_MIN_VERSION)

# find_path and find_library normally search standard locations
# before the specified paths. To search non-standard paths first,
# FIND_* is invoked first with specified paths and NO_DEFAULT_PATH
# and then again with no specified paths to search the default
# locations. When an earlier FIND_* succeeds, subsequent FIND_*s
# searching for the same item do nothing.

if (NOT WIN32)
    find_package(PkgConfig)
    pkg_check_modules(PC_LIBVLC libvlc)
    set(LIBVLC_DEFINITIONS ${PC_LIBVLC_CFLAGS_OTHER})
endif (NOT WIN32)

#Put here path to custom location
#example: /home/user/vlc/include etc..
find_path(LIBVLC_INCLUDE_DIR vlc/vlc.h
HINTS "$ENV{LIBVLC_INCLUDE_PATH}" ${PC_LIBVLC_INCLUDEDIR} ${PC_LIBVLC_INCLUDE_DIRS}
PATHS
    "$ENV{LIB_DIR}/include"
    "$ENV{LIB_DIR}/include/vlc"
    "/usr/include"
    "/usr/include/vlc"
    "/usr/local/include"
    "/usr/local/include/vlc"
    #mingw
    c:/msys/local/include
)
find_path(LIBVLC_INCLUDE_DIR PATHS "${CMAKE_INCLUDE_PATH}/vlc" NAMES vlc.h
        HINTS ${PC_LIBVLC_INCLUDEDIR} ${PC_LIBVLC_INCLUDE_DIRS})

#Put here path to custom location
#example: /home/user/vlc/lib etc..
find_library(LIBVLC_LIBRARY NAMES vlc libvlc
HINTS "$ENV{LIBVLC_LIBRARY_PATH}" ${PC_LIBVLC_LIBDIR} ${PC_LIBVLC_LIBRARY_DIRS}
PATHS
    "$ENV{LIB_DIR}/lib"
    #mingw
    c:/msys/local/lib
)
find_library(LIBVLC_LIBRARY NAMES vlc libvlc)
find_library(LIBVLCCORE_LIBRARY NAMES vlccore libvlccore
HINTS "$ENV{LIBVLC_LIBRARY_PATH}" ${PC_LIBVLC_LIBDIR} ${PC_LIBVLC_LIBRARY_DIRS}
PATHS
    "$ENV{LIB_DIR}/lib"
    #mingw
    c:/msys/local/lib
)
find_library(LIBVLCCORE_LIBRARY NAMES vlccore libvlccore)

set(LIBVLC_VERSION ${PC_LIBVLC_VERSION})
if (LIBVLC_INCLUDE_DIR AND NOT LIBVLC_VERSION)
    file(READ "${LIBVLC_INCLUDE_DIR}/vlc/libvlc_version.h" _libvlc_version_h)

    string(REGEX MATCH "# define LIBVLC_VERSION_MAJOR +\\(([0-9])\\)" _dummy "${_libvlc_version_h}")
    set(_version_major "${CMAKE_MATCH_1}")

    string(REGEX MATCH "# define LIBVLC_VERSION_MINOR +\\(([0-9])\\)" _dummy "${_libvlc_version_h}")
    set(_version_minor "${CMAKE_MATCH_1}")

    string(REGEX MATCH "# define LIBVLC_VERSION_REVISION +\\(([0-9])\\)" _dummy "${_libvlc_version_h}")
    set(_version_revision "${CMAKE_MATCH_1}")

    # Optionally, one could also parse LIBVLC_VERSION_EXTRA, but it does not
    # seem to be used by libvlc.pc.

    set(LIBVLC_VERSION "${_version_major}.${_version_minor}.${_version_revision}")
endif (LIBVLC_INCLUDE_DIR AND NOT LIBVLC_VERSION)

if (LIBVLC_INCLUDE_DIR AND LIBVLC_LIBRARY AND LIBVLCCORE_LIBRARY)
set(LIBVLC_FOUND TRUE)
endif (LIBVLC_INCLUDE_DIR AND LIBVLC_LIBRARY AND LIBVLCCORE_LIBRARY)

if (LIBVLC_VERSION STRLESS "${LIBVLC_MIN_VERSION}")
    message(WARNING "LibVLC version not found: version searched: ${LIBVLC_MIN_VERSION}, found ${LIBVLC_VERSION}\nUnless you are on Windows this is bound to fail.")
# TODO: only activate once version detection can be garunteed (which is currently not the case on windows)
#     set(LIBVLC_FOUND FALSE)
endif (LIBVLC_VERSION STRLESS "${LIBVLC_MIN_VERSION}")

if (LIBVLC_FOUND)
    if (NOT LIBVLC_FIND_QUIETLY)
        message(STATUS "Found LibVLC include-dir path: ${LIBVLC_INCLUDE_DIR}")
        message(STATUS "Found LibVLC library path:${LIBVLC_LIBRARY}")
        message(STATUS "Found LibVLCcore library path:${LIBVLCCORE_LIBRARY}")
        message(STATUS "Found LibVLC version: ${LIBVLC_VERSION} (searched for: ${LIBVLC_MIN_VERSION})")
    endif (NOT LIBVLC_FIND_QUIETLY)
else (LIBVLC_FOUND)
    if (LIBVLC_FIND_REQUIRED)
        message(FATAL_ERROR "Could not find LibVLC")
    endif (LIBVLC_FIND_REQUIRED)
endif (LIBVLC_FOUND)
