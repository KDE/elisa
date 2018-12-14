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

#include "albummodel.h"

#include "modeldataloader.h"
#include "musiclistenersmanager.h"

#include <QUrl>
#include <QTimer>
#include <QPointer>
#include <QVector>

class AlbumModelPrivate
{
public:

    QString mAlbumTitle;

    QString mAlbumArtist;

    AlbumModel::ListTrackDataType mCurrentAlbum;

    ModelDataLoader mDataLoader;

};

AlbumModel::AlbumModel(QObject *parent) : QAbstractItemModel(parent), d(std::make_unique<AlbumModelPrivate>())
{
}

AlbumModel::~AlbumModel()
= default;

int AlbumModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return d->mCurrentAlbum.count();
}

QHash<int, QByteArray> AlbumModel::roleNames() const
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

Qt::ItemFlags AlbumModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant AlbumModel::data(const QModelIndex &index, int role) const
{
    auto result = QVariant();

    Q_ASSERT(index.isValid());
    Q_ASSERT(index.column() == 0);
    Q_ASSERT(index.row() >= 0 && index.row() < d->mCurrentAlbum.count());
    Q_ASSERT(!index.parent().isValid());
    Q_ASSERT(index.model() == this);

    const auto &currentTrack = d->mCurrentAlbum[index.row()];

    switch(role)
    {
    case DatabaseInterface::ColumnsRoles::DurationRole:
    {
        auto trackDuration = QTime::fromMSecsSinceStartOfDay(currentTrack.duration());
        if (trackDuration.hour() == 0) {
            result = trackDuration.toString(QStringLiteral("mm:ss"));
        } else {
            result = trackDuration.toString();
        }
        break;
    }
    default:
        result = currentTrack[static_cast<TrackDataType::key_type>(role)];
    }

    return result;
}

QModelIndex AlbumModel::index(int row, int column, const QModelIndex &parent) const
{
    auto result = QModelIndex();

    if (column != 0) {
        return result;
    }

    if (parent.isValid()) {
        return result;
    }

    if (row >= d->mCurrentAlbum.count()) {
        return result;
    }

    return createIndex(row, column);
}

QModelIndex AlbumModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)
    return {};
}

int AlbumModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 1;
}

QString AlbumModel::title() const
{
    return d->mAlbumTitle;
}

QString AlbumModel::author() const
{
    return d->mAlbumArtist;
}

int AlbumModel::tracksCount() const
{
    return d->mCurrentAlbum.count();
}

void AlbumModel::initialize(const QString &albumTitle, const QString &albumArtist,
                            MusicListenersManager *manager)
{
    d->mAlbumTitle = albumTitle;
    d->mAlbumArtist = albumArtist;

    manager->connectModel(&d->mDataLoader);

    connect(manager->viewDatabase(), &DatabaseInterface::tracksAdded,
            this, &AlbumModel::tracksAdded);
    connect(manager->viewDatabase(), &DatabaseInterface::trackModified,
            this, &AlbumModel::trackModified);
    connect(manager->viewDatabase(), &DatabaseInterface::trackRemoved,
            this, &AlbumModel::trackRemoved);
    connect(this, &AlbumModel::needData,
            &d->mDataLoader, &ModelDataLoader::loadData);
    connect(&d->mDataLoader, &ModelDataLoader::allTracksData,
            this, &AlbumModel::tracksAdded);

    Q_EMIT needData(ElisaUtils::Track);
}

int AlbumModel::trackIndexFromId(qulonglong id) const
{
    int result;

    for (result = 0; result < d->mCurrentAlbum.size(); ++result) {
        if (d->mCurrentAlbum[result].databaseId() == id) {
            return result;
        }
    }

    result = -1;

    return result;
}

void AlbumModel::tracksAdded(const ListTrackDataType &newTracks)
{
    for (const auto &newTrack : newTracks) {
        if (newTrack.album() != d->mAlbumTitle) {
            continue;
        }

        if (newTrack.albumArtist() != d->mAlbumArtist) {
            continue;
        }

        auto trackIndex = trackIndexFromId(newTrack.databaseId());

        if (trackIndex != -1) {
            continue;
        }

        bool trackInserted = false;
        for (int trackIndex = 0; trackIndex < d->mCurrentAlbum.count(); ++trackIndex) {
            const auto &oneTrack = d->mCurrentAlbum[trackIndex];

            if (oneTrack.discNumber() == newTrack.discNumber() && oneTrack.trackNumber() > newTrack.trackNumber()) {
                beginInsertRows({}, trackIndex, trackIndex);
                d->mCurrentAlbum.insert(trackIndex, newTrack);
                endInsertRows();
                trackInserted = true;
                break;
            }
        }

        if (!trackInserted) {
            beginInsertRows({}, d->mCurrentAlbum.count(), d->mCurrentAlbum.count());
            d->mCurrentAlbum.insert(d->mCurrentAlbum.count(), newTrack);
            endInsertRows();
        }
    }
}

void AlbumModel::trackModified(const TrackDataType &modifiedTrack)
{
    if (modifiedTrack.album() != d->mAlbumTitle) {
        return;
    }

    auto trackIndex = trackIndexFromId(modifiedTrack.databaseId());

    if (trackIndex == -1) {
        return;
    }

    d->mCurrentAlbum[trackIndex] = modifiedTrack;
    Q_EMIT dataChanged(index(trackIndex, 0), index(trackIndex, 0));
}

void AlbumModel::trackRemoved(qulonglong trackId)
{
    auto trackIndex = trackIndexFromId(trackId);

    if (trackIndex == -1) {
        return;
    }

    beginRemoveRows({}, trackIndex, trackIndex);
    d->mCurrentAlbum.removeAt(trackIndex);
    endRemoveRows();
}

#include "moc_albummodel.cpp"
