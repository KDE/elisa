/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "alltracksmodel.h"

#include "elisautils.h"

#include <algorithm>

#include <QDebug>

class AllTracksModelPrivate
{
public:

    QHash<qulonglong, MusicAudioTrack> mAllTracks;

    QList<qulonglong> mIds;

};

AllTracksModel::AllTracksModel(QObject *parent) : QAbstractItemModel(parent), d(std::make_unique<AllTracksModelPrivate>())
{
}

AllTracksModel::~AllTracksModel()
= default;

int AllTracksModel::rowCount(const QModelIndex &parent) const
{
    auto tracksCount = 0;

    if (parent.isValid()) {
        return tracksCount;
    }

    tracksCount = d->mAllTracks.size();

    return tracksCount;
}

QHash<int, QByteArray> AllTracksModel::roleNames() const
{
    auto roles = QAbstractItemModel::roleNames();

    roles[static_cast<int>(ElisaUtils::ColumnsRoles::TitleRole)] = "title";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::DurationRole)] = "duration";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::ArtistRole)] = "artist";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::AlbumRole)] = "album";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::AlbumArtistRole)] = "albumArtist";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::TrackNumberRole)] = "trackNumber";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::DiscNumberRole)] = "discNumber";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::RatingRole)] = "rating";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::GenreRole)] = "genre";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::LyricistRole)] = "lyricist";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::ComposerRole)] = "composer";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::CommentRole)] = "comment";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::YearRole)] = "year";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::ChannelsRole)] = "channels";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::BitRateRole)] = "bitRate";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::SampleRateRole)] = "sampleRate";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::ImageRole)] = "image";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::DatabaseIdRole)] = "databaseId";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::IsSingleDiscAlbumRole)] = "isSingleDiscAlbum";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::ContainerDataRole)] = "containerData";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::ResourceRole)] = "trackResource";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::SecondaryTextRole)] = "secondaryText";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::ImageUrlRole)] = "imageUrl";
    roles[static_cast<int>(ElisaUtils::ColumnsRoles::ShadowForImageRole)] = "shadowForImage";

    return roles;
}

Qt::ItemFlags AllTracksModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant AllTracksModel::data(const QModelIndex &index, int role) const
{
    auto result = QVariant();

    const auto tracksCount = d->mAllTracks.size();

    Q_ASSERT(index.isValid());
    Q_ASSERT(index.column() == 0);
    Q_ASSERT(index.row() >= 0 && index.row() < tracksCount);
    Q_ASSERT(!index.parent().isValid());
    Q_ASSERT(index.model() == this);
    Q_ASSERT(index.internalId() == 0);

    const auto &track = d->mAllTracks[d->mIds[index.row()]];

    switch(role)
    {
    case ElisaUtils::ColumnsRoles::TitleRole:
        if (d->mAllTracks[d->mIds[index.row()]].title().isEmpty()) {
            result = d->mAllTracks[d->mIds[index.row()]].resourceURI().fileName();
        } else {
            result = d->mAllTracks[d->mIds[index.row()]].title();
        }
        break;
    case ElisaUtils::ColumnsRoles::MilliSecondsDurationRole:
        result = d->mAllTracks[d->mIds[index.row()]].duration().msecsSinceStartOfDay();
        break;
    case ElisaUtils::ColumnsRoles::DurationRole:
    {
        QTime trackDuration = d->mAllTracks[d->mIds[index.row()]].duration();
        if (trackDuration.hour() == 0) {
            result = trackDuration.toString(QStringLiteral("mm:ss"));
        } else {
            result = trackDuration.toString();
        }
        break;
    }
    case ElisaUtils::ColumnsRoles::ArtistRole:
        result = d->mAllTracks[d->mIds[index.row()]].artist();
        break;
    case ElisaUtils::ColumnsRoles::AlbumRole:
        result = d->mAllTracks[d->mIds[index.row()]].albumName();
        break;
    case ElisaUtils::ColumnsRoles::AlbumArtistRole:
        result = d->mAllTracks[d->mIds[index.row()]].albumArtist();
        break;
    case ElisaUtils::ColumnsRoles::TrackNumberRole:
        result = d->mAllTracks[d->mIds[index.row()]].trackNumber();
        break;
    case ElisaUtils::ColumnsRoles::DiscNumberRole:
        result = d->mAllTracks[d->mIds[index.row()]].discNumber();
        break;
    case ElisaUtils::ColumnsRoles::IsSingleDiscAlbumRole:
        result = d->mAllTracks[d->mIds[index.row()]].isSingleDiscAlbum();
        break;
    case ElisaUtils::ColumnsRoles::RatingRole:
        result = d->mAllTracks[d->mIds[index.row()]].rating();
        break;
    case ElisaUtils::ColumnsRoles::GenreRole:
        result = d->mAllTracks[d->mIds[index.row()]].genre();
        break;
    case ElisaUtils::ColumnsRoles::LyricistRole:
        result = d->mAllTracks[d->mIds[index.row()]].lyricist();
        break;
    case ElisaUtils::ColumnsRoles::ComposerRole:
        result = d->mAllTracks[d->mIds[index.row()]].composer();
        break;
    case ElisaUtils::ColumnsRoles::CommentRole:
        result = d->mAllTracks[d->mIds[index.row()]].comment();
        break;
    case ElisaUtils::ColumnsRoles::YearRole:
        result = d->mAllTracks[d->mIds[index.row()]].year();
        break;
    case ElisaUtils::ColumnsRoles::ChannelsRole:
        result = d->mAllTracks[d->mIds[index.row()]].channels();
        break;
    case ElisaUtils::ColumnsRoles::BitRateRole:
        result = d->mAllTracks[d->mIds[index.row()]].bitRate();
        break;
    case ElisaUtils::ColumnsRoles::SampleRateRole:
        result = d->mAllTracks[d->mIds[index.row()]].sampleRate();
        break;
    case ElisaUtils::ColumnsRoles::ImageRole:
    {
        const auto &imageUrl = d->mAllTracks[d->mIds[index.row()]].albumCover();
        if (imageUrl.isValid()) {
            result = imageUrl;
        }
        break;
    }
    case ElisaUtils::ColumnsRoles::ResourceRole:
        result = d->mAllTracks[d->mIds[index.row()]].resourceURI();
        break;
    case ElisaUtils::ColumnsRoles::IdRole:
        result = d->mAllTracks[d->mIds[index.row()]].title();
        break;
    case ElisaUtils::ColumnsRoles::DatabaseIdRole:
        result = d->mAllTracks[d->mIds[index.row()]].databaseId();
        break;
    case ElisaUtils::ColumnsRoles::ContainerDataRole:
        result = QVariant::fromValue(d->mAllTracks[d->mIds[index.row()]]);
        break;
    case Qt::DisplayRole:
        result = track.title();
        break;
    case ElisaUtils::ColumnsRoles::SecondaryTextRole:
    {
        auto secondaryText = QString();
        secondaryText = QStringLiteral("<b>%1 - %2</b>%3");

        secondaryText = secondaryText.arg(track.trackNumber());
        secondaryText = secondaryText.arg(track.title());

        if (track.artist() == track.albumArtist()) {
            secondaryText = secondaryText.arg(QString());
        } else {
            auto artistText = QString();
            artistText = QStringLiteral(" - <i>%1</i>");
            artistText = artistText.arg(track.artist());
            secondaryText = secondaryText.arg(artistText);
        }

        result = secondaryText;
        break;
    }
    case ElisaUtils::ColumnsRoles::ImageUrlRole:
    {
        const auto &imageUrl = d->mAllTracks[d->mIds[index.row()]].albumCover();
        if (imageUrl.isValid()) {
            result = imageUrl;
        } else {
            result = QUrl(QStringLiteral("image://icon/media-optical-audio"));
        }
        break;
    }
    case ElisaUtils::ColumnsRoles::ShadowForImageRole:
        result = d->mAllTracks[d->mIds[index.row()]].albumCover().isValid();
        break;
    }

    return result;
}

QModelIndex AllTracksModel::index(int row, int column, const QModelIndex &parent) const
{
    auto result = QModelIndex();

    if (column != 0) {
        return result;
    }

    if (parent.isValid()) {
        return result;
    }

    if (row > d->mAllTracks.size() - 1) {
        return result;
    }

    result = createIndex(row, column);

    return result;
}

QModelIndex AllTracksModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)

    auto result = QModelIndex();

    return result;
}

int AllTracksModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 1;
}

void AllTracksModel::tracksAdded(const QList<MusicAudioTrack> &allTracks)
{
    auto newAllTracks = d->mAllTracks;
    auto newTracksIds = QList<qulonglong>();

    int countNewTracks = 0;

    for (const auto &oneTrack : allTracks) {
        if (!newAllTracks.contains(oneTrack.databaseId())) {
            newAllTracks[oneTrack.databaseId()] = oneTrack;
            newTracksIds.push_back(oneTrack.databaseId());
            ++countNewTracks;
        }
    }

    if (countNewTracks > 0) {
        beginInsertRows({}, d->mAllTracks.size(), d->mAllTracks.size() + countNewTracks - 1);

        d->mAllTracks = newAllTracks;
        d->mIds.append(newTracksIds);

        endInsertRows();
    }
}

void AllTracksModel::trackRemoved(qulonglong removedTrackId)
{
    auto itTrack = std::find(d->mIds.begin(), d->mIds.end(), removedTrackId);
    if (itTrack == d->mIds.end()) {
        return;
    }

    auto position = itTrack - d->mIds.begin();

    beginRemoveRows({}, position, position);
    d->mIds.erase(itTrack);
    d->mAllTracks.remove(removedTrackId);
    endRemoveRows();
}

void AllTracksModel::trackModified(const MusicAudioTrack &modifiedTrack)
{
    auto trackExists = (d->mAllTracks.contains(modifiedTrack.databaseId()));
    if (!trackExists) {
        return;
    }

    auto itTrack = std::find(d->mIds.begin(), d->mIds.end(), modifiedTrack.databaseId());
    if (itTrack == d->mIds.end()) {
        return;
    }

    auto position = itTrack - d->mIds.begin();

    d->mAllTracks[modifiedTrack.databaseId()] = modifiedTrack;
    Q_EMIT dataChanged(index(position, 0), index(position, 0));
}


#include "moc_alltracksmodel.cpp"
