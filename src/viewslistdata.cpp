/*
 * Copyright 2020 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "viewslistdata.h"

#include <KI18n/KLocalizedString>

#include <QUrl>

struct ViewItem
{
    QString mTitle;

    QUrl mIconUrl;
};

class ViewsListDataPrivate
{
public:

    ViewsListDataPrivate()
    {
        mData = {{i18nc("Title of the view of the playlist", "Now Playing"),
                  QUrl{QStringLiteral("image://icon/view-media-lyrics")}},
                 {i18nc("Title of the view of recently played tracks", "Recently Played"),
                  QUrl{QStringLiteral("image://icon/media-playlist-play")}},
                 {i18nc("Title of the view of frequently played tracks", "Frequently Played"),
                  QUrl{QStringLiteral("image://icon/view-media-playcount")}},
                 {i18nc("Title of the view of all albums", "Albums"),
                  QUrl{QStringLiteral("image://icon/view-media-album-cover")}},
                 {i18nc("Title of the view of all artists", "Artists"),
                  QUrl{QStringLiteral("image://icon/view-media-artist")}},
                 {i18nc("Title of the view of all tracks", "Tracks"),
                  QUrl{QStringLiteral("image://icon/view-media-track")}},
                 {i18nc("Title of the view of all genres", "Genres"),
                  QUrl{QStringLiteral("image://icon/view-media-genre")}},
                 {i18nc("Title of the file browser view", "Files"),
                  QUrl{QStringLiteral("image://icon/document-open-folder")}},
                 {i18nc("Title of the file radios browser view", "Radios"),
                  QUrl{QStringLiteral("image://icon/radio")}}};

        mDefaultIcons = {{ViewManager::AllAlbums, QUrl{QStringLiteral("image://icon/view-media-album-cover")}},
                         {ViewManager::AllArtists, QUrl{QStringLiteral("image://icon/view-media-artist")}},
                         {ViewManager::AllGenres, QUrl{QStringLiteral("image://icon/view-media-genre")}},};
    }

    QList<ViewItem> mData;

    QMap<ViewManager::ViewsType, QUrl> mDefaultIcons;

};

ViewsListData::ViewsListData(QObject *parent) : QObject(parent), d(std::make_unique<ViewsListDataPrivate>())
{
}

ViewsListData::~ViewsListData() = default;

int ViewsListData::count() const
{
    return d->mData.count();
}

const QString &ViewsListData::title(int index) const
{
    return d->mData[index].mTitle;
}

const QUrl &ViewsListData::iconUrl(int index) const
{
    return d->mData[index].mIconUrl;
}


#include "moc_viewslistdata.cpp"
