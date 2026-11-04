/*
    SPDX-FileCopyrightText: 2026 Ramil Nurmanov <ramil2004nur@gmail.com>

    SPDX-License-Identifier: LGPL-3.0-or-later
*/

#ifndef KIOFUSEHELPER_H
#define KIOFUSEHELPER_H

#include "elisaLib_export.h"
#include <QUrl>

namespace KioFuseHelper
{
ELISALIB_EXPORT QUrl mostLocalUrl(const QUrl &source);
}

#endif // KIOFUSEHELPER_H
