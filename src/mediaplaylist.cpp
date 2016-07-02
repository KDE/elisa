/*
 * Copyright 2015 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "mediaplaylist.h"

#include <QtCore/QUrl>
#include <QtCore/QPersistentModelIndex>
#include <QtCore/QList>
#include <QtCore/QDebug>

class MediaPlayListPrivate
{
public:

    QList<QPersistentModelIndex> mData;

    QList<bool> mIsPlaying;

};

MediaPlayList::MediaPlayList(QObject *parent) : QAbstractListModel(parent), d(new MediaPlayListPrivate)
{
}

MediaPlayList::~MediaPlayList()
{
    delete d;
}

int MediaPlayList::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return d->mData.size();
}

QVariant MediaPlayList::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() < 0 || index.row() > d->mData.size()) {
        return QVariant();
    }

    if (role < ColumnsRoles::TitleRole || role > ColumnsRoles::IsPlayingRole) {
        return QVariant();
    }

    ColumnsRoles convertedRole = static_cast<ColumnsRoles>(role);

    switch(convertedRole)
    {
    case ColumnsRoles::TitleRole:
    case ColumnsRoles::DurationRole:
    case ColumnsRoles::MilliSecondsDurationRole:
    case ColumnsRoles::ArtistRole:
    case ColumnsRoles::AlbumRole:
    case ColumnsRoles::TrackNumberRole:
    case ColumnsRoles::RatingRole:
    case ColumnsRoles::ImageRole:
    case ColumnsRoles::ItemClassRole:
    case ColumnsRoles::CountRole:
    case ColumnsRoles::CreatorRole:
    case ColumnsRoles::ResourceRole:
    case ColumnsRoles::IdRole:
    case ColumnsRoles::ParentIdRole:
        return d->mData[index.row()].data(role);
    case ColumnsRoles::IsPlayingRole:
        return d->mIsPlaying[index.row()];
    }

    return QVariant();
}

bool MediaPlayList::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) {
        return false;
    }

    if (index.row() < 0 || index.row() >= d->mData.size()) {
        return false;
    }

    if (index.row() < 0 || index.row() >= d->mIsPlaying.size()) {
        return false;
    }

    if (role < ColumnsRoles::TitleRole || role > ColumnsRoles::IsPlayingRole) {
        return false;
    }

    ColumnsRoles convertedRole = static_cast<ColumnsRoles>(role);

    bool modelModified = false;

    switch(convertedRole)
    {
    case ColumnsRoles::IsPlayingRole:
        d->mIsPlaying[index.row()] = value.toBool();
        Q_EMIT dataChanged(index, index, {role});
        break;
    default:
        modelModified = false;
    }

    return modelModified;
}

QHash<int, QByteArray> MediaPlayList::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[static_cast<int>(ColumnsRoles::TitleRole)] = "title";
    roles[static_cast<int>(ColumnsRoles::DurationRole)] = "duration";
    roles[static_cast<int>(ColumnsRoles::ArtistRole)] = "artist";
    roles[static_cast<int>(ColumnsRoles::AlbumRole)] = "album";
    roles[static_cast<int>(ColumnsRoles::TrackNumberRole)] = "trackNumber";
    roles[static_cast<int>(ColumnsRoles::RatingRole)] = "rating";
    roles[static_cast<int>(ColumnsRoles::ImageRole)] = "image";
    roles[static_cast<int>(ColumnsRoles::ItemClassRole)] = "itemClass";
    roles[static_cast<int>(ColumnsRoles::CountRole)] = "count";
    roles[static_cast<int>(ColumnsRoles::IsPlayingRole)] = "isPlaying";

    return roles;
}

bool MediaPlayList::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    for (int i = row, cpt = 0; cpt < count; ++i, ++cpt) {
        d->mData.removeAt(i);
        d->mIsPlaying.removeAt(i);
    }
    endRemoveRows();

    return false;
}

int MediaPlayList::trackCount() const
{
    return d->mData.size();
}

void MediaPlayList::enqueue(const QModelIndex &newTrack)
{
    beginInsertRows(QModelIndex(), d->mData.size(), d->mData.size());
    qDebug() << "MediaPlayList::enqueue" << newTrack << newTrack.data(ColumnsRoles::TitleRole);
    d->mData.push_back(newTrack);
    d->mIsPlaying.push_back(false);
    endInsertRows();

    Q_EMIT trackHasBeenAdded(newTrack.data(ColumnsRoles::TitleRole).toString(), newTrack.data(ColumnsRoles::ImageRole).toUrl());
    Q_EMIT trackCountChanged();
}

void MediaPlayList::move(int from, int to, int n)
{
    QModelIndex moveParent;
    beginResetModel();
    for (auto cptItem = 0; cptItem < n; ++cptItem) {
        d->mData.move(from, to);
        d->mIsPlaying.move(from, to);
    }
    endResetModel();
}

#include "moc_mediaplaylist.cpp"
