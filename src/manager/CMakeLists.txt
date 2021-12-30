# SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
#
# SPDX-License-Identifier: LGPL-3.0-or-later

include_directories(${elisa_BINARY_DIR})

set(elisaManager_SOURCES
    main.cpp
    elisaManager.qrc
)

add_executable(elisaManager ${elisaManager_SOURCES})

target_include_directories(elisaManager PRIVATE ${KDSoap_INCLUDE_DIRS})

target_link_libraries(elisaManager
    LINK_PRIVATE
    elisaLib
    Qt::Widgets Qt::QuickControls2
    KF5::I18n KF5::CoreAddons KF5::ConfigCore KF5::ConfigGui
    KF5::IconThemes
    )

if (ANDROID)
    target_link_libraries(elisaManager
        PRIVATE
        Qt::AndroidExtras Qt::Svg Qt::Sql Qt::Concurrent KF5::Kirigami2
    )
endif()

if (KF5Crash_FOUND)
    target_link_libraries(elisaManager
        LINK_PRIVATE
        KF5::Crash
        )
endif()

if (KF5Declarative_FOUND)
    target_link_libraries(elisaManager
        LINK_PRIVATE
        KF5::Declarative
        KF5::QuickAddons
        )
endif()

if (NOT APPLE AND NOT WIN32)
    set_target_properties(elisaManager PROPERTIES
        INSTALL_RPATH "${KDE_INSTALL_FULL_LIBDIR}/elisa;${CMAKE_INSTALL_RPATH}"
        )
endif()

install(TARGETS elisaManager ${INSTALL_TARGETS_DEFAULT_ARGS})
