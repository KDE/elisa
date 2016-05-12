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

#include <QtCore/QVector>
#include <QtCore/QUrl>
#include <QtCore/QPointer>
#include <QtCore/QDebug>

class UpnpAlbumModelPrivate
{
public:

    UpnpControlContentDirectory *mContentDirectory = nullptr;

    bool mUseLocalIcons = false;

    DidlParser mDidlParser;

    QString mBrowseFlag;

    QString mFilter;

    QString mSortCriteria;

    QVector<QString> mChilds;

    QHash<QString, QPointer<DidlParser>> mAlbumParsers;

    QString mServerName;

};

UpnpAlbumModel::UpnpAlbumModel(QObject *parent) : AbstractAlbumModel(parent), d(new UpnpAlbumModelPrivate)
{
    d->mDidlParser.setSearchCriteria(QStringLiteral("upnp:class = \"object.container.album.musicAlbum\""));
    d->mDidlParser.setParentId(QStringLiteral("0"));

    connect(&d->mDidlParser, &DidlParser::isDataValidChanged, this, &UpnpAlbumModel::contentChanged);
}

UpnpAlbumModel::~UpnpAlbumModel()
{
    delete d;
}

bool UpnpAlbumModel::canFetchMore(const QModelIndex &parent) const
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

void UpnpAlbumModel::fetchMore(const QModelIndex &parent)
{
    if (!d->mAlbumParsers.contains(d->mChilds[parent.row()])) {
        d->mAlbumParsers[d->mChilds[parent.row()]] = new DidlParser(this);

        auto newParser = d->mAlbumParsers[d->mChilds[parent.row()]];

        newParser->setBrowseFlag(d->mDidlParser.browseFlag());
        newParser->setFilter(d->mDidlParser.filter());
        newParser->setSortCriteria(d->mDidlParser.sortCriteria());
        newParser->setContentDirectory(d->mDidlParser.contentDirectory());
        newParser->setParentId(d->mChilds[parent.row()]);

        connect(newParser, &DidlParser::isDataValidChanged, this, &UpnpAlbumModel::contentChanged);

        newParser->browse();
    }
}

QVariant UpnpAlbumModel::internalDataTrack(const QModelIndex &index, int role, DidlParser *currentParser) const
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
    case ColumnsRoles::TrackNumberRole:
        return currentParser->newMusicTracks()[musicTrackId].mTrackNumber;
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
    case ColumnsRoles::IsPlayingRole:
        return false;
    }

    return {};
}

UpnpControlContentDirectory *UpnpAlbumModel::contentDirectory() const
{
    return d->mContentDirectory;
}

DidlParser *UpnpAlbumModel::didlParser() const
{
    return &d->mDidlParser;
}

const QString &UpnpAlbumModel::browseFlag() const
{
    return d->mBrowseFlag;
}

const QString &UpnpAlbumModel::filter() const
{
    return d->mFilter;
}

const QString &UpnpAlbumModel::sortCriteria() const
{
    return d->mSortCriteria;
}

QString UpnpAlbumModel::serverName() const
{
    return d->mServerName;
}

bool UpnpAlbumModel::useLocalIcons() const
{
    return d->mUseLocalIcons;
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

    d->mDidlParser.setContentDirectory(directory);
    d->mDidlParser.setBrowseFlag(browseFlag());
    d->mDidlParser.setFilter(filter());
    d->mDidlParser.setSortCriteria(sortCriteria());
    d->mDidlParser.search();

    endResetModel();

    Q_EMIT contentDirectoryChanged();
}

void UpnpAlbumModel::setBrowseFlag(const QString &flag)
{
    d->mBrowseFlag = flag;
    Q_EMIT browseFlagChanged();
}

void UpnpAlbumModel::setFilter(const QString &flag)
{
    d->mFilter = flag;
    Q_EMIT filterChanged();
}

void UpnpAlbumModel::setSortCriteria(const QString &criteria)
{
    d->mSortCriteria = criteria;
    Q_EMIT sortCriteriaChanged();
}

void UpnpAlbumModel::setServerName(QString serverName)
{
    if (d->mServerName == serverName)
        return;

    d->mServerName = serverName;
    emit serverNameChanged();
}

void UpnpAlbumModel::setUseLocalIcons(bool useLocalIcons)
{
    if (d->mUseLocalIcons == useLocalIcons)
        return;

    d->mUseLocalIcons = useLocalIcons;
    emit useLocalIconsChanged();
}

void UpnpAlbumModel::contentChanged(const QString &parentId)
{
    if (parentId == QStringLiteral("0")) {
        d->mChilds = d->mDidlParser.newAlbumIds();

        QVector<MusicAlbum> allAlbums;

        const auto &allNewAlbums = d->mDidlParser.newAlbums();

        int cptAlbum = 0;
        const auto &allChilds = d->mChilds;
        for (const auto &newAlbumId : allChilds) {
            const auto &newAlbum = allNewAlbums[newAlbumId];

            allAlbums.push_back(newAlbum);
            ++cptAlbum;
        }

        albumsList(allAlbums);

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

    QHash<QString, QVector<MusicAudioTrack>> albumTracks;

    const auto &newTracks = childParser->newMusicTracks();
    for(const auto &oneTrack : newTracks) {
        albumTracks[oneTrack.mAlbumName].push_back(oneTrack);

        auto &allTracks = albumTracks[oneTrack.mAlbumName];
        std::sort(allTracks.begin(), allTracks.end(), [](const auto &left, const auto &right) {return left.mTrackNumber <= right.mTrackNumber;});
    }

    tracksList(albumTracks, {});
}


#include "moc_upnpalbummodel.cpp"
