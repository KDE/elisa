/*
 * Copyright 2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "modeldataloader.h"

ModelDataLoader::ModelDataLoader(QObject *parent) : QObject(parent)
{
}

void ModelDataLoader::setDatabase(DatabaseInterface *database)
{
    mDatabase = database;
}

void ModelDataLoader::loadData(ElisaUtils::PlayListEntryType dataType)
{
    if (!mDatabase) {
        return;
    }

    switch (dataType)
    {
    case ElisaUtils::Album:
        Q_EMIT allAlbumsData(mDatabase->allAlbumsData());
        break;
    case ElisaUtils::Artist:
        Q_EMIT allArtistsData(mDatabase->allArtistsData());
        break;
    case ElisaUtils::Composer:
        break;
    case ElisaUtils::Genre:
        Q_EMIT allGenresData(mDatabase->allGenresData());
        break;
    case ElisaUtils::Lyricist:
        break;
    case ElisaUtils::Track:
        Q_EMIT allTracksData(mDatabase->allTracksData());
        break;
    case ElisaUtils::FileName:
    case ElisaUtils::Unknown:
        break;
    }
}


#include "moc_modeldataloader.cpp"
