# SPDX-FileCopyrightText: 2017 (c) Aleix Pol <aleixpol@kde.org>
#
# SPDX-License-Identifier: LGPL-3.0-or-later

set(elisa_ICONS_PNG
    128-apps-elisa.png
    64-apps-elisa.png
    48-apps-elisa.png
    32-apps-elisa.png
    22-apps-elisa.png
    16-apps-elisa.png
)

ecm_install_icons(ICONS
    sc-apps-elisa.svg
    ${elisa_ICONS_PNG}
DESTINATION ${KDE_INSTALL_ICONDIR} THEME hicolor )

