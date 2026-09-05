/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "elisautils.h"

#include <QMimeType>

namespace ElisaUtils
{

std::optional<PlaylistFormat> playlistFormatForType(const QMimeType &mimeType)
{
    if (mimeType.inherits(QStringLiteral("audio/x-scpls"))) {
        return PlaylistFormat::Pls;
    }
    // M3U: checked via name as it can be both m3u and m3u8
    if (mimeType.name().contains(QStringLiteral("mpegurl"))) {
        return PlaylistFormat::M3u;
    }
    return {};
}

bool isPlayList(const QMimeType& mimeType)
{
    return playlistFormatForType(mimeType).has_value();
}

}

#include "moc_elisautils.cpp"
