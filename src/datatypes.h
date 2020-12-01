/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2019 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef DATATYPES_H
#define DATATYPES_H

#include "elisaLib_export.h"

#include "elisautils.h"

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
        ParentIdRole,
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
        FullDataRole,
        IsDirectoryRole,
        IsPlayListRole,
        FilePathRole,
        HasChildrenRole,
    };

    Q_ENUM(ColumnsRoles)

    using DataType = QMap<ColumnsRoles, QVariant>;

public:

    class MusicDataType : public DataType
    {
    public:
        using DataType::DataType;

        [[nodiscard]] bool hasDatabaseId() const
        {
            return find(key_type::DatabaseIdRole) != end();
        }

        [[nodiscard]] qulonglong databaseId() const
        {
            return operator[](key_type::DatabaseIdRole).toULongLong();
        }

        [[nodiscard]] bool hasElementType() const
        {
            return find(key_type::ElementTypeRole) != end();
        }

        [[nodiscard]] ElisaUtils::PlayListEntryType elementType() const
        {
            return operator[](key_type::ElementTypeRole).value<ElisaUtils::PlayListEntryType>();
        }
    };

    class TrackDataType : public MusicDataType
    {
    public:

        using MusicDataType::MusicDataType;

        TrackDataType(bool aValid, QString aId, QString aParentId, QString aTitle, QString aArtist, QString aAlbumName,
                      QString aAlbumArtist, int aTrackNumber, int aDiscNumber, QTime aDuration, QUrl aResourceURI,
                      const QDateTime &fileModificationTime, QUrl aAlbumCover, int rating, bool aIsSingleDiscAlbum,
                      QString aGenre, QString aComposer, QString aLyricist, bool aHasEmbeddedCover)
            : MusicDataType({{key_type::TitleRole, std::move(aTitle)}, {key_type::AlbumRole, std::move(aAlbumName)},
                             {key_type::ArtistRole, std::move(aArtist)}, {key_type::AlbumArtistRole, std::move(aAlbumArtist)},
                             {key_type::IdRole, std::move(aId)}, {key_type::ParentIdRole, std::move(aParentId)},
                             {key_type::TrackNumberRole, aTrackNumber}, {key_type::DiscNumberRole, aDiscNumber},
                             {key_type::DurationRole, aDuration}, {key_type::ResourceRole, std::move(aResourceURI)},
                             {key_type::FileModificationTime, fileModificationTime}, {key_type::ImageUrlRole, std::move(aAlbumCover)},
                             {key_type::RatingRole, rating}, {key_type::IsSingleDiscAlbumRole, aIsSingleDiscAlbum},
                             {key_type::GenreRole, std::move(aGenre)}, {key_type::ComposerRole, std::move(aComposer)},
                             {key_type::LyricistRole, std::move(aLyricist)}, {key_type::HasEmbeddedCover, aHasEmbeddedCover},
                             {key_type::ElementTypeRole, ElisaUtils::Track},})
        {
            Q_UNUSED(aValid)
        }

        [[nodiscard]] bool isValid() const
        {
            return !isEmpty() && duration().isValid();
        }

        [[nodiscard]] QString title() const
        {
            return operator[](key_type::TitleRole).toString();
        }

        [[nodiscard]] QString artist() const
        {
            return operator[](key_type::ArtistRole).toString();
        }

        [[nodiscard]] bool hasArtist() const
        {
            return find(key_type::ArtistRole) != end();
        }

        [[nodiscard]] qulonglong albumId() const
        {
            return operator[](key_type::AlbumIdRole).toULongLong();
        }

        [[nodiscard]] bool hasAlbum() const
        {
            return find(key_type::AlbumRole) != end();
        }

        [[nodiscard]] QString album() const
        {
            return operator[](key_type::AlbumRole).toString();
        }

        [[nodiscard]] QString albumArtist() const
        {
            return operator[](key_type::AlbumArtistRole).toString();
        }

        [[nodiscard]] bool hasAlbumArtist() const
        {
            return find(key_type::AlbumArtistRole) != end();
        }

        [[nodiscard]] bool hasTrackNumber() const
        {
            return find(key_type::TrackNumberRole) != end();
        }

        [[nodiscard]] int trackNumber() const
        {
            return operator[](key_type::TrackNumberRole).toInt();
        }

        [[nodiscard]] bool hasDiscNumber() const
        {
            return find(key_type::DiscNumberRole) != end();
        }

        [[nodiscard]] int discNumber() const
        {
            return operator[](key_type::DiscNumberRole).toInt();
        }

        [[nodiscard]] QTime duration() const
        {
            return operator[](key_type::DurationRole).toTime();
        }

        [[nodiscard]] QUrl resourceURI() const
        {
            return operator[](key_type::ResourceRole).toUrl();
        }

        [[nodiscard]] QUrl albumCover() const
        {
            return operator[](key_type::ImageUrlRole).toUrl();
        }

        [[nodiscard]] bool isSingleDiscAlbum() const
        {
            return operator[](key_type::IsSingleDiscAlbumRole).toBool();
        }

        [[nodiscard]] int rating() const
        {
            return operator[](key_type::RatingRole).toInt();
        }

        [[nodiscard]] QString genre() const
        {
            return operator[](key_type::GenreRole).toString();
        }

        [[nodiscard]] bool hasGenre() const
        {
            return find(key_type::GenreRole) != end();
        }

        [[nodiscard]] QString composer() const
        {
            return operator[](key_type::ComposerRole).toString();
        }

        [[nodiscard]] bool hasComposer() const
        {
            return find(key_type::ComposerRole) != end();
        }

        [[nodiscard]] QString lyricist() const
        {
            return operator[](key_type::LyricistRole).toString();
        }

        [[nodiscard]] bool hasLyricist() const
        {
            return find(key_type::LyricistRole) != end();
        }

        [[nodiscard]] QString lyrics() const
        {
            return operator[](key_type::LyricsRole).toString();
        }

        [[nodiscard]] bool hasLyrics() const
        {
            return find(key_type::LyricsRole) != end();
        }

        [[nodiscard]] QString comment() const
        {
            return operator[](key_type::CommentRole).toString();
        }

        [[nodiscard]] bool hasComment() const
        {
            return find(key_type::CommentRole) != end();
        }

        [[nodiscard]] int year() const
        {
            return operator[](key_type::YearRole).toInt();
        }

        [[nodiscard]] bool hasYear() const
        {
            return find(key_type::YearRole) != end();
        }

        [[nodiscard]] int channels() const
        {
            return operator[](key_type::ChannelsRole).toInt();
        }

        [[nodiscard]] bool hasChannels() const
        {
            return find(key_type::ChannelsRole) != end();
        }

        [[nodiscard]] int bitRate() const
        {
            return operator[](key_type::BitRateRole).toInt();
        }

        [[nodiscard]] bool hasBitRate() const
        {
            return find(key_type::BitRateRole) != end();
        }

        [[nodiscard]] int sampleRate() const
        {
            return operator[](key_type::SampleRateRole).toInt();
        }

        [[nodiscard]] bool hasSampleRate() const
        {
            return find(key_type::SampleRateRole) != end();
        }


        [[nodiscard]] bool hasEmbeddedCover() const
        {
            return operator[](key_type::HasEmbeddedCover).toBool();
        }

        [[nodiscard]] QDateTime fileModificationTime() const
        {
            return operator[](key_type::FileModificationTime).toDateTime();
        }
    };

    using ListTrackDataType = QList<TrackDataType>;

    using ListRadioDataType = QList<TrackDataType>;

    class AlbumDataType : public MusicDataType
    {
    public:

        using MusicDataType::MusicDataType;

        [[nodiscard]] QString title() const
        {
            return operator[](key_type::TitleRole).toString();
        }

        [[nodiscard]] QString artist() const
        {
            return operator[](key_type::ArtistRole).toString();
        }

        [[nodiscard]] bool isValidArtist() const
        {
            const auto &artistData = operator[](key_type::ArtistRole);
            return artistData.isValid() && !artistData.toString().isEmpty();
        }

        [[nodiscard]] QStringList genres() const
        {
            return operator[](key_type::GenreRole).toStringList();
        }

        [[nodiscard]] QUrl albumArtURI() const
        {
            return operator[](key_type::ImageUrlRole).toUrl();
        }

        [[nodiscard]] bool isSingleDiscAlbum() const
        {
            return operator[](key_type::IsSingleDiscAlbumRole).toBool();
        }

        [[nodiscard]] bool isValid() const
        {
            return !isEmpty();
        }

    };

    using ListAlbumDataType = QList<AlbumDataType>;

    class ArtistDataType : public MusicDataType
    {
    public:

        using MusicDataType::MusicDataType;

        [[nodiscard]] QString name() const
        {
            return operator[](key_type::TitleRole).toString();
        }

        [[nodiscard]] qulonglong databaseId() const
        {
            return operator[](key_type::DatabaseIdRole).toULongLong();
        }

    };

    using ListArtistDataType = QList<ArtistDataType>;

    class GenreDataType : public MusicDataType
    {
    public:

        using MusicDataType::MusicDataType;

        [[nodiscard]] QString title() const
        {
            return operator[](key_type::TitleRole).toString();
        }

    };

    using ListGenreDataType = QList<GenreDataType>;

    using EntryData = std::tuple<MusicDataType, QString, QUrl>;
    using EntryDataList = QList<EntryData>;

};

Q_DECLARE_METATYPE(DataTypes::MusicDataType)
Q_DECLARE_METATYPE(DataTypes::TrackDataType)
Q_DECLARE_METATYPE(DataTypes::AlbumDataType)
Q_DECLARE_METATYPE(DataTypes::ArtistDataType)
Q_DECLARE_METATYPE(DataTypes::GenreDataType)

Q_DECLARE_METATYPE(DataTypes::ListTrackDataType)
Q_DECLARE_METATYPE(DataTypes::ListAlbumDataType)
Q_DECLARE_METATYPE(DataTypes::ListArtistDataType)
Q_DECLARE_METATYPE(DataTypes::ListGenreDataType)

Q_DECLARE_METATYPE(DataTypes::EntryData)
Q_DECLARE_METATYPE(DataTypes::EntryDataList)

#endif // DATATYPES_H
