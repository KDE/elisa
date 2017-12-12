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
    KF5::ConfigCore
    KF5::CoreAddons
    KF5::Declarative
    KF5::I18n
    KF5::QuickAddons
    KF5::ConfigWidgets
)

kcoreaddons_desktop_to_json(kcm_elisa_local_file "kcm_elisa_local_file.desktop")

install(FILES kcm_elisa_local_file.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kcm_elisa_local_file DESTINATION ${PLUGIN_INSTALL_DIR}/kcms)

kpackage_install_package(package kcm_elisa_local_file kcms)
