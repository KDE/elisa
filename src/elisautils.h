/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef ELISAUTILS_H
#define ELISAUTILS_H

#include "elisaLib_export.h"

#include "datatypes.h"

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

using EntryData = std::tuple<DataTypes::TrackDataType, QString, QUrl>;
using EntryDataList = QList<EntryData>;

enum PlayListEntryType {
    Album,
    Artist,
    Genre,
    Lyricist,
    Composer,
    Track,
    FileName,
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
};

Q_ENUM_NS(FilterType)

}

Q_DECLARE_METATYPE(ElisaUtils::EntryData)
Q_DECLARE_METATYPE(ElisaUtils::EntryDataList)

#endif // ELISAUTILS_H
