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

#include "upnpalbummodel.h"
#include "musicstatistics.h"
#include "didlparser.h"

#include <upnpcontrolcontentdirectory.h>

#include <QtXml/QDomDocument>

#include <QtCore/QUrl>
#include <QtCore/QDebug>

class UpnpAlbumModelPrivate
{
public:

    UpnpControlContentDirectory *mContentDirectory = nullptr;

    quintptr mLastInternalId = 1;

    QHash<QString, quintptr> mUpnpIds;

    QHash<quintptr, QVector<quintptr> > mChilds;

    QHash<quintptr, QHash<UpnpAlbumModel::ColumnsRoles, QVariant> > mData;

    int mCurrentUpdateId = -1;

    bool mUseLocalIcons = false;

    MusicStatistics *mMusicDatabase = nullptr;

    DidlParser mDidlParser;

};

UpnpAlbumModel::UpnpAlbumModel(QObject *parent) : QAbstractItemModel(parent), d(new UpnpAlbumModelPrivate)
{
    d->mUpnpIds[QStringLiteral("0")] = d->mLastInternalId;

    d->mChilds[d->mLastInternalId] = QVector<quintptr>();

    d->mData[d->mLastInternalId] = QHash<UpnpAlbumModel::ColumnsRoles, QVariant>();
    d->mData[d->mLastInternalId][ColumnsRoles::IdRole] = QStringLiteral("0");

    d->mDidlParser.setSearchCriteria(QStringLiteral("upnp:class = \"object.container.album.musicAlbum\""));
}

UpnpAlbumModel::~UpnpAlbumModel()
{
    delete d;
}

int UpnpAlbumModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return 1;
    }

    if (!d->mChilds.contains(parent.internalId())) {
        return 0;
    }

    return d->mChilds[parent.internalId()].size();
}

QHash<int, QByteArray> UpnpAlbumModel::roleNames() const
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

bool UpnpAlbumModel::canFetchMore(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return false;
    }

    auto parentInternalId = parent.internalId();

    if (!d->mChilds.contains(parentInternalId)) {
        return true;
    }

    return d->mChilds[parentInternalId].isEmpty();
}

void UpnpAlbumModel::fetchMore(const QModelIndex &parent)
{
    qDebug() << "UpnpAlbumModel::fetchMore";

    if (!d->mContentDirectory) {
        return;
    }

    if (!parent.isValid()) {
        return;
    }

    auto parentInternalId = parent.internalId();

    if (!d->mData.contains(parentInternalId)) {
        return;
    }

    if (!d->mData[parentInternalId].contains(ColumnsRoles::IdRole)) {
        return;
    }

    qDebug() << "UpnpAlbumModel::fetchMore" << "OK";

    d->mDidlParser.setParentId(d->mData[parentInternalId][ColumnsRoles::IdRole].toString());

    if (d->mData[parentInternalId][ColumnsRoles::IdRole].toString() == QStringLiteral("0")) {
        d->mDidlParser.search();
    } else {
        d->mDidlParser.browse();
    }
}

Qt::ItemFlags UpnpAlbumModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant UpnpAlbumModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.column() != 0) {
        return QVariant();
    }

    if (index.row() < 0) {
        return QVariant();
    }

    if (!d->mData.contains(index.internalId())) {
        return QVariant();
    }

    ColumnsRoles convertedRole = static_cast<ColumnsRoles>(role);

    switch(role)
    {
    case ColumnsRoles::TitleRole:
        return d->mData[index.internalId()][convertedRole];
    case ColumnsRoles::DurationRole:
        return d->mData[index.internalId()][convertedRole];
    case ColumnsRoles::CreatorRole:
        return d->mData[index.internalId()][convertedRole];
    case ColumnsRoles::ArtistRole:
        return d->mData[index.internalId()][convertedRole];
    case ColumnsRoles::AlbumRole:
        return d->mData[index.internalId()][convertedRole];
    case ColumnsRoles::RatingRole:
        return 0;
    case ColumnsRoles::ImageRole:
        switch (d->mData[index.internalId()][ColumnsRoles::ItemClassRole].value<int>())
        {
        case UpnpAlbumModel::Album:
            if (!d->mData[index.internalId()][ColumnsRoles::ImageRole].toString().isEmpty()) {
                return d->mData[index.internalId()][ColumnsRoles::ImageRole].toUrl();
            } else {
                if (d->mUseLocalIcons) {
                    return QUrl(QStringLiteral("qrc:/media-optical-audio.svg"));
                } else {
                    return QUrl(QStringLiteral("image://icon/media-optical-audio"));
                }
            }
        case UpnpAlbumModel::Container:
            if (d->mUseLocalIcons) {
                return QUrl(QStringLiteral("qrc:/folder.svg"));
            } else {
                return QUrl(QStringLiteral("image://icon/folder"));
            }
        case UpnpAlbumModel::AudioTrack:
            return data(index.parent(), role);
        }
    case ColumnsRoles::ResourceRole:
        return d->mData[index.internalId()][convertedRole];
    case ColumnsRoles::ItemClassRole:
        return d->mData[index.internalId()][convertedRole];
    case ColumnsRoles::CountRole:
        return d->mData[index.internalId()][convertedRole];
    case ColumnsRoles::IdRole:
        return d->mData[index.internalId()][convertedRole];
    case ColumnsRoles::ParentIdRole:
        return d->mData[index.internalId()][convertedRole];
    case ColumnsRoles::IsPlayingRole:
        return d->mData[index.internalId()][convertedRole];
    }

    return QVariant();
}

QModelIndex UpnpAlbumModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return createIndex(0, 0, 1);
    }

    if (!d->mChilds.contains(parent.internalId())) {
        return {};
    }

    if (row < 0 && row >= d->mChilds[parent.internalId()].size()) {
        return {};
    }

    if (column != 0) {
        return {};
    }

    return createIndex(row, column, d->mChilds[parent.internalId()][row]);
}

QModelIndex UpnpAlbumModel::parent(const QModelIndex &child) const
{
    // child is valid
    if (!child.isValid()) {
        return {};
    }

    // data knows child internal id
    if (!d->mData.contains(child.internalId())) {
        return {};
    }

    const auto &childData = d->mData[child.internalId()];

    // child data has upnp id of parent
    if (!childData.contains(ColumnsRoles::ParentIdRole)) {
        return {};
    }

    const auto &parentStringId = childData[ColumnsRoles::ParentIdRole].toString();

    // upnp ids has the internal id of parent
    if (!d->mUpnpIds.contains(parentStringId)) {
        return {};
    }

    auto parentInternalId = d->mUpnpIds[parentStringId];

    // special case if we are already at top of model
    if (parentStringId == QStringLiteral("0")) {
        return createIndex(0, 0, parentInternalId);
    }

    // data knows parent internal id
    if (!d->mData.contains(parentInternalId)) {
        return {};
    }

    const auto &parentData = d->mData[parentInternalId];

    // parent data has upnp id of parent
    if (!parentData.contains(ColumnsRoles::ParentIdRole)) {
        return {};
    }

    const auto &grandParentStringId = parentData[ColumnsRoles::ParentIdRole].toString();

    // upnp ids has the internal id of grand parent
    if (!d->mUpnpIds.contains(grandParentStringId)) {
        return {};
    }

    auto grandParentInternalId = d->mUpnpIds[grandParentStringId];

    // childs of grand parent are known
    if (!d->mChilds.contains(grandParentInternalId)) {
        return {};
    }

    const auto &allUncles = d->mChilds[grandParentInternalId];

    // look for my parent
    for (int i = 0; i < allUncles.size(); ++i) {
        if (allUncles[i] == parentInternalId) {
            return createIndex(i, 0, parentInternalId);
        }
    }

    // I have no parent
    return {};
}

int UpnpAlbumModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 1;
}

QString UpnpAlbumModel::objectIdByIndex(const QModelIndex &index) const
{
    return data(index, ColumnsRoles::IdRole).toString();
}

QVariant UpnpAlbumModel::getUrl(const QModelIndex &index) const
{
    return data(index, ColumnsRoles::ResourceRole);
}

QModelIndex UpnpAlbumModel::indexFromId(const QString &id) const
{
    if (!d->mUpnpIds.contains(id)) {
        return {};
    }

    return indexFromInternalId(d->mUpnpIds[id]);
}

UpnpControlContentDirectory *UpnpAlbumModel::contentDirectory() const
{
    return d->mContentDirectory;
}

MusicStatistics *UpnpAlbumModel::musicDatabase() const
{
    return d->mMusicDatabase;
}

DidlParser *UpnpAlbumModel::didlParser() const
{
    return &d->mDidlParser;
}

void UpnpAlbumModel::setContentDirectory(UpnpControlContentDirectory *directory)
{
    if (directory) {
        beginResetModel();
    }
    if (d->mContentDirectory) {
        disconnect(d->mContentDirectory, &UpnpControlContentDirectory::systemUpdateIDChanged,
                   &d->mDidlParser, &DidlParser::systemUpdateIDChanged);
    }

    d->mContentDirectory = directory;

    if (!d->mContentDirectory) {
        Q_EMIT contentDirectoryChanged();
        return;
    }

    connect(d->mContentDirectory, &UpnpControlContentDirectory::systemUpdateIDChanged,
            &d->mDidlParser, &DidlParser::systemUpdateIDChanged);

    endResetModel();

    Q_EMIT contentDirectoryChanged();
}

void UpnpAlbumModel::setMusicDatabase(MusicStatistics *musicDatabase)
{
    if (d->mMusicDatabase == musicDatabase)
        return;

    if (d->mMusicDatabase) {
        disconnect(this, &UpnpAlbumModel::newAlbum, d->mMusicDatabase, &MusicStatistics::newAlbum);
        disconnect(this, &UpnpAlbumModel::newAudioTrack, d->mMusicDatabase, &MusicStatistics::newAudioTrack);
    }

    d->mMusicDatabase = musicDatabase;

    if (d->mMusicDatabase) {
        connect(this, &UpnpAlbumModel::newAlbum, d->mMusicDatabase, &MusicStatistics::newAlbum);
        connect(this, &UpnpAlbumModel::newAudioTrack, d->mMusicDatabase, &MusicStatistics::newAudioTrack);
    }

    emit musicDatabaseChanged();
}

QModelIndex UpnpAlbumModel::indexFromInternalId(quintptr internalId) const
{
    if (internalId == 1) {
        return createIndex(0, 0, internalId);
    }

    // data knows child internal id
    if (!d->mData.contains(internalId)) {
        return {};
    }

    const auto &childData = d->mData[internalId];

    // child data has upnp id of parent
    if (!childData.contains(ColumnsRoles::ParentIdRole)) {
        return {};
    }

    const auto &parentStringId = childData[ColumnsRoles::ParentIdRole].toString();

    // upnp ids has the internal id of parent
    if (!d->mUpnpIds.contains(parentStringId)) {
        return {};
    }

    auto parentInternalId = d->mUpnpIds[parentStringId];

    // childs of parent are known
    if (!d->mChilds.contains(parentInternalId)) {
        return {};
    }

    const auto &allUncles = d->mChilds[parentInternalId];

    // look for my parent
    for (int i = 0; i < allUncles.size(); ++i) {
        if (allUncles[i] == internalId) {
            return createIndex(i, 0, internalId);
        }
    }

    // I have no parent
    return {};
}


#include "moc_upnpalbummodel.cpp"
