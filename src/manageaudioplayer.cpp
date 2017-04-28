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

#include <QTimer>

ManageAudioPlayer::ManageAudioPlayer(QObject *parent) : QObject(parent)
{

}

QPersistentModelIndex ManageAudioPlayer::currentTrack() const
{
    return mCurrentTrack;
}

QAbstractItemModel *ManageAudioPlayer::playListModel() const
{
    return mPlayListModel;
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

int ManageAudioPlayer::playerPosition() const
{
    return mPlayerPosition;
}

int ManageAudioPlayer::playControlPosition() const
{
    return mPlayerPosition;
}

QVariantMap ManageAudioPlayer::persistentState() const
{
    auto persistentStateValue = QVariantMap();

    persistentStateValue[QStringLiteral("isPlaying")] = mPlayingState;
    persistentStateValue[QStringLiteral("playerPosition")] = mPlayerPosition;

    return persistentStateValue;
}

int ManageAudioPlayer::playListPosition() const
{
    if (mCurrentTrack.isValid()) {
        return mCurrentTrack.row();
    }

    return 0;
}

void ManageAudioPlayer::setCurrentTrack(const QPersistentModelIndex &currentTrack)
{
    if (mCurrentTrack == currentTrack) {
        return;
    }

    mOldCurrentTrack = mCurrentTrack;
    mCurrentTrack = currentTrack;
    Q_EMIT currentTrackChanged();

    switch (mPlayerPlaybackState) {
    case StoppedState:
        notifyPlayerSourceProperty();
        break;
    case PlayingState:
    case PausedState:
        triggerStop();
        mSkippingCurrentTrack = true;
        break;
    }
}

void ManageAudioPlayer::setPlayListModel(QAbstractItemModel *aPlayListModel)
{
    if (mPlayListModel == aPlayListModel) {
        return;
    }

    if (mPlayListModel) {
        disconnect(mPlayListModel, &QAbstractItemModel::dataChanged, this, &ManageAudioPlayer::tracksDataChanged);
    }

    mPlayListModel = aPlayListModel;

    if (mPlayListModel) {
        connect(mPlayListModel, &QAbstractItemModel::dataChanged, this, &ManageAudioPlayer::tracksDataChanged);
    }

    Q_EMIT playListModelChanged();
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

    switch (mPlayerStatus) {
    case NoMedia:
        break;
    case Loading:
        break;
    case Loaded:
        break;
    case Buffering:
        if (isFirstPlayTriggerPlay) {
            isFirstPlayTriggerPlay = false;
            auto isPlaying = mPersistentState.find(QStringLiteral("isPlaying"));
            if (isPlaying != mPersistentState.end()) {
                mPlayingState = isPlaying->toBool();
            }
        }
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
    qDebug() << "ManageAudioPlayer::setPlayerPlaybackState" << mPlayerPlaybackState;
    Q_EMIT playerPlaybackStateChanged();

    if (!mSkippingCurrentTrack) {
        switch(mPlayerPlaybackState) {
        case StoppedState:
            if (mPlayerStatus == EndOfMedia || mPlayerStatus == InvalidMedia) {
                triggerSkipNextTrack();
            }
            if (mPlayListModel && mCurrentTrack.isValid()) {
                mPlayListModel->setData(mCurrentTrack, false, mIsPlayingRole);
            }
            break;
        case PlayingState:
            if (isFirstPlayTriggerSeek) {
                isFirstPlayTriggerSeek = false;
                auto playerPosition = mPersistentState.find(QStringLiteral("playerPosition"));
                if (playerPosition != mPersistentState.end()) {
                    mPlayerPosition = playerPosition->toInt();
                    Q_EMIT seek(mPlayerPosition);
                }
            }
            if (mPlayListModel && mCurrentTrack.isValid()) {
                mPlayListModel->setData(mCurrentTrack, true, mIsPlayingRole);
            }
            break;
        case PausedState:
            break;
        }
    } else {
        switch(mPlayerPlaybackState) {
        case StoppedState:
            notifyPlayerSourceProperty();
            mSkippingCurrentTrack = false;
            if (mPlayListModel && mOldCurrentTrack.isValid()) {
                mPlayListModel->setData(mOldCurrentTrack, false, mIsPlayingRole);
            }
            break;
        case PlayingState:
            if (isFirstPlayTriggerSeek) {
                isFirstPlayTriggerSeek = false;
                auto playerPosition = mPersistentState.find(QStringLiteral("playerPosition"));
                if (playerPosition != mPersistentState.end()) {
                    mPlayerPosition = playerPosition->toInt();
                    Q_EMIT seek(mPlayerPosition);
                }
            }
            if (mPlayListModel && mCurrentTrack.isValid()) {
                mPlayListModel->setData(mCurrentTrack, true, mIsPlayingRole);
            }
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
}

void ManageAudioPlayer::ensurePlay()
{
    if (!mPlayingState) {
        playPause();
    }
}

void ManageAudioPlayer::playPause()
{
    mPlayingState = !mPlayingState;

    switch (mPlayerStatus) {
    case Loaded:
    case Buffering:
        if (isFirstPlayTriggerPlay) {
            isFirstPlayTriggerPlay = false;
            auto isPlaying = mPersistentState.find(QStringLiteral("isPlaying"));
            if (isPlaying != mPersistentState.end()) {
                mPlayingState = isPlaying->toBool();
            }
        }
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

void ManageAudioPlayer::setPlayerPosition(int playerPosition)
{
    if (mPlayerPosition == playerPosition) {
        return;
    }

    mPlayerPosition = playerPosition;
    Q_EMIT playerPositionChanged();
    QTimer::singleShot(0, [this]() {Q_EMIT playControlPositionChanged();});
}

void ManageAudioPlayer::setPlayControlPosition(int playerPosition)
{
    Q_EMIT seek(playerPosition);
}

void ManageAudioPlayer::setPersistentState(const QVariantMap &persistentStateValue)
{
    if (mPersistentState == persistentStateValue) {
        return;
    }

    mPersistentState = persistentStateValue;

    Q_EMIT persistentStateChanged();
}

void ManageAudioPlayer::playerSeek(int position)
{
    Q_EMIT seek(position);
}

void ManageAudioPlayer::playListFinished()
{
    mPlayingState = false;
}

void ManageAudioPlayer::tracksDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    if (!mCurrentTrack.isValid()) {
        return;
    }

    if (mCurrentTrack.row() > bottomRight.row() || mCurrentTrack.row() < topLeft.row()) {
        return;
    }

    if (mCurrentTrack.column() > bottomRight.column() || mCurrentTrack.column() < topLeft.column()) {
        return;
    }

    if (roles.isEmpty()) {
        notifyPlayerSourceProperty();
    } else {
        for(auto oneRole : roles) {
            if (oneRole == mUrlRole) {
                notifyPlayerSourceProperty();
            }
        }
    }
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
