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

#include "managemediaplayercontrol.h"

#include <QtCore/QDataStream>

ManageMediaPlayerControl::ManageMediaPlayerControl(QObject *parent) : QObject(parent)
{

}

bool ManageMediaPlayerControl::playControlEnabled() const
{
    if (!mPlayListModel) {
        return false;
    }

    if (!mCurrentTrack.isValid()) {
        return false;
    }

    return mPlayListModel->rowCount() > 0;
}

bool ManageMediaPlayerControl::skipBackwardControlEnabled() const
{
    if (!mPlayListModel) {
        return false;
    }

    if (!mCurrentTrack.isValid()) {
        return false;
    }

    return (mRandomOrContinuePlay || (mCurrentTrack.row() > 0)) && mIsInPlayingState;
}

bool ManageMediaPlayerControl::skipForwardControlEnabled() const
{
    if (!mPlayListModel) {
        return false;
    }

    if (!mCurrentTrack.isValid()) {
        return false;
    }

    return (mRandomOrContinuePlay || (mCurrentTrack.row() < mPlayListModel->rowCount() - 1)) && mIsInPlayingState;
}

bool ManageMediaPlayerControl::musicPlaying() const
{
    return mPlayerState == ManageMediaPlayerControl::PlayerState::Playing;
}

void ManageMediaPlayerControl::playerPaused()
{
    //qDebug() << "ManageMediaPlayerControl::playerPaused";

    if (!mIsInPlayingState) {
        auto oldPreviousTrackIsEnabled = skipBackwardControlEnabled();
        auto oldNextTrackIsEnabled = skipForwardControlEnabled();

        mIsInPlayingState = true;

        if (!mCurrentTrack.isValid()) {
            return;
        }

        auto newNextTrackIsEnabled = skipForwardControlEnabled();
        if (oldNextTrackIsEnabled != newNextTrackIsEnabled) {
            Q_EMIT skipForwardControlEnabledChanged();
        }

        auto newPreviousTrackIsEnabled = skipBackwardControlEnabled();
        if (oldPreviousTrackIsEnabled != newPreviousTrackIsEnabled) {
            Q_EMIT skipBackwardControlEnabledChanged();
        }
    }

    mPlayerState = ManageMediaPlayerControl::PlayerState::Paused;
    Q_EMIT musicPlayingChanged();
}

void ManageMediaPlayerControl::playerPlaying()
{
    if (!mIsInPlayingState) {
        auto oldPreviousTrackIsEnabled = skipBackwardControlEnabled();
        auto oldNextTrackIsEnabled = skipForwardControlEnabled();

        mIsInPlayingState = true;

        if (!mCurrentTrack.isValid()) {
            return;
        }

        auto newNextTrackIsEnabled = skipForwardControlEnabled();
        if (oldNextTrackIsEnabled != newNextTrackIsEnabled) {
            Q_EMIT skipForwardControlEnabledChanged();
        }

        auto newPreviousTrackIsEnabled = skipBackwardControlEnabled();
        if (oldPreviousTrackIsEnabled != newPreviousTrackIsEnabled) {
            Q_EMIT skipBackwardControlEnabledChanged();
        }
    }

    mPlayerState = ManageMediaPlayerControl::PlayerState::Playing;
    Q_EMIT musicPlayingChanged();
}

void ManageMediaPlayerControl::playerStopped()
{
    //qDebug() << "ManageMediaPlayerControl::playerStopped";

    if (mIsInPlayingState) {
        auto oldPreviousTrackIsEnabled = skipBackwardControlEnabled();
        auto oldNextTrackIsEnabled = skipForwardControlEnabled();

        mIsInPlayingState = false;

        if (mCurrentTrack.isValid()) {
            auto newNextTrackIsEnabled = skipForwardControlEnabled();
            if (oldNextTrackIsEnabled != newNextTrackIsEnabled) {
                Q_EMIT skipForwardControlEnabledChanged();
            }

            auto newPreviousTrackIsEnabled = skipBackwardControlEnabled();
            if (oldPreviousTrackIsEnabled != newPreviousTrackIsEnabled) {
                Q_EMIT skipBackwardControlEnabledChanged();
            }
        }
    }

    mPlayerState = ManageMediaPlayerControl::PlayerState::Stopped;
    Q_EMIT musicPlayingChanged();
}

void ManageMediaPlayerControl::setCurrentTrack(QPersistentModelIndex currentTrack)
{
    if (mCurrentTrack == currentTrack) {
        return;
    }

    bool oldPlayControlEnabled = playControlEnabled();
    bool oldValueSkipBackward = skipBackwardControlEnabled();
    bool oldValueSkipForward = skipForwardControlEnabled();

    mCurrentTrack = currentTrack;
    Q_EMIT currentTrackChanged();

    if (oldPlayControlEnabled != playControlEnabled()) {
        Q_EMIT playControlEnabledChanged();
    }

    if (!mIsInPlayingState) {
        return;
    }

    if (oldValueSkipBackward != skipBackwardControlEnabled()) {
        Q_EMIT skipBackwardControlEnabledChanged();
    }

    if (oldValueSkipForward != skipForwardControlEnabled()) {
        Q_EMIT skipForwardControlEnabledChanged();
    }
}

bool ManageMediaPlayerControl::randomOrContinuePlay() const
{
    return mRandomOrContinuePlay;
}

void ManageMediaPlayerControl::setRandomOrContinuePlay(bool randomOrContinuePlay)
{
    if (mRandomOrContinuePlay == randomOrContinuePlay) {
        return;
    }

    auto oldPreviousTrackIsEnabled = skipBackwardControlEnabled();
    auto oldNextTrackIsEnabled = skipForwardControlEnabled();

    mRandomOrContinuePlay = randomOrContinuePlay;
    Q_EMIT randomOrContinuePlayChanged();

    auto newNextTrackIsEnabled = skipForwardControlEnabled();
    if (oldNextTrackIsEnabled != newNextTrackIsEnabled) {
        Q_EMIT skipForwardControlEnabledChanged();
    }

    auto newPreviousTrackIsEnabled = skipBackwardControlEnabled();
    if (oldPreviousTrackIsEnabled != newPreviousTrackIsEnabled) {
        Q_EMIT skipBackwardControlEnabledChanged();
    }
}

void ManageMediaPlayerControl::playListTracksWillBeInserted(const QModelIndex &parent, int first, int last)
{
    mSkipBackwardControlWasEnabled = skipBackwardControlEnabled();
    mSkipForwardControlWasEnabled = skipForwardControlEnabled();
}

void ManageMediaPlayerControl::playListTracksInserted(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);

    if (!mCurrentTrack.isValid()) {
        mSkipBackwardControlWasEnabled = false;
        mSkipForwardControlWasEnabled = false;

        return;
    }

    auto newNextTrackIsEnabled = skipForwardControlEnabled();
    if (mSkipForwardControlWasEnabled != newNextTrackIsEnabled) {
        Q_EMIT skipForwardControlEnabledChanged();
    }

    auto newPreviousTrackIsEnabled = skipBackwardControlEnabled();
    if (mSkipBackwardControlWasEnabled != newPreviousTrackIsEnabled) {
        Q_EMIT skipBackwardControlEnabledChanged();
    }
}

void ManageMediaPlayerControl::playListTracksWillBeRemoved(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);

    if (!mCurrentTrack.isValid()) {
        mCurrentTrackWillBeRemoved = false;
        mSkipBackwardControlWasEnabled = false;
        mSkipForwardControlWasEnabled = false;

        return;
    }

    mCurrentTrackWillBeRemoved = (mCurrentTrack.row() >= first && mCurrentTrack.row() <= last);
    mSkipBackwardControlWasEnabled = skipBackwardControlEnabled();
    mSkipForwardControlWasEnabled = skipForwardControlEnabled();
}

void ManageMediaPlayerControl::playListTracksRemoved(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);

    if (mCurrentTrackWillBeRemoved) {
        Q_EMIT currentTrackChanged();
        Q_EMIT playControlEnabledChanged();
    }

    if (!mCurrentTrack.isValid()) {
        if (mSkipBackwardControlWasEnabled) {
            Q_EMIT skipBackwardControlEnabledChanged();
        }

        if (mSkipForwardControlWasEnabled) {
            Q_EMIT skipForwardControlEnabledChanged();
        }

        return;
    }

    auto newNextTrackIsEnabled = skipForwardControlEnabled();
    if (mSkipForwardControlWasEnabled != newNextTrackIsEnabled) {
        Q_EMIT skipForwardControlEnabledChanged();
    }

    auto newPreviousTrackIsEnabled = skipBackwardControlEnabled();
    if (mSkipBackwardControlWasEnabled != newPreviousTrackIsEnabled) {
        Q_EMIT skipBackwardControlEnabledChanged();
    }
}

void ManageMediaPlayerControl::playListReset()
{
}

void ManageMediaPlayerControl::tracksAboutToBeMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    Q_UNUSED(destination);
    Q_UNUSED(row);

    mSkipBackwardControlWasEnabled = skipBackwardControlEnabled();
    mSkipForwardControlWasEnabled = skipForwardControlEnabled();
}

void ManageMediaPlayerControl::tracksMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    Q_UNUSED(destination);
    Q_UNUSED(row);

    auto newNextTrackIsEnabled = skipForwardControlEnabled();
    if (mSkipForwardControlWasEnabled != newNextTrackIsEnabled) {
        Q_EMIT skipForwardControlEnabledChanged();
    }

    auto newPreviousTrackIsEnabled = skipBackwardControlEnabled();
    if (mSkipBackwardControlWasEnabled != newPreviousTrackIsEnabled) {
        Q_EMIT skipBackwardControlEnabledChanged();
    }
}

void ManageMediaPlayerControl::setPlayListModel(QAbstractItemModel *aPlayListModel)
{
    if (mPlayListModel) {
        disconnect(mPlayListModel, &QAbstractItemModel::rowsAboutToBeInserted, this, &ManageMediaPlayerControl::playListTracksWillBeInserted);
        disconnect(mPlayListModel, &QAbstractItemModel::rowsInserted, this, &ManageMediaPlayerControl::playListTracksInserted);
        disconnect(mPlayListModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, &ManageMediaPlayerControl::playListTracksWillBeRemoved);
        disconnect(mPlayListModel, &QAbstractItemModel::rowsRemoved, this, &ManageMediaPlayerControl::playListTracksRemoved);
        disconnect(mPlayListModel, &QAbstractItemModel::rowsMoved, this, &ManageMediaPlayerControl::tracksMoved);
        disconnect(mPlayListModel, &QAbstractItemModel::rowsAboutToBeMoved, this, &ManageMediaPlayerControl::tracksAboutToBeMoved);
        disconnect(mPlayListModel, &QAbstractItemModel::modelReset, this, &ManageMediaPlayerControl::playListReset);
    }

    mPlayListModel = aPlayListModel;

    if (mPlayListModel) {
        connect(mPlayListModel, &QAbstractItemModel::rowsAboutToBeInserted, this, &ManageMediaPlayerControl::playListTracksWillBeInserted);
        connect(mPlayListModel, &QAbstractItemModel::rowsInserted, this, &ManageMediaPlayerControl::playListTracksInserted);
        connect(mPlayListModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, &ManageMediaPlayerControl::playListTracksWillBeRemoved);
        connect(mPlayListModel, &QAbstractItemModel::rowsRemoved, this, &ManageMediaPlayerControl::playListTracksRemoved);
        connect(mPlayListModel, &QAbstractItemModel::rowsMoved, this, &ManageMediaPlayerControl::tracksMoved);
        connect(mPlayListModel, &QAbstractItemModel::rowsAboutToBeMoved, this, &ManageMediaPlayerControl::tracksAboutToBeMoved);
        connect(mPlayListModel, &QAbstractItemModel::modelReset, this, &ManageMediaPlayerControl::playListReset);
    }

    Q_EMIT playListModelChanged();
}

QAbstractItemModel *ManageMediaPlayerControl::playListModel() const
{
    return mPlayListModel;
}

QPersistentModelIndex ManageMediaPlayerControl::currentTrack() const
{
    return mCurrentTrack;
}

QDataStream &operator<<(QDataStream &out, const ManageMediaPlayerControl::PlayerState &state)
{
    out << static_cast<int>(state);
    return out;
}

QDataStream &operator>>(QDataStream &in, ManageMediaPlayerControl::PlayerState &state)
{
    int value;
    in >> value;
    state = static_cast<ManageMediaPlayerControl::PlayerState>(value);
    return in;
}


#include "moc_managemediaplayercontrol.cpp"
