/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "trackmetadatamodel.h"

#include "musiclistenersmanager.h"

#include <KI18n/KLocalizedString>

#include <QtConcurrent>

#include <algorithm>

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

    return mTrackKeys.count();
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
        case DataTypes::DurationRole:
        {
            auto trackDuration = mTrackData.duration();
            if (trackDuration.hour() == 0) {
                result = trackDuration.toString(QStringLiteral("mm:ss"));
            } else {
                result = trackDuration.toString();
            }
            break;
        }
        default:
            result = mTrackData[currentKey];
            break;
        }
        break;
    case ItemNameRole:
        result = nameFromRole(currentKey);
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
        case DataTypes::SampleRateRole:
            result = IntegerEntry;
            break;
        case DataTypes::BitRateRole:
            result = IntegerEntry;
            break;
        case DataTypes::ChannelsRole:
            result = IntegerEntry;
            break;
        case DataTypes::FirstPlayDate:
            result = DateEntry;
            break;
        case DataTypes::DurationRole:
            result = DurationEntry;
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
        case DataTypes::PlayFrequency:
        case DataTypes::ElementTypeRole:
        case DataTypes::FullDataRole:
        case DataTypes::IsDirectoryRole:
        case DataTypes::IsPlayListRole:
        case DataTypes::FilePathRole:
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
    if (mFileUrl.isLocalFile()) {
        return mFileUrl.toLocalFile();
    } else {
        return mFileUrl.toString();
    }
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
    if ((mDatabaseId != 0 && trackData.databaseId() != mDatabaseId) ||
            (!mFileUrl.isEmpty() && trackData.resourceURI() != mFileUrl) ||
            (!mFullData.isEmpty() && trackData.databaseId() != mFullData.databaseId())) {
        return;
    }

    const QList<DataTypes::ColumnsRoles> fieldsForTrack({DataTypes::TitleRole, DataTypes::ArtistRole,
                                                         DataTypes::AlbumRole, DataTypes::AlbumArtistRole,
                                                         DataTypes::TrackNumberRole, DataTypes::DiscNumberRole,
                                                         DataTypes::RatingRole, DataTypes::GenreRole,
                                                         DataTypes::LyricistRole, DataTypes::ComposerRole,
                                                         DataTypes::CommentRole, DataTypes::YearRole,
                                                         DataTypes::ChannelsRole, DataTypes::BitRateRole,
                                                         DataTypes::SampleRateRole, DataTypes::LyricsRole,
                                                         DataTypes::LastPlayDate, DataTypes::PlayCounter,
                                                         DataTypes::DurationRole});

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
            mTrackKeys.push_back(role);
            mTrackData[role] = trackData[role];
        }
    }
    filterDataFromTrackData();
    endResetModel();

    if (trackData.hasDatabaseId()) {
        fetchLyrics();
    }

    mDatabaseId = trackData[DataTypes::DatabaseIdRole].toULongLong();
    Q_EMIT databaseIdChanged();

    mCoverImage = trackData[DataTypes::ImageUrlRole].toUrl();
    Q_EMIT coverUrlChanged();

    auto rawFileUrl = trackData[DataTypes::ResourceRole].toUrl();

    mFileUrl = rawFileUrl;
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
    return mFullData;
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
    if (!mFullData.isEmpty()) {
        beginResetModel();
        mFullData.clear();
        mTrackData.clear();
        mCoverImage.clear();
        mFileUrl.clear();
        endResetModel();

        Q_EMIT lyricsChanged();
    }

    mFileUrl = url;
    mDatabaseId = databaseId;

    Q_EMIT needDataByDatabaseIdAndUrl(type, databaseId, url);
}

void TrackMetadataModel::initialize(MusicListenersManager *newManager, DatabaseInterface *trackDatabase)
{
    mManager = newManager;
    Q_EMIT managerChanged();

    if (mManager) {
        mDataLoader.setDatabase(mManager->viewDatabase());
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

ModelDataLoader &TrackMetadataModel::modelDataLoader()
{
    return mDataLoader;
}

const TrackMetadataModel::TrackDataType &TrackMetadataModel::displayedTrackData() const
{
    return mTrackData;
}

DataTypes::ColumnsRoles TrackMetadataModel::trackKey(int index) const
{
    return mTrackKeys[index];
}

void TrackMetadataModel::removeDataByIndex(int index)
{
    auto dataKey = mTrackKeys[index];

    mTrackData[dataKey] = {};
    mFullData[dataKey] = {};
    mTrackKeys.removeAt(index);
}

void TrackMetadataModel::addDataByName(const QString &name)
{
    DataTypes::ColumnsRoles newRole = DataTypes::TitleRole;

    if (name == i18nc("Track title for track metadata view", "Title")) {
        newRole = DataTypes::TitleRole;
    } else if (name == i18nc("Track artist for track metadata view", "Artist")) {
        newRole = DataTypes::ArtistRole;
    } else if (name == i18nc("Album name for track metadata view", "Album")) {
        newRole = DataTypes::AlbumRole;
    } else if (name == i18nc("Album artist for track metadata view", "Album Artist")) {
        newRole = DataTypes::AlbumArtistRole;
    } else if (name == i18nc("Track number for track metadata view", "Track Number")) {
        newRole = DataTypes::TrackNumberRole;
    } else if (name == i18nc("Disc number for track metadata view", "Disc Number")) {
        newRole = DataTypes::DiscNumberRole;
    } else if (name == i18nc("Rating label for information panel", "Rating")) {
        newRole = DataTypes::RatingRole;
    } else if (name == i18nc("Genre label for track metadata view", "Genre")) {
        newRole = DataTypes::GenreRole;
    } else if (name == i18nc("Lyricist label for track metadata view", "Lyricist")) {
        newRole = DataTypes::LyricistRole;
    } else if (name == i18nc("Composer name for track metadata view", "Composer")) {
        newRole = DataTypes::ComposerRole;
    } else if (name == i18nc("Comment label for track metadata view", "Comment")) {
        newRole = DataTypes::CommentRole;
    } else if (name == i18nc("Year label for track metadata view", "Year")) {
        newRole = DataTypes::YearRole;
    } else if (name == i18nc("Channels label for track metadata view", "Channels")) {
        newRole = DataTypes::ChannelsRole;
    } else if (name == i18nc("Bit rate label for track metadata view", "Bit Rate")) {
        newRole = DataTypes::BitRateRole;
    } else if (name == i18nc("Sample Rate label for track metadata view", "Sample Rate")) {
        newRole = DataTypes::SampleRateRole;
    } else if (name == i18nc("Lyrics label for track metadata view", "Lyrics")) {
        newRole = DataTypes::LyricsRole;
    } else if (name == i18nc("Duration label for track metadata view", "Duration")) {
        newRole = DataTypes::DurationRole;
    }

    mTrackData[newRole] = {};
    mFullData[newRole] = {};
    mTrackKeys.push_back(newRole);
}

QString TrackMetadataModel::nameFromRole(DataTypes::ColumnsRoles role)
{
    auto result = QString{};
    switch (role)
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
    case DataTypes::FirstPlayDate:
        result = i18nc("First play date label for track metadata view", "First played");
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
    case DataTypes::PlayFrequency:
    case DataTypes::ElementTypeRole:
    case DataTypes::FullDataRole:
    case DataTypes::IsDirectoryRole:
    case DataTypes::IsPlayListRole:
    case DataTypes::FilePathRole:
        break;
    }
    return result;
}

bool TrackMetadataModel::metadataExists(DataTypes::ColumnsRoles metadataRole) const
{
    return std::find(mTrackKeys.begin(), mTrackKeys.end(), metadataRole) != mTrackKeys.end();
}

void TrackMetadataModel::fetchLyrics()
{
    auto fileUrl = mFullData[DataTypes::ResourceRole].toUrl();
    auto lyricicsValue = QtConcurrent::run(QThreadPool::globalInstance(), [fileUrl, this]() {
        auto locker = QMutexLocker(&mFileScannerMutex);
        auto trackData = mFileScanner.scanOneFile(fileUrl);
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
    mFullData.clear();
    mTrackData.clear();
    mTrackKeys.clear();

    auto allRoles = {DataTypes::TitleRole, DataTypes::ResourceRole,
                     DataTypes::CommentRole, DataTypes::ImageUrlRole};

    for (auto role : allRoles) {
        mTrackKeys.push_back(role);
        if (role == DataTypes::DatabaseIdRole) {
            mFullData[role] = -1;
        } else {
            mFullData[role] = QString();
        }
    }
    mTrackData = mFullData;
    mFullData[DataTypes::ElementTypeRole] = ElisaUtils::Radio;
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

    mFileUrl = url;

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

void TrackMetadataModel::radioData(const TrackDataType &radiosData)
{
    if (!mFullData.isEmpty() && mFullData[DataTypes::DatabaseIdRole].toInt() != -1 &&
            mFullData.databaseId() != radiosData.databaseId()) {
        return;
    }

    const QList<DataTypes::ColumnsRoles> fieldsForTrack({DataTypes::TitleRole, DataTypes::ResourceRole,
                                                                 DataTypes::CommentRole, DataTypes::ImageUrlRole});

    fillDataFromTrackData(radiosData, fieldsForTrack);
}

#include "moc_trackmetadatamodel.cpp"
