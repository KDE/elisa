/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "alltracksmodel.h"

#include <algorithm>

#include <QDebug>

class AllTracksModelPrivate
{
public:

    QHash<qulonglong, MusicAudioTrack> mAllTracks;

    QList<qulonglong> mIds;

};

AllTracksModel::AllTracksModel(QObject *parent) : QAbstractItemModel(parent), d(new AllTracksModelPrivate)
{
}

AllTracksModel::~AllTracksModel()
{
    delete d;
}

int AllTracksModel::rowCount(const QModelIndex &parent) const
{
    auto albumCount = 0;

    if (parent.isValid()) {
        return albumCount;
    }

    albumCount = d->mAllTracks.size();

    return albumCount;
}

QHash<int, QByteArray> AllTracksModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[static_cast<int>(ColumnsRoles::TitleRole)] = "title";
    roles[static_cast<int>(ColumnsRoles::DurationRole)] = "duration";
    roles[static_cast<int>(ColumnsRoles::ArtistRole)] = "artist";
    roles[static_cast<int>(ColumnsRoles::AlbumRole)] = "album";
    roles[static_cast<int>(ColumnsRoles::TrackNumberRole)] = "trackNumber";
    roles[static_cast<int>(ColumnsRoles::DiscNumberRole)] = "discNumber";
    roles[static_cast<int>(ColumnsRoles::RatingRole)] = "rating";
    roles[static_cast<int>(ColumnsRoles::ImageRole)] = "image";
    roles[static_cast<int>(ColumnsRoles::DatabaseIdRole)] = "databaseId";
    roles[static_cast<int>(ColumnsRoles::TrackDataRole)] = "trackData";

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

    if (!index.isValid()) {
        return result;
    }

    if (index.column() != 0) {
        return result;
    }

    if (index.row() < 0) {
        return result;
    }

    if (index.parent().isValid()) {
        return result;
    }

    if (index.internalId() != 0) {
        return result;
    }

    if (index.row() < 0 || index.row() >= tracksCount) {
        return result;
    }

    ColumnsRoles convertedRole = static_cast<ColumnsRoles>(role);

    switch(convertedRole)
    {
    case ColumnsRoles::TitleRole:
        result = d->mAllTracks[index.row()].title();
        break;
    case ColumnsRoles::MilliSecondsDurationRole:
        result = d->mAllTracks[index.row()].duration().msecsSinceStartOfDay();
        break;
    case ColumnsRoles::DurationRole:
    {
        QTime trackDuration = d->mAllTracks[index.row()].duration();
        if (trackDuration.hour() == 0) {
            result = trackDuration.toString(QStringLiteral("mm:ss"));
        } else {
            result = trackDuration.toString();
        }
        break;
    }
    case ColumnsRoles::CreatorRole:
        result = d->mAllTracks[index.row()].artist();
        break;
    case ColumnsRoles::ArtistRole:
        result = d->mAllTracks[index.row()].artist();
        break;
    case ColumnsRoles::AlbumRole:
        result = d->mAllTracks[index.row()].albumName();
        break;
    case ColumnsRoles::TrackNumberRole:
        result = d->mAllTracks[index.row()].trackNumber();
        break;
    case ColumnsRoles::DiscNumberRole:
    {
        const auto discNumber = d->mAllTracks[index.row()].discNumber();
        if (discNumber > 0) {
            result = discNumber;
        }
        break;
    }
    case ColumnsRoles::RatingRole:
        result = d->mAllTracks[index.row()].rating();
        break;
    case ColumnsRoles::ImageRole:
    {
        const auto &imageUrl = d->mAllTracks[index.row()].albumCover();
        if (imageUrl.isValid()) {
            result = imageUrl;
        }
        break;
    }
    case ColumnsRoles::ResourceRole:
        result = d->mAllTracks[index.row()].resourceURI();
        break;
    case ColumnsRoles::IdRole:
        result = d->mAllTracks[index.row()].title();
        break;
    case ColumnsRoles::DatabaseIdRole:
        result = d->mAllTracks[index.row()].databaseId();
        break;
    case ColumnsRoles::TrackDataRole:
        result = QVariant::fromValue(d->mAllTracks[index.row()]);
        break;
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

void AllTracksModel::tracksAdded(const QList<MusicAudioTrack> &allTracks)
{
    beginInsertRows({}, 0, allTracks.size() - 1);

    for (const auto &oneTrack : allTracks) {
        d->mAllTracks[oneTrack.databaseId()] = oneTrack;
    }

    d->mIds = d->mAllTracks.keys();
    std::sort(d->mIds.begin(), d->mIds.end());

    endInsertRows();
}

void AllTracksModel::trackRemoved(qulonglong removedTrackId)
{
    auto itTrack = std::find(d->mIds.begin(), d->mIds.end(), removedTrackId);
    if (itTrack == d->mIds.end()) {
        return;
    }

    auto position = itTrack - d->mIds.begin();

    beginRemoveRows({}, position, position);
    d->mIds.erase(itTrack);
    d->mAllTracks.remove(removedTrackId);
    endRemoveRows();
}

void AllTracksModel::trackModified(const MusicAudioTrack &modifiedTrack)
{
}


#include "moc_alltracksmodel.cpp"
