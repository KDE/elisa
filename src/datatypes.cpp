/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2019 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "datatypes.h"

bool DataTypes::TrackDataType::albumInfoIsSame(const TrackDataType &other) const
{
    return hasAlbum() == other.hasAlbum() && album() == other.album() &&
           hasAlbumArtist() == other.hasAlbumArtist() && albumArtist() == other.albumArtist();
}


#include "moc_datatypes.cpp"
