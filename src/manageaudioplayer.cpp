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

#include "mediaplaylist.h"

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

QMediaPlayer::MediaStatus ManageAudioPlayer::playerStatus() const
{
    return mPlayerStatus;
}

int ManageAudioPlayer::playerPlaybackState() const
{
    return mPlayerPlaybackState;
}

QMediaPlayer::Error ManageAudioPlayer::playerError() const
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
    if (mCurrentTrack.isValid()) {
        persistentStateValue[QStringLiteral("audioPlayerCurrentTitle")] = mCurrentTrack.data(mTitleRole);
        persistentStateValue[QStringLiteral("audioPlayerCurrentArtistName")] = mCurrentTrack.data(mArtistNameRole);
        persistentStateValue[QStringLiteral("audioPlayerCurrentAlbumName")] = mCurrentTrack.data(mAlbumNameRole);
    } else {
        persistentStateValue[QStringLiteral("audioPlayerCurrentTitle")] = {};
        persistentStateValue[QStringLiteral("audioPlayerCurrentArtistName")] = {};
        persistentStateValue[QStringLiteral("audioPlayerCurrentAlbumName")] = {};
    }

    return persistentStateValue;
}

int ManageAudioPlayer::playListPosition() const
{
    if (mCurrentTrack.isValid()) {
        return mCurrentTrack.row();
    }

    return 0;
}

int ManageAudioPlayer::titleRole() const
{
    return mTitleRole;
}

int ManageAudioPlayer::artistNameRole() const
{
    return mArtistNameRole;
}

int ManageAudioPlayer::albumNameRole() const
{
    return mAlbumNameRole;
}

void ManageAudioPlayer::setCurrentTrack(const QPersistentModelIndex &currentTrack)
{
    if (mCurrentTrack == currentTrack) {
        return;
    }

    mOldCurrentTrack = mCurrentTrack;

    mCurrentTrack = currentTrack;

    if (mCurrentTrack.isValid()) {
        restorePreviousState();
    }

    mPlayerError = QMediaPlayer::NoError;

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
    restorePreviousState();
}

void ManageAudioPlayer::setIsPlayingRole(int value)
{
    if (mIsPlayingRole == value) {
        return;
    }

    mIsPlayingRole = value;
    Q_EMIT isPlayingRoleChanged();
}

void ManageAudioPlayer::setPlayerStatus(QMediaPlayer::MediaStatus playerStatus)
{
    if (mPlayerStatus == playerStatus) {
        return;
    }

    if (playerStatus < static_cast<int>(QMediaPlayer::UnknownMediaStatus) || playerStatus > static_cast<int>(QMediaPlayer::InvalidMedia)) {
        return;
    }

    mPlayerStatus = static_cast<QMediaPlayer::MediaStatus>(playerStatus);
    Q_EMIT playerStatusChanged();

    switch (mPlayerStatus) {
    case QMediaPlayer::NoMedia:
        break;
    case QMediaPlayer::LoadingMedia:
        break;
    case QMediaPlayer::LoadedMedia:
        if (mPlayingState) {
            triggerPlay();
        }
        break;
    case QMediaPlayer::BufferingMedia:
        break;
    case QMediaPlayer::StalledMedia:
        break;
    case QMediaPlayer::BufferedMedia:
        break;
    case QMediaPlayer::EndOfMedia:
        break;
    case QMediaPlayer::InvalidMedia:
        triggerSkipNextTrack();
        break;
    case QMediaPlayer::UnknownMediaStatus:
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

    if (!mSkippingCurrentTrack) {
        switch(mPlayerPlaybackState) {
        case StoppedState:
            if (mPlayerStatus == QMediaPlayer::EndOfMedia || mPlayerStatus == QMediaPlayer::InvalidMedia) {
                triggerSkipNextTrack();
            }
            if (mPlayListModel && mCurrentTrack.isValid()) {
                mPlayListModel->setData(mCurrentTrack, MediaPlayList::NotPlaying, mIsPlayingRole);
            }
            break;
        case PlayingState:
            if (mPlayListModel && mCurrentTrack.isValid()) {
                mPlayListModel->setData(mCurrentTrack, MediaPlayList::IsPlaying, mIsPlayingRole);
            }
            break;
        case PausedState:
            if (mPlayListModel && mCurrentTrack.isValid()) {
                mPlayListModel->setData(mCurrentTrack, MediaPlayList::IsPaused, mIsPlayingRole);
            }
            break;
        }
    } else {
        switch(mPlayerPlaybackState) {
        case StoppedState:
            notifyPlayerSourceProperty();
            mSkippingCurrentTrack = false;
            if (mPlayListModel && mOldCurrentTrack.isValid()) {
                mPlayListModel->setData(mOldCurrentTrack, MediaPlayList::NotPlaying, mIsPlayingRole);
            }
            break;
        case PlayingState:
            if (mPlayListModel && mCurrentTrack.isValid()) {
                mPlayListModel->setData(mCurrentTrack, MediaPlayList::IsPlaying, mIsPlayingRole);
            }
            break;
        case PausedState:
            if (mPlayListModel && mCurrentTrack.isValid()) {
                mPlayListModel->setData(mCurrentTrack, MediaPlayList::IsPaused, mIsPlayingRole);
            }
            break;
        }
    }
}

void ManageAudioPlayer::setPlayerError(QMediaPlayer::Error playerError)
{
    if (mPlayerError == playerError) {
        return;
    }

    mPlayerError = playerError;
    Q_EMIT playerErrorChanged();

    if (mPlayerError != QMediaPlayer::NoError) {
        Q_EMIT sourceInError(playerSource(), mPlayerError);
    }
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
    case QMediaPlayer::LoadedMedia:
    case QMediaPlayer::BufferingMedia:
    case QMediaPlayer::BufferedMedia:
    case QMediaPlayer::LoadingMedia:
        if (mPlayingState) {
            triggerPlay();
        } else {
            triggerPause();
        }
        break;
    case QMediaPlayer::EndOfMedia:
        if (mPlayerPlaybackState == PlayingState && !mPlayingState) {
            triggerPause();
        } else if (mPlayerPlaybackState == PausedState && mPlayingState) {
            triggerPlay();
        }
        break;
    case QMediaPlayer::NoMedia:
    case QMediaPlayer::StalledMedia:
    case QMediaPlayer::InvalidMedia:
    case QMediaPlayer::UnknownMediaStatus:
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

    if (mCurrentTrack.isValid()) {
        restorePreviousState();
    }
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
        restorePreviousState();
    } else {
        for(auto oneRole : roles) {
            if (oneRole == mUrlRole) {
                notifyPlayerSourceProperty();
                restorePreviousState();
            }
        }
    }
}

void ManageAudioPlayer::setTitleRole(int titleRole)
{
    if (mTitleRole == titleRole) {
        return;
    }

    mTitleRole = titleRole;
    Q_EMIT titleRoleChanged();

    if (mCurrentTrack.isValid()) {
        restorePreviousState();
    }
}

void ManageAudioPlayer::setArtistNameRole(int artistNameRole)
{
    if (mArtistNameRole == artistNameRole) {
        return;
    }

    mArtistNameRole = artistNameRole;
    Q_EMIT artistNameRoleChanged();

    if (mCurrentTrack.isValid()) {
        restorePreviousState();
    }
}

void ManageAudioPlayer::setAlbumNameRole(int albumNameRole)
{
    if (mAlbumNameRole == albumNameRole) {
        return;
    }

    mAlbumNameRole = albumNameRole;
    Q_EMIT albumNameRoleChanged();

    if (mCurrentTrack.isValid()) {
        restorePreviousState();
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

void ManageAudioPlayer::restorePreviousState()
{
    if (mPersistentState.isEmpty()) {
        return;
    }

    auto itTitle = mPersistentState.find(QStringLiteral("audioPlayerCurrentTitle"));
    auto itArtistName = mPersistentState.find(QStringLiteral("audioPlayerCurrentArtistName"));
    auto itAlbumName = mPersistentState.find(QStringLiteral("audioPlayerCurrentAlbumName"));

    if (itTitle == mPersistentState.end() || itArtistName == mPersistentState.end() ||
            itAlbumName == mPersistentState.end()) {
        return;
    }

    if (*itTitle != mCurrentTrack.data(mTitleRole) || *itArtistName != mCurrentTrack.data(mArtistNameRole) ||
            *itAlbumName != mCurrentTrack.data(mAlbumNameRole)) {
        if (mCurrentTrack.isValid() && mCurrentTrack.data(mTitleRole).isValid() && mCurrentTrack.data(mArtistNameRole).isValid() &&
                mCurrentTrack.data(mAlbumNameRole).isValid()) {
            mPersistentState.clear();
        }

        return;
    }

    if (!mCurrentTrack.data(mUrlRole).toUrl().isValid()) {
        return;
    }

    auto isPlaying = mPersistentState.find(QStringLiteral("isPlaying"));
    if (isPlaying != mPersistentState.end() && mPlayingState != isPlaying->toBool()) {
        mPlayingState = isPlaying->toBool();
    }

    auto playerPosition = mPersistentState.find(QStringLiteral("playerPosition"));
    if (playerPosition != mPersistentState.end()) {
        mPlayerPosition = playerPosition->toInt();
        Q_EMIT seek(mPlayerPosition);
    }

    mPersistentState.clear();
}


#include "moc_manageaudioplayer.cpp"
