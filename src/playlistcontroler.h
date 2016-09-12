/*
 * Copyright 2015 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef PLAYLISTCONTROLER_H
#define PLAYLISTCONTROLER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QPersistentModelIndex>
#include <QtCore/QAbstractItemModel>
#include <QtCore/QModelIndex>
#include <QtCore/QUrl>

class QDataStream;

class PlayListControler : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QUrl playerSource
               READ playerSource
               NOTIFY playerSourceChanged)

    Q_PROPERTY(int currentTrackPosition
               READ currentTrackPosition
               NOTIFY currentTrackPositionChanged)

    Q_PROPERTY(bool playControlEnabled
               READ playControlEnabled
               NOTIFY playControlEnabledChanged)

    Q_PROPERTY(bool skipBackwardControlEnabled
               READ skipBackwardControlEnabled
               NOTIFY skipBackwardControlEnabledChanged)

    Q_PROPERTY(bool skipForwardControlEnabled
               READ skipForwardControlEnabled
               NOTIFY skipForwardControlEnabledChanged)

    Q_PROPERTY(bool playerIsSeekable
               READ playerIsSeekable
               WRITE setPlayerIsSeekable
               NOTIFY playerIsSeekableChanged)

    Q_PROPERTY(bool musicPlaying
               READ musicPlaying
               NOTIFY musicPlayingChanged)

    Q_PROPERTY(bool musicPlayerStopped
               READ musicPlayerStopped
               NOTIFY musicPlayerStoppedChanged)

    Q_PROPERTY(QAbstractItemModel* playListModel
               READ playListModel
               WRITE setPlayListModel
               NOTIFY playListModelChanged)

    Q_PROPERTY(int urlRole
               READ urlRole
               WRITE setUrlRole
               NOTIFY urlRoleChanged)

    Q_PROPERTY(int isPlayingRole
               READ isPlayingRole
               WRITE setIsPlayingRole
               NOTIFY isPlayingRoleChanged)

    Q_PROPERTY(int artistRole
               READ artistRole
               WRITE setArtistRole
               NOTIFY artistRoleChanged)

    Q_PROPERTY(int titleRole
               READ titleRole
               WRITE setTitleRole
               NOTIFY titleRoleChanged)

    Q_PROPERTY(int albumRole
               READ albumRole
               WRITE setAlbumRole
               NOTIFY albumRoleChanged)

    Q_PROPERTY(int imageRole
               READ imageRole
               WRITE setImageRole
               NOTIFY imageRoleChanged)

    Q_PROPERTY(int isValidRole
               READ isValidRole
               WRITE setIsValidRole
               NOTIFY isValidRoleChanged)

    Q_PROPERTY(int audioPosition
               READ audioPosition
               WRITE setAudioPosition
               NOTIFY audioPositionChanged)

    Q_PROPERTY(int audioDuration
               READ audioDuration
               WRITE setAudioDuration
               NOTIFY audioDurationChanged)

    Q_PROPERTY(int playControlPosition
               READ playControlPosition
               WRITE setPlayControlPosition
               NOTIFY playControlPositionChanged)

    Q_PROPERTY(QVariant artist
               READ artist
               NOTIFY artistChanged)

    Q_PROPERTY(QVariant title
               READ title
               NOTIFY titleChanged)

    Q_PROPERTY(QVariant album
               READ album
               NOTIFY albumChanged)

    Q_PROPERTY(QVariant image
               READ image
               NOTIFY imageChanged)

    Q_PROPERTY(int remainingTracks
               READ remainingTracks
               NOTIFY remainingTracksChanged)

    Q_PROPERTY(bool randomPlay
               READ randomPlay
               WRITE setRandomPlay
               NOTIFY randomPlayChanged)

    Q_PROPERTY(bool randomPlayControl
               READ randomPlayControl
               WRITE setRandomPlayControl
               NOTIFY randomPlayControlChanged)

    Q_PROPERTY(bool repeatPlay
               READ repeatPlay
               WRITE setRepeatPlay
               NOTIFY repeatPlayChanged)

    Q_PROPERTY(bool repeatPlayControl
               READ repeatPlayControl
               WRITE setRepeatPlayControl
               NOTIFY repeatPlayControlChanged)

    Q_PROPERTY(QVariantMap persistentState
               READ persistentState
               WRITE setPersistentState
               NOTIFY persistentStateChanged)

public:

    enum class PlayerState
    {
        Playing,
        Paused,
        Stopped,
    };

#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
    Q_ENUM(PlayerState)
#else
    Q_ENUMS(PlayerState)
#endif

    explicit PlayListControler(QObject *parent = 0);

    QUrl playerSource() const;

    bool playControlEnabled() const;

    bool skipBackwardControlEnabled() const;

    bool skipForwardControlEnabled() const;

    bool musicPlaying() const;

    void setPlayListModel(QAbstractItemModel* aPlayListModel);

    QAbstractItemModel* playListModel() const;

    void setUrlRole(int value);

    int urlRole() const;

    void setIsPlayingRole(int value);

    int isPlayingRole() const;

    void setArtistRole(int value);

    int artistRole() const;

    void setTitleRole(int value);

    int titleRole() const;

    void setAlbumRole(int value);

    int albumRole() const;

    void setImageRole(int value);

    int imageRole() const;

    void setAudioPosition(int value);

    int audioPosition() const;

    void setAudioDuration(int value);

    int audioDuration() const;

    void setPlayControlPosition(int value);

    int playControlPosition() const;

    QVariant album() const;

    QVariant title() const;

    QVariant artist() const;

    QVariant image() const;

    int remainingTracks() const;

    void setRandomPlay(bool value);

    bool randomPlay() const;

    void setRepeatPlay(bool value);

    bool repeatPlay() const;

    bool musicPlayerStopped() const;

    int currentTrackPosition() const;

    bool playerIsSeekable() const;

    int isValidRole() const;

    QVariantMap persistentState() const;

    bool randomPlayControl() const;

    bool repeatPlayControl() const;

Q_SIGNALS:

    void playMusic();

    void pauseMusic();

    void stopMusic();

    void playerSourceChanged();

    void playControlEnabledChanged();

    void skipBackwardControlEnabledChanged();

    void skipForwardControlEnabledChanged();

    void musicPlayingChanged();

    void playListModelChanged();

    void urlRoleChanged();

    void isPlayingRoleChanged();

    void artistRoleChanged();

    void titleRoleChanged();

    void albumRoleChanged();

    void imageRoleChanged();

    void audioPositionChanged();

    void audioDurationChanged();

    void playControlPositionChanged();

    void artistChanged();

    void titleChanged();

    void albumChanged();

    void imageChanged();

    void remainingTracksChanged();

    void randomPlayChanged();

    void repeatPlayChanged();

    void musicPlayerStoppedChanged();

    void currentTrackPositionChanged();

    void playerIsSeekableChanged();

    void isValidRoleChanged();

    void persistentStateChanged();

    void randomPlayControlChanged();

    void repeatPlayControlChanged();

public Q_SLOTS:

    void playListReset();

    void playListLayoutChanged(const QList<QPersistentModelIndex> &parents = QList<QPersistentModelIndex>(),
                               QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::NoLayoutChangeHint);

    void tracksInserted(const QModelIndex &parent, int first, int last);

    void tracksMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);

    void tracksRemoved(const QModelIndex & parent, int first, int last);

    void tracksDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int> ());

    void playerPaused();

    void playerPlaying();

    void playerStopped();

    void skipNextTrack();

    void skipPreviousTrack();

    void playPause();

    void playerSeek(int position);

    void audioPlayerPositionChanged(int position);

    void audioPlayerFinished(bool finished);

    void skipToTrack(int position);

    void setPlayerIsSeekable(bool playerIsSeekable);

    void setIsValidRole(int isValidRole);

    void setPersistentState(QVariantMap persistentStateValue);

    void setRandomPlayControl(bool randomPlayControl);

    void setRepeatPlayControl(bool repeatPlayControl);

private:

    void startPlayer();

    void pausePlayer();

    void stopPlayer();

    void gotoNextTrack();

    void signaTrackChange();

    void resetCurrentTrack();

    void restorePlayListPosition();

    void restorePlayControlPosition();

    void restoreRandomPlay();

    void restoreRepeatPlay();

    void restorePlayerState();

    QAbstractItemModel *mPlayListModel = nullptr;

    QModelIndex mCurrentTrack;

    int mUrlRole = Qt::DisplayRole;

    int mIsPlayingRole = Qt::DisplayRole;

    int mArtistRole = Qt::DisplayRole;

    int mTitleRole = Qt::DisplayRole;

    int mAlbumRole = Qt::DisplayRole;

    int mImageRole = Qt::DisplayRole;

    PlayerState mPlayerState = PlayListControler::PlayerState::Stopped;

    int mAudioPosition = 0;

    int mAudioDuration = 0;

    int mRealAudioPosition = 0;

    int mPlayControlPosition = 0;

    bool mRandomPlay = false;

    bool mRepeatPlay = false;

    bool mIsInPlayingState = false;

    bool mPlayerIsSeekable = false;

    int mIsValidRole = Qt::DisplayRole;

    QVariantMap mPersistentState;

    bool mRandomPlayControl = false;

    bool mRepeatPlayControl = false;

};

QDataStream& operator<<(QDataStream &out, const PlayListControler::PlayerState &state);
QDataStream& operator>>(QDataStream &in, PlayListControler::PlayerState &state);

#endif // PLAYLISTCONTROLER_H
