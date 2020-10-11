/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef MANAGEAUDIOPLAYER_H
#define MANAGEAUDIOPLAYER_H

#include "elisaLib_export.h"

#include <QObject>
#include <QPersistentModelIndex>
#include <QAbstractItemModel>
#include <QUrl>
#include <QMediaPlayer>

class QDateTime;

class ELISALIB_EXPORT ManageAudioPlayer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QPersistentModelIndex currentTrack
               READ currentTrack
               WRITE setCurrentTrack
               NOTIFY currentTrackChanged)

    Q_PROPERTY(QAbstractItemModel* playListModel
               READ playListModel
               WRITE setPlayListModel
               NOTIFY playListModelChanged)

    Q_PROPERTY(QUrl playerSource
               READ playerSource
               NOTIFY playerSourceChanged)

    Q_PROPERTY(int titleRole
               READ titleRole
               WRITE setTitleRole
               NOTIFY titleRoleChanged)

    Q_PROPERTY(int artistNameRole
               READ artistNameRole
               WRITE setArtistNameRole
               NOTIFY artistNameRoleChanged)

    Q_PROPERTY(int albumNameRole
               READ albumNameRole
               WRITE setAlbumNameRole
               NOTIFY albumNameRoleChanged)

    Q_PROPERTY(int urlRole
               READ urlRole
               WRITE setUrlRole
               NOTIFY urlRoleChanged)

    Q_PROPERTY(int isPlayingRole
               READ isPlayingRole
               WRITE setIsPlayingRole
               NOTIFY isPlayingRoleChanged)

    Q_PROPERTY(QMediaPlayer::MediaStatus playerStatus
               READ playerStatus
               WRITE setPlayerStatus
               NOTIFY playerStatusChanged)

    Q_PROPERTY(QMediaPlayer::State playerPlaybackState
               READ playerPlaybackState
               WRITE setPlayerPlaybackState
               NOTIFY playerPlaybackStateChanged)

    Q_PROPERTY(QMediaPlayer::Error playerError
               READ playerError
               WRITE setPlayerError
               NOTIFY playerErrorChanged)

    Q_PROPERTY(qint64 audioDuration
               READ audioDuration
               WRITE setAudioDuration
               NOTIFY audioDurationChanged)

    Q_PROPERTY(bool playerIsSeekable
               READ playerIsSeekable
               WRITE setPlayerIsSeekable
               NOTIFY playerIsSeekableChanged)

    Q_PROPERTY(qint64 playerPosition
               READ playerPosition
               WRITE setPlayerPosition
               NOTIFY playerPositionChanged)

    Q_PROPERTY(qint64 playControlPosition
               READ playControlPosition
               WRITE setPlayControlPosition
               NOTIFY playControlPositionChanged)

    Q_PROPERTY(QVariantMap persistentState
               READ persistentState
               WRITE setPersistentState
               NOTIFY persistentStateChanged)

public:

    explicit ManageAudioPlayer(QObject *parent = nullptr);

    [[nodiscard]] QPersistentModelIndex currentTrack() const;

    [[nodiscard]] QAbstractItemModel* playListModel() const;

    [[nodiscard]] int urlRole() const;

    [[nodiscard]] int isPlayingRole() const;

    [[nodiscard]] QUrl playerSource() const;

    [[nodiscard]] QMediaPlayer::MediaStatus playerStatus() const;

    [[nodiscard]] QMediaPlayer::State playerPlaybackState() const;

    [[nodiscard]] QMediaPlayer::Error playerError() const;

    [[nodiscard]] qint64 audioDuration() const;

    [[nodiscard]] bool playerIsSeekable() const;

    [[nodiscard]] qint64 playerPosition() const;

    [[nodiscard]] qint64 playControlPosition() const;

    [[nodiscard]] QVariantMap persistentState() const;

    [[nodiscard]] int playListPosition() const;

    [[nodiscard]] int titleRole() const;

    [[nodiscard]] int artistNameRole() const;

    [[nodiscard]] int albumNameRole() const;

Q_SIGNALS:

    void currentTrackChanged();

    void playListModelChanged();

    void playerSourceChanged(const QUrl &url);

    void urlRoleChanged();

    void isPlayingRoleChanged();

    void playerStatusChanged();

    void playerPlaybackStateChanged();

    void playerErrorChanged();

    void playerPlay();

    void playerPause();

    void playerStop();

    void skipNextTrack();

    void audioDurationChanged();

    void playerIsSeekableChanged();

    void playerPositionChanged();

    void playControlPositionChanged();

    void persistentStateChanged();

    void seek(qint64 position);

    void saveUndoPositionInAudioWrapper(qint64 position);

    void restoreUndoPositionInAudioWrapper();

    void titleRoleChanged();

    void artistNameRoleChanged();

    void albumNameRoleChanged();

    void sourceInError(const QUrl &source, QMediaPlayer::Error playerError);

    void displayTrackError(const QString &fileName);

    void startedPlayingTrack(const QUrl &fileName, const QDateTime &time);

    void updateData(const QPersistentModelIndex &index, const QVariant &value, int role);

public Q_SLOTS:

    void setCurrentTrack(const QPersistentModelIndex &currentTrack);

    void saveForUndoClearPlaylist();

    void restoreForUndoClearPlaylist();

    void setPlayListModel(QAbstractItemModel* aPlayListModel);

    void setUrlRole(int value);

    void setIsPlayingRole(int value);

    void setPlayerStatus(QMediaPlayer::MediaStatus playerStatus);

    void setPlayerPlaybackState(QMediaPlayer::State playerPlaybackState);

    void setPlayerError(QMediaPlayer::Error playerError);

    void ensurePause();

    void ensurePlay();

    void playPause();

    void stop();

    void setAudioDuration(qint64 audioDuration);

    void setPlayerIsSeekable(bool playerIsSeekable);

    void setPlayerPosition(qint64 playerPosition);

    void setCurrentPlayingForRadios(const QString &title, const QString &nowPlaying);

    void setPlayControlPosition(int playerPosition);

    void setPersistentState(const QVariantMap &persistentStateValue);

    void playerSeek(int position);

    void playListFinished();

    void tracksDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);

    void setTitleRole(int titleRole);

    void setArtistNameRole(int artistNameRole);

    void setAlbumNameRole(int albumNameRole);

private:

    void notifyPlayerSourceProperty();

    void triggerPlay();

    void triggerPause();

    void triggerStop();

    void triggerSkipNextTrack();

    void restorePreviousState();

    QPersistentModelIndex mCurrentTrack;

    QPersistentModelIndex mOldCurrentTrack;

    QAbstractItemModel *mPlayListModel = nullptr;

    int mTitleRole = Qt::DisplayRole;

    int mArtistNameRole = Qt::DisplayRole;

    int mAlbumNameRole = Qt::DisplayRole;

    int mUrlRole = Qt::DisplayRole;

    int mIsPlayingRole = Qt::DisplayRole;

    QVariant mOldPlayerSource;

    QMediaPlayer::MediaStatus mPlayerStatus = QMediaPlayer::NoMedia;

    QMediaPlayer::State mPlayerPlaybackState = QMediaPlayer::StoppedState;

    QMediaPlayer::Error mPlayerError = QMediaPlayer::NoError;

    bool mPlayingState = false;

    bool mSkippingCurrentTrack = false;

    int mAudioDuration = 0;

    bool mPlayerIsSeekable = false;

    qint64 mPlayerPosition = 0;

    QVariantMap mPersistentState;

    bool mUndoPlayingState = false;

    qint64 mUndoPlayerPosition = 0;

};

#endif // MANAGEAUDIOPLAYER_H
