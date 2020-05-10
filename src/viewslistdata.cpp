/*
   SPDX-FileCopyrightText: 2020 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
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
                                               ViewManager::GenericDataModel,
                                               ElisaUtils::FilterByRecentlyPlayed,
                                               ElisaUtils::Track,
                                               DataTypes::LastPlayDate,
                                               Qt::DescendingOrder,
                                               ViewManager::MultipleAlbum,
                                               ViewManager::NoDiscHeaders,
                                               ViewManager::IsTrack},
                                              {{i18nc("Title of the view of frequently played tracks", "Frequently Played")},
                                               QUrl{QStringLiteral("image://icon/view-media-playcount")},
                                               ViewManager::ListView,
                                               ViewManager::GenericDataModel,
                                               ElisaUtils::FilterByFrequentlyPlayed,
                                               ElisaUtils::Track,
                                               DataTypes::PlayFrequency,
                                               Qt::DescendingOrder,
                                               ViewManager::MultipleAlbum,
                                               ViewManager::NoDiscHeaders,
                                               ViewManager::IsTrack},
                                              {{i18nc("Title of the view of all albums", "Albums")},
                                               QUrl{QStringLiteral("image://icon/view-media-album-cover")},
                                               ViewManager::GridView,
                                               ViewManager::GenericDataModel,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Album,
                                               QUrl{QStringLiteral("image://icon/media-optical-audio")},
                                               ViewManager::DelegateWithSecondaryText,
                                               ViewManager::ViewShowRating},
                                              {{i18nc("Title of the view of all artists", "Artists")},
                                               QUrl{QStringLiteral("image://icon/view-media-artist")},
                                               ViewManager::GridView,
                                               ViewManager::GenericDataModel,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Artist,
                                               QUrl{QStringLiteral("image://icon/view-media-artist")},
                                               ViewManager::DelegateWithoutSecondaryText,
                                               ViewManager::ViewHideRating},
                                              {{i18nc("Title of the view of all tracks", "Tracks")},
                                               QUrl{QStringLiteral("image://icon/view-media-track")},
                                               ViewManager::ListView,
                                               ViewManager::GenericDataModel,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Track,
                                               Qt::DisplayRole,
                                               Qt::AscendingOrder,
                                               ViewManager::MultipleAlbum,
                                               ViewManager::NoDiscHeaders,
                                               ViewManager::IsTrack},
                                              {{i18nc("Title of the view of all genres", "Genres")},
                                               QUrl{QStringLiteral("image://icon/view-media-genre")},
                                               ViewManager::GridView,
                                               ViewManager::GenericDataModel,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Genre,
                                               QUrl{QStringLiteral("image://icon/view-media-genre")},
                                               ViewManager::DelegateWithoutSecondaryText,
                                               ViewManager::ViewHideRating},
                                              {{i18nc("Title of the file browser view", "Files")},
                                               QUrl{QStringLiteral("image://icon/document-open-folder")},
                                               ViewManager::GridView,
                                               ViewManager::FileBrowserModel,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::FileName,
                                               QUrl{QStringLiteral("image://icon/view-media-genre")},
                                               ViewManager::DelegateWithoutSecondaryText,
                                               ViewManager::ViewHideRating
                                              },
                                              {{i18nc("Title of the file radios browser view", "Radios")},
                                               QUrl{QStringLiteral("image://icon/radio")},
                                               ViewManager::ListView,
                                               ViewManager::GenericDataModel,
                                               ElisaUtils::NoFilter,
                                               ElisaUtils::Radio,
                                               Qt::DisplayRole,
                                               Qt::AscendingOrder,
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
