/*
 * Copyright 2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "viewsmodel.h"

#include "viewmanager.h"

#include <KI18n/KLocalizedString>

#include <QUrl>

class ViewsModelPrivate
{

public:

    QList<ViewManager::ViewsType> mTypes;

    QHash<ViewManager::ViewsType, QString> mNames;

    QHash<ViewManager::ViewsType, QUrl> mIcons;

    ViewsModelPrivate()
    {
        mTypes = {ViewManager::NoViews, ViewManager::AllAlbums,
                  ViewManager::AllArtists, ViewManager::AllTracks,
                  ViewManager::AllGenres, ViewManager::FilesBrowser};

        mNames = {{ViewManager::NoViews, {i18nc("Title of the view of the playlist", "Now Playing")}},
                  {ViewManager::AllAlbums, {i18nc("Title of the view of all albums", "Albums")}},
                  {ViewManager::AllArtists, {i18nc("Title of the view of all artists", "Artists")}},
                  {ViewManager::AllTracks, {i18nc("Title of the view of all tracks", "Tracks")}},
                  {ViewManager::AllGenres, {i18nc("Title of the view of all genres", "Genres")}},
                  {ViewManager::FilesBrowser, {i18nc("Title of the file browser view", "Files")}}};

        mIcons = {{ViewManager::NoViews, QUrl{QStringLiteral("image://icon/view-media-playlist")}},
                  {ViewManager::AllAlbums, QUrl{QStringLiteral("image://icon/view-media-album-cover")}},
                  {ViewManager::AllArtists, QUrl{QStringLiteral("image://icon/view-media-artist")}},
                  {ViewManager::AllTracks, QUrl{QStringLiteral("image://icon/view-media-track")}},
                  {ViewManager::AllGenres, QUrl{QStringLiteral("image://icon/view-media-genre")}},
                  {ViewManager::FilesBrowser, QUrl{QStringLiteral("image://icon/document-open-folder")}}};
    }

};

ViewsModel::ViewsModel(QObject *parent)
    : QAbstractListModel(parent), d(std::make_unique<ViewsModelPrivate>())
{
}

ViewsModel::~ViewsModel() = default;

QHash<int, QByteArray> ViewsModel::roleNames() const
{
    auto result = QAbstractListModel::roleNames();

    result[ItemType] = "type";
    result[ImageName] = "image";

    return result;
}

Qt::ItemFlags ViewsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

int ViewsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return d->mTypes.count();
}

QVariant ViewsModel::data(const QModelIndex &index, int role) const
{
    auto result = QVariant{};

    if (!index.isValid()) {
        return result;
    }

    switch(role)
    {
    case Qt::DisplayRole:
        result = d->mNames[d->mTypes[index.row()]];
        break;
    case ColumnRoles::ImageName:
        result = d->mIcons[d->mTypes[index.row()]];
        break;
    case ColumnRoles::ItemType:
        result = d->mTypes[index.row()];
        break;
    }

    return result;
}

QModelIndex ViewsModel::index(int row, int column, const QModelIndex &parent) const
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

QModelIndex ViewsModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)

    auto result = QModelIndex();

    return result;
}


#include "moc_viewsmodel.cpp"
