/*
 * Copyright 2015-2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "localalbummodel.h"
#include "musicstatistics.h"
#include "localbaloofilelisting.h"
#include "localbalootrack.h"
#include "localbalooalbum.h"

#include <Baloo/File>

#include <QtCore/QUrl>
#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <QtCore/QPointer>

class LocalAlbumModelPrivate
{
public:

    bool mUseLocalIcons = false;

    MusicStatistics *mMusicDatabase = nullptr;

    QThread mBalooQueryThread;

    LocalBalooFileListing mFileListing;

    QMap<quintptr, LocalBalooAlbum> mAlbumsData;

    QList<quintptr> mAlbumIds;

    QMap<quintptr, quintptr> mTracksInAlbums;

};

LocalAlbumModel::LocalAlbumModel(QObject *parent) : QAbstractItemModel(parent), d(new LocalAlbumModelPrivate)
{
    d->mBalooQueryThread.start();
    d->mFileListing.moveToThread(&d->mBalooQueryThread);

    connect(this, &LocalAlbumModel::refreshContent, &d->mFileListing, &LocalBalooFileListing::refreshContent, Qt::QueuedConnection);
    connect(&d->mFileListing, &LocalBalooFileListing::tracksList, this, &LocalAlbumModel::tracksList);

    Q_EMIT refreshContent();
}

LocalAlbumModel::~LocalAlbumModel()
{
    delete d;
}

int LocalAlbumModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return d->mAlbumsData.size();
    }

    auto itAlbum = d->mAlbumsData.find(parent.internalId());
    if (itAlbum == d->mAlbumsData.end()) {
        return 0;
    }

    return itAlbum->mTrackIds.size();
}

QHash<int, QByteArray> LocalAlbumModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[static_cast<int>(ColumnsRoles::TitleRole)] = "title";
    roles[static_cast<int>(ColumnsRoles::DurationRole)] = "duration";
    roles[static_cast<int>(ColumnsRoles::ArtistRole)] = "artist";
    roles[static_cast<int>(ColumnsRoles::AlbumRole)] = "album";
    roles[static_cast<int>(ColumnsRoles::RatingRole)] = "rating";
    roles[static_cast<int>(ColumnsRoles::ImageRole)] = "image";
    roles[static_cast<int>(ColumnsRoles::ItemClassRole)] = "itemClass";
    roles[static_cast<int>(ColumnsRoles::CountRole)] = "count";
    roles[static_cast<int>(ColumnsRoles::IsPlayingRole)] = "isPlaying";

    return roles;
}

Qt::ItemFlags LocalAlbumModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant LocalAlbumModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    if (index.column() != 0) {
        return {};
    }

    if (index.row() < 0) {
        return {};
    }

    auto itAlbum = d->mAlbumsData.find(index.internalId());
    if (itAlbum != d->mAlbumsData.end()) {
        return internalDataAlbum(index, role);
    }

    auto itTrack = d->mTracksInAlbums.find(index.internalId());
    if (itTrack == d->mTracksInAlbums.end()) {
        return {};
    }

    auto albumId = itTrack.value();
    itAlbum = d->mAlbumsData.find(albumId);
    if (itAlbum == d->mAlbumsData.end()) {
        return {};
    }

    const auto &currentAlbum = *itAlbum;
    if (index.row() < 0 || index.row() >= currentAlbum.mNbTracks) {
        return {};
    }

    auto itTrackInCurrentAlbum = currentAlbum.mTracks.find(index.internalId());
    if (itTrackInCurrentAlbum == currentAlbum.mTracks.end()) {
        return {};
    }

    return internalDataTrack(itTrackInCurrentAlbum.value(), index, role);
}

QVariant LocalAlbumModel::internalDataAlbum(const QModelIndex &index, int role) const
{
    ColumnsRoles convertedRole = static_cast<ColumnsRoles>(role);

    const auto &albumData = d->mAlbumsData[index.internalId()];

    switch(convertedRole)
    {
    case ColumnsRoles::TitleRole:
        return albumData.mTitle;
    case ColumnsRoles::DurationRole:
        return {};
    case ColumnsRoles::CreatorRole:
        return {};
    case ColumnsRoles::ArtistRole:
        return albumData.mArtist;
    case ColumnsRoles::AlbumRole:
        return {};
    case ColumnsRoles::RatingRole:
        return {};
    case ColumnsRoles::ImageRole:
        if (albumData.mCoverFile.isValid()) {
            return albumData.mCoverFile;
        } else {
            if (d->mUseLocalIcons) {
                return QUrl(QStringLiteral("qrc:/media-optical-audio.svg"));
            } else {
                return QUrl(QStringLiteral("image://icon/media-optical-audio"));
            }
        }
    case ColumnsRoles::ResourceRole:
        return {};
    case ColumnsRoles::ItemClassRole:
        return {};
    case ColumnsRoles::CountRole:
        return albumData.mNbTracks;
    case ColumnsRoles::IdRole:
        return albumData.mTitle;
    case ColumnsRoles::IsPlayingRole:
        return {};
    }

    return {};
}

QVariant LocalAlbumModel::internalDataTrack(const LocalBalooTrack &track, const QModelIndex &index, int role) const
{
    ColumnsRoles convertedRole = static_cast<ColumnsRoles>(role);

    switch(convertedRole)
    {
    case ColumnsRoles::TitleRole:
        return track.mTitle;
    case ColumnsRoles::DurationRole:
    {
        QTime trackDuration = QTime::fromMSecsSinceStartOfDay(1000 * track.mDuration);
        if (trackDuration.hour() == 0) {
            return trackDuration.toString(QStringLiteral("mm:ss"));
        } else {
            return trackDuration.toString();
        }
    }
    case ColumnsRoles::CreatorRole:
        return track.mArtist;
    case ColumnsRoles::ArtistRole:
        return track.mArtist;
    case ColumnsRoles::AlbumRole:
        return track.mAlbum;
    case ColumnsRoles::RatingRole:
        return 0;
    case ColumnsRoles::ImageRole:
        return data(index.parent(), role);
    case ColumnsRoles::ResourceRole:
        return track.mFile;
    case ColumnsRoles::ItemClassRole:
        return {};
    case ColumnsRoles::CountRole:
        return {};
    case ColumnsRoles::IdRole:
        return track.mTitle;
    case ColumnsRoles::IsPlayingRole:
        return false;
    }

    return {};
}

QModelIndex LocalAlbumModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column != 0) {
        return {};
    }

    if (!parent.isValid()) {
        return createIndex(row, column, quintptr(d->mAlbumIds[row]));
    }

    auto itAlbum = d->mAlbumsData.find(parent.internalId());
    if (itAlbum == d->mAlbumsData.end()) {
        return {};
    }

    const auto &currentAlbum = *itAlbum;

    if (row >= currentAlbum.mTrackIds.size()) {
        return {};
    }

    return createIndex(row, column, currentAlbum.mTrackIds[row]);
}

QModelIndex LocalAlbumModel::parent(const QModelIndex &child) const
{
    // child is valid
    if (!child.isValid()) {
        return {};
    }

    if (child.internalId() == 0) {
        return {};
    }

    auto itAlbum = d->mAlbumsData.find(child.internalId());
    if (itAlbum != d->mAlbumsData.end()) {
        return {};
    }

    auto itTrack = d->mTracksInAlbums.find(child.internalId());
    if (itTrack == d->mTracksInAlbums.end()) {
        return {};
    }

    auto albumId = itTrack.value();
    int albumPosition = d->mAlbumIds.indexOf(albumId);
    if (albumPosition == -1) {
        return {};
    }

    return index(albumPosition, 0);
}

int LocalAlbumModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 1;
}

MusicStatistics *LocalAlbumModel::musicDatabase() const
{
    return d->mMusicDatabase;
}

void LocalAlbumModel::setMusicDatabase(MusicStatistics *musicDatabase)
{
    if (d->mMusicDatabase == musicDatabase)
        return;

    if (d->mMusicDatabase) {
        disconnect(this, &LocalAlbumModel::newAlbum, d->mMusicDatabase, &MusicStatistics::newAlbum);
        disconnect(this, &LocalAlbumModel::newAudioTrack, d->mMusicDatabase, &MusicStatistics::newAudioTrack);
    }

    d->mMusicDatabase = musicDatabase;

    if (d->mMusicDatabase) {
        connect(this, &LocalAlbumModel::newAlbum, d->mMusicDatabase, &MusicStatistics::newAlbum);
        connect(this, &LocalAlbumModel::newAudioTrack, d->mMusicDatabase, &MusicStatistics::newAudioTrack);
    }

    emit musicDatabaseChanged();
}

void LocalAlbumModel::tracksList(const QHash<QString, QList<LocalBalooTrack> > &tracks, const QHash<QString, QString> &covers)
{
    beginResetModel();

    d->mAlbumsData.clear();
    d->mAlbumIds.clear();

    quintptr currentElementId = 1;
    for (auto album : tracks) {
        const auto &firstTrack = album.first();
        d->mAlbumIds.push_back(currentElementId);
        auto &newAlbum = d->mAlbumsData[currentElementId];
        auto albumId = currentElementId;
        ++currentElementId;

        newAlbum.mArtist = firstTrack.mArtist;
        newAlbum.mTitle = firstTrack.mAlbum;
        newAlbum.mNbTracks = album.size();
        newAlbum.mCoverFile = QUrl::fromLocalFile(covers[firstTrack.mAlbum]);

        for(auto track : album) {
            newAlbum.mTracks[currentElementId] = track;
            newAlbum.mTrackIds.push_back(currentElementId);
            d->mTracksInAlbums[currentElementId] = albumId;
            ++currentElementId;
        }
    }

    endResetModel();
}

#include "moc_localalbummodel.cpp"
