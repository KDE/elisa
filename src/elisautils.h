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
#include <QMimeType>
#include <QQmlEngine>

namespace ElisaUtils {

Q_NAMESPACE

QML_NAMED_ELEMENT(ElisaUtils)

enum PlayListEnqueueMode {
    AppendPlayList,
    ReplacePlayList,
    AfterCurrentTrack,
};

Q_ENUM_NS(PlayListEnqueueMode)

enum PlayListEnqueueTriggerPlay {
    DoNotTriggerPlay,
    TriggerPlay,
};

Q_ENUM_NS(PlayListEnqueueTriggerPlay)

enum SkipReason {
    Automatic,  // e.g. song ended
    Manual,     // e.g. user pressed skip forward button

};

Q_ENUM_NS(SkipReason)

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
    PlayList,
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

bool isPlayList(const QMimeType& mimeType);

}

#endif // ELISAUTILS_H
