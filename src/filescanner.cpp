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

#include "filescanner.h"

#include "config-upnp-qt.h"

#include "abstractfile/indexercommon.h"

#if defined KF5FileMetaData_FOUND && KF5FileMetaData_FOUND

#include <KFileMetaData/ExtractorCollection>
#include <KFileMetaData/Extractor>
#include <KFileMetaData/SimpleExtractionResult>
#include <KFileMetaData/UserMetaData>
#include <KFileMetaData/Properties>
#include <KFileMetaData/EmbeddedImageData>

#if defined KF5Baloo_FOUND && KF5Baloo_FOUND

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
#if defined KF5FileMetaData_FOUND && KF5FileMetaData_FOUND
    KFileMetaData::ExtractorCollection mAllExtractors;

    KFileMetaData::PropertyMap mAllProperties;

    KFileMetaData::EmbeddedImageData mImageScanner;
#endif

    QMimeDatabase mMimeDb;

#if defined KF5FileMetaData_FOUND && KF5FileMetaData_FOUND
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

    const QStringList constSearchStrings = {
        QStringLiteral("*[Cc]over*.jpg"),
        QStringLiteral("*[Cc]over*.png"),
        QStringLiteral("*[Ff]older*.jpg"),
        QStringLiteral("*[Ff]older*.png"),
        QStringLiteral("*[Ff]ront*.jpg"),
        QStringLiteral("*[Ff]ront*.png")
    };
};

FileScanner::FileScanner() : d(std::make_unique<FileScannerPrivate>())
{
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

#if defined KF5FileMetaData_FOUND && KF5FileMetaData_FOUND
    const auto &fileMimeType = d->mMimeDb.mimeTypeForFile(localFileName);
    if (!fileMimeType.name().startsWith(QLatin1String("audio/"))) {
        return newTrack;
    }

    const auto &mimetype = fileMimeType.name();

    const QList<KFileMetaData::Extractor*> &exList = d->mAllExtractors.fetchExtractors(mimetype);

    if (exList.isEmpty()) {
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
#if defined KF5Baloo_FOUND && KF5Baloo_FOUND
    const auto &localFileName = scanFile.toLocalFile();

    newTrack[DataTypes::FileModificationTime] = scanFileInfo.metadataChangeTime();
    newTrack[DataTypes::ResourceRole] = scanFile;
    newTrack[DataTypes::RatingRole] = 0;

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
#if defined KF5FileMetaData_FOUND && KF5FileMetaData_FOUND
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
        const auto &translatedKey = d->propertyTranslation.find(key);
        if (translatedKey.value() == DataTypes::DurationRole) {
            trackData.insert(translatedKey.value(), QTime::fromMSecsSinceStartOfDay(int(1000 * (*rangeBegin).second.toDouble())));
        } else if (translatedKey != d->propertyTranslation.end()) {
            trackData.insert(translatedKey.value(), (*rangeBegin).second);
        }
        rangeBegin = rangeEnd;
    }

    if (!trackData.isValid()) {
        return;
    }

    trackData[DataTypes::HasEmbeddedCover] = checkEmbeddedCoverImage(localFileName);

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
    trackFileDir.setFilter(QDir::Files);
    trackFileDir.setNameFilters(d->constSearchStrings);
    QFileInfoList coverFiles = trackFileDir.entryInfoList();
    if (coverFiles.isEmpty()) {
        const QString dirNamePattern = QLatin1String("*") + trackFileDir.dirName() + QLatin1String("*");
        const QString dirNameNoSpaces = QLatin1String("*") + trackFileDir.dirName().remove(QLatin1Char(' ')) + QLatin1String("*");
        const QStringList filters = {
            dirNamePattern + QStringLiteral(".jpg"),
            dirNamePattern + QStringLiteral(".png"),
            dirNameNoSpaces + QStringLiteral(".jpg"),
            dirNameNoSpaces + QStringLiteral(".png")
        };
        trackFileDir.setNameFilters(filters);
        coverFiles = trackFileDir.entryInfoList();
    }
    if (coverFiles.isEmpty()) {
        return QUrl();
    }
    return QUrl::fromLocalFile(coverFiles.first().absoluteFilePath());
}

bool FileScanner::checkEmbeddedCoverImage(const QString &localFileName)
{
#if defined KF5FileMetaData_FOUND && KF5FileMetaData_FOUND
    const auto &imageData = d->mImageScanner.imageData(localFileName);

    if (imageData.contains(KFileMetaData::EmbeddedImageData::FrontCover)) {
        if (!imageData[KFileMetaData::EmbeddedImageData::FrontCover].isEmpty()) {
            return true;
        }
    }
#else
    Q_UNUSED(localFileName)
#endif

    return false;
}
