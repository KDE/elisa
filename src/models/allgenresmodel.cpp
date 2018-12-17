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

class AllGenresModelPrivate
{
public:

    AllGenresModel::ListGenreDataType mAllGenres;

    ModelDataLoader mDataLoader;

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

    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::IdRole)] = "databaseId";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::SecondaryTextRole)] = "secondaryText";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::ImageUrlRole)] = "imageUrl";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::ShadowForImageRole)] = "shadowForImage";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::ContainerDataRole)] = "containerData";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::ChildModelRole)] = "childModel";

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

    Q_ASSERT(index.isValid());
    Q_ASSERT(index.column() == 0);
    Q_ASSERT(index.row() >= 0 && index.row() < artistsCount);
    Q_ASSERT(!index.parent().isValid());
    Q_ASSERT(index.model() == this);
    Q_ASSERT(index.internalId() == 0);

    switch(role)
    {
    case Qt::DisplayRole:
        result = d->mAllGenres[index.row()][GenreDataType::key_type::TitleRole];
        break;
    default:
        result = d->mAllGenres[index.row()][static_cast<GenreDataType::key_type>(role)];
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

void AllGenresModel::genresAdded(ListGenreDataType newGenres)
{
    if (d->mAllGenres.isEmpty()) {
        beginInsertRows({}, d->mAllGenres.size(), newGenres.size() - 1);
        d->mAllGenres.swap(newGenres);
        endInsertRows();
    } else {
        beginInsertRows({}, d->mAllGenres.size(), d->mAllGenres.size() + newGenres.size() - 1);
        d->mAllGenres.append(newGenres);
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
