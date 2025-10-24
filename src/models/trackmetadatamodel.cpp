/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2024 (c) Gary Wang <opensource@blumia.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "trackmetadatamodel.h"

#include "musiclistenersmanager.h"

#include "lyricsLogging.h"

#include <KLocalizedString>

#include <QtConcurrentRun>

#include <algorithm>

#include <KEncodingProber>
#include <KFormat>
#include <QDir>
#include <QFileInfo>
#include <QStringDecoder>

using namespace Qt::Literals::StringLiterals;

QList<DataTypes::ColumnsRoles> displayFields(const ElisaUtils::PlayListEntryType dataType)
{
    switch (dataType) {
    case ElisaUtils::Track:
        return {
            DataTypes::TitleRole,      DataTypes::ArtistRole,  DataTypes::AlbumRole,    DataTypes::AlbumArtistRole, DataTypes::TrackNumberRole,
            DataTypes::DiscNumberRole, DataTypes::RatingRole,  DataTypes::GenreRole,    DataTypes::LyricistRole,    DataTypes::ComposerRole,
            DataTypes::CommentRole,    DataTypes::YearRole,    DataTypes::ChannelsRole, DataTypes::BitRateRole,     DataTypes::SampleRateRole,
            DataTypes::LastPlayDate,   DataTypes::PlayCounter, DataTypes::DurationRole, DataTypes::LyricsRole,
        };
    case ElisaUtils::Radio:
        return {DataTypes::TitleRole, DataTypes::ResourceRole, DataTypes::CommentRole, DataTypes::ImageUrlRole};
    default:
        return {};
    }
}

TrackMetadataModel::TrackMetadataModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(&mLyricsValueWatcher, &QFutureWatcher<std::pair<QString, QString>>::finished, this, &TrackMetadataModel::lyricsValueIsReady);
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

    return mDisplayKeys.count();
}

QVariant TrackMetadataModel::data(const QModelIndex &index, int role) const
{
    auto result = QVariant{};

    const auto currentKey = mDisplayKeys[index.row()];

    switch (role)
    {
    case Qt::DisplayRole:
        switch (currentKey)
        {
        case DataTypes::TrackNumberRole:
        {
            auto trackNumber = mDisplayData.trackNumber();
            if (trackNumber > 0) {
                result = trackNumber;
            }
            break;
        }
        case DataTypes::DiscNumberRole:
        {
            auto discNumber = mDisplayData.discNumber();
            if (discNumber > 0) {
                result = discNumber;
            }
            break;
        }
        case DataTypes::ChannelsRole:
        {
            auto channels = mDisplayData.channels();
            if (channels > 0) {
                result = channels;
            }
            break;
        }
        case DataTypes::BitRateRole:
        {
            auto bitRate = mDisplayData.bitRate();
            if (bitRate > 0) {
                result = KFormat().formatValue(bitRate, QStringLiteral("bit/s"), 0,
                                               KFormat::UnitPrefix::Kilo, KFormat::MetricBinaryDialect);
            }
            break;
        }
        case DataTypes::SampleRateRole:
        {
            auto sampleRate = mDisplayData.sampleRate();
            if (sampleRate > 0) {
                result = KFormat().formatValue(sampleRate, KFormat::Unit::Hertz, 1,
                                               KFormat::UnitPrefix::Kilo, KFormat::MetricBinaryDialect);
            }
            break;
        }
        case DataTypes::DurationRole:
        {
            auto trackDuration = mDisplayData.duration();
            if (trackDuration.hour() == 0) {
                result = trackDuration.toString(QStringLiteral("mm:ss"));
            } else {
                result = trackDuration.toString();
            }
            break;
        }
        default:
            result = mDisplayData[currentKey];
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
        case DataTypes::ElementTypeRole:
        case DataTypes::FullDataRole:
        case DataTypes::IsDirectoryRole:
        case DataTypes::IsPlayListRole:
        case DataTypes::FilePathRole:
        case DataTypes::HasChildrenRole:
        case DataTypes::MultipleImageUrlsRole:
        case DataTypes::LyricsLocationRole:
        case DataTypes::TracksCountRole:
            break;
        }
        break;
    case HasDataRole:
        result = mDisplayData.contains(currentKey);
        break;
    }

    return result;
}

bool TrackMetadataModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        auto dataType = mDisplayKeys[index.row()];

        mDisplayData[dataType] = value;

        Q_EMIT dataChanged(index, index, QList<int>() << role);
        return true;
    }
    return false;
}

void TrackMetadataModel::saveChanges()
{
    for (const auto role : std::as_const(mDisplayKeys)) {
        if (mDisplayData.contains(role)) {
            mFullData[role] = mDisplayData[role];
        } else {
            mFullData.remove(role);
        }
    }
}

void TrackMetadataModel::undoChanges()
{
    resetDisplayData();
}

QHash<int, QByteArray> TrackMetadataModel::roleNames() const
{
    auto names = QAbstractListModel::roleNames();

    names[ItemNameRole] = "name";
    names[ItemTypeRole] = "type";
    names[HasDataRole] = "hasData";

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
        return QUrl(QStringLiteral("image://icon/media-default-album"));
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

QString TrackMetadataModel::lyricsLocation() const
{
    return mFullData[TrackDataType::key_type::LyricsLocationRole].toString();
}

qulonglong TrackMetadataModel::databaseId() const
{
    return mDatabaseId;
}

void TrackMetadataModel::trackData(const TrackMetadataModel::TrackDataType &trackData)
{
    if (!mFileUrl.isEmpty() && trackData.resourceURI() != mFileUrl) {
        return;
    }

    fillDataFromTrackData(trackData);
}

void TrackMetadataModel::fillDataFromTrackData(const TrackMetadataModel::TrackDataType &trackData)
{
    mFullData = trackData;
    if (!mFullData.hasElementType()) {
        mFullData[DataTypes::ElementTypeRole] = ElisaUtils::Track;
    }
    resetDisplayData();

    if (trackData.hasDatabaseId()) {
        fetchLyrics();
    } else {
        Q_EMIT lyricsChanged();
    }

    mDatabaseId = trackData[DataTypes::DatabaseIdRole].toULongLong();
    Q_EMIT databaseIdChanged();

    mCoverImage = trackData[DataTypes::ImageUrlRole].toUrl();
    Q_EMIT coverUrlChanged();

    auto rawFileUrl = trackData[DataTypes::ResourceRole].toUrl();

    mFileUrl = rawFileUrl;
    Q_EMIT fileUrlChanged();
}

void TrackMetadataModel::resetDisplayData()
{
    beginResetModel();
    mDisplayData.clear();
    mDisplayKeys = displayFields(mFullData.elementType());

    for (DataTypes::ColumnsRoles role : std::as_const(mDisplayKeys)) {
        if (mFullData.constFind(role) != mFullData.constEnd()) {
            mDisplayData[role] = mFullData[role];
        }
    }
    filterDataFromTrackData();
    endResetModel();
}

void TrackMetadataModel::filterDataFromTrackData()
{
}

void TrackMetadataModel::removeMetaData(DataTypes::ColumnsRoles metaData)
{
    auto itMetaData = std::find(mDisplayKeys.cbegin(), mDisplayKeys.cend(), metaData);
    if (itMetaData == mDisplayKeys.cend()) {
        return;
    }

    mDisplayKeys.erase(itMetaData);
    mDisplayData.remove(metaData);
}

TrackMetadataModel::TrackDataType::mapped_type TrackMetadataModel::dataFromType(TrackDataType::key_type metaData) const
{
    return mDisplayData[metaData];
}

void TrackMetadataModel::fillLyricsDataFromTrack()
{
    const auto result = mLyricsValueWatcher.result();
    mDisplayData[DataTypes::LyricsRole] = result.first;
    mDisplayData[DataTypes::LyricsLocationRole] = result.second;
    // This is safe as long as we keep the lyrics as the final tag
    const auto lyricsIndex = index(mDisplayKeys.size() - 1);
    Q_EMIT dataChanged(lyricsIndex, lyricsIndex);
}

const TrackMetadataModel::TrackDataType &TrackMetadataModel::allTrackData() const
{
    return mFullData;
}

void TrackMetadataModel::lyricsValueIsReady()
{
    auto result = mLyricsValueWatcher.result();
    if (!result.first.isEmpty()) {
        fillLyricsDataFromTrack();

        mFullData[DataTypes::LyricsRole] = result.first;
        mFullData[DataTypes::LyricsLocationRole] = result.second;

        Q_EMIT lyricsChanged();
    }
}

void TrackMetadataModel::initializeByIdAndUrl(ElisaUtils::PlayListEntryType type, qulonglong databaseId, const QUrl &url)
{
    if (!mFullData.isEmpty()) {
        beginResetModel();
        mFullData.clear();
        mDisplayData.clear();
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
    return mDisplayData;
}

DataTypes::ColumnsRoles TrackMetadataModel::trackKey(int index) const
{
    return mDisplayKeys[index];
}

void TrackMetadataModel::removeDataByIndex(int index)
{
    const auto modelIndex = this->index(index);
    if (!modelIndex.isValid()) {
        return;
    }

    const auto dataKey = mDisplayKeys[index];
    if (mDisplayData.contains(dataKey)) {
        mDisplayData.remove(dataKey);
        Q_EMIT dataChanged(modelIndex, modelIndex);
    }
}

void TrackMetadataModel::addDataByIndex(int index)
{
    const auto modelIndex = this->index(index);
    if (!modelIndex.isValid()) {
        return;
    }

    const auto newRole = mDisplayKeys[index];
    if (!mDisplayData.contains(newRole)) {
        mDisplayData.insert(newRole, QString());
        Q_EMIT dataChanged(modelIndex, modelIndex);
    }
}

QString TrackMetadataModel::nameFromRole(DataTypes::ColumnsRoles role)
{
    auto result = QString{};
    switch (role)
    {
    case DataTypes::TitleRole:
        result = i18nc("@label:textbox Track title for track metadata view", "Title");
        break;
    case DataTypes::DurationRole:
        result = i18nc("@label:textbox Duration label for track metadata view", "Duration");
        break;
    case DataTypes::ArtistRole:
        result = i18nc("@label:textbox Track artist for track metadata view", "Artist");
        break;
    case DataTypes::AlbumRole:
        result = i18nc("@label:textbox Album name for track metadata view", "Album");
        break;
    case DataTypes::AlbumArtistRole:
        result = i18nc("@label:textbox Album artist for track metadata view", "Album Artist");
        break;
    case DataTypes::TrackNumberRole:
        result = i18nc("@label:textbox Track number for track metadata view", "Track Number");
        break;
    case DataTypes::DiscNumberRole:
        result = i18nc("@label:textbox Disc number for track metadata view", "Disc Number");
        break;
    case DataTypes::RatingRole:
        result = i18nc("@label:textbox Rating label for information panel", "Rating");
        break;
    case DataTypes::GenreRole:
        result = i18nc("@label:textbox Genre label for track metadata view", "Genre");
        break;
    case DataTypes::LyricistRole:
        result = i18nc("@label:textbox Lyricist label for track metadata view", "Lyricist");
        break;
    case DataTypes::ComposerRole:
        result = i18nc("@label:textbox Composer name for track metadata view", "Composer");
        break;
    case DataTypes::CommentRole:
        result = i18nc("@label:textbox Comment label for track metadata view", "Comment");
        break;
    case DataTypes::YearRole:
        result = i18nc("@label:textbox Year label for track metadata view", "Year");
        break;
    case DataTypes::ChannelsRole:
        result = i18nc("@label:textbox Channels label for track metadata view", "Channels");
        break;
    case DataTypes::BitRateRole:
        result = i18nc("@label:textbox Bit rate label for track metadata view", "Bit Rate");
        break;
    case DataTypes::SampleRateRole:
        result = i18nc("@label:textbox Sample Rate label for track metadata view", "Sample Rate");
        break;
    case DataTypes::LastPlayDate:
        result = i18nc("@label:textbox Last play date label for track metadata view", "Last played");
        break;
    case DataTypes::FirstPlayDate:
        result = i18nc("@label:textbox First play date label for track metadata view", "First played");
        break;
    case DataTypes::PlayCounter:
        result = i18nc("@label:textbox Play counter label for track metadata view", "Play count");
        break;
    case DataTypes::LyricsRole:
        result = i18nc("@label:textbox Lyrics label for track metadata view", "Lyrics");
        break;
    case DataTypes::ResourceRole:
        result = i18nc("@label:textbox Radio HTTP address for radio metadata view", "Stream HTTP Address");
        break;
    case DataTypes::ImageUrlRole:
        result = i18nc("@label:textbox Image address for radio metadata view", "Image Address");
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
    case DataTypes::ElementTypeRole:
    case DataTypes::FullDataRole:
    case DataTypes::IsDirectoryRole:
    case DataTypes::IsPlayListRole:
    case DataTypes::FilePathRole:
    case DataTypes::HasChildrenRole:
    case DataTypes::MultipleImageUrlsRole:
    case DataTypes::LyricsLocationRole:
    case DataTypes::TracksCountRole:
        break;
    }
    return result;
}

bool TrackMetadataModel::metadataExists(DataTypes::ColumnsRoles metadataRole) const
{
    return std::find(mDisplayKeys.begin(), mDisplayKeys.end(), metadataRole) != mDisplayKeys.end();
}

void TrackMetadataModel::fetchLyrics()
{
    auto fileUrl = mFullData[DataTypes::ResourceRole].toUrl();
    auto lyricsValue = QtConcurrent::run(QThreadPool::globalInstance(), [fileUrl, this]() {
        auto locker = QMutexLocker(&mFileScannerMutex);
        if (fileUrl.isLocalFile()) {
            QFileInfo fileInfo(fileUrl.toLocalFile());
            QDir lyricsDir(fileInfo.dir());
            const QStringList lyricsFileExtensions = {u".lrc"_s, u".LRC"_s, u".txt"_s, u".TXT"_s};
            QString lyricsFileName;

            QByteArray fileContent;
            for (const QString &ext : lyricsFileExtensions) {
                lyricsFileName = fileInfo.completeBaseName() + ext;
                if (lyricsDir.exists(lyricsFileName)) {
                    QFile file(lyricsDir.filePath(lyricsFileName));
                    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        fileContent = file.readAll();
                        break;
                    }
                }
            }
            if (!fileContent.isEmpty()) {
                KEncodingProber prober(KEncodingProber::Universal);
                prober.feed(fileContent);
                const QByteArray encoding(prober.encoding());
                QString decodedContent;
                
                QStringDecoder toUtf16;
                if (prober.confidence() > 0.5 && prober.state() != KEncodingProber::Probing) {

#if QT_VERSION < QT_VERSION_CHECK(6, 8, 0)
                    toUtf16 = QStringDecoder(encoding.constData());
#else
                    toUtf16 = QStringDecoder(encoding);
#endif // QT_VERSION < QT_VERSION_CHECK(6, 8, 0)

                } else {
                    toUtf16 = QStringDecoder(QStringDecoder::Utf8);
                }
                // Don't use `QStringConverter::availableCodecs().contains(QString(encoding))` here, since the charset
                // encoding name might not match, e.g. GB18030 (from availableCodecs) != gb18030 (from KEncodingProber)
                if (toUtf16.isValid()) {
                    decodedContent = toUtf16(fileContent);
                } else {
                    // Developers who attempted to build Elisa against Qt without ICU feature enabled might facing this issue.
                    // Qt's official binary release didn't have ICU enabled, while KDE Craft do.
                    qCDebug(orgKdeElisaLyrics) << "No codec for the detected encoding:" << encoding
                                               << ", Available codecs are:" << QStringConverter::availableCodecs();
                    decodedContent = QString::fromLocal8Bit(fileContent);
                }
                return std::make_pair(decodedContent, lyricsFileName);
            }
        }
        auto trackData = mFileScanner.scanOneFile(fileUrl);
        if (!trackData.lyrics().isEmpty()) {
            return std::make_pair(trackData.lyrics(), QString{});
        }
        return std::make_pair(QString{}, QString{});
    });

    mLyricsValueWatcher.setFuture(lyricsValue);
}

void TrackMetadataModel::initializeForNewRadio()
{
    mFullData.clear();
    mDisplayData.clear();

    fillDataForNewRadio();
}

void TrackMetadataModel::fillDataForNewRadio()
{
    beginResetModel();
    mFullData.clear();
    mDisplayData.clear();
    mDisplayKeys.clear();

    auto allRoles = {DataTypes::TitleRole, DataTypes::ResourceRole,
                     DataTypes::CommentRole, DataTypes::ImageUrlRole};

    for (auto role : allRoles) {
        mDisplayKeys.push_back(role);
        if (role == DataTypes::DatabaseIdRole) {
            mFullData[role] = -1;
        } else {
            mFullData[role] = QString();
        }
    }
    mDisplayData = mFullData;
    mFullData[DataTypes::ElementTypeRole] = ElisaUtils::Radio;
    filterDataFromTrackData();
    endResetModel();
}

void TrackMetadataModel::initializeByUrl(ElisaUtils::PlayListEntryType type, const QUrl &url)
{
    mFullData.clear();
    mDisplayData.clear();
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

    fillDataFromTrackData(radiosData);
}

#include "moc_trackmetadatamodel.cpp"
