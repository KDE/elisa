set(KCM_ELISA_LOCAL_FILE_SRCS
    localfileconfiguration.cpp
    kcm_elisa_local_file.desktop
    package/contents/ui/main.qml
    package/metadata.desktop
)

kconfig_add_kcfg_files(KCM_ELISA_LOCAL_FILE_SRCS ../elisa_settings.kcfgc )
set(KCM_ELISA_LOCAL_FILE_SRCS
    ${KCM_ELISA_LOCAL_FILE_SRCS}
    ../elisa_core.kcfg
)

add_library(kcm_elisa_local_file MODULE ${KCM_ELISA_LOCAL_FILE_SRCS})

target_link_libraries(kcm_elisa_local_file
    Qt5::Core
    KF5::ConfigCore
    KF5::I18n
    KF5::CoreAddons
)

if (KF5Declarative_FOUND)
    target_link_libraries(kcm_elisa_local_file
        KF5::Declarative
        KF5::QuickAddons
    )
endif()

if (KF5ConfigWidgets_FOUND)
    target_link_libraries(kcm_elisa_local_file
        KF5::ConfigWidgets
    )
endif()

kcoreaddons_desktop_to_json(kcm_elisa_local_file "kcm_elisa_local_file.desktop")

install(FILES kcm_elisa_local_file.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
install(TARGETS kcm_elisa_local_file DESTINATION ${KDE_INSTALL_PLUGINDIR}/kcms)

if (KF5Package_FOUND)
    kpackage_install_bundled_package(package kcm_elisa_local_file kcms)
endif()
