/*
 * Copyright 2015-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "allgenresmodel.h"

#include "modeldataloader.h"
#include "musiclistenersmanager.h"

#include <QUrl>
#include <QTimer>
#include <QPointer>
#include <QVector>
#include <QDebug>

#include <algorithm>

class AllGenresModelPrivate
{
public:

    AllGenresModel::ListGenreDataType mAllData;

    ModelDataLoader mDataLoader;

};

AllGenresModel::AllGenresModel(QObject *parent) : QAbstractListModel(parent), d(std::make_unique<AllGenresModelPrivate>())
{
}

AllGenresModel::~AllGenresModel()
= default;

int AllGenresModel::rowCount(const QModelIndex &parent) const
{
    auto dataCount = 0;

    if (parent.isValid()) {
        return dataCount;
    }

    dataCount = d->mAllData.size();

    return dataCount;
}

QHash<int, QByteArray> AllGenresModel::roleNames() const
{
    auto roles = QAbstractListModel::roleNames();

    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::TitleRole)] = "title";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::SecondaryTextRole)] = "secondaryText";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::ImageUrlRole)] = "imageUrl";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::DatabaseIdRole)] = "databaseId";

    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::ArtistRole)] = "artist";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::AllArtistsRole)] = "allArtists";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::HighestTrackRating)] = "highestTrackRating";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::GenreRole)] = "genre";

    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::AlbumRole)] = "album";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::AlbumArtistRole)] = "albumArtist";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::DurationRole)] = "duration";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::TrackNumberRole)] = "trackNumber";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::DiscNumberRole)] = "discNumber";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::RatingRole)] = "rating";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::IsSingleDiscAlbumRole)] = "isSingleDiscAlbum";

    return roles;
}

Qt::ItemFlags AllGenresModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant AllGenresModel::data(const QModelIndex &index, int role) const
{
    auto result = QVariant();

    const auto dataCount = d->mAllData.size();

    Q_ASSERT(index.isValid());
    Q_ASSERT(index.column() == 0);
    Q_ASSERT(index.row() >= 0 && index.row() < dataCount);
    Q_ASSERT(!index.parent().isValid());
    Q_ASSERT(index.model() == this);
    Q_ASSERT(index.internalId() == 0);

    switch(role)
    {
    case Qt::DisplayRole:
        result = d->mAllData[index.row()][GenreDataType::key_type::TitleRole];
        break;
    default:
        result = d->mAllData[index.row()][static_cast<GenreDataType::key_type>(role)];
    }

    return result;
}

QModelIndex AllGenresModel::index(int row, int column, const QModelIndex &parent) const
{
    auto result = QModelIndex();

    if (column != 0) {
        return result;
    }

    if (parent.isValid()) {
        return result;
    }

    result = createIndex(row, column);

    return result;
}

QModelIndex AllGenresModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)

    auto result = QModelIndex();

    return result;
}

void AllGenresModel::genresAdded(ListGenreDataType newData)
{
    if (d->mAllData.isEmpty()) {
        beginInsertRows({}, d->mAllData.size(), newData.size() - 1);
        d->mAllData.swap(newData);
        endInsertRows();
    } else {
        beginInsertRows({}, d->mAllData.size(), d->mAllData.size() + newData.size() - 1);
        d->mAllData.append(newData);
        endInsertRows();
    }
}

void AllGenresModel::initialize(MusicListenersManager *manager)
{
    manager->connectModel(&d->mDataLoader);

    connect(manager->viewDatabase(), &DatabaseInterface::genresAdded,
            this, &AllGenresModel::genresAdded);
    connect(this, &AllGenresModel::needData,
            &d->mDataLoader, &ModelDataLoader::loadData);
    connect(&d->mDataLoader, &ModelDataLoader::allGenresData,
            this, &AllGenresModel::genresAdded);

    Q_EMIT needData(ElisaUtils::Genre);
}

#include "moc_allgenresmodel.cpp"
