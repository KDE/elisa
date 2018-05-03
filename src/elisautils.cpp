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

#include <KFileMetaData/Properties>
#include <KFileMetaData/ExtractorCollection>
#include <KFileMetaData/Extractor>
#include <KFileMetaData/SimpleExtractionResult>
#include <KFileMetaData/UserMetaData>

#include <QFileInfo>
#include <QFile>

MusicAudioTrack ElisaUtils::scanOneFile(const QUrl &scanFile, const QMimeDatabase &mimeDatabase,
                                        const KFileMetaData::ExtractorCollection &allExtractors)
{
    MusicAudioTrack newTrack;

    auto localFileName = scanFile.toLocalFile();

    const auto &fileMimeType = mimeDatabase.mimeTypeForFile(localFileName);
    if (!fileMimeType.name().startsWith(QStringLiteral("audio/"))) {
        return newTrack;
    }

    QString mimetype = fileMimeType.name();

    QList<KFileMetaData::Extractor*> exList = allExtractors.fetchExtractors(mimetype);

    if (exList.isEmpty()) {
        return newTrack;
    }

    QFileInfo fileInfo(localFileName);
    newTrack.setFileModificationTime(fileInfo.fileTime(QFile::FileModificationTime));

    KFileMetaData::Extractor* ex = exList.first();
    KFileMetaData::SimpleExtractionResult result(localFileName, mimetype,
                                                 KFileMetaData::ExtractionResult::ExtractMetaData);

    ex->extract(&result);

    const auto &allProperties = result.properties();

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

        newTrack.setAlbumName(albumValue);

        if (artistProperty != allProperties.end()) {
            newTrack.setArtist(artistProperty->toStringList().join(QStringLiteral(", ")));
        }

        if (durationProperty != allProperties.end()) {
            newTrack.setDuration(QTime::fromMSecsSinceStartOfDay(int(1000 * durationProperty->toDouble())));
        }

        if (titleProperty != allProperties.end()) {
            newTrack.setTitle(titleProperty->toString());
        }

        if (trackNumberProperty != allProperties.end()) {
            newTrack.setTrackNumber(trackNumberProperty->toInt());
        }

        if (discNumberProperty != allProperties.end()) {
            newTrack.setDiscNumber(discNumberProperty->toInt());
        } else {
            newTrack.setDiscNumber(1);
        }

        if (albumArtistProperty != allProperties.end()) {
            newTrack.setAlbumArtist(albumArtistProperty->toStringList().join(QStringLiteral(", ")));
        }

        if (yearProperty != allProperties.end()) {
            newTrack.setYear(yearProperty->toInt());
        }

        if (channelsProperty != allProperties.end()) {
            newTrack.setChannels(channelsProperty->toInt());
        }

        if (bitRateProperty != allProperties.end()) {
            newTrack.setBitRate(bitRateProperty->toInt());
        }

        if (sampleRateProperty != allProperties.end()) {
            newTrack.setSampleRate(sampleRateProperty->toInt());
        }

        if (genreProperty != allProperties.end()) {
            newTrack.setGenre(genreProperty->toStringList().join(QStringLiteral(", ")));
        }

        if (composerProperty != allProperties.end()) {
            newTrack.setComposer(composerProperty->toStringList().join(QStringLiteral(", ")));
        }

        if (lyricistProperty != allProperties.end()) {
            newTrack.setLyricist(lyricistProperty->toStringList().join(QStringLiteral(", ")));
        }

        if (commentProperty != allProperties.end()) {
            newTrack.setComment(commentProperty->toString());
        }

        if (newTrack.artist().isEmpty()) {
            newTrack.setArtist(newTrack.albumArtist());
        }

        newTrack.setResourceURI(scanFile);

#if defined Q_OS_LINUX && !defined Q_OS_ANDROID
        newTrack.setRating(fileData.rating());
#else
        newTrack.setRating(0);
#endif

        if (newTrack.title().isEmpty()) {
            return newTrack;
        }

        if (newTrack.artist().isEmpty()) {
            return newTrack;
        }

        if (newTrack.albumName().isEmpty()) {
            return newTrack;
        }

        if (!newTrack.duration().isValid()) {
            return newTrack;
        }

        newTrack.setValid(true);
    }

    return newTrack;
}


#include "moc_elisautils.cpp"
