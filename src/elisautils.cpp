/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "elisautils.h"

namespace ElisaUtils
{

bool isPlayList(const QMimeType& mimeType)
{
    return mimeType.inherits(QStringLiteral("audio/x-ms-wax")) || mimeType.inherits(QStringLiteral("audio/x-scpls"))
        || mimeType.inherits(QStringLiteral("audio/x-mpegurl")) || mimeType.inherits(QStringLiteral("audio/mpegurl"))
        || mimeType.inherits(QStringLiteral("application/mpegurl")) || mimeType.inherits(QStringLiteral("application/x-mpegurl"))
        || mimeType.inherits(QStringLiteral("application/vnd.apple.mpegurl")) || mimeType.inherits(QStringLiteral("application/vnd.apple.mpegurl.audio"))
        || mimeType.inherits(QStringLiteral("audio/vnd.rn-realaudio")) || mimeType.inherits(QStringLiteral("audio/x-pn-realaudio"));
}

}

#include "moc_elisautils.cpp"
