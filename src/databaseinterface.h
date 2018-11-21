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
#include "musicalbum.h"
#include "musicaudiotrack.h"
#include "musicaudiogenre.h"

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
        ImageRole,
        ResourceRole,
        IdRole,
        DatabaseIdRole,
        IsSingleDiscAlbumRole,
        ContainerDataRole,
        IsPartialDataRole,
    };

    Q_ENUM(ColumnsRoles)

    using DataType = QMap<ColumnsRoles, QVariant>;

    using DataListType = QList<DataType>;

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

    MusicAlbum albumFromTitleAndArtist(const QString &title, const QString &artist);

    DataListType allData(DataUtils::DataType aType);

    QList<MusicAudioTrack> allTracks();

    QList<MusicAudioTrack> allTracksFromSource(const QString &musicSource);

    QList<MusicAlbum> allAlbums();

    QList<MusicAudioGenre> allGenres();

    QList<MusicAudioTrack> tracksFromAuthor(const QString &artistName);

    MusicAudioTrack trackFromDatabaseId(qulonglong id);

    qulonglong trackIdFromTitleAlbumTrackDiscNumber(const QString &title, const QString &artist, const QString &album,
                                                    int trackNumber, int discNumber);

    qulonglong trackIdFromFileName(const QUrl &fileName);

    void applicationAboutToQuit();

Q_SIGNALS:

    void artistsAdded(const DataListType &newArtists);

    void composersAdded(const DataListType &newComposers);

    void lyricistsAdded(const DataListType &newLyricists);

    void albumsAdded(const QList<MusicAlbum> &newAlbum);

    void tracksAdded(const QList<MusicAudioTrack> &allTracks);

    void genresAdded(const QList<MusicAudioGenre> &allGenres);

    void artistRemoved(qulonglong removedArtistId);

    void albumRemoved(const MusicAlbum &removedAlbum, qulonglong removedAlbumId);

    void trackRemoved(qulonglong id);

    void albumModified(const MusicAlbum &modifiedAlbum, qulonglong modifiedAlbumId);

    void trackModified(const MusicAudioTrack &modifiedTrack);

    void sentAlbumData(const MusicAlbum albumData);

    void requestsInitDone();

    void databaseError();

    void restoredTracks(const QString &musicSource, QHash<QUrl, QDateTime> allFiles);

public Q_SLOTS:

    void insertTracksList(const QList<MusicAudioTrack> &tracks, const QHash<QString, QUrl> &covers, const QString &musicSource);

    void removeTracksList(const QList<QUrl> &removedTracks);

    void modifyTracksList(const QList<MusicAudioTrack> &modifiedTracks, const QHash<QString, QUrl> &covers, const QString &musicSource);

    void removeAllTracksFromSource(const QString &sourceName);

    void getAlbumFromAlbumId(qulonglong id);

    void askRestoredTracks(const QString &musicSource);

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

    QList<MusicAudioTrack> fetchTracks(qulonglong albumId);

    QList<qulonglong> fetchTrackIds(qulonglong albumId);

    MusicAlbum internalAlbumFromId(qulonglong albumId);

    MusicAlbum internalAlbumFromTitleAndArtist(const QString &title, const QString &artist);

    qulonglong internalAlbumIdFromTitleAndArtist(const QString &title, const QString &artist);

    MusicAudioTrack internalTrackFromDatabaseId(qulonglong id);

    qulonglong internalTrackIdFromTitleAlbumTracDiscNumber(const QString &title, const QString &artist, const QString &album,
                                                           int trackNumber, int discNumber);

    qulonglong getDuplicateTrackIdFromTitleAlbumTrackDiscNumber(const QString &title, const QString &album,
                                                                const QString &albumArtist, const QString &trackPath,
                                                                int trackNumber, int discNumber);

    qulonglong internalTrackIdFromFileName(const QUrl &fileName);

    QList<MusicAudioTrack> internalTracksFromAuthor(const QString &artistName);

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

    MusicAudioGenre internalGenreFromId(qulonglong genreId);

    void removeTrackInDatabase(qulonglong trackId);

    void updateTrackInDatabase(const MusicAudioTrack &oneTrack, const QString &albumPath);

    void removeAlbumInDatabase(qulonglong albumId);

    void removeArtistInDatabase(qulonglong artistId);

    void reloadExistingDatabase();

    qulonglong initialId(DataUtils::DataType aType);

    qulonglong genericInitialId(QSqlQuery &request);

    qulonglong insertMusicSource(const QString &name);

    void insertTrackOrigin(const QUrl &fileNameURI, const QDateTime &fileModifiedTime, qulonglong discoverId);

    void updateTrackOrigin(qulonglong trackId, const QUrl &fileName, const QDateTime &fileModifiedTime);

    int computeTrackPriority(qulonglong trackId, const QUrl &fileName);

    qulonglong internalInsertTrack(const MusicAudioTrack &oneModifiedTrack, const QHash<QString, QUrl> &covers,
                                   qulonglong originTrackId, TrackFileInsertType insertType);

    MusicAudioTrack buildTrackFromDatabaseRecord(const QSqlRecord &trackRecord) const;

    void internalRemoveTracksList(const QList<QUrl> &removedTracks);

    void internalRemoveTracksList(const QHash<QUrl, QDateTime> &removedTracks, qulonglong sourceId);

    void internalRemoveTracksWithoutMapping();

    QUrl internalAlbumArtUriFromAlbumId(qulonglong albumId);

    bool isValidArtist(qulonglong albumId);

    qulonglong insertComposer(const QString &name);

    qulonglong insertLyricist(const QString &name);

    qulonglong internalSourceIdFromName(const QString &sourceName);

    QHash<QUrl, QDateTime> internalAllFileNameFromSource(qulonglong sourceId);

    bool internalAllGenericPartialData(QSqlQuery &query);

    DataListType internalAllArtistsPartialData();

    DataListType internalAllAlbumsPartialData();

    DataListType internalAllTracksPartialData();

    DataListType internalAllGenresPartialData();

    DataListType internalAllComposersPartialData();

    DataListType internalAllLyricistsPartialData();

    bool prepareQuery(QSqlQuery &query, const QString &queryText) const;

    void updateAlbumArtist(qulonglong albumId, const QString &title, const QString &albumPath,
                           const QString &artistName);

    std::unique_ptr<DatabaseInterfacePrivate> d;

};



#endif // DATABASEINTERFACE_H
