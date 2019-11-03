/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * Copyright 2019 Alexander Stippich <a.stippich@gmx.net>
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

#ifndef DATATYPES_H
#define DATATYPES_H

#include "elisaLib_export.h"

#include <QObject>
#include <QString>
#include <QList>
#include <QVariant>
#include <QUrl>
#include <QDateTime>
#include <QMap>

class ELISALIB_EXPORT DataTypes : public QObject
{
    Q_OBJECT

public:

    enum ColumnsRoles {
        TitleRole = Qt::UserRole + 1,
        SecondaryTextRole,
        ImageUrlRole,
        ShadowForImageRole,
        ChildModelRole,
        DurationRole,
        StringDurationRole,
        ArtistRole,
        AllArtistsRole,
        HighestTrackRating,
        AlbumRole,
        AlbumArtistRole,
        IsValidAlbumArtistRole,
        TrackNumberRole,
        DiscNumberRole,
        RatingRole,
        GenreRole,
        LyricistRole,
        ComposerRole,
        CommentRole,
        YearRole,
        ChannelsRole,
        BitRateRole,
        SampleRateRole,
        ResourceRole,
        IdRole,
        DatabaseIdRole,
        IsSingleDiscAlbumRole,
        ContainerDataRole,
        IsPartialDataRole,
        AlbumIdRole,
        HasEmbeddedCover,
        FileModificationTime,
        FirstPlayDate,
        LastPlayDate,
        PlayCounter,
        PlayFrequency,
        ElementTypeRole,
        LyricsRole,
    };

    Q_ENUM(ColumnsRoles)

private:

    using DataType = QMap<ColumnsRoles, QVariant>;

public:

    class TrackDataType : public DataType
    {
    public:

        using DataType::DataType;

        bool isValid() const
        {
            return !isEmpty() && duration().isValid();
        }

        qulonglong databaseId() const
        {
            return operator[](key_type::DatabaseIdRole).toULongLong();
        }

        QString title() const
        {
            return operator[](key_type::TitleRole).toString();
        }

        QString artist() const
        {
            return operator[](key_type::ArtistRole).toString();
        }

        qulonglong albumId() const
        {
            return operator[](key_type::AlbumIdRole).toULongLong();
        }

        bool hasAlbum() const
        {
            return find(key_type::AlbumRole) != end();
        }

        QString album() const
        {
            return operator[](key_type::AlbumRole).toString();
        }

        QString albumArtist() const
        {
            return operator[](key_type::AlbumArtistRole).toString();
        }

        bool hasAlbumArtist() const
        {
            return find(key_type::AlbumArtistRole) != end();
        }

        bool hasTrackNumber() const
        {
            return find(key_type::TrackNumberRole) != end();
        }

        int trackNumber() const
        {
            return operator[](key_type::TrackNumberRole).toInt();
        }

        bool hasDiscNumber() const
        {
            return find(key_type::DiscNumberRole) != end();
        }

        int discNumber() const
        {
            return operator[](key_type::DiscNumberRole).toInt();
        }

        QTime duration() const
        {
            return operator[](key_type::DurationRole).toTime();
        }

        QUrl resourceURI() const
        {
            return operator[](key_type::ResourceRole).toUrl();
        }

        QUrl albumCover() const
        {
            return operator[](key_type::ImageUrlRole).toUrl();
        }

        bool isSingleDiscAlbum() const
        {
            return operator[](key_type::IsSingleDiscAlbumRole).toBool();
        }

        int rating() const
        {
            return operator[](key_type::RatingRole).toInt();
        }

        QString genre() const
        {
            return operator[](key_type::GenreRole).toString();
        }

        QString composer() const
        {
            return operator[](key_type::ComposerRole).toString();
        }

        QString lyricist() const
        {
            return operator[](key_type::LyricistRole).toString();
        }

        QString lyrics() const
        {
            return operator[](key_type::LyricsRole).toString();
        }

        QString comment() const
        {
            return operator[](key_type::CommentRole).toString();
        }

        int year() const
        {
            return operator[](key_type::YearRole).toInt();
        }

        int channels() const
        {
            return operator[](key_type::ChannelsRole).toInt();
        }

        bool hasChannels() const
        {
            return find(key_type::ChannelsRole) != end();
        }

        int bitRate() const
        {
            return operator[](key_type::BitRateRole).toInt();
        }

        bool hasBitRate() const
        {
            return find(key_type::BitRateRole) != end();
        }

        int sampleRate() const
        {
            return operator[](key_type::SampleRateRole).toInt();
        }

        bool hasSampleRate() const
        {
            return find(key_type::SampleRateRole) != end();
        }


        bool hasEmbeddedCover() const
        {
            return operator[](key_type::HasEmbeddedCover).toBool();
        }

        QDateTime fileModificationTime() const
        {
            return operator[](key_type::FileModificationTime).toDateTime();
        }
    };

    using ListTrackDataType = QList<TrackDataType>;

    using ListRadioDataType = QList<TrackDataType>;

    class AlbumDataType : public DataType
    {
    public:

        using DataType::DataType;

        qulonglong databaseId() const
        {
            return operator[](key_type::DatabaseIdRole).toULongLong();
        }

        QString title() const
        {
            return operator[](key_type::TitleRole).toString();
        }

        QString artist() const
        {
            return operator[](key_type::ArtistRole).toString();
        }

        bool isValidArtist() const
        {
            const auto &artistData = operator[](key_type::ArtistRole);
            return artistData.isValid() && !artistData.toString().isEmpty();
        }

        QStringList genres() const
        {
            return operator[](key_type::GenreRole).toStringList();
        }

        QUrl albumArtURI() const
        {
            return operator[](key_type::ImageUrlRole).toUrl();
        }

        bool isSingleDiscAlbum() const
        {
            return operator[](key_type::IsSingleDiscAlbumRole).toBool();
        }

        bool isValid() const
        {
            return !isEmpty();
        }

    };

    using ListAlbumDataType = QList<AlbumDataType>;

    class ArtistDataType : public DataType
    {
    public:

        using DataType::DataType;

        qulonglong databaseId() const
        {
            return operator[](key_type::DatabaseIdRole).toULongLong();
        }

    };

    using ListArtistDataType = QList<ArtistDataType>;

    class GenreDataType : public DataType
    {
    public:

        using DataType::DataType;

        qulonglong databaseId() const
        {
            return operator[](key_type::DatabaseIdRole).toULongLong();
        }

        QString title() const
        {
            return operator[](key_type::TitleRole).toString();
        }

    };

    using ListGenreDataType = QList<GenreDataType>;

};

Q_DECLARE_METATYPE(DataTypes::TrackDataType)
Q_DECLARE_METATYPE(DataTypes::AlbumDataType)
Q_DECLARE_METATYPE(DataTypes::ArtistDataType)
Q_DECLARE_METATYPE(DataTypes::GenreDataType)

Q_DECLARE_METATYPE(DataTypes::ListTrackDataType)
Q_DECLARE_METATYPE(DataTypes::ListAlbumDataType)
Q_DECLARE_METATYPE(DataTypes::ListArtistDataType)
Q_DECLARE_METATYPE(DataTypes::ListGenreDataType)

#endif // DATABASEINTERFACE_H
