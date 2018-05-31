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

#include "allgenresmodel.h"
#include "databaseinterface.h"
#include "musicaudiogenre.h"

#include <QUrl>
#include <QTimer>
#include <QPointer>
#include <QVector>

class AllGenresModelPrivate
{
public:

    AllGenresModelPrivate() = default;

    QVector<MusicAudioGenre> mAllGenres;

};

AllGenresModel::AllGenresModel(QObject *parent) : QAbstractItemModel(parent), d(std::make_unique<AllGenresModelPrivate>())
{
}

AllGenresModel::~AllGenresModel()
= default;

int AllGenresModel::rowCount(const QModelIndex &parent) const
{
    auto artistCount = 0;

    if (parent.isValid()) {
        return artistCount;
    }

    artistCount = d->mAllGenres.size();

    return artistCount;
}

QHash<int, QByteArray> AllGenresModel::roleNames() const
{
    auto roles = QAbstractItemModel::roleNames();

    //roles[static_cast<int>(ElisaUtils::ColumnsRoles::NameRole)] = "name";
    //roles[static_cast<int>(ElisaUtils::ColumnsRoles::ArtistsCountRole)] = "albumsCount";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::ImageRole)] = "image";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::IdRole)] = "databaseId";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::SecondaryTextRole)] = "secondaryText";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::ImageUrlRole)] = "imageUrl";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::ShadowForImageRole)] = "shadowForImage";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::ContainerDataRole)] = "containerData";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::ChildModelRole)] = "childModel";
    //roles[static_cast<int>(ElisaUtils::ColumnsRoles::IsTracksContainerRole)] = "isTracksContainer";

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

    const auto artistsCount = d->mAllGenres.size();

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
    case Qt::DisplayRole:
        result = d->mAllGenres[index.row()].name();
        break;
    case ElisaUtils::ColumnsRoles::ImageRole:
        break;
    case ElisaUtils::ColumnsRoles::IdRole:
        break;
    case ElisaUtils::ColumnsRoles::SecondaryTextRole:
        result = QString();
        break;
    case ElisaUtils::ColumnsRoles::ImageUrlRole:
        result = QUrl(QStringLiteral("image://icon/view-media-genre"));
        break;
    case ElisaUtils::ColumnsRoles::ShadowForImageRole:
        result = false;
        break;
    case ElisaUtils::ColumnsRoles::ContainerDataRole:
        result = QVariant::fromValue(d->mAllGenres[index.row()]);
        break;
    case ElisaUtils::ColumnsRoles::ChildModelRole:
        result = d->mAllGenres[index.row()].name();
        break;
    /*case ElisaUtils::ColumnsRoles::IsTracksContainerRole:
        result = false;
        break;*/
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

int AllGenresModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 1;
}

void AllGenresModel::genresAdded(const QList<MusicAudioGenre> &newGenres)
{
    if (!newGenres.isEmpty()) {
        beginInsertRows({}, d->mAllGenres.size(), d->mAllGenres.size() + newGenres.size() - 1);
        d->mAllGenres += newGenres.toVector();
        endInsertRows();
    }
}

void AllGenresModel::genreRemoved(const MusicAudioGenre &removedGenre)
{
    auto removedGenreIterator = std::find(d->mAllGenres.begin(), d->mAllGenres.end(), removedGenre);

    if (removedGenreIterator == d->mAllGenres.end()) {
        return;
    }

    int genreIndex = removedGenreIterator - d->mAllGenres.begin();

    beginRemoveRows({}, genreIndex, genreIndex);
    d->mAllGenres.erase(removedGenreIterator);
    endRemoveRows();
}

void AllGenresModel::genreModified(const MusicAudioGenre &modifiedGenre)
{
    Q_UNUSED(modifiedGenre);
}

#include "moc_allgenresmodel.cpp"
