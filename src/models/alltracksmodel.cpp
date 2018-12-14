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

#include "alltracksmodel.h"

#include "modeldataloader.h"
#include "musiclistenersmanager.h"

#include <algorithm>

#include <QDebug>

class AllTracksModelPrivate
{
public:

    AllTracksModel::ListTrackDataType mAllTracks;

    ModelDataLoader mDataLoader;

};

AllTracksModel::AllTracksModel(QObject *parent) : QAbstractItemModel(parent), d(std::make_unique<AllTracksModelPrivate>())
{
}

AllTracksModel::~AllTracksModel()
= default;

int AllTracksModel::rowCount(const QModelIndex &parent) const
{
    auto tracksCount = 0;

    if (parent.isValid()) {
        return tracksCount;
    }

    tracksCount = d->mAllTracks.size();

    return tracksCount;
}

QHash<int, QByteArray> AllTracksModel::roleNames() const
{
    auto roles = QAbstractItemModel::roleNames();

    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::DatabaseIdRole)] = "databaseId";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::TitleRole)] = "title";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::ArtistRole)] = "artist";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::AlbumRole)] = "album";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::AlbumArtistRole)] = "albumArtist";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::DurationRole)] = "duration";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::ImageUrlRole)] = "imageUrl";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::TrackNumberRole)] = "trackNumber";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::DiscNumberRole)] = "discNumber";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::RatingRole)] = "rating";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::IsSingleDiscAlbumRole)] = "isSingleDiscAlbum";

    return roles;
}

Qt::ItemFlags AllTracksModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant AllTracksModel::data(const QModelIndex &index, int role) const
{
    auto result = QVariant();

    const auto tracksCount = d->mAllTracks.size();

    Q_ASSERT(index.isValid());
    Q_ASSERT(index.column() == 0);
    Q_ASSERT(index.row() >= 0 && index.row() < tracksCount);
    Q_ASSERT(!index.parent().isValid());
    Q_ASSERT(index.model() == this);
    Q_ASSERT(index.internalId() == 0);

    const auto &track = d->mAllTracks[index.row()];

    switch(role)
    {
    case DatabaseInterface::ColumnsRoles::DurationRole:
    {
        auto trackDuration = QTime::fromMSecsSinceStartOfDay(track.duration());
        if (trackDuration.hour() == 0) {
            result = trackDuration.toString(QStringLiteral("mm:ss"));
        } else {
            result = trackDuration.toString();
        }
        break;
    }
    default:
        result = track[static_cast<TrackDataType::key_type>(role)];
    }

    return result;
}

QModelIndex AllTracksModel::index(int row, int column, const QModelIndex &parent) const
{
    auto result = QModelIndex();

    if (column != 0) {
        return result;
    }

    if (parent.isValid()) {
        return result;
    }

    if (row > d->mAllTracks.size() - 1) {
        return result;
    }

    result = createIndex(row, column);

    return result;
}

QModelIndex AllTracksModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)

    auto result = QModelIndex();

    return result;
}

int AllTracksModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 1;
}

void AllTracksModel::tracksAdded(ListTrackDataType allTracks)
{
    if (allTracks.isEmpty()) {
        return;
    }

    if (d->mAllTracks.isEmpty()) {
        beginInsertRows({}, 0, allTracks.size() - 1);
        d->mAllTracks.swap(allTracks);
        endInsertRows();
    } else {
        beginInsertRows({}, d->mAllTracks.size(), d->mAllTracks.size() + allTracks.size() - 1);
        d->mAllTracks.append(allTracks);
        endInsertRows();
    }
}

void AllTracksModel::trackRemoved(qulonglong removedTrackId)
{
    auto itTrack = std::find_if(d->mAllTracks.begin(), d->mAllTracks.end(),
                                [removedTrackId](auto track) {return track.databaseId() == removedTrackId;});

    if (itTrack == d->mAllTracks.end()) {
        return;
    }

    auto position = itTrack - d->mAllTracks.begin();

    beginRemoveRows({}, position, position);
    d->mAllTracks.erase(itTrack);
    endRemoveRows();
}

void AllTracksModel::trackModified(const TrackDataType &modifiedTrack)
{
    auto itTrack = std::find_if(d->mAllTracks.begin(), d->mAllTracks.end(),
                                [modifiedTrack](auto track) {
        return track.databaseId() == modifiedTrack.databaseId();
    });

    if (itTrack == d->mAllTracks.end()) {
        return;
    }

    auto position = itTrack - d->mAllTracks.begin();

    d->mAllTracks[position] = modifiedTrack;

    Q_EMIT dataChanged(index(position, 0), index(position, 0));
}

void AllTracksModel::initialize(MusicListenersManager *manager)
{
    manager->connectModel(&d->mDataLoader);

    connect(manager->viewDatabase(), &DatabaseInterface::tracksAdded,
            this, &AllTracksModel::tracksAdded);
    connect(manager->viewDatabase(), &DatabaseInterface::trackModified,
            this, &AllTracksModel::trackModified);
    connect(manager->viewDatabase(), &DatabaseInterface::trackRemoved,
            this, &AllTracksModel::trackRemoved);
    connect(this, &AllTracksModel::needData,
            &d->mDataLoader, &ModelDataLoader::loadData);
    connect(&d->mDataLoader, &ModelDataLoader::allTracksData,
            this, &AllTracksModel::tracksAdded);

    Q_EMIT needData(ElisaUtils::Track);
}


#include "moc_alltracksmodel.cpp"
