/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef DATABASEINTERFACE_H
#define DATABASEINTERFACE_H

#include "elisaLib_export.h"

#include "datatype.h"
#include "elisautils.h"

#include <QObject>
#include <QString>
#include <QHash>
#include <QList>
#include <QVariant>
#include <QUrl>
#include <QDateTime>
#include <QPair>

#include <memory>

class DatabaseInterfacePrivate;
class QMutex;
class QSqlRecord;
class QSqlQuery;
class MusicAudioTrack;

class ELISALIB_EXPORT DatabaseInterface : public QObject
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
        MilliSecondsDurationRole,
        ArtistRole,
        AllArtistsRole,
        HighestTrackRating,
        AlbumRole,
        AlbumArtistRole,
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
            return !isEmpty();
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

        QString album() const
        {
            return operator[](key_type::AlbumRole).toString();
        }

        QString albumArtist() const
        {
            return operator[](key_type::AlbumArtistRole).toString();
        }

        int trackNumber() const
        {
            return operator[](key_type::TrackNumberRole).toInt();
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

    };

    using ListGenreDataType = QList<GenreDataType>;

    enum PropertyType {
        DatabaseId,
        DisplayRole,
        SecondaryRole,
    };

    Q_ENUM(PropertyType)

    enum AlbumDiscsCount {
        SingleDiscAlbum,
        MultipleDiscsAlbum,
    };

    Q_ENUM(AlbumDiscsCount)

    explicit DatabaseInterface(QObject *parent = nullptr);

    ~DatabaseInterface() override;

    Q_INVOKABLE void init(const QString &dbName, const QString &databaseFileName = {});

    qulonglong albumIdFromTitleAndArtist(const QString &title, const QString &artist);

    ListTrackDataType allTracksData();

    ListTrackDataType recentlyPlayedTracksData(int count);

    ListTrackDataType frequentlyPlayedTracksData(int count);

    ListAlbumDataType allAlbumsData();

    ListAlbumDataType allAlbumsDataByGenreAndArtist(const QString &genre, const QString &artist);

    ListAlbumDataType allAlbumsDataByArtist(const QString &artist);

    AlbumDataType albumDataFromDatabaseId(qulonglong id);

    ListTrackDataType albumData(qulonglong databaseId);

    ListArtistDataType allArtistsData();

    ListArtistDataType allArtistsDataByGenre(const QString &genre);

    ListGenreDataType allGenresData();

    DataType oneData(ElisaUtils::PlayListEntryType aType, qulonglong databaseId);

    ListTrackDataType tracksDataFromAuthor(const QString &artistName);

    TrackDataType trackDataFromDatabaseId(qulonglong id);

    qulonglong trackIdFromTitleAlbumTrackDiscNumber(const QString &title, const QString &artist, const QString &album,
                                                    int trackNumber, int discNumber);

    qulonglong trackIdFromFileName(const QUrl &fileName);

    void applicationAboutToQuit();

Q_SIGNALS:

    void artistsAdded(const DatabaseInterface::ListArtistDataType &newArtists);

    void composersAdded(const DatabaseInterface::ListArtistDataType &newComposers);

    void lyricistsAdded(const DatabaseInterface::ListArtistDataType &newLyricists);

    void albumsAdded(const DatabaseInterface::ListAlbumDataType &newAlbums);

    void tracksAdded(const DatabaseInterface::ListTrackDataType &allTracks);

    void genresAdded(const DatabaseInterface::ListGenreDataType &allGenres);

    void artistRemoved(qulonglong removedArtistId);

    void albumRemoved(qulonglong removedAlbumId);

    void trackRemoved(qulonglong id);

    void albumModified(const DatabaseInterface::AlbumDataType &modifiedAlbum, qulonglong modifiedAlbumId);

    void trackModified(const DatabaseInterface::TrackDataType &modifiedTrack);

    void requestsInitDone();

    void databaseError();

    void restoredTracks(const QString &musicSource, QHash<QUrl, QDateTime> allFiles);

public Q_SLOTS:

    void insertTracksList(const QList<MusicAudioTrack> &tracks, const QHash<QString, QUrl> &covers, const QString &musicSource);

    void removeTracksList(const QList<QUrl> &removedTracks);

    void removeAllTracksFromSource(const QString &sourceName);

    void askRestoredTracks(const QString &musicSource);

    void trackHasStartedPlaying(const QUrl &fileName, const QDateTime &time);

private:

    enum class TrackFileInsertType {
        NewTrackFileInsert,
        ModifiedTrackFileInsert,
    };

    void initChangesTrackers();

    void recordModifiedTrack(qulonglong trackId);

    void recordModifiedAlbum(qulonglong albumId);

    bool startTransaction() const;

    bool finishTransaction() const;

    bool rollBackTransaction() const;

    QList<qulonglong> fetchTrackIds(qulonglong albumId);

    qulonglong internalAlbumIdFromTitleAndArtist(const QString &title, const QString &artist);

    MusicAudioTrack internalTrackFromDatabaseId(qulonglong id);

    qulonglong internalTrackIdFromTitleAlbumTracDiscNumber(const QString &title, const QString &artist, const QString &album,
                                                           int trackNumber, int discNumber);

    qulonglong getDuplicateTrackIdFromTitleAlbumTrackDiscNumber(const QString &title, const QString &trackArtist, const QString &album,
                                                                const QString &albumArtist, const QString &trackPath, int priority);

    qulonglong internalTrackIdFromFileName(const QUrl &fileName);

    ListTrackDataType internalTracksFromAuthor(const QString &artistName);

    QList<qulonglong> internalAlbumIdsFromAuthor(const QString &artistName);

    void initDatabase();

    void initRequest();

    qulonglong insertAlbum(const QString &title, const QString &albumArtist, const QString &trackArtist,
                           const QString &trackPath, const QUrl &albumArtURI);

    bool updateAlbumFromId(qulonglong albumId, const QUrl &albumArtUri,
                           const MusicAudioTrack &currentTrack, const QString &albumPath);

    qulonglong insertArtist(const QString &name);

    qulonglong internalArtistIdFromName(const QString &name);

    qulonglong insertGenre(const QString &name);

    void removeTrackInDatabase(qulonglong trackId);

    void updateTrackInDatabase(const MusicAudioTrack &oneTrack, const QString &albumPath);

    void removeAlbumInDatabase(qulonglong albumId);

    void removeArtistInDatabase(qulonglong artistId);

    void reloadExistingDatabase();

    qulonglong initialId(DataUtils::DataType aType);

    qulonglong genericInitialId(QSqlQuery &request);

    qulonglong insertMusicSource(const QString &name);

    void insertTrackOrigin(const QUrl &fileNameURI, const QDateTime &fileModifiedTime, const QDateTime &importDate, qulonglong discoverId);

    void updateTrackOrigin(const QUrl &fileName, const QDateTime &fileModifiedTime);

    int computeTrackPriority(const QString &title, const QString &trackArtist, const QString &album, const QString &albumArtist, const QString &trackPath);

    qulonglong internalInsertTrack(const MusicAudioTrack &oneModifiedTrack, const QHash<QString, QUrl> &covers);

    MusicAudioTrack buildTrackFromDatabaseRecord(const QSqlRecord &trackRecord) const;

    TrackDataType buildTrackDataFromDatabaseRecord(const QSqlRecord &trackRecord) const;

    void internalRemoveTracksList(const QList<QUrl> &removedTracks);

    void internalRemoveTracksList(const QHash<QUrl, QDateTime> &removedTracks, qulonglong sourceId);

    QUrl internalAlbumArtUriFromAlbumId(qulonglong albumId);

    bool isValidArtist(qulonglong albumId);

    qulonglong insertComposer(const QString &name);

    qulonglong insertLyricist(const QString &name);

    qulonglong internalSourceIdFromName(const QString &sourceName);

    QHash<QUrl, QDateTime> internalAllFileNameFromSource(qulonglong sourceId);

    bool internalGenericPartialData(QSqlQuery &query);

    ListArtistDataType internalAllArtistsPartialData(QSqlQuery &artistsQuery);

    ArtistDataType internalOneArtistPartialData(qulonglong databaseId);

    ListAlbumDataType internalAllAlbumsPartialData(QSqlQuery &query);

    AlbumDataType internalOneAlbumPartialData(qulonglong databaseId);

    ListTrackDataType internalAllTracksPartialData();

    ListTrackDataType internalRecentlyPlayedTracksData(int count);

    ListTrackDataType internalFrequentlyPlayedTracksData(int count);

    TrackDataType internalOneTrackPartialData(qulonglong databaseId);

    ListGenreDataType internalAllGenresPartialData();

    GenreDataType internalOneGenrePartialData(qulonglong databaseId);

    ListArtistDataType internalAllComposersPartialData();

    ArtistDataType internalOneComposerPartialData(qulonglong databaseId);

    ListArtistDataType internalAllLyricistsPartialData();

    ArtistDataType internalOneLyricistPartialData(qulonglong databaseId);

    bool prepareQuery(QSqlQuery &query, const QString &queryText) const;

    void updateAlbumArtist(qulonglong albumId, const QString &title, const QString &albumPath,
                           const QString &artistName);

    void updateTrackStatistics(const QUrl &fileName, const QDateTime &time);

    void createDatabaseV9();

    void upgradeDatabaseV11();

    std::unique_ptr<DatabaseInterfacePrivate> d;

};

Q_DECLARE_METATYPE(DatabaseInterface::TrackDataType)
Q_DECLARE_METATYPE(DatabaseInterface::AlbumDataType)
Q_DECLARE_METATYPE(DatabaseInterface::ArtistDataType)
Q_DECLARE_METATYPE(DatabaseInterface::GenreDataType)

Q_DECLARE_METATYPE(DatabaseInterface::ListTrackDataType)
Q_DECLARE_METATYPE(DatabaseInterface::ListAlbumDataType)
Q_DECLARE_METATYPE(DatabaseInterface::ListArtistDataType)
Q_DECLARE_METATYPE(DatabaseInterface::ListGenreDataType)

#endif // DATABASEINTERFACE_H
