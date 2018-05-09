/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "elisautils.h"

#include <KFileMetaData/ExtractorCollection>
#include <KFileMetaData/Extractor>
#include <KFileMetaData/SimpleExtractionResult>
#include <KFileMetaData/UserMetaData>

#include <QFileInfo>

MusicAudioTrack ElisaUtils::scanOneFile(const QUrl &scanFile, const QMimeDatabase &mimeDatabase,
                                        const KFileMetaData::ExtractorCollection &allExtractors)
{
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

    QList<KFileMetaData::Extractor*> exList = allExtractors.fetchExtractors(mimetype);

    if (exList.isEmpty()) {
        return newTrack;
    }

    KFileMetaData::Extractor* ex = exList.first();
    KFileMetaData::SimpleExtractionResult result(localFileName, mimetype,
                                                 KFileMetaData::ExtractionResult::ExtractMetaData);

    ex->extract(&result);

    const auto &allProperties = result.properties();

    ElisaUtils::scanProperties(localFileName, allProperties, newTrack);

    return newTrack;
}

void ElisaUtils::scanProperties(const QString localFileName, const KFileMetaData::PropertyMap &allProperties, MusicAudioTrack &trackData)
{
    auto titleProperty = allProperties.find(KFileMetaData::Property::Title);
    auto durationProperty = allProperties.find(KFileMetaData::Property::Duration);
    auto artistProperty = allProperties.find(KFileMetaData::Property::Artist);
    auto albumProperty = allProperties.find(KFileMetaData::Property::Album);
    auto albumArtistProperty = allProperties.find(KFileMetaData::Property::AlbumArtist);
    auto trackNumberProperty = allProperties.find(KFileMetaData::Property::TrackNumber);
    auto discNumberProperty = allProperties.find(KFileMetaData::Property::DiscNumber);
    auto genreProperty = allProperties.find(KFileMetaData::Property::Genre);
    auto yearProperty = allProperties.find(KFileMetaData::Property::ReleaseYear);
    auto composerProperty = allProperties.find(KFileMetaData::Property::Composer);
    auto lyricistProperty = allProperties.find(KFileMetaData::Property::Lyricist);
    auto channelsProperty = allProperties.find(KFileMetaData::Property::Channels);
    auto bitRateProperty = allProperties.find(KFileMetaData::Property::BitRate);
    auto sampleRateProperty = allProperties.find(KFileMetaData::Property::SampleRate);
    auto commentProperty = allProperties.find(KFileMetaData::Property::Comment);
#if defined Q_OS_LINUX && !defined Q_OS_ANDROID
    auto fileData = KFileMetaData::UserMetaData(localFileName);
#endif

    if (albumProperty != allProperties.end()) {
        auto albumValue = albumProperty->toString();

        trackData.setAlbumName(albumValue);

        if (artistProperty != allProperties.end()) {
            trackData.setArtist(artistProperty->toStringList().join(QStringLiteral(", ")));
        }

        if (durationProperty != allProperties.end()) {
            trackData.setDuration(QTime::fromMSecsSinceStartOfDay(int(1000 * durationProperty->toDouble())));
        }

        if (titleProperty != allProperties.end()) {
            trackData.setTitle(titleProperty->toString());
        }

        if (trackNumberProperty != allProperties.end()) {
            trackData.setTrackNumber(trackNumberProperty->toInt());
        }

        if (discNumberProperty != allProperties.end()) {
            trackData.setDiscNumber(discNumberProperty->toInt());
        } else {
            trackData.setDiscNumber(1);
        }

        if (albumArtistProperty != allProperties.end()) {
            trackData.setAlbumArtist(albumArtistProperty->toStringList().join(QStringLiteral(", ")));
        }

        if (yearProperty != allProperties.end()) {
            trackData.setYear(yearProperty->toInt());
        }

        if (channelsProperty != allProperties.end()) {
            trackData.setChannels(channelsProperty->toInt());
        }

        if (bitRateProperty != allProperties.end()) {
            trackData.setBitRate(bitRateProperty->toInt());
        }

        if (sampleRateProperty != allProperties.end()) {
            trackData.setSampleRate(sampleRateProperty->toInt());
        }

        if (genreProperty != allProperties.end()) {
            trackData.setGenre(genreProperty->toStringList().join(QStringLiteral(", ")));
        }

        if (composerProperty != allProperties.end()) {
            trackData.setComposer(composerProperty->toStringList().join(QStringLiteral(", ")));
        }

        if (lyricistProperty != allProperties.end()) {
            trackData.setLyricist(lyricistProperty->toStringList().join(QStringLiteral(", ")));
        }

        if (commentProperty != allProperties.end()) {
            trackData.setComment(commentProperty->toString());
        }

        if (trackData.artist().isEmpty()) {
            trackData.setArtist(trackData.albumArtist());
        }

#if defined Q_OS_LINUX && !defined Q_OS_ANDROID
        trackData.setRating(fileData.rating());
#else
        trackData.setRating(0);
#endif

        if (trackData.title().isEmpty()) {
            return;
        }

        if (trackData.artist().isEmpty()) {
            return;
        }

        if (trackData.albumName().isEmpty()) {
            return;
        }

        if (!trackData.duration().isValid()) {
            return;
        }

        trackData.setValid(true);
    }
}


#include "moc_elisautils.cpp"
