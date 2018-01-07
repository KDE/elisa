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

MusicAudioTrack ElisaUtils::scanOneFile(const QUrl &scanFile, const QMimeDatabase &mimeDatabase,
                                        const KFileMetaData::ExtractorCollection &allExtractors)
{
    MusicAudioTrack newTrack;

    const auto &fileMimeType = mimeDatabase.mimeTypeForFile(scanFile.toLocalFile());
    if (!fileMimeType.name().startsWith(QStringLiteral("audio/"))) {
        return newTrack;
    }

    QString mimetype = fileMimeType.name();

    QList<KFileMetaData::Extractor*> exList = allExtractors.fetchExtractors(mimetype);

    if (exList.isEmpty()) {
        return newTrack;
    }

    KFileMetaData::Extractor* ex = exList.first();
    KFileMetaData::SimpleExtractionResult result(scanFile.toLocalFile(), mimetype,
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
#if defined Q_OS_LINUX && !defined Q_OS_ANDROID
    auto fileData = KFileMetaData::UserMetaData(scanFile.toLocalFile());
#endif

    if (albumProperty != allProperties.end()) {
        auto albumValue = albumProperty->toString();

        newTrack.setAlbumName(albumValue);

        if (artistProperty != allProperties.end()) {
            newTrack.setArtist(artistProperty->toString());
        }

        if (durationProperty != allProperties.end()) {
            newTrack.setDuration(QTime::fromMSecsSinceStartOfDay(1000 * durationProperty->toDouble()));
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
            if (albumArtistProperty->canConvert<QString>()) {
                newTrack.setAlbumArtist(albumArtistProperty->toString());
            } else if (albumArtistProperty->canConvert<QStringList>()) {
                newTrack.setAlbumArtist(albumArtistProperty->toStringList().join(QStringLiteral(", ")));
            }
        }

        if (newTrack.artist().isEmpty()) {
            newTrack.setArtist(newTrack.albumArtist());
        }

        newTrack.setResourceURI(scanFile);

#if defined Q_OS_LINUX && !defined Q_OS_ANDROID
        newTrack.setRating(fileData.rating());
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
