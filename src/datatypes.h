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
#include <optional>
#include <QDebug>

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
        ElementTypeRole,
        LyricsRole,
        FullDataRole,
        IsDirectoryRole,
        IsPlayListRole,
        FilePathRole,
        HasChildrenRole,
        MultipleImageUrlsRole,
    };

    Q_ENUM(ColumnsRoles)

    using DataType = QMap<ColumnsRoles, QVariant>;

public:
    class LyricsData
    {
    public:
        LyricsData(const QString& lyrics = QString(), bool fromMetaData = true, const QString &filePath = QString())
            : m_fromMetaData(fromMetaData)
            , m_filePath(filePath)
            , m_lyrics(lyrics)
            {};
        const QString &lyrics() const {
            return m_lyrics;
        }
        bool fromMetaData() const {
            return m_fromMetaData;
        }
        /**
         * @brief filePath
         * @return  path to the file lyrics defined in, empty if this lyrics is from MetaData
         */
        const QString &filePath() const {
            return m_filePath;
        }

    private:
        bool m_fromMetaData;
        QString m_filePath;
        QString m_lyrics;
    };

    class TrackDataType
    {
    public:
        TrackDataType(bool aValid, QString aId, QString aParentId, QString aTitle, QString aArtist, QString aAlbumName,
                      QString aAlbumArtist, int aTrackNumber, int aDiscNumber, QTime aDuration, QUrl aResourceURI,
                      const QDateTime &fileModificationTime, QUrl aAlbumCover, int rating, bool aIsSingleDiscAlbum,
                      QString aGenre, QString aComposer, QString aLyricist, bool aHasEmbeddedCover)
            : m_id(aId)
            , m_parentId(aParentId)
            , m_title(aTitle)
            , m_artist(aArtist)
            , m_albumName(aAlbumName)
            , m_albumArtist(aAlbumArtist)
            , m_trackNumber(aTrackNumber)
            , m_discNumber(aDiscNumber)
            , m_duration(aDuration)
            , m_resourceURI(aResourceURI)
            , m_fileModificationTime(fileModificationTime)
            , m_albumCover(aAlbumCover)
            , m_rating(rating)
            , m_isSingleDiscAlbum(aIsSingleDiscAlbum)
            , m_genre(aGenre)
            , m_composer(aComposer)
            , m_lyricist(aLyricist)
            , m_hasEmbeddedCover(aHasEmbeddedCover)
        {
            Q_UNUSED(aValid)
        }
        TrackDataType() {};

        [[nodiscard]] bool isValid() const
        {
            return duration().isValid();
        }

        [[nodiscard]] QString title() const
        {
            return m_title;
        }

        void setTitle(const QString &title)
        {
            m_title = title;
        }

        [[nodiscard]] QString secondaryText() const
        {
            return m_secondaryText;
        }

        void setSecondaryText(const QString &text)
        {
            m_secondaryText = text;
        }

        [[nodiscard]] std::optional<QString> artist() const
        {
            return m_artist;
        }

        void setArtist(const QString &artist)
        {
            m_artist = artist;
        }

        [[nodiscard]] qulonglong albumId() const
        {
            return m_albumId;
        }

        void setAlbumId(qulonglong id)
        {
            m_albumId = id;
        }

        [[nodiscard]] std::optional<QString> album() const
        {
            return m_albumName;
        }

        void setAlbum(const QString &album)
        {
            m_albumName = album;
        }

        [[nodiscard]] std::optional<QString> albumArtist() const
        {
            return m_albumArtist;
        }

        void setAlbumArtist(const QString &artist)
        {
            m_albumArtist = artist;
        }

        [[nodiscard]] std::optional<int> trackNumber() const
        {
            return m_trackNumber;
        }

        void setTrackNumber(int number)
        {
            m_trackNumber = number;
        }

        [[nodiscard]] std::optional<int> discNumber() const
        {
            return m_discNumber;
        }

        void setDiscNumber(int number)
        {
            m_discNumber = number;
        }

        [[nodiscard]] QTime duration() const
        {
            return m_duration;
        }

        void setDuration(const QTime &time)
        {
            m_duration = time;
        }

        [[nodiscard]] QUrl resourceURI() const
        {
            return m_resourceURI;
        }

        void setResourceURI(const QUrl &url)
        {
            m_resourceURI = url;
        }

        [[nodiscard]] QUrl albumCover() const
        {
            return m_albumCover;
        }

        void setAlbumCover(const QUrl &cover)
        {
            m_albumCover = cover;
        }

        [[nodiscard]] bool isSingleDiscAlbum() const
        {
            return m_isSingleDiscAlbum;
        }

        void setIsSingleDiscAlbum(bool is)
        {
            m_isSingleDiscAlbum = is;
        }

        [[nodiscard]] int rating() const
        {
            return m_rating;
        }

        void setRating(int rating)
        {
            m_rating = rating;
        }

        [[nodiscard]] int playCounter() const
        {
            return m_playCounter;
        }

        void setPlayCounter(int count)
        {
            m_playCounter = count;
        }

        [[nodiscard]] std::optional<QString> genre() const
        {
            return m_genre;
        }

        void setGenre(const QString &genre)
        {
            m_genre = genre;
        }

        [[nodiscard]] std::optional<QString> composer() const
        {
            return m_composer;
        }

        void setComposer(const QString &composer)
        {
            m_composer = composer;
        }

        [[nodiscard]] std::optional<QString> lyricist() const
        {
            return m_lyricist;
        }

        void setLyricist(const QString &lyricist)
        {
            m_lyricist =  lyricist;
        }

        [[nodiscard]] std::optional<LyricsData> lyrics() const
        {
            return m_lyrics;
        }

        void setLyrics(const LyricsData &data)
        {
            m_lyrics = data;
        }

        [[nodiscard]] std::optional<QString> comment() const
        {
            return m_comment;
        }

        void setComment(const QString &comment)
        {
            m_comment = comment;
        }

        [[nodiscard]] std::optional<int> year() const
        {
            return m_year;
        }

        void setYear(int year)
        {
            m_year = year;
        }

        [[nodiscard]] std::optional<int> channels() const
        {
            return m_channels;
        }

        void setChannels(int channels)
        {
            m_channels = channels;
        }

        [[nodiscard]] std::optional<int> bitRate() const
        {
            return m_bitRate;
        }

        void setBitRate(int bitRate)
        {
            m_bitRate = bitRate;
        }

        [[nodiscard]] std::optional<int> sampleRate() const
        {
            return m_sampleRate;
        }

        void setSampleRate(int rate)
        {
            m_sampleRate = rate;
        }

        [[nodiscard]] bool hasEmbeddedCover() const
        {
            return m_hasEmbeddedCover;
        }

        void setHasEmbeddedCover(bool cover)
        {
            m_hasEmbeddedCover = cover;
        }

        [[nodiscard]] QDateTime fileModificationTime() const
        {
            return m_fileModificationTime;
        }

        void setFileModificationTime(const QDateTime &time)
        {
            m_fileModificationTime = time;
        }
    private:
        QString m_id;
        QString m_parentId;
        QString m_title;
        QString m_secondaryText;
        std::optional<QString> m_artist;
        std::optional<QString> m_albumName;
        std::optional<QString> m_albumArtist;
        qulonglong m_albumId;
        std::optional<int> m_trackNumber;
        std::optional<int> m_discNumber;
        std::optional<int> m_year;
        std::optional<int> m_channels;
        std::optional<int> m_bitRate;
        std::optional<int> m_sampleRate;
        QTime m_duration;
        QUrl m_resourceURI;
        QDateTime m_fileModificationTime;
        QUrl m_albumCover;
        int m_rating = 0;
        int m_playCounter = 0;
        bool m_isSingleDiscAlbum;
        std::optional<QString> m_genre;
        std::optional<QString> m_composer;
        std::optional<QString> m_lyricist;
        std::optional<LyricsData> m_lyrics;
        std::optional<QString> m_comment;
        bool m_hasEmbeddedCover;
    };

    using ListTrackDataType = QList<TrackDataType>;

    using ListRadioDataType = QList<TrackDataType>;

    class AlbumDataType
    {
    public:

        AlbumDataType() {};

        [[nodiscard]] QString title() const
        {
            return m_title;
        }

        [[nodiscard]] std::optional<QString> artist() const
        {
            return m_artist;
        }

        [[nodiscard]] bool isValidArtist() const
        {
            return m_artist.has_value() && !m_artist->isEmpty();
        }

        [[nodiscard]] QStringList genres() const
        {
            return m_genres;
        }

        [[nodiscard]] QUrl albumArtURI() const
        {
            return m_albumArtURI;
        }

        [[nodiscard]] bool isSingleDiscAlbum() const
        {
            return m_isSingleDiscAlbum;
        }

        [[nodiscard]] bool isValid() const
        {
            return !m_title.isEmpty();
        }
    private:
        QString m_title;
        std::optional<QString> m_artist;
        QStringList m_genres;
        QUrl m_albumArtURI;
        bool m_isSingleDiscAlbum;
    };

    using ListAlbumDataType = QList<AlbumDataType>;

    class ArtistDataType
    {
    public:
        ArtistDataType() {};
        [[nodiscard]] QString name() const
        {
            return m_name;
        }

        [[nodiscard]] qulonglong databaseId() const
        {
            return m_databaseId;
        }

       [[nodiscard]] QUrl artistArtURI() const
        {
            return m_artistArtURI;
        }
    private:
        QString m_name;
        qulonglong m_databaseId;
        QUrl m_artistArtURI;
    };

    using ListArtistDataType = QList<ArtistDataType>;

    class GenreDataType
    {
    public:
        GenreDataType() {};

        [[nodiscard]] QString title() const
        {
            return m_title;
        }
    private:
        QString m_title;
    };

    using ListGenreDataType = QList<GenreDataType>;

    class MusicDataType
    {
    public:
        [[nodiscard]] std::optional<qulonglong> databaseId() const
        {
            return m_databaseId;
        }

        [[nodiscard]] ElisaUtils::PlayListEntryType elementType() const
        {
            return m_elementType;
        }

        void setTrackData(const TrackDataType &data)
        {
            m_elementType = ElisaUtils::PlayListEntryType::Track;
            m_data.trackData = data;
        }

        void setAlbumData(const AlbumDataType &data)
        {
            m_elementType = ElisaUtils::PlayListEntryType::Album;
            m_data.albumData = data;
        }

        void setArtistData(const ArtistDataType &data)
        {
            m_elementType = ElisaUtils::PlayListEntryType::Artist;
            m_data.artistData = data;
        }

        void setGenreData(const GenreDataType &data)
        {
            m_elementType= ElisaUtils::PlayListEntryType::Genre;
            m_data.genreData = data;
        }

        TrackDataType &trackData()
        {
            assert(m_elementType == ElisaUtils::PlayListEntryType::Track);
            return m_data.trackData;
        }

        const TrackDataType &trackData() const
        {
            assert(m_elementType == ElisaUtils::PlayListEntryType::Track);
            return m_data.trackData;
        }

        AlbumDataType &albumData()
        {
            assert(m_elementType == ElisaUtils::PlayListEntryType::Album);
            return m_data.albumData;
        }

        const AlbumDataType &albumData() const
        {
            assert(m_elementType == ElisaUtils::PlayListEntryType::Album);
            return m_data.albumData;
        }

        ArtistDataType &artistData()
        {
            assert(m_elementType == ElisaUtils::PlayListEntryType::Artist);
            return m_data.artistData;
        }

        const ArtistDataType &artistData() const
        {
            assert(m_elementType == ElisaUtils::PlayListEntryType::Artist);
            return m_data.artistData;
        }

        GenreDataType &genreData()
        {
            assert(m_elementType == ElisaUtils::PlayListEntryType::Genre);
            return m_data.genreData;
        }

        const GenreDataType &genreData() const
        {
            assert(m_elementType == ElisaUtils::PlayListEntryType::Genre);
            return m_data.genreData;
        }

        [[nodiscard]] bool isValid() const
        {
            return m_databaseId.has_value() && m_elementType != ElisaUtils::PlayListEntryType::Unknown;
        }

    private:
        std::optional<qulonglong> m_databaseId;
        ElisaUtils::PlayListEntryType m_elementType = ElisaUtils::PlayListEntryType::Unknown;
        union data {
            DataTypes::TrackDataType trackData;
            DataTypes::AlbumDataType albumData;
            DataTypes::ArtistDataType artistData;
            DataTypes::GenreDataType genreData;
        };
        data m_data;
    };
    using EntryData = std::tuple<MusicDataType, QString, QUrl>;
    using EntryDataList = QList<EntryData>;
};
QDebug operator<<(QDebug debug, const DataTypes::MusicDataType &c);

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
Q_DECLARE_METATYPE(DataTypes::LyricsData);

#endif // DATATYPES_H
