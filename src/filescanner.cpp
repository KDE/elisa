/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "filescanner.h"

#include "config-upnp-qt.h"

#include "abstractfile/indexercommon.h"

#if KFFileMetaData_FOUND

#include <KFileMetaData/ExtractorCollection>
#include <KFileMetaData/Extractor>
#include <KFileMetaData/SimpleExtractionResult>
#include <KFileMetaData/UserMetaData>
#include <KFileMetaData/Properties>

#if KFBaloo_FOUND

#include <Baloo/File>

#endif

#endif

#include <QFileInfo>
#include <QLocale>
#include <QDir>
#include <QHash>
#include <QMimeDatabase>

class FileScannerPrivate
{
public:
#if KFFileMetaData_FOUND
    KFileMetaData::ExtractorCollection mAllExtractors;

#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    KFileMetaData::PropertyMultiMap mAllProperties;
#else
    KFileMetaData::PropertyMap mAllProperties;
#endif

    KFileMetaData::EmbeddedImageData mImageScanner;
#endif

    QMimeDatabase mMimeDb;

#if KFFileMetaData_FOUND
    const QHash<KFileMetaData::Property::Property, DataTypes::ColumnsRoles> propertyTranslation = {
        {KFileMetaData::Property::Artist, DataTypes::ColumnsRoles::ArtistRole},
        {KFileMetaData::Property::AlbumArtist, DataTypes::ColumnsRoles::AlbumArtistRole},
        {KFileMetaData::Property::Genre, DataTypes::ColumnsRoles::GenreRole},
        {KFileMetaData::Property::Composer, DataTypes::ColumnsRoles::ComposerRole},
        {KFileMetaData::Property::Lyricist, DataTypes::ColumnsRoles::LyricistRole},
        {KFileMetaData::Property::Title, DataTypes::ColumnsRoles::TitleRole},
        {KFileMetaData::Property::Album, DataTypes::ColumnsRoles::AlbumRole},
        {KFileMetaData::Property::TrackNumber, DataTypes::ColumnsRoles::TrackNumberRole},
        {KFileMetaData::Property::DiscNumber, DataTypes::ColumnsRoles::DiscNumberRole},
        {KFileMetaData::Property::ReleaseYear, DataTypes::ColumnsRoles::YearRole},
        {KFileMetaData::Property::Lyrics, DataTypes::ColumnsRoles::LyricsRole},
        {KFileMetaData::Property::Comment, DataTypes::ColumnsRoles::CommentRole},
        {KFileMetaData::Property::Rating, DataTypes::ColumnsRoles::RatingRole},
        {KFileMetaData::Property::Channels, DataTypes::ColumnsRoles::ChannelsRole},
        {KFileMetaData::Property::SampleRate, DataTypes::ColumnsRoles::SampleRateRole},
        {KFileMetaData::Property::BitRate, DataTypes::ColumnsRoles::BitRateRole},
        {KFileMetaData::Property::Duration, DataTypes::ColumnsRoles::DurationRole},
    };
#endif

    const QStringList constCoverExtensions {
        QStringLiteral(".jpg")
        ,QStringLiteral(".jpeg")
        ,QStringLiteral(".png")
    };

    const QStringList constCoverNames {
        QStringLiteral("[Cc]over")
        ,QStringLiteral("[Cc]over[Ii]mage")
        ,QStringLiteral("[Ff]older")
        ,QStringLiteral("[Aa]lbumart")
    };

    const QStringList constCoverGlobs = {
        QStringLiteral("*[Cc]over*")
        ,QStringLiteral("*[Ff]older*")
        ,QStringLiteral("*[Ff]ront*")
        ,QStringLiteral("*[Aa]lbumart*")
    };

    QStringList coverFileAllImages;
    QStringList coverFileNames;
    QStringList coverFileGlobs;
};

QStringList buildCoverFileNames(const QStringList &fileNames, const QStringList &fileExtensions)
{
    QStringList covers {};
    for (auto fileName : fileNames) {
        for (auto fileExtension : fileExtensions) {
            covers.push_back(fileName + fileExtension);
        }
    }
    return covers;
}

FileScanner::FileScanner() : d(std::make_unique<FileScannerPrivate>())
{
    d->coverFileAllImages = buildCoverFileNames({QStringLiteral("*")}, d->constCoverExtensions);
    d->coverFileNames = buildCoverFileNames(d->constCoverNames, d->constCoverExtensions);
    d->coverFileGlobs = buildCoverFileNames(d->constCoverGlobs, d->constCoverExtensions);
}

bool FileScanner::shouldScanFile(const QString &scanFile)
{
    const auto &fileMimeType = d->mMimeDb.mimeTypeForFile(scanFile);
    return fileMimeType.name().startsWith(QLatin1String("audio/"));
}

FileScanner::~FileScanner() = default;

DataTypes::TrackDataType FileScanner::scanOneFile(const QUrl &scanFile, const QFileInfo &scanFileInfo)
{
    DataTypes::TrackDataType newTrack;

    if (!scanFile.isLocalFile() && !scanFile.scheme().isEmpty()) {
        return newTrack;
    }

    const auto &localFileName = scanFile.toLocalFile();

    newTrack[DataTypes::FileModificationTime] = scanFileInfo.metadataChangeTime();
    newTrack[DataTypes::ResourceRole] = scanFile;
    newTrack[DataTypes::RatingRole] = 0;
    newTrack[DataTypes::ElementTypeRole] = ElisaUtils::Track;

#if KFFileMetaData_FOUND
    const auto &fileMimeType = d->mMimeDb.mimeTypeForFile(localFileName);
    if (!fileMimeType.name().startsWith(QLatin1String("audio/"))) {
        return newTrack;
    }

    const auto &mimetype = fileMimeType.name();

    const QList<KFileMetaData::Extractor*> &exList = d->mAllExtractors.fetchExtractors(mimetype);

    if (exList.isEmpty()) {
        // when no extractors exist and we have an audio file, we fallback to filling the minimal
        // set of properties to let Elisa be able to recognise and play the file.

        qCDebug(orgKdeElisaIndexer()) << "FileScanner::shouldScanFile" << scanFile << localFileName << "no extractors" << fileMimeType;

        newTrack[DataTypes::FileModificationTime] = scanFileInfo.metadataChangeTime();
        newTrack[DataTypes::ResourceRole] = scanFile;
        newTrack[DataTypes::RatingRole] = 0;
        newTrack[DataTypes::DurationRole] = QTime::fromMSecsSinceStartOfDay(1);
        newTrack[DataTypes::ElementTypeRole] = ElisaUtils::Track;

        return newTrack;
    }

    KFileMetaData::Extractor* ex = exList.first();
    KFileMetaData::SimpleExtractionResult result(localFileName, mimetype,
                                                 KFileMetaData::ExtractionResult::ExtractMetaData);

    ex->extract(&result);

    d->mAllProperties = result.properties();

    scanProperties(localFileName, newTrack);

    qCDebug(orgKdeElisaIndexer()) << "scanOneFile" << scanFile << "using KFileMetaData" << newTrack;
#else
    Q_UNUSED(scanFile)
    Q_UNUSED(scanFileInfo)

    qCDebug(orgKdeElisaIndexer()) << "scanOneFile" << scanFile << "no metadata provider" << newTrack;
#endif

    // try to find lyric in FILENAME.lrc if no lyric in metadata
    if (!newTrack.hasLyrics()) {
        QString baseName = scanFileInfo.baseName();
        QDir dir = scanFileInfo.dir();
        QString lyricPath = dir.filePath(baseName + QStringLiteral(".lrc"));
        if (!QFile::exists(lyricPath)) {
            lyricPath = dir.filePath(baseName + QStringLiteral(".LRC"));
        }
        QFile lyric(lyricPath);
        if (lyric.exists() && lyric.open(QFile::ReadOnly)) {
            QString lyricString = QString::fromUtf8(lyric.readAll());
            if (!lyricString.isEmpty()) {
                newTrack[DataTypes::LyricsRole] = lyricString;
            }
        }
    }

    return newTrack;
}

DataTypes::TrackDataType FileScanner::scanOneFile(const QUrl &scanFile)
{
    if (!scanFile.isLocalFile()){
        return {};
    } else {
        const QFileInfo scanFileInfo(scanFile.toLocalFile());
        return FileScanner::scanOneFile(scanFile, scanFileInfo);
    }
}

DataTypes::TrackDataType FileScanner::scanOneBalooFile(const QUrl &scanFile, const QFileInfo &scanFileInfo)
{
    DataTypes::TrackDataType newTrack;
#if KFBaloo_FOUND
    const auto &localFileName = scanFile.toLocalFile();

    newTrack[DataTypes::FileModificationTime] = scanFileInfo.metadataChangeTime();
    newTrack[DataTypes::ResourceRole] = scanFile;
    newTrack[DataTypes::RatingRole] = 0;
    newTrack[DataTypes::ElementTypeRole] = ElisaUtils::Track;

    Baloo::File match(localFileName);

    match.load();

    d->mAllProperties = match.properties();
    scanProperties(match.path(), newTrack);

    qCDebug(orgKdeElisaIndexer()) << "scanOneFile" << scanFile << "using Baloo" << newTrack;
#else
    Q_UNUSED(scanFile)
    Q_UNUSED(scanFileInfo)

    qCDebug(orgKdeElisaIndexer()) << "scanOneFile" << scanFile << "no baloo metadata provider" << newTrack;
#endif
    return newTrack;
}

void FileScanner::scanProperties(const QString &localFileName, DataTypes::TrackDataType &trackData)
{
#if KFFileMetaData_FOUND
    if (d->mAllProperties.isEmpty()) {
        return;
    }
    using entry = std::pair<const KFileMetaData::Property::Property&, const QVariant&>;

    auto rangeBegin = d->mAllProperties.constKeyValueBegin();
    QVariant value;
    while (rangeBegin != d->mAllProperties.constKeyValueEnd()) {
        const auto key = (*rangeBegin).first;

        const auto rangeEnd = std::find_if(rangeBegin, d->mAllProperties.constKeyValueEnd(),
                                     [key](entry e) { return e.first != key; });

        const auto distance = std::distance(rangeBegin, rangeEnd);
        if (distance > 1) {
            QStringList list;
            list.reserve(static_cast<int>(distance));
            std::for_each(rangeBegin, rangeEnd, [&list](entry s) { list.append(s.second.toString()); });
            value = QLocale().createSeparatedList(list);
        } else {
            value = (*rangeBegin).second;
        }
        if (d->propertyTranslation.contains(key)) {
            const auto &translatedKey = d->propertyTranslation.find(key);
            if (translatedKey.value() == DataTypes::DurationRole) {
                trackData.insert(translatedKey.value(), QTime::fromMSecsSinceStartOfDay(int(1000 * (*rangeBegin).second.toDouble())));
            } else if (translatedKey != d->propertyTranslation.end()) {
                trackData.insert(translatedKey.value(), (*rangeBegin).second);
            }
        }
        rangeBegin = rangeEnd;
    }

    if (!trackData.isValid()) {
        return;
    }

    if (checkEmbeddedCoverImage(localFileName)) {
        trackData[DataTypes::HasEmbeddedCover] = true;
        trackData[DataTypes::ImageUrlRole] = QUrl(QLatin1String("image://cover/") + localFileName);
    } else {
        trackData[DataTypes::HasEmbeddedCover] = false;
    }

#if !defined Q_OS_ANDROID && !defined Q_OS_WIN
    const auto fileData = KFileMetaData::UserMetaData(localFileName);
    const auto &comment = fileData.userComment();
    if (!comment.isEmpty()) {
        trackData[DataTypes::CommentRole] = comment;
    }

    const auto rating = fileData.rating();
    if (rating >= 0) {
        trackData[DataTypes::RatingRole] = rating;
    }
#endif

#else
    Q_UNUSED(localFileName)
    Q_UNUSED(trackData)
#endif
}

QUrl FileScanner::searchForCoverFile(const QString &localFileName)
{
    const QFileInfo trackFilePath(localFileName);
    QDir trackFileDir = trackFilePath.absoluteDir();

    static QHash<QString, QUrl> directoryCache;
    if (directoryCache.contains(trackFileDir.path())) {
        return directoryCache.value(trackFileDir.path());
    }

    trackFileDir.setFilter(QDir::Files);
    trackFileDir.setNameFilters(d->coverFileAllImages);
    QFileInfoList coverFiles = trackFileDir.entryInfoList();

    if (coverFiles.length() != 1) {
        trackFileDir.setNameFilters(d->coverFileNames);
        coverFiles = trackFileDir.entryInfoList();
    }

    if (coverFiles.isEmpty()) {
        trackFileDir.setNameFilters(d->coverFileGlobs);
        coverFiles = trackFileDir.entryInfoList();
    }

    if (coverFiles.isEmpty()) {
        const QString dirNamePattern = QLatin1String("*") + trackFileDir.dirName() + QLatin1String("*");
        const QString dirNameNoSpaces = QLatin1String("*") + trackFileDir.dirName().remove(QLatin1Char(' ')) + QLatin1String("*");
        auto filters = buildCoverFileNames({dirNamePattern, dirNameNoSpaces}, d->constCoverExtensions);
        trackFileDir.setNameFilters(filters);
        coverFiles = trackFileDir.entryInfoList();
    }

    if (coverFiles.isEmpty()) {
        trackFileDir.setNameFilters(d->coverFileAllImages);
        coverFiles = trackFileDir.entryInfoList();
    }

    if (coverFiles.isEmpty()) {
        directoryCache.insert(trackFileDir.path(), QUrl());
        return QUrl();
    }

    const QUrl url = QUrl::fromLocalFile(coverFiles.first().absoluteFilePath());
    directoryCache.insert(trackFileDir.path(), url);

    return url;
}

bool FileScanner::checkEmbeddedCoverImage(const QString &localFileName)
{
#if KFFileMetaData_FOUND
    const auto &mimeType = QMimeDatabase().mimeTypeForFile(localFileName).name();
    auto extractors = d->mAllExtractors.fetchExtractors(mimeType);

    for (const auto &extractor : extractors) {
        KFileMetaData::SimpleExtractionResult result(localFileName, mimeType, KFileMetaData::ExtractionResult::ExtractImageData);
        extractor->extract(&result);
        if (!result.imageData().isEmpty()) {
            return true;
        }
    }

#else
    Q_UNUSED(localFileName)
#endif

    return false;
}
