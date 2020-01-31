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

#include "trackmetadatamodel.h"

#include "musiclistenersmanager.h"

#include <KI18n/KLocalizedString>

#include <QtConcurrent>

TrackMetadataModel::TrackMetadataModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(&mLyricsValueWatcher, &QFutureWatcher<QString>::finished,
            this, &TrackMetadataModel::lyricsValueIsReady);
}

TrackMetadataModel::~TrackMetadataModel()
{
    if (mLyricsValueWatcher.isRunning() && !mLyricsValueWatcher.isFinished()) {
        mLyricsValueWatcher.waitForFinished();
    }
}

int TrackMetadataModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return mTrackData.count();
}

QVariant TrackMetadataModel::data(const QModelIndex &index, int role) const
{
    auto result = QVariant{};

    const auto currentKey = mTrackKeys[index.row()];

    switch (role)
    {
    case Qt::DisplayRole:
        switch (currentKey)
        {
        case DataTypes::TrackNumberRole:
        {
            auto trackNumber = mTrackData.trackNumber();
            if (trackNumber > 0) {
                result = trackNumber;
            }
            break;
        }
        case DataTypes::DiscNumberRole:
        {
            auto discNumber = mTrackData.discNumber();
            if (discNumber > 0) {
                result = discNumber;
            }
            break;
        }
        case DataTypes::ChannelsRole:
        {
            auto channels = mTrackData.channels();
            if (channels > 0) {
                result = channels;
            }
            break;
        }
        case DataTypes::BitRateRole:
        {
            auto bitRate = mTrackData.bitRate();
            if (bitRate > 0) {
                result = bitRate;
            }
            break;
        }
        case DataTypes::SampleRateRole:
        {
            auto sampleRate = mTrackData.sampleRate();
            if (sampleRate > 0) {
                result = sampleRate;
            }
            break;
        }
        default:
            result = mTrackData[currentKey];
            break;
        }
        break;
    case ItemNameRole:
        switch (currentKey)
        {
        case DataTypes::TitleRole:
            result = i18nc("Track title for track metadata view", "Title");
            break;
        case DataTypes::DurationRole:
            result = i18nc("Duration label for track metadata view", "Duration");
            break;
        case DataTypes::ArtistRole:
            result = i18nc("Track artist for track metadata view", "Artist");
            break;
        case DataTypes::AlbumRole:
            result = i18nc("Album name for track metadata view", "Album");
            break;
        case DataTypes::AlbumArtistRole:
            result = i18nc("Album artist for track metadata view", "Album Artist");
            break;
        case DataTypes::TrackNumberRole:
            result = i18nc("Track number for track metadata view", "Track Number");
            break;
        case DataTypes::DiscNumberRole:
            result = i18nc("Disc number for track metadata view", "Disc Number");
            break;
        case DataTypes::RatingRole:
            result = i18nc("Rating label for information panel", "Rating");
            break;
        case DataTypes::GenreRole:
            result = i18nc("Genre label for track metadata view", "Genre");
            break;
        case DataTypes::LyricistRole:
            result = i18nc("Lyricist label for track metadata view", "Lyricist");
            break;
        case DataTypes::ComposerRole:
            result = i18nc("Composer name for track metadata view", "Composer");
            break;
        case DataTypes::CommentRole:
            result = i18nc("Comment label for track metadata view", "Comment");
            break;
        case DataTypes::YearRole:
            result = i18nc("Year label for track metadata view", "Year");
            break;
        case DataTypes::ChannelsRole:
            result = i18nc("Channels label for track metadata view", "Channels");
            break;
        case DataTypes::BitRateRole:
            result = i18nc("Bit rate label for track metadata view", "Bit Rate");
            break;
        case DataTypes::SampleRateRole:
            result = i18nc("Sample Rate label for track metadata view", "Sample Rate");
            break;
        case DataTypes::LastPlayDate:
            result = i18nc("Last play date label for track metadata view", "Last played");
            break;
        case DataTypes::PlayCounter:
            result = i18nc("Play counter label for track metadata view", "Play count");
            break;
        case DataTypes::LyricsRole:
            result = i18nc("Lyrics label for track metadata view", "Lyrics");
            break;
        case DataTypes::ResourceRole:
            result = i18nc("Radio HTTP address for radio metadata view", "Stream Http Address");
            break;
        case DataTypes::ImageUrlRole:
            result = i18nc("Image address for radio metadata view", "Image Address");
            break;
        case DataTypes::SecondaryTextRole:
        case DataTypes::ShadowForImageRole:
        case DataTypes::ChildModelRole:
        case DataTypes::StringDurationRole:
        case DataTypes::IsValidAlbumArtistRole:
        case DataTypes::AllArtistsRole:
        case DataTypes::HighestTrackRating:
        case DataTypes::IdRole:
        case DataTypes::ParentIdRole:
        case DataTypes::DatabaseIdRole:
        case DataTypes::IsSingleDiscAlbumRole:
        case DataTypes::ContainerDataRole:
        case DataTypes::IsPartialDataRole:
        case DataTypes::AlbumIdRole:
        case DataTypes::HasEmbeddedCover:
        case DataTypes::FileModificationTime:
        case DataTypes::FirstPlayDate:
        case DataTypes::PlayFrequency:
        case DataTypes::ElementTypeRole:
        case DataTypes::FullDataRole:
            break;
        }
        break;
    case ItemTypeRole:
        switch (currentKey)
        {
        case DataTypes::TitleRole:
            result = TextEntry;
            break;
        case DataTypes::ResourceRole:
        case DataTypes::ImageUrlRole:
            result = UrlEntry;
            break;
        case DataTypes::ArtistRole:
            result = TextEntry;
            break;
        case DataTypes::AlbumRole:
            result = TextEntry;
            break;
        case DataTypes::AlbumArtistRole:
            result = TextEntry;
            break;
        case DataTypes::TrackNumberRole:
            result = IntegerEntry;
            break;
        case DataTypes::DiscNumberRole:
            result = IntegerEntry;
            break;
        case DataTypes::RatingRole:
            result = RatingEntry;
            break;
        case DataTypes::GenreRole:
            result = TextEntry;
            break;
        case DataTypes::LyricistRole:
            result = TextEntry;
            break;
        case DataTypes::ComposerRole:
            result = TextEntry;
            break;
        case DataTypes::CommentRole:
            result = TextEntry;
            break;
        case DataTypes::YearRole:
            result = IntegerEntry;
            break;
        case DataTypes::LastPlayDate:
            result = DateEntry;
            break;
        case DataTypes::PlayCounter:
            result = IntegerEntry;
            break;
        case DataTypes::LyricsRole:
            result = LongTextEntry;
            break;
        case DataTypes::DurationRole:
        case DataTypes::SampleRateRole:
        case DataTypes::BitRateRole:
        case DataTypes::ChannelsRole:
        case DataTypes::SecondaryTextRole:
        case DataTypes::ShadowForImageRole:
        case DataTypes::ChildModelRole:
        case DataTypes::StringDurationRole:
        case DataTypes::IsValidAlbumArtistRole:
        case DataTypes::AllArtistsRole:
        case DataTypes::HighestTrackRating:
        case DataTypes::IdRole:
        case DataTypes::ParentIdRole:
        case DataTypes::DatabaseIdRole:
        case DataTypes::IsSingleDiscAlbumRole:
        case DataTypes::ContainerDataRole:
        case DataTypes::IsPartialDataRole:
        case DataTypes::AlbumIdRole:
        case DataTypes::HasEmbeddedCover:
        case DataTypes::FileModificationTime:
        case DataTypes::FirstPlayDate:
        case DataTypes::PlayFrequency:
        case DataTypes::ElementTypeRole:
        case DataTypes::FullDataRole:
            break;
        }
        break;
    }

    return result;
}

bool TrackMetadataModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        auto dataType = mTrackKeys[index.row()];

        mTrackData[dataType] = value;
        mFullData[dataType] = value;

        Q_EMIT dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

QHash<int, QByteArray> TrackMetadataModel::roleNames() const
{
    auto names = QAbstractListModel::roleNames();

    names[ItemNameRole] = "name";
    names[ItemTypeRole] = "type";

    return names;
}

QString TrackMetadataModel::fileUrl() const
{
    return mFileUrl;
}

QUrl TrackMetadataModel::coverUrl() const
{
    if (mCoverImage.isEmpty()) {
        return QUrl(QStringLiteral("image://icon/media-optical-audio"));
    } else {
        return mCoverImage;
    }
}

MusicListenersManager *TrackMetadataModel::manager() const
{
    return mManager;
}

QString TrackMetadataModel::lyrics() const
{
    return mFullData[TrackDataType::key_type::LyricsRole].toString();
}

qulonglong TrackMetadataModel::databaseId() const
{
    return mDatabaseId;
}

void TrackMetadataModel::trackData(const TrackMetadataModel::TrackDataType &trackData)
{
    if (!mFullData.isEmpty() && trackData.databaseId() != mFullData.databaseId()) {
        return;
    }

    const QList<DataTypes::ColumnsRoles> fieldsForTrack({DataTypes::TitleRole, DataTypes::ArtistRole,
                                                                 DataTypes::AlbumRole, DataTypes::AlbumArtistRole,
                                                                 DataTypes::TrackNumberRole, DataTypes::DiscNumberRole,
                                                                 DataTypes::RatingRole, DataTypes::GenreRole,
                                                                 DataTypes::LyricistRole, DataTypes::ComposerRole,
                                                                 DataTypes::CommentRole, DataTypes::YearRole,
                                                                 DataTypes::LastPlayDate, DataTypes::PlayCounter});

    fillDataFromTrackData(trackData, fieldsForTrack);
}

void TrackMetadataModel::fillDataFromTrackData(const TrackMetadataModel::TrackDataType &trackData,
                                               const QList<DataTypes::ColumnsRoles> &fieldsForTrack)
{
    beginResetModel();
    mFullData = trackData;
    mTrackData.clear();
    mTrackKeys.clear();

    for (DataTypes::ColumnsRoles role : fieldsForTrack) {
        if (trackData.constFind(role) != trackData.constEnd()) {
            if (role == DataTypes::RatingRole) {
                if (trackData[role].toInt() == 0) {
                    continue;
                }
            }

            mTrackKeys.push_back(role);
            mTrackData[role] = trackData[role];
        }
    }
    filterDataFromTrackData();
    endResetModel();

    fetchLyrics();

    mDatabaseId = trackData[DataTypes::DatabaseIdRole].toULongLong();
    Q_EMIT databaseIdChanged();

    mCoverImage = trackData[DataTypes::ImageUrlRole].toUrl();
    Q_EMIT coverUrlChanged();

    auto rawFileUrl = trackData[DataTypes::ResourceRole].toUrl();

    if (rawFileUrl.isLocalFile()) {
        mFileUrl = rawFileUrl.toLocalFile();
    } else {
        mFileUrl = rawFileUrl.toString();
    }
    Q_EMIT fileUrlChanged();
}

void TrackMetadataModel::filterDataFromTrackData()
{
}

void TrackMetadataModel::removeMetaData(DataTypes::ColumnsRoles metaData)
{
    auto itMetaData = std::find(mTrackKeys.begin(), mTrackKeys.end(), metaData);
    if (itMetaData == mTrackKeys.end()) {
        return;
    }

    mTrackKeys.erase(itMetaData);
    mTrackData.remove(metaData);
}

TrackMetadataModel::TrackDataType::mapped_type TrackMetadataModel::dataFromType(TrackDataType::key_type metaData) const
{
    return mFullData[metaData];
}

void TrackMetadataModel::fillLyricsDataFromTrack()
{
    beginInsertRows({}, mTrackData.size(), mTrackData.size());
    mTrackKeys.push_back(DataTypes::LyricsRole);
    mTrackData[DataTypes::LyricsRole] = mLyricsValueWatcher.result();
    endInsertRows();
}

const TrackMetadataModel::TrackDataType &TrackMetadataModel::allTrackData() const
{
    return mTrackData;
}

void TrackMetadataModel::lyricsValueIsReady()
{
    if (!mLyricsValueWatcher.result().isEmpty()) {
        fillLyricsDataFromTrack();

        mFullData[DataTypes::LyricsRole] = mLyricsValueWatcher.result();

        Q_EMIT lyricsChanged();
    }
}

void TrackMetadataModel::initializeByIdAndUrl(ElisaUtils::PlayListEntryType type, qulonglong databaseId, const QUrl &url)
{
    mFullData.clear();
    mTrackData.clear();
    mCoverImage.clear();
    mFileUrl.clear();

    Q_EMIT lyricsChanged();

    Q_EMIT needDataByDatabaseIdAndUrl(type, databaseId, url);
}

void TrackMetadataModel::initialize(MusicListenersManager *newManager, DatabaseInterface *trackDatabase)
{
    mManager = newManager;
    Q_EMIT managerChanged();

    if (mManager) {
        mDataLoader.setDatabase(mManager->viewDatabase());
        connect(this, &TrackMetadataModel::needDataByUrl,
                mManager->tracksListener(), &TracksListener::trackByFileNameInList);
        connect(mManager->tracksListener(), &TracksListener::trackHasChanged,
                this, &TrackMetadataModel::trackData);
    } else if (trackDatabase) {
        mDataLoader.setDatabase(trackDatabase);
    }

    if (mManager) {
        mManager->connectModel(&mDataLoader);
    }

    connect(this, &TrackMetadataModel::needDataByDatabaseIdAndUrl,
            &mDataLoader, &ModelDataLoader::loadDataByDatabaseIdAndUrl);
    connect(this, &TrackMetadataModel::needDataByUrl,
            &mDataLoader, &ModelDataLoader::loadDataByUrl);
    connect(this, &TrackMetadataModel::saveRadioData,
            &mDataLoader, &ModelDataLoader::saveRadioModified);
    connect(this, &TrackMetadataModel::deleteRadioData,
            &mDataLoader, &ModelDataLoader::removeRadio);
    connect(&mDataLoader, &ModelDataLoader::trackModified,
            this, &TrackMetadataModel::trackData);
    connect(&mDataLoader, &ModelDataLoader::allTrackData,
            this, &TrackMetadataModel::trackData);
    connect(&mDataLoader, &ModelDataLoader::allRadioData,
            this, &TrackMetadataModel::radioData);
    connect(&mDataLoader, &ModelDataLoader::radioAdded,
            this, &TrackMetadataModel::radioData);
    connect(&mDataLoader, &ModelDataLoader::radioModified,
            this, &TrackMetadataModel::radioData);
}

void TrackMetadataModel::fetchLyrics()
{
    auto lyricicsValue = QtConcurrent::run(QThreadPool::globalInstance(), [=]() {
        auto trackData = mFileScanner.scanOneFile(mFullData[DataTypes::ResourceRole].toUrl());
        if (!trackData.lyrics().isEmpty()) {
            return trackData.lyrics();
        }
        return QString{};
    });

    mLyricsValueWatcher.setFuture(lyricicsValue);
}

void TrackMetadataModel::initializeForNewRadio()
{
    mFullData.clear();
    mTrackData.clear();

    fillDataForNewRadio();
}

void TrackMetadataModel::fillDataForNewRadio()
{
    beginResetModel();
    mTrackData.clear();
    mTrackKeys.clear();

    auto allRoles = {DataTypes::TitleRole, DataTypes::ResourceRole,
                     DataTypes::CommentRole, DataTypes::ImageUrlRole,
                     DataTypes::DatabaseIdRole};

    for (auto role : allRoles) {
        mTrackKeys.push_back(role);
        if (role == DataTypes::DatabaseIdRole) {
            mTrackData[role] = -1;
        } else {
            mTrackData[role] = QString();
        }

    }
    filterDataFromTrackData();
    endResetModel();
}

void TrackMetadataModel::initializeByUrl(ElisaUtils::PlayListEntryType type, const QUrl &url)
{
    mFullData.clear();
    mTrackData.clear();
    mCoverImage.clear();
    mFileUrl.clear();

    Q_EMIT lyricsChanged();

    Q_EMIT needDataByUrl(type, url);
}

void TrackMetadataModel::setManager(MusicListenersManager *newManager)
{
    initialize(newManager, nullptr);
}

void TrackMetadataModel::setDatabase(DatabaseInterface *trackDatabase)
{
    initialize(nullptr, trackDatabase);
}

void TrackMetadataModel::deleteRadio()
{
    if (mTrackData[DataTypes::DatabaseIdRole]>=0) {
        Q_EMIT deleteRadioData(mTrackData[DataTypes::DatabaseIdRole].toULongLong());
    }
}

void TrackMetadataModel::radioData(const TrackDataType &radiosData)
{
    if (!mFullData.isEmpty() && mFullData[DataTypes::DatabaseIdRole].toInt() != -1 &&
            mFullData.databaseId() != radiosData.databaseId()) {
        return;
    }

    const QList<DataTypes::ColumnsRoles> fieldsForTrack({DataTypes::TitleRole, DataTypes::ResourceRole,
                                                                 DataTypes::CommentRole, DataTypes::ImageUrlRole,
                                                                 DataTypes::DatabaseIdRole});

    fillDataFromTrackData(radiosData, fieldsForTrack);
}

#include "moc_trackmetadatamodel.cpp"
