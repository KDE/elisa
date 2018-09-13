/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef MANAGEMEDIAPLAYERCONTROL_H
#define MANAGEMEDIAPLAYERCONTROL_H

#include "elisaLib_export.h"

#include <QObject>
#include <QModelIndex>

class QAbstractItemModel;

class ELISALIB_EXPORT ManageMediaPlayerControl : public QObject
{

    Q_OBJECT

    Q_PROPERTY(bool playControlEnabled
               READ playControlEnabled
               NOTIFY playControlEnabledChanged)

    Q_PROPERTY(bool skipBackwardControlEnabled
               READ skipBackwardControlEnabled
               NOTIFY skipBackwardControlEnabledChanged)

    Q_PROPERTY(bool skipForwardControlEnabled
               READ skipForwardControlEnabled
               NOTIFY skipForwardControlEnabledChanged)

    Q_PROPERTY(bool musicPlaying
               READ musicPlaying
               NOTIFY musicPlayingChanged)

    Q_PROPERTY(QAbstractItemModel* playListModel
               READ playListModel
               WRITE setPlayListModel
               NOTIFY playListModelChanged)

    Q_PROPERTY(QPersistentModelIndex currentTrack
               READ currentTrack
               WRITE setCurrentTrack
               NOTIFY currentTrackChanged)

    Q_PROPERTY(bool randomOrContinuePlay
               READ randomOrContinuePlay
               WRITE setRandomOrContinuePlay
               NOTIFY randomOrContinuePlayChanged)

public:

    enum class PlayerState
    {
        Playing,
        Paused,
        Stopped,
    };

    Q_ENUM(PlayerState)

    explicit ManageMediaPlayerControl(QObject *parent = nullptr);

    bool playControlEnabled() const;

    bool skipBackwardControlEnabled() const;

    bool skipForwardControlEnabled() const;

    bool musicPlaying() const;

    QAbstractItemModel* playListModel() const;

    QPersistentModelIndex currentTrack() const;

    bool randomOrContinuePlay() const;

Q_SIGNALS:

    void playControlEnabledChanged();

    void skipBackwardControlEnabledChanged();

    void skipForwardControlEnabledChanged();

    void musicPlayingChanged();

    void playListModelChanged();

    void currentTrackChanged();

    void randomOrContinuePlayChanged();

public Q_SLOTS:

    void setPlayListModel(QAbstractItemModel* aPlayListModel);

    void playerPaused();

    void playerPlaying();

    void playerStopped();

    void setCurrentTrack(const QPersistentModelIndex &currentTrack);

    void setRandomOrContinuePlay(bool randomOrContinuePlay);

private Q_SLOTS:

    void playListTracksWillBeInserted(const QModelIndex &parent, int first, int last);

    void playListTracksInserted(const QModelIndex &parent, int first, int last);

    void playListTracksWillBeRemoved(const QModelIndex & parent, int first, int last);

    void playListTracksRemoved(const QModelIndex & parent, int first, int last);

    void playListReset();

    void tracksAboutToBeMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);

    void tracksMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);

private:

    QAbstractItemModel *mPlayListModel = nullptr;

    QPersistentModelIndex mCurrentTrack;

    bool mCurrentTrackWillBeRemoved = false;

    bool mSkipBackwardControlWasEnabled = false;

    bool mSkipForwardControlWasEnabled = false;

    bool mIsInPlayingState = false;

    PlayerState mPlayerState = ManageMediaPlayerControl::PlayerState::Stopped;

    bool mRandomOrContinuePlay = false;

};

ELISALIB_EXPORT QDataStream &operator<<(QDataStream &out, const ManageMediaPlayerControl::PlayerState &state);
ELISALIB_EXPORT QDataStream &operator>>(QDataStream &in, ManageMediaPlayerControl::PlayerState &state);

#endif // MANAGEMEDIAPLAYERCONTROL_H
