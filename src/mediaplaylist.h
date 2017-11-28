/*
 * Copyright 2015-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef MEDIAPLAYLIST_H
#define MEDIAPLAYLIST_H

#include "musicaudiotrack.h"
#include "musicalbum.h"

#include <QAbstractListModel>
#include <QVector>
#include <utility>

class MediaPlayListPrivate;
class DatabaseInterface;
class MusicListenersManager;
class MediaPlayListEntry;

class MediaPlayList : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QList<QVariant> persistentState
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

public:

    enum ColumnsRoles {
        IsValidRole = Qt::UserRole + 1,
        TitleRole,
        DurationRole,
        MilliSecondsDurationRole,
        CreatorRole,
        ArtistRole,
        AlbumArtistRole,
        AlbumRole,
        TrackNumberRole,
        DiscNumberRole,
        RatingRole,
        ImageRole,
        ResourceRole,
        CountRole,
        IsPlayingRole,
        HasAlbumHeader,
        IsSingleDiscAlbumHeader,
    };

    Q_ENUM(ColumnsRoles)

    enum PlayState {
        NotPlaying,
        IsPlaying,
        IsPaused,
    };

    Q_ENUM(PlayState)

    explicit MediaPlayList(QObject *parent = nullptr);

    ~MediaPlayList() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    Q_INVOKABLE void enqueue(qulonglong newTrackId);

    Q_INVOKABLE void enqueue(const MusicAudioTrack &newTrack);

    Q_INVOKABLE void enqueue(const MediaPlayListEntry &newEntry, const MusicAudioTrack &audioTrack = {});

    Q_INVOKABLE void enqueue(const MusicAlbum &album);

    Q_INVOKABLE void enqueue(const QString &artistName);

    Q_INVOKABLE void clearAndEnqueue(qulonglong newTrackId);

    Q_INVOKABLE void clearAndEnqueue(const MusicAudioTrack &newTrack);

    Q_INVOKABLE void clearAndEnqueue(const MusicAlbum &album);

    Q_INVOKABLE void clearAndEnqueue(const QString &artistName);

    Q_INVOKABLE bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;

    Q_INVOKABLE void move(int from, int to, int n);

    Q_INVOKABLE void clearPlayList();

    QList<QVariant> persistentState() const;

    MusicListenersManager* musicListenersManager() const;

    int tracksCount() const;

Q_SIGNALS:

    void newTrackByNameInList(const QString &title, const QString &artist, const QString &album, int trackNumber, int discNumber);

    void newTrackByIdInList(qulonglong newTrackId);

    void newArtistInList(const QString &artist);

    void trackHasBeenAdded(const QString &title, const QUrl &image);

    void persistentStateChanged();

    void musicListenersManagerChanged();

    void tracksCountChanged();

public Q_SLOTS:

    void setPersistentState(const QList<QVariant> &persistentState);

    void removeSelection(QList<int> selection);

    void albumAdded(const QList<MusicAudioTrack> &tracks);

    void trackChanged(const MusicAudioTrack &track);

    void trackRemoved(qulonglong trackId);

    void setMusicListenersManager(MusicListenersManager* musicListenersManager);

private Q_SLOTS:

    bool rowHasHeader(int row) const;

private:

    std::unique_ptr<MediaPlayListPrivate> d;

};

class MediaPlayListEntry
{
public:

    MediaPlayListEntry() {
    }

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

    explicit MediaPlayListEntry(QString artist) : mArtist(std::move(artist)), mIsArtist(true) {
    }

    QString mTitle;

    QString mAlbum;

    QString mArtist;

    int mTrackNumber = -1;

    int mDiscNumber = -1;

    qulonglong mId = 0;

    bool mIsValid = false;

    bool mIsArtist = false;

    MediaPlayList::PlayState mIsPlaying = MediaPlayList::NotPlaying;

};



#endif // MEDIAPLAYLIST_H
