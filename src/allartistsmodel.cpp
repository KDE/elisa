/*
 * Copyright 2015-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "allartistsmodel.h"
#include "databaseinterface.h"
#include "musicartist.h"
#include "allalbumsmodel.h"

#include <QUrl>
#include <QTimer>
#include <QPointer>
#include <QVector>
#include <QSortFilterProxyModel>

class AllArtistsModelPrivate
{
public:

    AllArtistsModelPrivate()
    {
    }

    QVector<MusicArtist> mAllArtists;

    AllAlbumsModel *mAllAlbumsModel = nullptr;

};

AllArtistsModel::AllArtistsModel(QObject *parent) : QAbstractItemModel(parent), d(std::make_unique<AllArtistsModelPrivate>())
{
}

AllArtistsModel::~AllArtistsModel()
= default;

int AllArtistsModel::rowCount(const QModelIndex &parent) const
{
    auto artistCount = 0;

    if (parent.isValid()) {
        return artistCount;
    }

    artistCount = d->mAllArtists.size();

    return artistCount;
}

QHash<int, QByteArray> AllArtistsModel::roleNames() const
{
    auto roles = QAbstractItemModel::roleNames();

    roles[static_cast<int>(ColumnsRoles::NameRole)] = "name";
    roles[static_cast<int>(ColumnsRoles::ArtistsCountRole)] = "albumsCount";
    roles[static_cast<int>(ColumnsRoles::ImageRole)] = "image";
    roles[static_cast<int>(ColumnsRoles::IdRole)] = "databaseId";
    roles[static_cast<int>(ColumnsRoles::SecondaryTextRole)] = "secondaryText";
    roles[static_cast<int>(ColumnsRoles::ImageUrlRole)] = "imageUrl";
    roles[static_cast<int>(ColumnsRoles::ShadowForImageRole)] = "shadowForImage";
    roles[static_cast<int>(ColumnsRoles::ContainerDataRole)] = "containerData";
    roles[static_cast<int>(ColumnsRoles::ChildModelRole)] = "childModel";
    roles[static_cast<int>(ColumnsRoles::IsTracksContainerRole)] = "isTracksContainer";

    return roles;
}

Qt::ItemFlags AllArtistsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant AllArtistsModel::data(const QModelIndex &index, int role) const
{
    auto result = QVariant();

    const auto artistsCount = d->mAllArtists.size();

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

    if (index.row() < 0 || index.row() >= artistsCount) {
        return result;
    }

    switch(role)
    {
    case ColumnsRoles::NameRole:
        result = d->mAllArtists[index.row()].name();
        break;
    case ColumnsRoles::ArtistsCountRole:
        result = d->mAllArtists[index.row()].albumsCount();
        break;
    case ColumnsRoles::ImageRole:
        break;
    case ColumnsRoles::IdRole:
        break;
    case ColumnsRoles::SecondaryTextRole:
        result = QString();
        break;
    case Qt::DisplayRole:
        result = d->mAllArtists[index.row()].name();
        break;
    case ColumnsRoles::ImageUrlRole:
        result = QUrl(QStringLiteral("image://icon/view-media-artist"));
        break;
    case ColumnsRoles::ShadowForImageRole:
        result = false;
        break;
    case ColumnsRoles::ContainerDataRole:
        result = QVariant::fromValue(d->mAllArtists[index.row()]);
        break;
    case ColumnsRoles::ChildModelRole:
    {
        auto newModel = new QSortFilterProxyModel;
        newModel->setSourceModel(d->mAllAlbumsModel);
        newModel->setFilterRole(AllAlbumsModel::AllArtistsRole);
        newModel->setFilterRegExp(QRegExp(QStringLiteral(".*") + d->mAllArtists[index.row()].name() + QStringLiteral(".*"),
                                  Qt::CaseInsensitive));
        result = QVariant::fromValue(newModel);
        break;
    }
    case ColumnsRoles::IsTracksContainerRole:
        result = false;
        break;
    }

    return result;
}

QModelIndex AllArtistsModel::index(int row, int column, const QModelIndex &parent) const
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

QModelIndex AllArtistsModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)

    auto result = QModelIndex();

    return result;
}

int AllArtistsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 1;
}

AllAlbumsModel *AllArtistsModel::allAlbums() const
{
    return d->mAllAlbumsModel;
}

QAbstractItemModel *AllArtistsModel::itemModelForName(const QString &name) const
{
    auto newModel = new QSortFilterProxyModel;
    newModel->setSourceModel(d->mAllAlbumsModel);
    newModel->setFilterRole(AllAlbumsModel::AllArtistsRole);
    newModel->setFilterRegExp(QRegExp(QStringLiteral(".*") + name + QStringLiteral(".*"),
                              Qt::CaseInsensitive));

    return newModel;
}

void AllArtistsModel::artistAdded(const MusicArtist &newArtist)
{
    if (newArtist.isValid()) {
        beginInsertRows({}, d->mAllArtists.size(), d->mAllArtists.size());
        d->mAllArtists.push_back(newArtist);
        endInsertRows();
    }
}

void AllArtistsModel::artistRemoved(const MusicArtist &removedArtist)
{
    auto removedArtistIterator = std::find(d->mAllArtists.begin(), d->mAllArtists.end(), removedArtist);

    if (removedArtistIterator == d->mAllArtists.end()) {
        return;
    }

    int artistIndex = removedArtistIterator - d->mAllArtists.begin();

    beginRemoveRows({}, artistIndex, artistIndex);
    d->mAllArtists.erase(removedArtistIterator);
    endRemoveRows();
}

void AllArtistsModel::artistModified(const MusicArtist &modifiedArtist)
{
    Q_UNUSED(modifiedArtist);
}

void AllArtistsModel::setAllAlbums(AllAlbumsModel *model)
{
    if (d->mAllAlbumsModel == model) {
        return;
    }

    d->mAllAlbumsModel = model;

    Q_EMIT allAlbumsChanged();
}

#include "moc_allartistsmodel.cpp"
