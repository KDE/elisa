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

#include <Baloo/File>

#include <QtCore/QUrl>
#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <QtCore/QPointer>
#include <QtCore/QDebug>

class LocalAlbumModelPrivate
{
public:

    bool mUseLocalIcons = false;

    MusicStatistics *mMusicDatabase = nullptr;

    QList<QString> mChilds;

    QHash<QString, QPointer<QObject>> mAlbumParsers;

    QThread mBalooQueryThread;

    LocalBalooFileListing mFileListing;
};

LocalAlbumModel::LocalAlbumModel(QObject *parent) : QAbstractItemModel(parent), d(new LocalAlbumModelPrivate)
{
    d->mBalooQueryThread.start();
    d->mFileListing.moveToThread(&d->mBalooQueryThread);

    connect(this, &LocalAlbumModel::refreshContent, &d->mFileListing, &LocalBalooFileListing::refreshContent, Qt::QueuedConnection);

    Q_EMIT refreshContent();
    qDebug() << "LocalAlbumModel::LocalAlbumModel" << QThread::currentThreadId();
}

LocalAlbumModel::~LocalAlbumModel()
{
    delete d;
}

int LocalAlbumModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return 0;
    }

    if (!d->mAlbumParsers.contains(d->mChilds[parent.row()])) {
        return 0;
    }

    auto childParser = d->mAlbumParsers[d->mChilds[parent.row()]];

    if (!childParser) {
        return 0;
    }

    return 0;
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

bool LocalAlbumModel::canFetchMore(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return false;
    }

    if (parent.internalId() != 0) {
        return false;
    }

    if (!d->mAlbumParsers.contains(d->mChilds[parent.row()])) {
        return true;
    }

    auto childParser = d->mAlbumParsers[d->mChilds[parent.row()]];

    if (!childParser) {
        return true;
    }

    return false;
}

void LocalAlbumModel::fetchMore(const QModelIndex &parent)
{
    if (!d->mAlbumParsers.contains(d->mChilds[parent.row()])) {
    }
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

    if (index.row() >= 0) {
        return {};
    }

    if (index.internalId() != 0) {
        if (!d->mAlbumParsers.contains(d->mChilds[index.internalId() - 1])) {
            return 0;
        }

        auto childParser = d->mAlbumParsers[d->mChilds[index.internalId() - 1]];

        if (!childParser) {
            return 0;
        }

        return {};
    }

    return {};
}

#if 0
QVariant LocalAlbumModel::internalDataAlbum(const QModelIndex &index, int role, DidlParser *currentParser) const
{
    ColumnsRoles convertedRole = static_cast<ColumnsRoles>(role);

    const auto &albumId = currentParser->newAlbumIds()[index.row()];

    switch(convertedRole)
    {
    case ColumnsRoles::TitleRole:
        return currentParser->newAlbums()[albumId].mTitle;
    case ColumnsRoles::DurationRole:
        return {};
    case ColumnsRoles::CreatorRole:
        return {};
    case ColumnsRoles::ArtistRole:
        return currentParser->newAlbums()[albumId].mArtist;
    case ColumnsRoles::AlbumRole:
        return {};
    case ColumnsRoles::RatingRole:
        return {};
    case ColumnsRoles::ImageRole:
        if (currentParser->newAlbums()[albumId].mAlbumArtURI.isValid()) {
            return currentParser->newAlbums()[albumId].mAlbumArtURI;
        } else {
            if (d->mUseLocalIcons) {
                return QUrl(QStringLiteral("qrc:/media-optical-audio.svg"));
            } else {
                return QUrl(QStringLiteral("image://icon/media-optical-audio"));
            }
        }
    case ColumnsRoles::ResourceRole:
        return currentParser->newAlbums()[albumId].mResourceURI;
    case ColumnsRoles::ItemClassRole:
        return {};
    case ColumnsRoles::CountRole:
        return currentParser->newAlbums()[albumId].mTracksCount;
    case ColumnsRoles::IdRole:
        return currentParser->newAlbums()[albumId].mId;
    case ColumnsRoles::ParentIdRole:
        return currentParser->newAlbums()[albumId].mParentId;
    case ColumnsRoles::IsPlayingRole:
        return {};
    }

    return {};
}

QVariant LocalAlbumModel::internalDataTrack(const QModelIndex &index, int role, DidlParser *currentParser) const
{
    ColumnsRoles convertedRole = static_cast<ColumnsRoles>(role);

    if (index.row() < 0 || index.row() >= currentParser->newMusicTrackIds().size()) {
        return {};
    }

    const auto &musicTrackId = currentParser->newMusicTrackIds()[index.row()];

    switch(convertedRole)
    {
    case ColumnsRoles::TitleRole:
        return currentParser->newMusicTracks()[musicTrackId].mTitle;
    case ColumnsRoles::DurationRole:
        if (currentParser->newMusicTracks()[musicTrackId].mDuration.hour() == 0) {
            return currentParser->newMusicTracks()[musicTrackId].mDuration.toString(QStringLiteral("mm:ss"));
        } else {
            return currentParser->newMusicTracks()[musicTrackId].mDuration.toString();
        }
    case ColumnsRoles::CreatorRole:
        return currentParser->newMusicTracks()[musicTrackId].mArtist;
    case ColumnsRoles::ArtistRole:
        return currentParser->newMusicTracks()[musicTrackId].mArtist;
    case ColumnsRoles::AlbumRole:
        return currentParser->newMusicTracks()[musicTrackId].mAlbumName;
    case ColumnsRoles::RatingRole:
        return 0;
    case ColumnsRoles::ImageRole:
        return data(index.parent(), role);
    case ColumnsRoles::ResourceRole:
        return currentParser->newMusicTracks()[musicTrackId].mResourceURI;
    case ColumnsRoles::ItemClassRole:
        return {};
    case ColumnsRoles::CountRole:
        return {};
    case ColumnsRoles::IdRole:
        return currentParser->newMusicTracks()[musicTrackId].mId;
    case ColumnsRoles::ParentIdRole:
        return currentParser->newMusicTracks()[musicTrackId].mParentId;
    case ColumnsRoles::IsPlayingRole:
        return false;
    }

    return {};
}
#endif

QModelIndex LocalAlbumModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return createIndex(row, column, quintptr(0));
    }

    return createIndex(row, column, quintptr(parent.row()) + 1);
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

    return createIndex(int(child.internalId() - 1), 0, quintptr(0));
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

void LocalAlbumModel::contentChanged(const QString &parentId)
{
    if (parentId == QStringLiteral("0")) {
        beginResetModel();

        d->mChilds = {};

        endResetModel();
        return;
    }

    auto indexChild = d->mChilds.indexOf(parentId);

    if (indexChild == -1) {
        return;
    }

    if (!d->mAlbumParsers.contains(parentId)) {
        return;
    }

    auto childParser = d->mAlbumParsers[parentId];

    if (!childParser) {
        return;
    }

    beginInsertRows(index(indexChild, 0), 0, 0);

    endInsertRows();
}

#include "moc_localalbummodel.cpp"
