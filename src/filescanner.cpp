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

#if defined KF5FileMetaData_FOUND && KF5FileMetaData_FOUND

#include <KFileMetaData/ExtractorCollection>
#include <KFileMetaData/Extractor>
#include <KFileMetaData/SimpleExtractionResult>
#include <KFileMetaData/UserMetaData>
#include <KFileMetaData/Properties>

#if defined KF5Baloo_FOUND && KF5Baloo_FOUND

#include <Baloo/File>

#endif

#endif

#include <QFileInfo>

class FileScannerPrivate
{
public:

#if defined KF5FileMetaData_FOUND && KF5FileMetaData_FOUND
    KFileMetaData::ExtractorCollection mAllExtractors;

    KFileMetaData::PropertyMap mAllProperties;
#endif

};

FileScanner::FileScanner() : d(std::make_unique<FileScannerPrivate>())
{
}

FileScanner::~FileScanner() = default;

MusicAudioTrack FileScanner::scanOneFile(const QUrl &scanFile, const QMimeDatabase &mimeDatabase)
{
#if defined KF5FileMetaData_FOUND && KF5FileMetaData_FOUND
    MusicAudioTrack newTrack;

    auto localFileName = scanFile.toLocalFile();

    QFileInfo scanFileInfo(localFileName);
    newTrack.setFileModificationTime(scanFileInfo.fileTime(QFile::FileModificationTime));
    newTrack.setResourceURI(scanFile);

    const auto &fileMimeType = mimeDatabase.mimeTypeForFile(localFileName);
    if (!fileMimeType.name().startsWith(QStringLiteral("audio/"))) {
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

    return newTrack;
#else
    Q_UNUSED(scanFile)
    Q_UNUSED(mimeDatabase)

    return {};
#endif
}

void FileScanner::scanProperties(const Baloo::File &match, MusicAudioTrack &trackData)
{
#if defined KF5Baloo_FOUND && KF5Baloo_FOUND
    d->mAllProperties = match.properties();
    scanProperties(match.path(), trackData);
#else
    Q_UNUSED(match)
    Q_UNUSED(trackData)
#endif
}

void FileScanner::scanProperties(const QString &localFileName, MusicAudioTrack &trackData)
{
#if defined KF5FileMetaData_FOUND && KF5FileMetaData_FOUND
    auto titleProperty = d->mAllProperties.find(KFileMetaData::Property::Title);
    auto durationProperty = d->mAllProperties.find(KFileMetaData::Property::Duration);
    auto artistProperty = d->mAllProperties.find(KFileMetaData::Property::Artist);
    auto albumProperty = d->mAllProperties.find(KFileMetaData::Property::Album);
    auto albumArtistProperty = d->mAllProperties.find(KFileMetaData::Property::AlbumArtist);
    auto trackNumberProperty = d->mAllProperties.find(KFileMetaData::Property::TrackNumber);
    auto discNumberProperty = d->mAllProperties.find(KFileMetaData::Property::DiscNumber);
    auto genreProperty = d->mAllProperties.find(KFileMetaData::Property::Genre);
    auto yearProperty = d->mAllProperties.find(KFileMetaData::Property::ReleaseYear);
    auto composerProperty = d->mAllProperties.find(KFileMetaData::Property::Composer);
    auto lyricistProperty = d->mAllProperties.find(KFileMetaData::Property::Lyricist);
    auto channelsProperty = d->mAllProperties.find(KFileMetaData::Property::Channels);
    auto bitRateProperty = d->mAllProperties.find(KFileMetaData::Property::BitRate);
    auto sampleRateProperty = d->mAllProperties.find(KFileMetaData::Property::SampleRate);
    auto commentProperty = d->mAllProperties.find(KFileMetaData::Property::Comment);
    auto ratingProperty = d->mAllProperties.find(KFileMetaData::Property::Rating);
#if !defined Q_OS_ANDROID
    auto fileData = KFileMetaData::UserMetaData(localFileName);
#endif

    if (albumProperty != d->mAllProperties.end()) {
        trackData.setAlbumName(albumProperty->toString());
    }

    if (artistProperty != d->mAllProperties.end()) {
        trackData.setArtist(artistProperty->toStringList().join(QStringLiteral(", ")));
    }

    if (durationProperty != d->mAllProperties.end()) {
        trackData.setDuration(QTime::fromMSecsSinceStartOfDay(int(1000 * durationProperty->toDouble())));
    }

    if (titleProperty != d->mAllProperties.end()) {
        trackData.setTitle(titleProperty->toString());
    }

    if (trackNumberProperty != d->mAllProperties.end()) {
        trackData.setTrackNumber(trackNumberProperty->toInt());
    }

    if (discNumberProperty != d->mAllProperties.end()) {
        trackData.setDiscNumber(discNumberProperty->toInt());
    } else {
        trackData.setDiscNumber(1);
    }

    if (albumArtistProperty != d->mAllProperties.end()) {
        trackData.setAlbumArtist(albumArtistProperty->toStringList().join(QStringLiteral(", ")));
    }

    if (yearProperty != d->mAllProperties.end()) {
        trackData.setYear(yearProperty->toInt());
    }

    if (channelsProperty != d->mAllProperties.end()) {
        trackData.setChannels(channelsProperty->toInt());
    }

    if (bitRateProperty != d->mAllProperties.end()) {
        trackData.setBitRate(bitRateProperty->toInt());
    }

    if (sampleRateProperty != d->mAllProperties.end()) {
        trackData.setSampleRate(sampleRateProperty->toInt());
    }

    if (genreProperty != d->mAllProperties.end()) {
        trackData.setGenre(genreProperty->toStringList().join(QStringLiteral(", ")));
    }

    if (composerProperty != d->mAllProperties.end()) {
        trackData.setComposer(composerProperty->toStringList().join(QStringLiteral(", ")));
    }

    if (lyricistProperty != d->mAllProperties.end()) {
        trackData.setLyricist(lyricistProperty->toStringList().join(QStringLiteral(", ")));
    }

    if (trackData.artist().isEmpty()) {
        trackData.setArtist(trackData.albumArtist());
    }

#if !defined Q_OS_ANDROID
    QString comment = fileData.userComment();
    if (!comment.isEmpty()) {
        trackData.setComment(comment);
    } else if (commentProperty != d->mAllProperties.end()) {
        trackData.setComment(commentProperty->toString());
    }

    int rating = fileData.rating();
    if (rating > 0) {
        trackData.setRating(rating);
    } else if (ratingProperty != d->mAllProperties.end()) {
        trackData.setRating(ratingProperty->toInt());
    } else {
        trackData.setRating(0);
    }
#else
    if (ratingProperty != d->mAllProperties.end()) {
        trackData.setRating(ratingProperty->toInt());
    } else {
        trackData.setRating(0);
    }

    if (commentProperty != d->mAllProperties.end()) {
        trackData.setComment(commentProperty->toString());
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
