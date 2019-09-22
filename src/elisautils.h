/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef ELISAUTILS_H
#define ELISAUTILS_H

#include "elisaLib_export.h"

#include <QList>
#include <QUrl>
#include <QString>
#include <QMetaType>

namespace ElisaUtils {

Q_NAMESPACE

enum PlayListEnqueueMode {
    AppendPlayList,
    ReplacePlayList,
};

Q_ENUM_NS(PlayListEnqueueMode)

enum PlayListEnqueueTriggerPlay {
    DoNotTriggerPlay,
    TriggerPlay,
};

Q_ENUM_NS(PlayListEnqueueTriggerPlay)

enum PlayListEntryType {
    Album,
    Artist,
    Genre,
    Lyricist,
    Composer,
    Track,
    FileName,
    Container,
    Radio,
    Unknown,
};

Q_ENUM_NS(PlayListEntryType)

enum FilterType {
    UnknownFilter,
    NoFilter,
    FilterById,
    FilterByGenre,
    FilterByArtist,
    FilterByGenreAndArtist,
    FilterByRecentlyPlayed,
    FilterByFrequentlyPlayed,
    FilterByPath,
};

Q_ENUM_NS(FilterType)

enum IconUseColorOverlay
{
    UseColorOverlay = true,
    DoNotUseColorOverlay = false,
};

Q_ENUM_NS(IconUseColorOverlay)

}

#endif // ELISAUTILS_H
