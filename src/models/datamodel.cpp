/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "datamodel.h"

#include "modeldataloader.h"
#include "musiclistenersmanager.h"

#include "models/modelLogging.h"

#include <algorithm>

class DataModelPrivate
{
public:

    DataModel::ListTrackDataType mAllTrackData;

    DataModel::ListRadioDataType mAllRadiosData;

    DataModel::ListAlbumDataType mAllAlbumData;

    DataModel::ListArtistDataType mAllArtistData;

    DataModel::ListGenreDataType mAllGenreData;

    ModelDataLoader *mDataLoader = nullptr;

    ElisaUtils::PlayListEntryType mModelType = ElisaUtils::Unknown;

    ElisaUtils::FilterType mFilterType = ElisaUtils::UnknownFilter;

    QString mArtist;

    QString mAlbumTitle;

    QString mAlbumArtist;

    QString mGenre;

    qulonglong mDatabaseId = 0;

    bool mIsBusy = false;

};

DataModel::DataModel(QObject *parent) : QAbstractListModel(parent), d(std::make_unique<DataModelPrivate>())
{
    d->mDataLoader = new ModelDataLoader;
    connect(this, &DataModel::destroyed, d->mDataLoader, &ModelDataLoader::deleteLater);
}

DataModel::~DataModel()
= default;

int DataModel::rowCount(const QModelIndex &parent) const
{
    auto dataCount = 0;

    if (parent.isValid()) {
        return dataCount;
    }

    dataCount = d->mModelType == ElisaUtils::Radio ? d->mAllRadiosData.size()
                                                   : d->mAllTrackData.size() + d->mAllAlbumData.size() + d->mAllArtistData.size() + d->mAllGenreData.size();

    return dataCount;
}

QHash<int, QByteArray> DataModel::roleNames() const
{
    auto roles = QAbstractListModel::roleNames();

    roles[static_cast<int>(DataTypes::ColumnsRoles::TitleRole)] = "title";
    roles[static_cast<int>(DataTypes::ColumnsRoles::SecondaryTextRole)] = "secondaryText";
    roles[static_cast<int>(DataTypes::ColumnsRoles::ImageUrlRole)] = "imageUrl";
    roles[static_cast<int>(DataTypes::ColumnsRoles::MultipleImageUrlsRole)] = "multipleImageUrls";
    roles[static_cast<int>(DataTypes::ColumnsRoles::DatabaseIdRole)] = "databaseId";
    roles[static_cast<int>(DataTypes::ColumnsRoles::ElementTypeRole)] = "dataType";
    roles[static_cast<int>(DataTypes::ColumnsRoles::ResourceRole)] = "url";

    roles[static_cast<int>(DataTypes::ColumnsRoles::ArtistRole)] = "artist";
    roles[static_cast<int>(DataTypes::ColumnsRoles::AllArtistsRole)] = "allArtists";
    roles[static_cast<int>(DataTypes::ColumnsRoles::HighestTrackRating)] = "highestTrackRating";
    roles[static_cast<int>(DataTypes::ColumnsRoles::GenreRole)] = "genre";

    roles[static_cast<int>(DataTypes::ColumnsRoles::AlbumRole)] = "album";
    roles[static_cast<int>(DataTypes::ColumnsRoles::AlbumArtistRole)] = "albumArtist";
    roles[static_cast<int>(DataTypes::ColumnsRoles::StringDurationRole)] = "duration";
    roles[static_cast<int>(DataTypes::ColumnsRoles::TrackNumberRole)] = "trackNumber";
    roles[static_cast<int>(DataTypes::ColumnsRoles::DiscNumberRole)] = "discNumber";
    roles[static_cast<int>(DataTypes::ColumnsRoles::RatingRole)] = "rating";
    roles[static_cast<int>(DataTypes::ColumnsRoles::YearRole)] = "year";
    roles[static_cast<int>(DataTypes::ColumnsRoles::IsSingleDiscAlbumRole)] = "isSingleDiscAlbum";
    roles[static_cast<int>(DataTypes::ColumnsRoles::FullDataRole)] = "fullData";
    roles[static_cast<int>(DataTypes::ColumnsRoles::HasChildrenRole)] = "hasChildren";

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

    if (!index.isValid()) {
        return result;
    }

    const auto dataCount = d->mModelType == ElisaUtils::Radio ? d->mAllRadiosData.size() : d->mAllTrackData.size() + d->mAllAlbumData.size() + d->mAllArtistData.size() + d->mAllGenreData.size();

    Q_ASSERT(index.isValid());
    Q_ASSERT(index.column() == 0);
    Q_ASSERT(!index.parent().isValid());
    Q_ASSERT(index.model() == this);
    Q_ASSERT(index.internalId() == 0);
    Q_ASSERT(index.row() >= 0 && index.row() < dataCount);

    switch(role)
    {
    case Qt::DisplayRole:
        switch(d->mModelType)
        {
        case ElisaUtils::Track:
            result = d->mAllTrackData[index.row()][TrackDataType::key_type::TitleRole];
            if (result.toString().isEmpty()) {
                result = d->mAllTrackData[index.row()][TrackDataType::key_type::ResourceRole].toUrl().fileName();
            }
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
        case ElisaUtils::Radio:
            result = d->mAllRadiosData[index.row()][GenreDataType::key_type::TitleRole];
            break;
        case ElisaUtils::Lyricist:
        case ElisaUtils::Composer:
        case ElisaUtils::FileName:
        case ElisaUtils::Container:
        case ElisaUtils::Unknown:
        case ElisaUtils::PlayList:
            break;
        }
        break;
    case DataTypes::ColumnsRoles::StringDurationRole:
    {
        switch (d->mModelType)
        {
        case ElisaUtils::Track:
        {
            auto trackDuration = d->mAllTrackData[index.row()][TrackDataType::key_type::DurationRole].toTime();
            if (trackDuration.hour() == 0) {
                result = trackDuration.toString(QStringLiteral("mm:ss"));
            } else {
                result = trackDuration.toString();
            }
            break;
        }
        case ElisaUtils::Album:
        case ElisaUtils::Artist:
        case ElisaUtils::Genre:
        case ElisaUtils::Lyricist:
        case ElisaUtils::Composer:
        case ElisaUtils::FileName:
        case ElisaUtils::Radio:
        case ElisaUtils::Container:
        case ElisaUtils::Unknown:
        case ElisaUtils::PlayList:
            break;
        }
        break;
    }
    case DataTypes::ColumnsRoles::IsSingleDiscAlbumRole:
    {
        switch (d->mModelType)
        {
        case ElisaUtils::Track:
            result = d->mAllTrackData[index.row()][TrackDataType::key_type::IsSingleDiscAlbumRole];
            break;
        case ElisaUtils::Radio:
            result = false;
            break;
        case ElisaUtils::Album:
            result = d->mAllAlbumData[index.row()][AlbumDataType::key_type::IsSingleDiscAlbumRole];
            break;
        case ElisaUtils::Artist:
        case ElisaUtils::Genre:
        case ElisaUtils::Lyricist:
        case ElisaUtils::Composer:
        case ElisaUtils::FileName:
        case ElisaUtils::Container:
        case ElisaUtils::Unknown:
        case ElisaUtils::PlayList:
            break;
        }
        break;
    }
    case DataTypes::ColumnsRoles::ArtistRole:
    {
        switch (d->mModelType)
        {
        case ElisaUtils::Track:
        {
            auto itArtist = d->mAllTrackData[index.row()].find(TrackDataType::key_type::ArtistRole);
            if (itArtist != d->mAllTrackData[index.row()].end()) {
                result = d->mAllTrackData[index.row()][TrackDataType::key_type::ArtistRole];
            } else {
                result = d->mAllTrackData[index.row()][TrackDataType::key_type::AlbumArtistRole];
            }
            break;
        }
        case ElisaUtils::Album:
            result = d->mAllAlbumData[index.row()][static_cast<AlbumDataType::key_type>(role)];
            break;
        case ElisaUtils::Artist:
            result = d->mAllArtistData[index.row()][static_cast<ArtistDataType::key_type>(role)];
            break;
        case ElisaUtils::Genre:
            result = d->mAllGenreData[index.row()][static_cast<GenreDataType::key_type>(role)];
            break;
        case ElisaUtils::Radio:
            result = d->mAllRadiosData[index.row()][static_cast<TrackDataType::key_type>(role)];
            break;
        case ElisaUtils::Lyricist:
        case ElisaUtils::Composer:
        case ElisaUtils::FileName:
        case ElisaUtils::Container:
        case ElisaUtils::Unknown:
        case ElisaUtils::PlayList:
            break;
        }
        break;
    }
    case DataTypes::ColumnsRoles::FullDataRole:
        switch (d->mModelType)
        {
        case ElisaUtils::Track:
            result = QVariant::fromValue(static_cast<DataTypes::MusicDataType>(d->mAllTrackData[index.row()]));
            break;
        case ElisaUtils::Radio:
            result = QVariant::fromValue(static_cast<DataTypes::MusicDataType>(d->mAllRadiosData[index.row()]));
            break;
        case ElisaUtils::Album:
            result = QVariant::fromValue(static_cast<DataTypes::MusicDataType>(d->mAllAlbumData[index.row()]));
            break;
        case ElisaUtils::Artist:
            result = QVariant::fromValue(static_cast<DataTypes::MusicDataType>(d->mAllArtistData[index.row()]));
            break;
        case ElisaUtils::Genre:
            result = QVariant::fromValue(static_cast<DataTypes::MusicDataType>(d->mAllGenreData[index.row()]));
            break;
        case ElisaUtils::Lyricist:
        case ElisaUtils::Composer:
        case ElisaUtils::FileName:
        case ElisaUtils::Container:
        case ElisaUtils::Unknown:
        case ElisaUtils::PlayList:
            break;
        }
        break;
    case DataTypes::ColumnsRoles::ResourceRole:
    {
        switch (d->mModelType)
        {
        case ElisaUtils::Track:
        case ElisaUtils::FileName:
            result = d->mAllTrackData[index.row()][TrackDataType::key_type::ResourceRole];
            break;
        case ElisaUtils::Radio:
            result = d->mAllRadiosData[index.row()][TrackDataType::key_type::ResourceRole];
            break;
        case ElisaUtils::Album:
        case ElisaUtils::Artist:
        case ElisaUtils::Genre:
        case ElisaUtils::Lyricist:
        case ElisaUtils::Composer:
        case ElisaUtils::Container:
        case ElisaUtils::Unknown:
        case ElisaUtils::PlayList:
            result = QUrl{};
            break;
        }
        break;
    }
    case DataTypes::ColumnsRoles::HasChildrenRole:
    {
        switch (d->mModelType)
        {
        case ElisaUtils::Track:
        case ElisaUtils::FileName:
        case ElisaUtils::Radio:
        case ElisaUtils::Unknown:
            result = false;
            break;
        case ElisaUtils::Album:
        case ElisaUtils::Artist:
        case ElisaUtils::Genre:
        case ElisaUtils::Lyricist:
        case ElisaUtils::Composer:
        case ElisaUtils::Container:
        case ElisaUtils::PlayList:
            result = true;
            break;
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
        case ElisaUtils::Radio:
            result = d->mAllRadiosData[index.row()][static_cast<TrackDataType::key_type>(role)];
            break;
        case ElisaUtils::Lyricist:
        case ElisaUtils::Composer:
        case ElisaUtils::FileName:
        case ElisaUtils::Container:
        case ElisaUtils::Unknown:
        case ElisaUtils::PlayList:
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

bool DataModel::isBusy() const
{
    return d->mIsBusy;
}

void DataModel::initializeByData(MusicListenersManager *manager, DatabaseInterface *database,
                                 ElisaUtils::PlayListEntryType modelType, ElisaUtils::FilterType filter,
                                 const DataTypes::DataType &dataFilter)
{
    qCDebug(orgKdeElisaModel()) << "DataModel::initialize" << modelType << filter << dataFilter;

    d->mDatabaseId = dataFilter[DataTypes::DatabaseIdRole].toULongLong();
    d->mGenre = dataFilter[DataTypes::GenreRole].toString();
    d->mArtist = dataFilter[DataTypes::ArtistRole].toString();

    initializeModel(manager, database, modelType, filter);
}

void DataModel::setBusy(bool value)
{
    if (d->mIsBusy == value) {
        return;
    }

    d->mIsBusy = value;
    Q_EMIT isBusyChanged();
}

void DataModel::initializeModel(MusicListenersManager *manager, DatabaseInterface *database,
                                ElisaUtils::PlayListEntryType modelType, DataModel::FilterType type)
{
    d->mModelType = modelType;
    d->mFilterType = type;

    if (manager) {
        manager->connectModel(d->mDataLoader);
    }

    if (manager) {
        connectModel(manager->viewDatabase());
    } else if (database) {
        connectModel(database);
    } else {
        return;
    }

    switch(d->mFilterType)
    {
    case ElisaUtils::NoFilter:
        connect(this, &DataModel::needData,
                d->mDataLoader, &ModelDataLoader::loadData);
        break;
    case ElisaUtils::FilterById:
        connect(this, &DataModel::needDataById,
                d->mDataLoader, &ModelDataLoader::loadDataByAlbumId);
        break;
    case ElisaUtils::FilterByGenre:
        connect(this, &DataModel::needDataByGenre,
                d->mDataLoader, &ModelDataLoader::loadDataByGenre);
        break;
    case ElisaUtils::FilterByArtist:
        connect(this, &DataModel::needDataByArtist,
                d->mDataLoader, &ModelDataLoader::loadDataByArtist);
        break;
    case ElisaUtils::FilterByGenreAndArtist:
        connect(this, &DataModel::needDataByGenreAndArtist,
                d->mDataLoader, &ModelDataLoader::loadDataByGenreAndArtist);
        break;
    case ElisaUtils::FilterByRecentlyPlayed:
        connect(this, &DataModel::needRecentlyPlayedData,
                d->mDataLoader, &ModelDataLoader::loadRecentlyPlayedData);
        break;
    case ElisaUtils::FilterByFrequentlyPlayed:
        connect(this, &DataModel::needFrequentlyPlayedData,
                d->mDataLoader, &ModelDataLoader::loadFrequentlyPlayedData);
        break;
    case ElisaUtils::FilterByPath:
    case ElisaUtils::UnknownFilter:
        break;
    }

    setBusy(true);

    askModelData();
}

void DataModel::askModelData()
{
    switch(d->mFilterType)
    {
    case ElisaUtils::NoFilter:
        Q_EMIT needData(d->mModelType);
        break;
    case ElisaUtils::FilterById:
        Q_EMIT needDataById(d->mModelType, d->mDatabaseId);
        break;
    case ElisaUtils::FilterByGenre:
        Q_EMIT needDataByGenre(d->mModelType, d->mGenre);
        break;
    case ElisaUtils::FilterByArtist:
        Q_EMIT needDataByArtist(d->mModelType, d->mArtist);
        break;
    case ElisaUtils::FilterByGenreAndArtist:
        Q_EMIT needDataByGenreAndArtist(d->mModelType, d->mGenre, d->mArtist);
        break;
    case ElisaUtils::FilterByRecentlyPlayed:
        Q_EMIT needRecentlyPlayedData(d->mModelType);
        break;
    case ElisaUtils::FilterByFrequentlyPlayed:
        Q_EMIT needFrequentlyPlayedData(d->mModelType);
        break;
    case ElisaUtils::FilterByPath:
    case ElisaUtils::UnknownFilter:
        break;
    }
}

int DataModel::indexFromId(qulonglong id) const
{
    int result;
    DataModel::ListTrackDataType mAllData = d->mModelType == ElisaUtils::Radio ? d->mAllRadiosData: d->mAllTrackData;

    for (result = 0; result < mAllData.size(); ++result) {
        if (mAllData[result].databaseId() == id) {
            return result;
        }
    }

    result = -1;

    return result;
}

void DataModel::connectModel(DatabaseInterface *database)
{
    d->mDataLoader->setDatabase(database);

    connect(d->mDataLoader, &ModelDataLoader::allTracksData,
            this, &DataModel::tracksAdded);
    connect(d->mDataLoader, &ModelDataLoader::allRadiosData,
            this, &DataModel::radiosAdded);
    connect(d->mDataLoader, &ModelDataLoader::allAlbumsData,
            this, &DataModel::albumsAdded);
    connect(d->mDataLoader, &ModelDataLoader::allArtistsData,
            this, &DataModel::artistsAdded);
    connect(d->mDataLoader, &ModelDataLoader::allGenresData,
            this, &DataModel::genresAdded);
    connect(d->mDataLoader, &ModelDataLoader::genresAdded,
            this, &DataModel::genresAdded);
    connect(d->mDataLoader, &ModelDataLoader::genreRemoved, this, &DataModel::genreRemoved);
    connect(d->mDataLoader, &ModelDataLoader::albumsAdded,
            this, &DataModel::albumsAdded);
    connect(d->mDataLoader, &ModelDataLoader::albumModified,
            this, &DataModel::albumModified);
    connect(d->mDataLoader, &ModelDataLoader::albumRemoved,
            this, &DataModel::albumRemoved);
    connect(d->mDataLoader, &ModelDataLoader::tracksAdded,
            this, &DataModel::tracksAdded);
    connect(d->mDataLoader, &ModelDataLoader::trackModified,
            this, &DataModel::trackModified);
    connect(d->mDataLoader, &ModelDataLoader::trackRemoved,
            this, &DataModel::trackRemoved);
    connect(d->mDataLoader, &ModelDataLoader::artistsAdded,
            this, &DataModel::artistsAdded);
    connect(d->mDataLoader, &ModelDataLoader::artistRemoved,
            this, &DataModel::artistRemoved);
    connect(d->mDataLoader, &ModelDataLoader::radioAdded,
            this, &DataModel::radioAdded);
    connect(d->mDataLoader, &ModelDataLoader::radioModified,
            this, &DataModel::radioModified);
    connect(d->mDataLoader, &ModelDataLoader::radioRemoved,
            this, &DataModel::radioRemoved);
    connect(d->mDataLoader, &ModelDataLoader::clearedDatabase,
            this, &DataModel::cleanedDatabase);
}

void DataModel::tracksAdded(ListTrackDataType newData)
{
    if (newData.isEmpty() && d->mModelType == ElisaUtils::Track) {
        setBusy(false);
    }

    if (newData.isEmpty() || d->mModelType != ElisaUtils::Track) {
        return;
    }

    if (d->mFilterType == ElisaUtils::FilterById && !d->mAllTrackData.isEmpty()) {
        for (const auto &newTrack : newData) {
            auto trackIndex = indexFromId(newTrack.databaseId());

            if (trackIndex != -1) {
                continue;
            }

            bool trackInserted = false;
            for (int trackIndex = 0; trackIndex < d->mAllTrackData.count(); ++trackIndex) {
                const auto &oneTrack = d->mAllTrackData[trackIndex];

                if (oneTrack.discNumber() >= newTrack.discNumber() && oneTrack.trackNumber() > newTrack.trackNumber()) {
                    beginInsertRows({}, trackIndex, trackIndex);
                    d->mAllTrackData.insert(trackIndex, newTrack);
                    endInsertRows();

                    if (d->mAllTrackData.size() == 1) {
                        setBusy(false);
                    }

                    trackInserted = true;
                    break;
                }
            }

            if (!trackInserted) {
                beginInsertRows({}, d->mAllTrackData.count(), d->mAllTrackData.count());
                d->mAllTrackData.insert(d->mAllTrackData.count(), newTrack);
                endInsertRows();

                if (d->mAllTrackData.size() == 1) {
                    setBusy(false);
                }
            }
        }
    } else {
        if (d->mAllTrackData.isEmpty()) {
            beginInsertRows({}, 0, newData.size() - 1);
            d->mAllTrackData.swap(newData);
            endInsertRows();

            setBusy(false);
        } else {
            beginInsertRows({}, d->mAllTrackData.size(), d->mAllTrackData.size() + newData.size() - 1);
            d->mAllTrackData.append(newData);
            endInsertRows();
        }
    }
}

void DataModel::radiosAdded(ListRadioDataType newData)
{
    if (newData.isEmpty() && d->mModelType == ElisaUtils::Radio) {
        setBusy(false);
    }

    if (newData.isEmpty() || d->mModelType != ElisaUtils::Radio) {
        return;
    }

    if (d->mFilterType == ElisaUtils::FilterById && !d->mAllRadiosData.isEmpty()) {
        for (const auto &newTrack : newData) {
            auto trackIndex = indexFromId(newTrack.databaseId());

            if (trackIndex != -1) {
                continue;
            }

            bool trackInserted = false;
            for (int trackIndex = 0; trackIndex < d->mAllRadiosData.count(); ++trackIndex) {
                const auto &oneTrack = d->mAllRadiosData[trackIndex];

                if (oneTrack.trackNumber() > newTrack.trackNumber()) {
                    beginInsertRows({}, trackIndex, trackIndex);
                    d->mAllRadiosData.insert(trackIndex, newTrack);
                    endInsertRows();

                    if (d->mAllRadiosData.size() == 1) {
                        setBusy(false);
                    }

                    trackInserted = true;
                    break;
                }
            }

            if (!trackInserted) {
                beginInsertRows({}, d->mAllRadiosData.count(), d->mAllRadiosData.count());
                d->mAllRadiosData.insert(d->mAllRadiosData.count(), newTrack);
                endInsertRows();

                if (d->mAllRadiosData.size() == 1) {
                    setBusy(false);
                }
            }
        }
    } else {
        if (d->mAllRadiosData.isEmpty()) {
            beginInsertRows({}, 0, newData.size() - 1);
            d->mAllRadiosData.swap(newData);
            endInsertRows();

            setBusy(false);
        } else {
            beginInsertRows({}, d->mAllRadiosData.size(), d->mAllRadiosData.size() + newData.size() - 1);
            d->mAllRadiosData.append(newData);
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

        auto trackIndex = indexFromId(modifiedTrack.databaseId());

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

void DataModel::radioModified(const TrackDataType &modifiedRadio)
{
    if (d->mModelType != ElisaUtils::Radio) {
        return;
    }

    auto trackIndex = indexFromId(modifiedRadio.databaseId());

    if (trackIndex == -1) {
        return;
    }

    d->mAllRadiosData[trackIndex] = modifiedRadio;
    Q_EMIT dataChanged(index(trackIndex, 0), index(trackIndex, 0));
}

void DataModel::trackRemoved(qulonglong removedTrackId)
{
    if (d->mModelType != ElisaUtils::Track) {
        return;
    }

    if (!d->mAlbumTitle.isEmpty() && !d->mAlbumArtist.isEmpty()) {
        auto trackIndex = indexFromId(removedTrackId);

        if (trackIndex == -1) {
            return;
        }

        beginRemoveRows({}, trackIndex, trackIndex);
        d->mAllTrackData.removeAt(trackIndex);
        endRemoveRows();
    } else {
        const auto itTrack = std::find_if(d->mAllTrackData.cbegin(), d->mAllTrackData.cend(),
                                         [removedTrackId](auto track) {return track.databaseId() == removedTrackId;});

        if (itTrack == d->mAllTrackData.cend()) {
            return;
        }

        auto position = itTrack - d->mAllTrackData.cbegin();

        beginRemoveRows({}, position, position);
        d->mAllTrackData.erase(itTrack);
        endRemoveRows();
    }
}

void DataModel::radioRemoved(qulonglong removedRadioId)
{
    if (d->mModelType != ElisaUtils::Radio) {
        return;
    }


    const auto itRadio = std::find_if(d->mAllRadiosData.cbegin(), d->mAllRadiosData.cend(),
                                      [removedRadioId](auto track) {return track.databaseId() == removedRadioId;});

    if (itRadio == d->mAllRadiosData.cend()) {
        return;
    }

    auto position = itRadio - d->mAllRadiosData.cbegin();

    beginRemoveRows({}, position, position);
    d->mAllRadiosData.erase(itRadio);
    endRemoveRows();
}

void DataModel::radioAdded(const DataModel::TrackDataType &radioData)
{
    if (d->mModelType != ElisaUtils::Radio) {
        return;
    }

    ListRadioDataType list;
    list.append(radioData);
    radiosAdded(list);
}

void DataModel::removeRadios()
{
    if (d->mModelType != ElisaUtils::Radio) {
        return;
    }

    beginRemoveRows({}, 0, d->mAllRadiosData.size());
    d->mAllRadiosData.clear();
    endRemoveRows();
}

void DataModel::genresAdded(DataModel::ListGenreDataType newData)
{
    if (newData.isEmpty() && d->mModelType == ElisaUtils::Genre) {
        setBusy(false);
    }

    if (newData.isEmpty() || d->mModelType != ElisaUtils::Genre) {
        return;
    }

    if (d->mAllGenreData.isEmpty()) {
        beginInsertRows({}, d->mAllGenreData.size(), newData.size() - 1);
        d->mAllGenreData.swap(newData);
        endInsertRows();

        setBusy(false);
    } else {
        beginInsertRows({}, d->mAllGenreData.size(), d->mAllGenreData.size() + newData.size() - 1);
        d->mAllGenreData.append(newData);
        endInsertRows();
    }
}

void DataModel::genreRemoved(qulonglong removedDatabaseId)
{
    if (d->mModelType != ElisaUtils::Genre) {
        return;
    }

    const auto removedDataIterator = std::find_if(d->mAllGenreData.cbegin(), d->mAllGenreData.cend(), [removedDatabaseId](const auto &genre) {
        return genre.databaseId() == removedDatabaseId;
    });

    if (removedDataIterator == d->mAllGenreData.cend()) {
        return;
    }

    int dataIndex = removedDataIterator - d->mAllGenreData.cbegin();

    beginRemoveRows({}, dataIndex, dataIndex);

    d->mAllGenreData.erase(removedDataIterator);

    endRemoveRows();
}

void DataModel::artistsAdded(DataModel::ListArtistDataType newData)
{
    if (newData.isEmpty() && d->mModelType == ElisaUtils::Artist) {
        setBusy(false);
    }

    if (newData.isEmpty() || d->mModelType != ElisaUtils::Artist) {
        return;
    }

    if (d->mAllArtistData.isEmpty()) {
        beginInsertRows({}, d->mAllArtistData.size(), newData.size() - 1);
        d->mAllArtistData.swap(newData);
        endInsertRows();

        setBusy(false);
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

    const auto removedDataIterator = std::find_if(d->mAllArtistData.cbegin(), d->mAllArtistData.cend(),
                                                  [removedDatabaseId](auto album) {return album.databaseId() == removedDatabaseId;});

    if (removedDataIterator == d->mAllArtistData.cend()) {
        return;
    }

    int dataIndex = removedDataIterator - d->mAllArtistData.cbegin();

    beginRemoveRows({}, dataIndex, dataIndex);

    d->mAllArtistData.erase(removedDataIterator);

    endRemoveRows();
}

void DataModel::albumsAdded(DataModel::ListAlbumDataType newData)
{
    if (newData.isEmpty() && d->mModelType == ElisaUtils::Album) {
        setBusy(false);
    }

    if (newData.isEmpty() || d->mModelType != ElisaUtils::Album) {
        return;
    }

    if (d->mAllAlbumData.isEmpty()) {
        beginInsertRows({}, d->mAllAlbumData.size(), newData.size() - 1);
        d->mAllAlbumData.swap(newData);
        endInsertRows();

        setBusy(false);
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

    auto removedDataIterator = d->mAllAlbumData.cend();

    removedDataIterator = std::find_if(d->mAllAlbumData.cbegin(), d->mAllAlbumData.cend(),
                                       [removedDatabaseId](auto album) {return album.databaseId() == removedDatabaseId;});

    if (removedDataIterator == d->mAllAlbumData.cend()) {
        return;
    }

    int dataIndex = removedDataIterator - d->mAllAlbumData.cbegin();

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

void DataModel::initialize(MusicListenersManager *manager, DatabaseInterface *database,
                           ElisaUtils::PlayListEntryType modelType, ElisaUtils::FilterType filter,
                           const QString &genre, const QString &artist, qulonglong databaseId,
                           const QUrl &pathFilter)
{
    Q_UNUSED(pathFilter)

    qCDebug(orgKdeElisaModel()) << "DataModel::initialize" << modelType << filter << databaseId << genre << artist;

    d->mDatabaseId = databaseId;
    d->mGenre = genre;
    d->mArtist = artist;

    initializeModel(manager, database, modelType, filter);
}

void DataModel::cleanedDatabase()
{
    beginResetModel();
    d->mAllAlbumData.clear();
    d->mAllGenreData.clear();
    d->mAllTrackData.clear();
    d->mAllArtistData.clear();
    endResetModel();
}

#include "moc_datamodel.cpp"
