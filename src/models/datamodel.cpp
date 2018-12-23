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

#include "datamodel.h"

#include "modeldataloader.h"
#include "musiclistenersmanager.h"

#include <QUrl>
#include <QTimer>
#include <QPointer>
#include <QVector>
#include <QDebug>

#include <algorithm>

class DataModelPrivate
{
public:

    DataModel::ListTrackDataType mAllTrackData;

    DataModel::ListAlbumDataType mAllAlbumData;

    DataModel::ListArtistDataType mAllArtistData;

    DataModel::ListGenreDataType mAllGenreData;

    ModelDataLoader mDataLoader;

    ElisaUtils::PlayListEntryType mModelType = ElisaUtils::Unknown;

    QString mAlbumTitle;

    QString mAlbumArtist;

    QString mGenre;

};

DataModel::DataModel(QObject *parent) : QAbstractListModel(parent), d(std::make_unique<DataModelPrivate>())
{
}

DataModel::~DataModel()
= default;

int DataModel::rowCount(const QModelIndex &parent) const
{
    auto dataCount = 0;

    if (parent.isValid()) {
        return dataCount;
    }

    dataCount = d->mAllTrackData.size() + d->mAllAlbumData.size() + d->mAllArtistData.size() + d->mAllGenreData.size();

    return dataCount;
}

QHash<int, QByteArray> DataModel::roleNames() const
{
    auto roles = QAbstractListModel::roleNames();

    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::TitleRole)] = "title";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::SecondaryTextRole)] = "secondaryText";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::ImageUrlRole)] = "imageUrl";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::DatabaseIdRole)] = "databaseId";

    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::ArtistRole)] = "artist";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::AllArtistsRole)] = "allArtists";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::HighestTrackRating)] = "highestTrackRating";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::GenreRole)] = "genre";

    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::AlbumRole)] = "album";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::AlbumArtistRole)] = "albumArtist";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::DurationRole)] = "duration";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::TrackNumberRole)] = "trackNumber";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::DiscNumberRole)] = "discNumber";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::RatingRole)] = "rating";
    roles[static_cast<int>(DatabaseInterface::ColumnsRoles::IsSingleDiscAlbumRole)] = "isSingleDiscAlbum";

    return roles;
}

Qt::ItemFlags DataModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant DataModel::data(const QModelIndex &index, int role) const
{
    auto result = QVariant();

    const auto dataCount = d->mAllTrackData.size() + d->mAllAlbumData.size() + d->mAllArtistData.size() + d->mAllGenreData.size();

    Q_ASSERT(index.isValid());
    Q_ASSERT(index.column() == 0);
    Q_ASSERT(index.row() >= 0 && index.row() < dataCount);
    Q_ASSERT(!index.parent().isValid());
    Q_ASSERT(index.model() == this);
    Q_ASSERT(index.internalId() == 0);

    switch(role)
    {
    case Qt::DisplayRole:
        switch(d->mModelType)
        {
        case ElisaUtils::Track:
            result = d->mAllTrackData[index.row()][TrackDataType::key_type::TitleRole];
            break;
        case ElisaUtils::Album:
            result = d->mAllAlbumData[index.row()][AlbumDataType::key_type::TitleRole];
            break;
        case ElisaUtils::Artist:
            result = d->mAllArtistData[index.row()][ArtistDataType::key_type::TitleRole];
            break;
        case ElisaUtils::Genre:
            result = d->mAllGenreData[index.row()][GenreDataType::key_type::TitleRole];
            break;
        case ElisaUtils::Lyricist:
        case ElisaUtils::Composer:
        case ElisaUtils::FileName:
        case ElisaUtils::Unknown:
            break;
        }
        break;
    case DatabaseInterface::ColumnsRoles::DurationRole:
    {
        if (d->mModelType == ElisaUtils::Track) {
            auto trackDuration = d->mAllTrackData[index.row()][TrackDataType::key_type::DurationRole].toTime();
            if (trackDuration.hour() == 0) {
                result = trackDuration.toString(QStringLiteral("mm:ss"));
            } else {
                result = trackDuration.toString();
            }
        }
        break;
    }
    default:
        switch(d->mModelType)
        {
        case ElisaUtils::Track:
            result = d->mAllTrackData[index.row()][static_cast<TrackDataType::key_type>(role)];
            break;
        case ElisaUtils::Album:
            result = d->mAllAlbumData[index.row()][static_cast<AlbumDataType::key_type>(role)];
            break;
        case ElisaUtils::Artist:
            result = d->mAllArtistData[index.row()][static_cast<ArtistDataType::key_type>(role)];
            break;
        case ElisaUtils::Genre:
            result = d->mAllGenreData[index.row()][static_cast<GenreDataType::key_type>(role)];
            break;
        case ElisaUtils::Lyricist:
        case ElisaUtils::Composer:
        case ElisaUtils::FileName:
        case ElisaUtils::Unknown:
            break;
        }
    }

    return result;
}

QModelIndex DataModel::index(int row, int column, const QModelIndex &parent) const
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

QModelIndex DataModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)

    auto result = QModelIndex();

    return result;
}

QString DataModel::title() const
{
    return d->mAlbumTitle;
}

QString DataModel::author() const
{
    return d->mAlbumArtist;
}

void DataModel::initialize(MusicListenersManager *manager, ElisaUtils::PlayListEntryType modelType)
{
    d->mModelType = modelType;

    if (!manager) {
        return;
    }
    manager->connectModel(&d->mDataLoader);

    connectModel(manager);

    connect(this, &DataModel::needData,
            &d->mDataLoader, &ModelDataLoader::loadData);

    Q_EMIT needData(d->mModelType);
}

void DataModel::initializeByAlbumTitleAndArtist(MusicListenersManager *manager, ElisaUtils::PlayListEntryType modelType,
                                                const QString &albumTitle, const QString &albumArtist)
{
    d->mModelType = modelType;
    d->mAlbumTitle = albumTitle;
    d->mAlbumArtist = albumArtist;

    if (!manager) {
        return;
    }

    manager->connectModel(&d->mDataLoader);

    connectModel(manager);

    connect(this, &DataModel::needData,
            &d->mDataLoader, &ModelDataLoader::loadData);

    Q_EMIT needData(d->mModelType);
}

void DataModel::initializeByGenre(MusicListenersManager *manager, ElisaUtils::PlayListEntryType modelType,
                                  const QString &genre)
{
    d->mModelType = modelType;
    d->mGenre = genre;

    if (!manager) {
        return;
    }

    manager->connectModel(&d->mDataLoader);

    connectModel(manager);

    connect(this, &DataModel::needDataByGenre,
            &d->mDataLoader, &ModelDataLoader::loadDataByGenre);

    Q_EMIT needDataByGenre(d->mModelType, genre);
}

void DataModel::initializeByArtist(MusicListenersManager *manager, ElisaUtils::PlayListEntryType modelType,
                                   const QString &artist)
{
    d->mModelType = modelType;
    d->mAlbumArtist = artist;

    if (!manager) {
        return;
    }

    manager->connectModel(&d->mDataLoader);

    connectModel(manager);

    connect(this, &DataModel::needDataByArtist,
            &d->mDataLoader, &ModelDataLoader::loadDataByArtist);

    Q_EMIT needDataByArtist(d->mModelType, artist);
}

void DataModel::initializeByGenreAndArtist(MusicListenersManager *manager, ElisaUtils::PlayListEntryType modelType,
                                           const QString &genre, const QString &artist)
{
    d->mModelType = modelType;
    d->mGenre = genre;
    d->mAlbumArtist = artist;

    if (!manager) {
        return;
    }

    manager->connectModel(&d->mDataLoader);

    connectModel(manager);

    connect(this, &DataModel::needDataByGenreAndArtist,
            &d->mDataLoader, &ModelDataLoader::loadDataByGenreAndArtist);

    Q_EMIT needDataByGenreAndArtist(d->mModelType, genre, artist);
}

int DataModel::trackIndexFromId(qulonglong id) const
{
    int result;

    for (result = 0; result < d->mAllTrackData.size(); ++result) {
        if (d->mAllTrackData[result].databaseId() == id) {
            return result;
        }
    }

    result = -1;

    return result;
}

void DataModel::connectModel(MusicListenersManager *manager)
{
    connect(manager->viewDatabase(), &DatabaseInterface::genresAdded,
            this, &DataModel::genresAdded);
    connect(manager->viewDatabase(), &DatabaseInterface::albumsAdded,
            this, &DataModel::albumsAdded);
    connect(manager->viewDatabase(), &DatabaseInterface::albumModified,
            this, &DataModel::albumModified);
    connect(manager->viewDatabase(), &DatabaseInterface::albumRemoved,
            this, &DataModel::albumRemoved);
    connect(manager->viewDatabase(), &DatabaseInterface::tracksAdded,
            this, &DataModel::tracksAdded);
    connect(manager->viewDatabase(), &DatabaseInterface::trackModified,
            this, &DataModel::trackModified);
    connect(manager->viewDatabase(), &DatabaseInterface::trackRemoved,
            this, &DataModel::trackRemoved);
    connect(manager->viewDatabase(), &DatabaseInterface::artistsAdded,
            this, &DataModel::artistsAdded);
    connect(manager->viewDatabase(), &DatabaseInterface::artistRemoved,
            this, &DataModel::artistRemoved);

    connect(&d->mDataLoader, &ModelDataLoader::allTracksData,
            this, &DataModel::tracksAdded);
    connect(&d->mDataLoader, &ModelDataLoader::allAlbumsData,
            this, &DataModel::albumsAdded);
    connect(&d->mDataLoader, &ModelDataLoader::allArtistsData,
            this, &DataModel::artistsAdded);
    connect(&d->mDataLoader, &ModelDataLoader::allGenresData,
            this, &DataModel::genresAdded);
}

void DataModel::tracksAdded(ListTrackDataType newData)
{
    if (newData.isEmpty() || d->mModelType != ElisaUtils::Track) {
        return;
    }

    if (!d->mAlbumTitle.isEmpty() && !d->mAlbumArtist.isEmpty()) {
        for (const auto &newTrack : newData) {
            if (newTrack.album() != d->mAlbumTitle) {
                continue;
            }

            if (newTrack.albumArtist() != d->mAlbumArtist) {
                continue;
            }

            auto trackIndex = trackIndexFromId(newTrack.databaseId());

            if (trackIndex != -1) {
                continue;
            }

            bool trackInserted = false;
            for (int trackIndex = 0; trackIndex < d->mAllTrackData.count(); ++trackIndex) {
                const auto &oneTrack = d->mAllTrackData[trackIndex];

                if (oneTrack.discNumber() == newTrack.discNumber() && oneTrack.trackNumber() > newTrack.trackNumber()) {
                    beginInsertRows({}, trackIndex, trackIndex);
                    d->mAllTrackData.insert(trackIndex, newTrack);
                    endInsertRows();
                    trackInserted = true;
                    break;
                }
            }

            if (!trackInserted) {
                beginInsertRows({}, d->mAllTrackData.count(), d->mAllTrackData.count());
                d->mAllTrackData.insert(d->mAllTrackData.count(), newTrack);
                endInsertRows();
            }
        }
    } else {
        if (d->mAllTrackData.isEmpty()) {
            beginInsertRows({}, 0, newData.size() - 1);
            d->mAllTrackData.swap(newData);
            endInsertRows();
        } else {
            beginInsertRows({}, d->mAllTrackData.size(), d->mAllTrackData.size() + newData.size() - 1);
            d->mAllTrackData.append(newData);
            endInsertRows();
        }
    }
}

void DataModel::trackModified(const TrackDataType &modifiedTrack)
{
    if (d->mModelType != ElisaUtils::Track) {
        return;
    }

    if (!d->mAlbumTitle.isEmpty() && !d->mAlbumArtist.isEmpty()) {
        if (modifiedTrack.album() != d->mAlbumTitle) {
            return;
        }

        auto trackIndex = trackIndexFromId(modifiedTrack.databaseId());

        if (trackIndex == -1) {
            return;
        }

        d->mAllTrackData[trackIndex] = modifiedTrack;
        Q_EMIT dataChanged(index(trackIndex, 0), index(trackIndex, 0));
    } else {
        auto itTrack = std::find_if(d->mAllTrackData.begin(), d->mAllTrackData.end(),
                                    [modifiedTrack](auto track) {
            return track.databaseId() == modifiedTrack.databaseId();
        });

        if (itTrack == d->mAllTrackData.end()) {
            return;
        }

        auto position = itTrack - d->mAllTrackData.begin();

        d->mAllTrackData[position] = modifiedTrack;

        Q_EMIT dataChanged(index(position, 0), index(position, 0));
    }
}

void DataModel::trackRemoved(qulonglong removedTrackId)
{
    if (d->mModelType != ElisaUtils::Track) {
        return;
    }

    if (!d->mAlbumTitle.isEmpty() && !d->mAlbumArtist.isEmpty()) {
        auto trackIndex = trackIndexFromId(removedTrackId);

        if (trackIndex == -1) {
            return;
        }

        beginRemoveRows({}, trackIndex, trackIndex);
        d->mAllTrackData.removeAt(trackIndex);
        endRemoveRows();
    } else {
        auto itTrack = std::find_if(d->mAllTrackData.begin(), d->mAllTrackData.end(),
                                    [removedTrackId](auto track) {return track.databaseId() == removedTrackId;});

        if (itTrack == d->mAllTrackData.end()) {
            return;
        }

        auto position = itTrack - d->mAllTrackData.begin();

        beginRemoveRows({}, position, position);
        d->mAllTrackData.erase(itTrack);
        endRemoveRows();
    }
}

void DataModel::genresAdded(DataModel::ListGenreDataType newData)
{
    if (newData.isEmpty() || d->mModelType != ElisaUtils::Genre) {
        return;
    }

    if (d->mAllGenreData.isEmpty()) {
        beginInsertRows({}, d->mAllGenreData.size(), newData.size() - 1);
        d->mAllGenreData.swap(newData);
        endInsertRows();
    } else {
        beginInsertRows({}, d->mAllGenreData.size(), d->mAllGenreData.size() + newData.size() - 1);
        d->mAllGenreData.append(newData);
        endInsertRows();
    }
}

void DataModel::artistsAdded(DataModel::ListArtistDataType newData)
{
    if (newData.isEmpty() || d->mModelType != ElisaUtils::Artist) {
        return;
    }

    if (d->mAllArtistData.isEmpty()) {
        beginInsertRows({}, d->mAllArtistData.size(), newData.size() - 1);
        d->mAllArtistData.swap(newData);
        endInsertRows();
    } else {
        beginInsertRows({}, d->mAllArtistData.size(), d->mAllArtistData.size() + newData.size() - 1);
        d->mAllArtistData.append(newData);
        endInsertRows();
    }
}

void DataModel::artistRemoved(qulonglong removedDatabaseId)
{
    if (d->mModelType != ElisaUtils::Artist) {
        return;
    }

    auto removedDataIterator = d->mAllArtistData.end();

    removedDataIterator = std::find_if(d->mAllArtistData.begin(), d->mAllArtistData.end(),
                                       [removedDatabaseId](auto album) {return album.databaseId() == removedDatabaseId;});

    if (removedDataIterator == d->mAllArtistData.end()) {
        return;
    }

    int dataIndex = removedDataIterator - d->mAllArtistData.begin();

    beginRemoveRows({}, dataIndex, dataIndex);

    d->mAllArtistData.erase(removedDataIterator);

    endRemoveRows();
}

void DataModel::albumsAdded(DataModel::ListAlbumDataType newData)
{
    if (newData.isEmpty() || d->mModelType != ElisaUtils::Album) {
        return;
    }

    if (d->mAllAlbumData.isEmpty()) {
        beginInsertRows({}, d->mAllAlbumData.size(), newData.size() - 1);
        d->mAllAlbumData.swap(newData);
        endInsertRows();
    } else {
        beginInsertRows({}, d->mAllAlbumData.size(), d->mAllAlbumData.size() + newData.size() - 1);
        d->mAllAlbumData.append(newData);
        endInsertRows();
    }
}

void DataModel::albumRemoved(qulonglong removedDatabaseId)
{
    if (d->mModelType != ElisaUtils::Album) {
        return;
    }

    auto removedDataIterator = d->mAllAlbumData.end();

    removedDataIterator = std::find_if(d->mAllAlbumData.begin(), d->mAllAlbumData.end(),
                                       [removedDatabaseId](auto album) {return album.databaseId() == removedDatabaseId;});

    if (removedDataIterator == d->mAllAlbumData.end()) {
        return;
    }

    int dataIndex = removedDataIterator - d->mAllAlbumData.begin();

    beginRemoveRows({}, dataIndex, dataIndex);

    d->mAllAlbumData.erase(removedDataIterator);

    endRemoveRows();
}

void DataModel::albumModified(const DataModel::AlbumDataType &modifiedAlbum)
{
    if (d->mModelType != ElisaUtils::Album) {
        return;
    }

    auto modifiedAlbumIterator = std::find_if(d->mAllAlbumData.begin(), d->mAllAlbumData.end(),
                                              [modifiedAlbum](auto album) {
        return album.databaseId() == modifiedAlbum.databaseId();
    });

    if (modifiedAlbumIterator == d->mAllAlbumData.end()) {
        return;
    }

    auto albumIndex = modifiedAlbumIterator - d->mAllAlbumData.begin();

    Q_EMIT dataChanged(index(albumIndex, 0), index(albumIndex, 0));
}

#include "moc_datamodel.cpp"
