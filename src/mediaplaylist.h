/*
 * Copyright 2015-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef MEDIAPLAYLIST_H
#define MEDIAPLAYLIST_H

#include "elisaLib_export.h"

#include "musicaudiotrack.h"
#include "musicalbum.h"
#include "elisautils.h"
#include "databaseinterface.h"

#include <QAbstractListModel>
#include <QVector>
#include <QMediaPlayer>

#include <utility>
#include <tuple>

class MediaPlayListPrivate;
class MusicListenersManager;
class MediaPlayListEntry;
class QDebug;

class ELISALIB_EXPORT MediaPlayList : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QVariantMap persistentState
               READ persistentState
               WRITE setPersistentState
               NOTIFY persistentStateChanged)

    Q_PROPERTY(MusicListenersManager* musicListenersManager
               READ musicListenersManager
               WRITE setMusicListenersManager
               NOTIFY musicListenersManagerChanged)

    Q_PROPERTY(int tracksCount
               READ tracksCount
               NOTIFY tracksCountChanged)

    Q_PROPERTY(QPersistentModelIndex currentTrack
               READ currentTrack
               NOTIFY currentTrackChanged)

    Q_PROPERTY(int currentTrackRow
               READ currentTrackRow
               NOTIFY currentTrackRowChanged)

    Q_PROPERTY(bool randomPlay
               READ randomPlay
               WRITE setRandomPlay
               NOTIFY randomPlayChanged)

    Q_PROPERTY(bool repeatPlay
               READ repeatPlay
               WRITE setRepeatPlay
               NOTIFY repeatPlayChanged)

public:

    using EntryData = std::tuple<qulonglong, QString>;

    enum PlayListEntryType {
        Album,
        Artist,
        Genre,
        Track,
        FileName,
        Unknown,
    };

    Q_ENUM(PlayListEntryType)

    enum ColumnsRoles {
        TitleRole = DatabaseInterface::TitleRole,
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
        ResourceRole,
        IdRole,
        DatabaseIdRole,
        IsSingleDiscAlbumRole,
        ContainerDataRole,
        IsPartialDataRole,
        AlbumIdRole,
        IsValidRole,
        TrackDataRole,
        CountRole,
        IsPlayingRole,
        HasAlbumHeader,
    };

    Q_ENUM(ColumnsRoles)

    enum PlayState {
        NotPlaying,
        IsPlaying,
        IsPaused,
    };

    Q_ENUM(PlayState)

    using ListTrackDataType = DatabaseInterface::ListTrackDataType;

    using TrackDataType = DatabaseInterface::TrackDataType;

    explicit MediaPlayList(QObject *parent = nullptr);

    ~MediaPlayList() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    Q_INVOKABLE bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;

    Q_INVOKABLE void move(int from, int to, int n);

    Q_INVOKABLE void clearPlayList();

    Q_INVOKABLE bool savePlaylist(const QUrl &fileName);

    QVariantMap persistentState() const;

    MusicListenersManager* musicListenersManager() const;

    int tracksCount() const;

    QPersistentModelIndex currentTrack() const;

    int currentTrackRow() const;

    bool randomPlay() const;

    bool repeatPlay() const;

Q_SIGNALS:

    void newTrackByNameInList(const QString &title, const QString &artist, const QString &album, int trackNumber, int discNumber);

    void newTrackByFileNameInList(const QUrl &fileName);

    void newArtistInList(const QString &artist);

    void newEntryInList(qulonglong newDatabaseId,
                        const QString &entryTitle,
                        PlayListEntryType databaseIdType);

    void trackHasBeenAdded(const QString &title, const QUrl &image);

    void persistentStateChanged();

    void musicListenersManagerChanged();

    void tracksCountChanged();

    void currentTrackChanged(QPersistentModelIndex currentTrack);

    void currentTrackRowChanged();

    void randomPlayChanged();

    void repeatPlayChanged();

    void playListFinished();

    void playListLoaded();

    void playListLoadFailed();

    void ensurePlay();

public Q_SLOTS:

    void setPersistentState(const QVariantMap &persistentState);

    void removeSelection(QList<int> selection);

    void albumAdded(const ListTrackDataType &tracks);

    void trackChanged(const TrackDataType &track);

    void trackRemoved(qulonglong trackId);

    void setMusicListenersManager(MusicListenersManager* musicListenersManager);

    void setRandomPlay(bool value);

    void setRepeatPlay(bool value);

    void skipNextTrack();

    void skipPreviousTrack();

    void seedRandomGenerator(uint seed);

    void switchTo(int row);

    void loadPlaylist(const QUrl &fileName);

    void enqueue(EntryData newEntry, PlayListEntryType databaseIdType);

    void enqueue(const QList<EntryData> &newEntries, PlayListEntryType databaseIdType);

    void enqueue(const TrackDataType &newTrack);

    void enqueue(const MediaPlayListEntry &newEntry, const TrackDataType &audioTrack = {});

    void enqueue(const MusicAlbum &album);

    void enqueue(const QList<MusicAlbum> &albums,
                 ElisaUtils::PlayListEnqueueMode enqueueMode,
                 ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

    void enqueue(const QList<MusicAudioTrack> &tracks,
                 ElisaUtils::PlayListEnqueueMode enqueueMode,
                 ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

    void enqueueArtists(const QList<QString> &artistName,
                        ElisaUtils::PlayListEnqueueMode enqueueMode,
                        ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

    void enqueue(const QList<QUrl> &tracks,
                 ElisaUtils::PlayListEnqueueMode enqueueMode,
                 ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

    void enqueueAndPlay(const QStringList &files);

    void replaceAndPlay(EntryData newEntry, PlayListEntryType databaseIdType);

    void replaceAndPlay(qulonglong newTrackId);

    void replaceAndPlay(const TrackDataType &newTrack);

    void replaceAndPlay(const MusicAlbum &album);

    void replaceAndPlay(const QString &artistName);

    void replaceAndPlay(const QUrl &fileName);

    void trackInError(const QUrl &sourceInError, QMediaPlayer::Error playerError);

private Q_SLOTS:

    void loadPlayListLoaded();

    void loadPlayListLoadFailed();

private:

    bool rowHasHeader(int row) const;

    void resetCurrentTrack();

    void notifyCurrentTrackChanged();

    void restorePlayListPosition();

    void restoreRandomPlay();

    void restoreRepeatPlay();

    void enqueue(const QString &artistName);

    void enqueue(const QUrl &fileName);

    void enqueueFilesList(const QList<EntryData> &newEntries);

    void enqueueTracksListById(const QList<EntryData> &newEntries);

    std::unique_ptr<MediaPlayListPrivate> d;

};

class MediaPlayListEntry
{
public:

    MediaPlayListEntry() = default;

    explicit MediaPlayListEntry(qulonglong id) : mId(id), mIsValid(true) {
    }

    MediaPlayListEntry(QString title, QString artist, QString album, int trackNumber, int discNumber)
        : mTitle(std::move(title)), mAlbum(std::move(album)), mArtist(std::move(artist)),
          mTrackNumber(trackNumber), mDiscNumber(discNumber) {
    }

    explicit MediaPlayListEntry(const MusicAudioTrack &track)
        : mTitle(track.title()), mAlbum(track.albumName()), mTrackNumber(track.trackNumber()),
          mDiscNumber(track.discNumber()), mId(track.databaseId()), mIsValid(true) {
    }

    explicit MediaPlayListEntry(const MediaPlayList::TrackDataType &track)
        : mTitle(track[DatabaseInterface::TitleRole]),
          mAlbum(track[DatabaseInterface::AlbumRole]),
          mTrackNumber(track[DatabaseInterface::TrackNumberRole]),
          mDiscNumber(track[DatabaseInterface::DiscNumberRole]),
          mId(track[DatabaseInterface::DatabaseIdRole].toULongLong()),
          mIsValid(true) {
    }

    explicit MediaPlayListEntry(QString artist) : mArtist(std::move(artist)), mEntryType(MediaPlayList::Artist) {
    }

    explicit MediaPlayListEntry(QUrl fileName) : mTrackUrl(std::move(fileName)) {
    }

    explicit MediaPlayListEntry(qulonglong id,
                                const QString &entryTitle,
                                MediaPlayList::PlayListEntryType type)
        : mTitle(entryTitle), mId(id), mIsValid(type == MediaPlayList::Track), mEntryType(type) {
    }

    QVariant mTitle;

    QVariant mAlbum;

    QVariant mArtist;

    QVariant mTrackUrl;

    QVariant mTrackNumber = -1;

    QVariant mDiscNumber = -1;

    qulonglong mId = 0;

    bool mIsValid = false;

    MediaPlayList::PlayListEntryType mEntryType = MediaPlayList::PlayListEntryType::Unknown;

    MediaPlayList::PlayState mIsPlaying = MediaPlayList::NotPlaying;

};

QDebug operator<<(QDebug stream, const MediaPlayListEntry &data);



#endif // MEDIAPLAYLIST_H
