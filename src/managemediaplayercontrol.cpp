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

#include "managemediaplayercontrol.h"


ManageMediaPlayerControl::ManageMediaPlayerControl(QObject *parent) : QObject(parent)
{

}

bool ManageMediaPlayerControl::playControlEnabled() const
{
    return mCurrentTrack.isValid();
}

bool ManageMediaPlayerControl::skipBackwardControlEnabled() const
{
    return (mPreviousTrack.isValid() && mIsInPlayingState);
}

bool ManageMediaPlayerControl::skipForwardControlEnabled() const
{
    return (mNextTrack.isValid() && mIsInPlayingState);
}

bool ManageMediaPlayerControl::musicPlaying() const
{
    return mIsInPlayingState;
}

void ManageMediaPlayerControl::playerPausedOrStopped()
{
    if (mIsInPlayingState) {
        auto oldPreviousTrackIsEnabled = skipBackwardControlEnabled();
        auto oldNextTrackIsEnabled = skipForwardControlEnabled();

        mIsInPlayingState = false;
        Q_EMIT musicPlayingChanged();

        if (!mCurrentTrack.isValid()) {
            return;
        }

        if (oldNextTrackIsEnabled != skipForwardControlEnabled()) {
            Q_EMIT skipForwardControlEnabledChanged();
        }

        if (oldPreviousTrackIsEnabled != skipBackwardControlEnabled()) {
            Q_EMIT skipBackwardControlEnabledChanged();
        }
    }
}

void ManageMediaPlayerControl::playerPlaying()
{
    if (!mIsInPlayingState) {
        auto oldPreviousTrackIsEnabled = skipBackwardControlEnabled();
        auto oldNextTrackIsEnabled = skipForwardControlEnabled();

        mIsInPlayingState = true;
        Q_EMIT musicPlayingChanged();

        if (!mCurrentTrack.isValid()) {
            return;
        }

        if (oldNextTrackIsEnabled != skipForwardControlEnabled()) {
            Q_EMIT skipForwardControlEnabledChanged();
        }

        if (oldPreviousTrackIsEnabled != skipBackwardControlEnabled()) {
            Q_EMIT skipBackwardControlEnabledChanged();
        }
    }
}

void ManageMediaPlayerControl::setPreviousTrack(const QPersistentModelIndex &previousTrack)
{
    if (mPreviousTrack == previousTrack) {
        return;
    }

    bool oldValueSkipBackward = skipBackwardControlEnabled();

    mPreviousTrack = previousTrack;
    Q_EMIT previousTrackChanged();

    if (oldValueSkipBackward != skipBackwardControlEnabled()) {
        Q_EMIT skipBackwardControlEnabledChanged();
    }
}

void ManageMediaPlayerControl::setCurrentTrack(const QPersistentModelIndex &currentTrack)
{
    if (mCurrentTrack == currentTrack) {
        return;
    }

    bool oldPlayControlEnabled = playControlEnabled();

    mCurrentTrack = currentTrack;
    Q_EMIT currentTrackChanged();

    if (oldPlayControlEnabled != playControlEnabled()) {
        Q_EMIT playControlEnabledChanged();
    }
}

void ManageMediaPlayerControl::setNextTrack(const QPersistentModelIndex &nextTrack)
{
    if (mNextTrack == nextTrack) {
        return;
    }

    bool oldValueSkipForward = skipForwardControlEnabled();

    mNextTrack = nextTrack;
    Q_EMIT nextTrackChanged();

    if (oldValueSkipForward != skipForwardControlEnabled()) {
        Q_EMIT skipForwardControlEnabledChanged();
    }
}

QPersistentModelIndex ManageMediaPlayerControl::previousTrack() const
{
    return mPreviousTrack;
}

QPersistentModelIndex ManageMediaPlayerControl::currentTrack() const
{
    return mCurrentTrack;
}

QPersistentModelIndex ManageMediaPlayerControl::nextTrack() const
{
    return mNextTrack;
}

#include "moc_managemediaplayercontrol.cpp"
