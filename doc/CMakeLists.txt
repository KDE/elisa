# SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
#
# SPDX-License-Identifier: LGPL-3.0-or-later

# documentation

if (KF5DocTools_FOUND)
    KDOCTOOLS_CREATE_HANDBOOK(
        index.docbook
            INSTALL_DESTINATION ${KDE_INSTALL_DOCBUNDLEDIR}/en
            SUBDIR elisa
    )
endif()
