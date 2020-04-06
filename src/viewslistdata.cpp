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

class ViewsListDataPrivate
{
public:
    QList<ViewParameters> mViewsParameters = {{{i18nc("Title of the view of the playlist", "Now Playing")},
                                               QUrl{QStringLiteral("image://icon/view-media-lyrics")},
                                               ViewManager::ContextView},
                                              {{i18nc("Title of the view of recently played tracks", "Recently Played")},
                                               QUrl{QStringLiteral("image://icon/media-playlist-play")},
                                               ViewManager::ListView,
                                               ElisaUtils::FilterByRecentlyPlayed,
                                               ElisaUtils::Track,
                                               DataTypes::LastPlayDate,
                                               ViewManager::SortDescending,
                                               ViewManager::MultipleAlbum,
                                               ViewManager::NoDiscHeaders,
                                               ViewManager::IsTrack},
                                              {{i18nc("Title of the view of frequently played tracks", "Frequently Played")},
                                               QUrl{QStringLiteral("image://icon/view-media-playcount")},
                                               ViewManager::ListView,
                                               ElisaUtils::FilterByFrequentlyPlayed,
                                               ElisaUtils::Track,
                                               DataTypes::PlayFrequency,
                                               ViewManager::SortDescending,
                                               ViewManager::MultipleAlbum,
                                               ViewManager::NoDiscHeaders,
                                               ViewManager::IsTrack},
                                              {{i18nc("Title of the view of all albums", "Albums")},
                                               QUrl{QStringLiteral("image://icon/view-media-album-cover")},
                                               ViewManager::GridView,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Album,
                                               QUrl{QStringLiteral("image://icon/media-optical-audio")},
                                               ViewManager::DelegateWithSecondaryText,
                                               ViewManager::ViewShowRating},
                                              {{i18nc("Title of the view of all artists", "Artists")},
                                               QUrl{QStringLiteral("image://icon/view-media-artist")},
                                               ViewManager::GridView,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Artist,
                                               QUrl{QStringLiteral("image://icon/view-media-artist")},
                                               ViewManager::DelegateWithoutSecondaryText,
                                               ViewManager::ViewHideRating},
                                              {{i18nc("Title of the view of all tracks", "Tracks")},
                                               QUrl{QStringLiteral("image://icon/view-media-track")},
                                               ViewManager::ListView,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Track,
                                               Qt::DisplayRole,
                                               ViewManager::SortAscending,
                                               ViewManager::MultipleAlbum,
                                               ViewManager::NoDiscHeaders,
                                               ViewManager::IsTrack},
                                              {{i18nc("Title of the view of all genres", "Genres")},
                                               QUrl{QStringLiteral("image://icon/view-media-genre")},
                                               ViewManager::GridView,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Genre,
                                               QUrl{QStringLiteral("image://icon/view-media-genre")},
                                               ViewManager::DelegateWithoutSecondaryText,
                                               ViewManager::ViewHideRating},
                                              {{i18nc("Title of the file browser view", "Files")},
                                               QUrl{QStringLiteral("image://icon/document-open-folder")},
                                               ViewManager::FileBrowserView},
                                              {{i18nc("Title of the file radios browser view", "Radios")},
                                               QUrl{QStringLiteral("image://icon/radio")},
                                               ViewManager::ListView,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Radio,
                                               Qt::DisplayRole,
                                               ViewManager::SortAscending,
                                               ViewManager::MultipleAlbum,
                                               ViewManager::NoDiscHeaders,
                                               ViewManager::IsRadio}};

};

ViewsListData::ViewsListData(QObject *parent) : QObject(parent), d(std::make_unique<ViewsListDataPrivate>())
{
}

ViewsListData::~ViewsListData() = default;

int ViewsListData::count() const
{
    return d->mViewsParameters.count();
}

const ViewParameters &ViewsListData::viewParameters(int index) const
{
    return d->mViewsParameters[index];
}

const QString &ViewsListData::title(int index) const
{
    return d->mViewsParameters[index].mMainTitle;
}

const QUrl &ViewsListData::iconUrl(int index) const
{
    return d->mViewsParameters[index].mMainImage;
}


#include "moc_viewslistdata.cpp"
