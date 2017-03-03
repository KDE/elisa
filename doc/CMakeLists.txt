# documentation

if (KF5DocTools_FOUND)
    KDOCTOOLS_CREATE_HANDBOOK(
        index.docbook
            INSTALL_DESTINATION ${KDE_INSTALL_DOCBUNDLEDIR}/en
            SUBDIR elisa
    )
endif()
