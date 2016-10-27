/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef MANAGEMEDIAPLAYERCONTROL_H
#define MANAGEMEDIAPLAYERCONTROL_H

#include <QObject>
#include <QModelIndex>

class QAbstractItemModel;

class ManageMediaPlayerControl : public QObject
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

#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
    Q_ENUM(PlayerState)
#else
    Q_ENUMS(PlayerState)
#endif

    explicit ManageMediaPlayerControl(QObject *parent = 0);

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

    void setCurrentTrack(QPersistentModelIndex currentTrack);

    void setRandomOrContinuePlay(bool randomOrContinuePlay);

private Q_SLOTS:

    void playListTracksInserted(const QModelIndex &parent, int first, int last);

    void playListTracksWillBeRemoved(const QModelIndex & parent, int first, int last);

    void playListTracksRemoved(const QModelIndex & parent, int first, int last);

    void playListReset();

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

QDataStream &operator<<(QDataStream &out, const ManageMediaPlayerControl::PlayerState &state);
QDataStream &operator>>(QDataStream &in, ManageMediaPlayerControl::PlayerState &state);

#endif // MANAGEMEDIAPLAYERCONTROL_H
