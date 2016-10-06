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

#include "manageaudioplayer.h"

#include <QtCore/QTimer>

#include <QtCore/QDebug>

ManageAudioPlayer::ManageAudioPlayer(QObject *parent) : QObject(parent)
{

}

QPersistentModelIndex ManageAudioPlayer::currentTrack() const
{
    return mCurrentTrack;
}

int ManageAudioPlayer::urlRole() const
{
    return mUrlRole;
}

int ManageAudioPlayer::isPlayingRole() const
{
    return mIsPlayingRole;
}

QUrl ManageAudioPlayer::playerSource() const
{
    if (!mCurrentTrack.isValid()) {
        return QUrl();
    }

    return mCurrentTrack.data(mUrlRole).toUrl();
}

int ManageAudioPlayer::playerStatus() const
{
    return mPlayerStatus;
}

int ManageAudioPlayer::playerPlaybackState() const
{
    return mPlayerPlaybackState;
}

int ManageAudioPlayer::playerError() const
{
    return mPlayerError;
}

int ManageAudioPlayer::audioDuration() const
{
    return mAudioDuration;
}

bool ManageAudioPlayer::playerIsSeekable() const
{
    return mPlayerIsSeekable;
}

int ManageAudioPlayer::playControlPosition() const
{
    return mPlayControlPosition;
}

void ManageAudioPlayer::setCurrentTrack(QPersistentModelIndex currentTrack)
{
    if (mCurrentTrack == currentTrack) {
        return;
    }

    mCurrentTrack = currentTrack;
    Q_EMIT currentTrackChanged();

    switch (mPlayerPlaybackState) {
    case StoppedState:
        notifyPlayerSourceProperty();
        break;
    case PlayingState:
        triggerStop();
        mSkippingCurrentTrack = true;
        break;
    case PausedState:
        triggerStop();
        break;
    }
}

void ManageAudioPlayer::setUrlRole(int value)
{
    mUrlRole = value;
    Q_EMIT urlRoleChanged();
    notifyPlayerSourceProperty();
}

void ManageAudioPlayer::setIsPlayingRole(int value)
{
    if (mIsPlayingRole == value) {
        return;
    }

    mIsPlayingRole = value;
    Q_EMIT isPlayingRoleChanged();
}

void ManageAudioPlayer::setPlayerStatus(int playerStatus)
{
    if (mPlayerStatus == playerStatus) {
        return;
    }

    if (playerStatus < static_cast<int>(NoMedia) || playerStatus > static_cast<int>(UnknownStatus)) {
        return;
    }

    mPlayerStatus = static_cast<PlayerStatus>(playerStatus);
    Q_EMIT playerStatusChanged();
    qDebug() << "ManageAudioPlayer::setPlayerStatus" << mPlayerStatus;

    switch (mPlayerStatus) {
    case NoMedia:
        break;
    case Loading:
        break;
    case Loaded:
        if (mPlayingState) {
            triggerPlay();
        }
        break;
    case Buffering:
        if (mPlayingState) {
            triggerPlay();
        }
        break;
    case Stalled:
        break;
    case Buffered:
        break;
    case EndOfMedia:
        break;
    case InvalidMedia:
        break;
    case UnknownStatus:
        break;
    }
}

void ManageAudioPlayer::setPlayerPlaybackState(int playerPlaybackState)
{
    if (mPlayerPlaybackState == playerPlaybackState) {
        return;
    }

    if (playerPlaybackState < static_cast<int>(StoppedState) || playerPlaybackState > static_cast<int>(PausedState)) {
        return;
    }

    mPlayerPlaybackState = static_cast<PlayerPlaybackState>(playerPlaybackState);
    Q_EMIT playerPlaybackStateChanged();
    qDebug() << "ManageAudioPlayer::setPlayerPlaybackState" << mPlayerPlaybackState;

    if (!mSkippingCurrentTrack) {
        switch(mPlayerPlaybackState) {
        case StoppedState:
            if (mPlayerStatus == EndOfMedia) {
                triggerSkipNextTrack();
            }
            break;
        case PlayingState:
            break;
        case PausedState:
            break;
        }
    } else {
        switch(mPlayerPlaybackState) {
        case StoppedState:
            notifyPlayerSourceProperty();
            /*if (mPlayingState) {
                triggerPlay();
            }*/
            mSkippingCurrentTrack = false;
            break;
        case PlayingState:
            break;
        case PausedState:
            break;
        }
    }
}

void ManageAudioPlayer::setPlayerError(int playerError)
{
    if (mPlayerError == playerError) {
        return;
    }

    if (playerError < static_cast<int>(NoError) || playerError > static_cast<int>(ServiceMissing)) {
        return;
    }

    mPlayerError = static_cast<PlayerErrorState>(playerError);
    Q_EMIT playerErrorChanged();
    qDebug() << "ManageAudioPlayer::setPlayerError" << mPlayerError;
}

void ManageAudioPlayer::playPause()
{
    mPlayingState = !mPlayingState;

    switch (mPlayerStatus) {
    case Loaded:
    case Buffering:
        if (mPlayingState) {
            triggerPlay();
        } else {
            triggerPause();
        }
        break;
    case EndOfMedia:
        if (mPlayerPlaybackState == PlayingState && !mPlayingState) {
            triggerPause();
        } else if (mPlayerPlaybackState == PausedState && mPlayingState) {
            triggerPlay();
        }
        break;
    case NoMedia:
    case Loading:
    case Stalled:
    case Buffered:
    case InvalidMedia:
    case UnknownStatus:
        break;
    }
}

void ManageAudioPlayer::setAudioDuration(int audioDuration)
{
    if (mAudioDuration == audioDuration) {
        return;
    }

    mAudioDuration = audioDuration;
    Q_EMIT audioDurationChanged();
}

void ManageAudioPlayer::setPlayerIsSeekable(bool playerIsSeekable)
{
    if (mPlayerIsSeekable == playerIsSeekable) {
        return;
    }

    mPlayerIsSeekable = playerIsSeekable;
    Q_EMIT playerIsSeekableChanged();
}

void ManageAudioPlayer::setPlayControlPosition(int playControlPosition)
{
    if (mPlayControlPosition == playControlPosition) {
        return;
    }

    mPlayControlPosition = playControlPosition;
    Q_EMIT playControlPositionChanged();
}

void ManageAudioPlayer::playerSeek(int position)
{

}

void ManageAudioPlayer::notifyPlayerSourceProperty()
{
    auto newUrlValue = mCurrentTrack.data(mUrlRole);
    if (mOldPlayerSource != newUrlValue) {
        Q_EMIT playerSourceChanged();

        mOldPlayerSource = newUrlValue;
    }
}

void ManageAudioPlayer::triggerPlay()
{
    QTimer::singleShot(0, [this]() {Q_EMIT playerPlay();});
}

void ManageAudioPlayer::triggerPause()
{
    QTimer::singleShot(0, [this]() {Q_EMIT playerPause();});
}

void ManageAudioPlayer::triggerStop()
{
    QTimer::singleShot(0, [this]() {Q_EMIT playerStop();});
}

void ManageAudioPlayer::triggerSkipNextTrack()
{
    QTimer::singleShot(0, [this]() {Q_EMIT skipNextTrack();});
}


#include "moc_manageaudioplayer.cpp"
