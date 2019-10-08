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

#include <QLoggingCategory>
#include <QFileInfo>
#include <QLocale>
#include <QDir>

class FileScannerPrivate
{
public:
    static const QStringList constSearchStrings;
#if defined KF5FileMetaData_FOUND && KF5FileMetaData_FOUND
    KFileMetaData::ExtractorCollection mAllExtractors;

    KFileMetaData::PropertyMap mAllProperties;

    KFileMetaData::EmbeddedImageData mImageScanner;
#endif

};

const QStringList FileScannerPrivate::constSearchStrings = {
    QStringLiteral("*[Cc]over*.jpg"),
    QStringLiteral("*[Cc]over*.png"),
    QStringLiteral("*[Ff]older*.jpg"),
    QStringLiteral("*[Ff]older*.png"),
    QStringLiteral("*[Ff]ront*.jpg"),
    QStringLiteral("*[Ff]ront*.png")
};

FileScanner::FileScanner() : d(std::make_unique<FileScannerPrivate>())
{
}

FileScanner::~FileScanner() = default;

MusicAudioTrack FileScanner::scanOneFile(const QUrl &scanFile, const QMimeDatabase &mimeDatabase)
{
    MusicAudioTrack newTrack;

    auto localFileName = scanFile.toLocalFile();

    QFileInfo scanFileInfo(localFileName);
    newTrack.setFileModificationTime(scanFileInfo.fileTime(QFile::FileModificationTime));
    newTrack.setResourceURI(scanFile);
    newTrack.setRating(0);

#if defined KF5FileMetaData_FOUND && KF5FileMetaData_FOUND
    const auto &fileMimeType = mimeDatabase.mimeTypeForFile(localFileName);
    if (!fileMimeType.name().startsWith(QLatin1String("audio/"))) {
        return newTrack;
    }

    QString mimetype = fileMimeType.name();

    QList<KFileMetaData::Extractor*> exList = d->mAllExtractors.fetchExtractors(mimetype);

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
    Q_UNUSED(mimeDatabase)

    newTrack.setTitle(scanFileInfo.fileName());
    newTrack.setValid(true);

    qCDebug(orgKdeElisaIndexer()) << "scanOneFile" << scanFile << "no metadata provider" << newTrack;
#endif

    return newTrack;
}

void FileScanner::scanProperties(const Baloo::File &match, MusicAudioTrack &trackData)
{
#if defined KF5Baloo_FOUND && KF5Baloo_FOUND
    d->mAllProperties = match.properties();
    scanProperties(match.path(), trackData);

    qCDebug(orgKdeElisaIndexer()) << "scanProperties" << match.path() << "using Baloo" << trackData;
#else
    Q_UNUSED(match)
    Q_UNUSED(trackData)

    qCDebug(orgKdeElisaIndexer()) << "scanProperties" << "no metadata provider" << trackData;
#endif
}

void FileScanner::scanProperties(const QString &localFileName, MusicAudioTrack &trackData)
{
#if defined KF5FileMetaData_FOUND && KF5FileMetaData_FOUND
    if (d->mAllProperties.isEmpty()) {
        return;
    }
    using entry = std::pair<const KFileMetaData::Property::Property&, const QVariant&>;

    auto rangeBegin = d->mAllProperties.constKeyValueBegin();
    QVariant value;
    while (rangeBegin != d->mAllProperties.constKeyValueEnd()) {
        auto key = (*rangeBegin).first;

        auto rangeEnd = std::find_if(rangeBegin, d->mAllProperties.constKeyValueEnd(),
                                     [key](const entry& e) { return e.first != key; });

        auto distance = std::distance(rangeBegin, rangeEnd);
        if (distance > 1) {
            QStringList list;
            list.reserve(static_cast<int>(distance));
            std::for_each(rangeBegin, rangeEnd, [&list](const entry& s) { list.append(s.second.toString()); });
            value = QLocale().createSeparatedList(list);
        } else {
            value = (*rangeBegin).second;
            if (value.canConvert<QStringList>()) {
                value = QLocale().createSeparatedList(value.toStringList());
            }
        }
        switch (key)
        {
        case KFileMetaData::Property::Artist:
            trackData.setArtist(value.toString());
            break;
        case KFileMetaData::Property::AlbumArtist:
            trackData.setAlbumArtist(value.toString());
            break;
        case KFileMetaData::Property::Genre:
            trackData.setGenre(value.toString());
            break;
        case KFileMetaData::Property::Composer:
            trackData.setComposer(value.toString());
            break;
        case KFileMetaData::Property::Lyricist:
            trackData.setLyricist(value.toString());
            break;
        case KFileMetaData::Property::Title:
            trackData.setTitle(value.toString());
            break;
        case KFileMetaData::Property::Duration:
            trackData.setDuration(QTime::fromMSecsSinceStartOfDay(int(1000 * value.toDouble())));
            break;
        case KFileMetaData::Property::Album:
            trackData.setAlbumName(value.toString());
            break;
        case KFileMetaData::Property::TrackNumber:
            trackData.setTrackNumber(value.toInt());
            break;
        case KFileMetaData::Property::DiscNumber:
            trackData.setDiscNumber(value.toInt());
            break;
        case KFileMetaData::Property::ReleaseYear:
            trackData.setYear(value.toInt());
            break;
        case KFileMetaData::Property::Lyrics:
            trackData.setLyrics(value.toString());
            break;
        case KFileMetaData::Property::Channels:
            trackData.setChannels(value.toInt());
            break;
        case KFileMetaData::Property::BitRate:
            trackData.setBitRate(value.toInt());
            break;
        case KFileMetaData::Property::SampleRate:
            trackData.setSampleRate(value.toInt());
            break;
        case KFileMetaData::Property::Comment:
            trackData.setComment(value.toString());
            break;
        case KFileMetaData::Property::Rating:
            trackData.setRating(value.toInt());
            break;
        default:
            break;
        }
        rangeBegin = rangeEnd;
    }

#if !defined Q_OS_ANDROID && !defined Q_OS_WIN
    auto fileData = KFileMetaData::UserMetaData(localFileName);
    QString comment = fileData.userComment();
    if (!comment.isEmpty()) {
        trackData.setComment(comment);
    }

    int rating = fileData.rating();
    if (rating > 0) {
        trackData.setRating(rating);
    }
#endif

    if (!trackData.duration().isValid()) {
        return;
    }

    trackData.setValid(true);
#else
    Q_UNUSED(localFileName)
    Q_UNUSED(trackData)
#endif
}

QUrl FileScanner::searchForCoverFile(const QString &localFileName)
{
    QFileInfo trackFilePath(localFileName);
    QDir trackFileDir = trackFilePath.absoluteDir();
    trackFileDir.setFilter(QDir::Files);
    trackFileDir.setNameFilters(d->constSearchStrings);
    QFileInfoList coverFiles = trackFileDir.entryInfoList();
    if (coverFiles.isEmpty()) {
        QString dirNamePattern = QLatin1String("*") + trackFileDir.dirName() + QLatin1String("*");
        QString dirNameNoSpaces = dirNamePattern.remove(QLatin1Char(' '));
        QStringList filters = {
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
    auto imageData = d->mImageScanner.imageData(localFileName);

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
