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

    return mCurrentTrack.row() > 0 && mIsInPlayingState;
}

bool ManageMediaPlayerControl::skipForwardControlEnabled() const
{
    if (!mPlayListModel) {
        return false;
    }

    if (!mCurrentTrack.isValid()) {
        return false;
    }

    return (mCurrentTrack.row() < mPlayListModel->rowCount() - 1) && mIsInPlayingState;
}

int ManageMediaPlayerControl::playControlPosition() const
{
    return mPlayControlPosition;
}

bool ManageMediaPlayerControl::musicPlaying() const
{
    return mPlayerState == ManageMediaPlayerControl::PlayerState::Playing;
}

void ManageMediaPlayerControl::playerPaused()
{
    //qDebug() << "ManageMediaPlayerControl::playerPaused";

    if (!mIsInPlayingState) {
        mIsInPlayingState = true;

        if (!mCurrentTrack.isValid()) {
            return;
        }

        if (mCurrentTrack.row() < mPlayListModel->rowCount() - 1) {
            Q_EMIT skipForwardControlEnabledChanged();
        }

        if (mCurrentTrack.row() > 0) {
            Q_EMIT skipBackwardControlEnabledChanged();
        }
    }

    mPlayerState = ManageMediaPlayerControl::PlayerState::Paused;
    Q_EMIT musicPlayingChanged();
}

void ManageMediaPlayerControl::playerPlaying()
{
    //qDebug() << "ManageMediaPlayerControl::playerPlaying";

    if (!mIsInPlayingState) {
        mIsInPlayingState = true;

        if (!mCurrentTrack.isValid()) {
            return;
        }

        if (mCurrentTrack.row() < mPlayListModel->rowCount() - 1) {
            Q_EMIT skipForwardControlEnabledChanged();
        }

        if (mCurrentTrack.row() > 0) {
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
        mIsInPlayingState = false;

        if (!mCurrentTrack.isValid()) {
            return;
        }

        if (mCurrentTrack.row() < mPlayListModel->rowCount() - 1) {
            Q_EMIT skipForwardControlEnabledChanged();
        }

        if (mCurrentTrack.row() > 0) {
            Q_EMIT skipBackwardControlEnabledChanged();
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

void ManageMediaPlayerControl::setPlayControlPosition(int playControlPosition)
{
    if (mPlayControlPosition == playControlPosition) {
        return;
    }

    mPlayControlPosition = playControlPosition;
    Q_EMIT playControlPositionChanged();
}

void ManageMediaPlayerControl::playListTracksInserted(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);

    if (!mCurrentTrack.isValid()) {
        return;
    }

    if (first > mCurrentTrack.row() && last + 1 == mPlayListModel->rowCount()) {
        Q_EMIT skipForwardControlEnabledChanged();
    }

    if (mCurrentTrack.row() == last + 1 && first == 0) {
        Q_EMIT skipBackwardControlEnabledChanged();
    }
}

void ManageMediaPlayerControl::playListTracksWillBeRemoved(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);

    if (!mCurrentTrack.isValid()) {
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

    if (mSkipBackwardControlWasEnabled && mCurrentTrack.row() == 0) {
        Q_EMIT skipBackwardControlEnabledChanged();
    }

    if (mSkipForwardControlWasEnabled && mCurrentTrack.row() == (mPlayListModel->rowCount() - 1)) {
        Q_EMIT skipForwardControlEnabledChanged();
    }
}

void ManageMediaPlayerControl::playListReset()
{
}

void ManageMediaPlayerControl::setPlayListModel(QAbstractItemModel *aPlayListModel)
{
    if (mPlayListModel) {
        disconnect(mPlayListModel, &QAbstractItemModel::rowsInserted, this, &ManageMediaPlayerControl::playListTracksInserted);
        disconnect(mPlayListModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, &ManageMediaPlayerControl::playListTracksWillBeRemoved);
        disconnect(mPlayListModel, &QAbstractItemModel::rowsRemoved, this, &ManageMediaPlayerControl::playListTracksRemoved);
        disconnect(mPlayListModel, &QAbstractItemModel::modelReset, this, &ManageMediaPlayerControl::playListReset);
    }

    mPlayListModel = aPlayListModel;

    connect(mPlayListModel, &QAbstractItemModel::rowsInserted, this, &ManageMediaPlayerControl::playListTracksInserted);
    connect(mPlayListModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, &ManageMediaPlayerControl::playListTracksWillBeRemoved);
    connect(mPlayListModel, &QAbstractItemModel::rowsRemoved, this, &ManageMediaPlayerControl::playListTracksRemoved);
    connect(mPlayListModel, &QAbstractItemModel::modelReset, this, &ManageMediaPlayerControl::playListReset);

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


#include "moc_managemediaplayercontrol.cpp"
