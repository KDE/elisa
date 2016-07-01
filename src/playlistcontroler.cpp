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

#include "playlistcontroler.h"

#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtCore/QDebug>

#include <cstdlib>

PlayListControler::PlayListControler(QObject *parent)
    : QObject(parent), mPlayListModel(nullptr), mCurrentTrack(), mUrlRole(Qt::DisplayRole),
      mIsPlayingRole(Qt::DisplayRole), mArtistRole(Qt::DisplayRole), mTitleRole(Qt::DisplayRole),
      mAlbumRole(Qt::DisplayRole), mImageRole(Qt::DisplayRole), mPlayerState(PlayListControler::PlayerState::Stopped),
      mAudioPosition(0), mPlayControlPosition(0), mRandomPlay(false), mRepeatPlay(false), mIsInPlayingState(false)
{
}

QUrl PlayListControler::playerSource() const
{
    if (!mPlayListModel) {
        return QUrl();
    }

    return mPlayListModel->data(mCurrentTrack, mUrlRole).toUrl();
}

bool PlayListControler::playControlEnabled() const
{
    if (!mPlayListModel) {
        return false;
    }

    if (!mCurrentTrack.isValid()) {
        return mPlayListModel->rowCount() > 0;
    }

    return mPlayListModel->rowCount(mCurrentTrack.parent()) > 0;
}

bool PlayListControler::skipBackwardControlEnabled() const
{
    if (!mPlayListModel) {
        return false;
    }

    if (!mCurrentTrack.isValid()) {
        return false;
    }

    return mCurrentTrack.row() > 0 && mIsInPlayingState;
}

bool PlayListControler::skipForwardControlEnabled() const
{
    if (!mPlayListModel) {
        return false;
    }

    if (!mCurrentTrack.isValid()) {
        return false;
    }

    return (mCurrentTrack.row() < mPlayListModel->rowCount(mCurrentTrack.parent()) - 1) && mIsInPlayingState;
}

bool PlayListControler::musicPlaying() const
{
    return mPlayerState == PlayListControler::PlayerState::Playing;
}

void PlayListControler::setPlayListModel(QAbstractItemModel *aPlayListModel)
{
    if (mPlayListModel) {
        disconnect(mPlayListModel);
    }

    mPlayListModel = aPlayListModel;

    connect(mPlayListModel, &QAbstractItemModel::rowsInserted, this, &PlayListControler::tracksInserted);
    connect(mPlayListModel, &QAbstractItemModel::rowsMoved, this, &PlayListControler::tracksMoved);
    connect(mPlayListModel, &QAbstractItemModel::rowsRemoved, this, &PlayListControler::tracksRemoved);
    connect(mPlayListModel, &QAbstractItemModel::modelReset, this, &PlayListControler::playListReset);
    connect(mPlayListModel, &QAbstractItemModel::layoutChanged, this, &PlayListControler::playListLayoutChanged);

    Q_EMIT playListModelChanged();
    playListReset();
}

QAbstractItemModel *PlayListControler::playListModel() const
{
    return mPlayListModel;
}

void PlayListControler::setUrlRole(int value)
{
    mUrlRole = value;
    Q_EMIT urlRoleChanged();
}

int PlayListControler::urlRole() const
{
    return mUrlRole;
}

void PlayListControler::setIsPlayingRole(int value)
{
    mIsPlayingRole = value;
    Q_EMIT isPlayingRoleChanged();
}

int PlayListControler::isPlayingRole() const
{
    return mIsPlayingRole;
}

void PlayListControler::setArtistRole(int value)
{
    mArtistRole = value;
    Q_EMIT artistRoleChanged();
}

int PlayListControler::artistRole() const
{
    return mArtistRole;
}

void PlayListControler::setTitleRole(int value)
{
    mTitleRole = value;
    Q_EMIT titleRoleChanged();
}

int PlayListControler::titleRole() const
{
    return mTitleRole;
}

void PlayListControler::setAlbumRole(int value)
{
    mAlbumRole = value;
    Q_EMIT albumRoleChanged();
}

int PlayListControler::albumRole() const
{
    return mAlbumRole;
}

void PlayListControler::setImageRole(int value)
{
    mImageRole = value;
    Q_EMIT imageRoleChanged();
}

int PlayListControler::imageRole() const
{
    return mImageRole;
}

void PlayListControler::setAudioPosition(int value)
{
    mAudioPosition = value;
    Q_EMIT audioPositionChanged();
}

int PlayListControler::audioPosition() const
{
    return mAudioPosition;
}

void PlayListControler::setAudioDuration(int value)
{
    mAudioDuration = value;
    Q_EMIT audioDurationChanged();
}

int PlayListControler::audioDuration() const
{
    return mAudioDuration;
}

void PlayListControler::setPlayControlPosition(int value)
{
    mPlayControlPosition = value;
    Q_EMIT playControlPositionChanged();
}

int PlayListControler::playControlPosition() const
{
    return mPlayControlPosition;
}

QVariant PlayListControler::album() const
{
    if (!mCurrentTrack.isValid()) {
        return QStringLiteral("");
    }

    return mCurrentTrack.data(mAlbumRole);
}

QVariant PlayListControler::title() const
{
    if (!mCurrentTrack.isValid()) {
        return QStringLiteral("");
    }

    return mCurrentTrack.data(mTitleRole);
}

QVariant PlayListControler::artist() const
{
    if (!mCurrentTrack.isValid()) {
        return QStringLiteral("");
    }

    return mCurrentTrack.data(mArtistRole);
}

QVariant PlayListControler::image() const
{
    if (!mCurrentTrack.isValid()) {
        return QStringLiteral("");
    }

    return mCurrentTrack.data(mImageRole);
}

int PlayListControler::remainingTracks() const
{
    if (!mCurrentTrack.isValid()) {
        return -1;
    }

    return mPlayListModel->rowCount(mCurrentTrack.parent()) - mCurrentTrack.row() - 1;
}

void PlayListControler::setRandomPlay(bool value)
{
    mRandomPlay = value;
    Q_EMIT randomPlayChanged();
}

bool PlayListControler::randomPlay() const
{
    return mRandomPlay;
}

void PlayListControler::setRepeatPlay(bool value)
{
    mRepeatPlay = value;
    Q_EMIT repeatPlayChanged();
}

bool PlayListControler::repeatPlay() const
{
    return mRepeatPlay;
}

bool PlayListControler::musicPlayerStopped() const
{
    return mPlayerState == PlayerState::Stopped;
}

int PlayListControler::currentTrackPosition() const
{
    return mCurrentTrack.row();
}

void PlayListControler::playListReset()
{
    if (!mCurrentTrack.isValid()) {
        resetCurrentTrack();
        Q_EMIT playControlEnabledChanged();
        return;
    }

    Q_EMIT remainingTracksChanged();
    Q_EMIT playControlEnabledChanged();
    Q_EMIT skipBackwardControlEnabledChanged();
    Q_EMIT skipForwardControlEnabledChanged();
}

void PlayListControler::playListLayoutChanged(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint)
{
    Q_UNUSED(parents);
    Q_UNUSED(hint);

    qDebug() << "PlayListControler::playListLayoutChanged" << "not implemented";
}

void PlayListControler::tracksInserted(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);
    Q_UNUSED(first);
    Q_UNUSED(last);

    if (!mCurrentTrack.isValid()) {
        resetCurrentTrack();
    }

    Q_EMIT remainingTracksChanged();
    Q_EMIT playControlEnabledChanged();
    Q_EMIT skipForwardControlEnabledChanged();
}

void PlayListControler::tracksMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    Q_UNUSED(destination);
    Q_UNUSED(row);

    qDebug() << "PlayListControler::tracksMoved" << "not implemented";
}

void PlayListControler::tracksRemoved(const QModelIndex &parent, int first, int last)
{
    if (mCurrentTrack.parent() != parent) {
        Q_EMIT remainingTracksChanged();
        Q_EMIT playControlEnabledChanged();
        Q_EMIT skipBackwardControlEnabledChanged();
        Q_EMIT skipForwardControlEnabledChanged();
        return;
    }

    if (mCurrentTrack.row() < first || mCurrentTrack.row() > last) {
        if (mCurrentTrack.row() > last) {
            mCurrentTrack = mPlayListModel->index(mCurrentTrack.row() - (last - first + 1), 0);
        }

        Q_EMIT remainingTracksChanged();
        Q_EMIT playControlEnabledChanged();
        Q_EMIT skipBackwardControlEnabledChanged();
        Q_EMIT skipForwardControlEnabledChanged();
        return;
    }

    if (mPlayListModel->rowCount(parent) <= first) {
        stopPlayer();
        resetCurrentTrack();
        return;
    }

    mCurrentTrack = mPlayListModel->index(first, 0);
    signaTrackChange();

    if (mIsInPlayingState) {
        // one cannot call startPlayer directly here or the isPlaying information will end up in the wrong delegate instance
        // seems something to check
        QTimer::singleShot(0, [this]() {this->startPlayer();});
    }
}

void PlayListControler::playerPaused()
{
    mPlayerState = PlayListControler::PlayerState::Paused;
    Q_EMIT musicPlayingChanged();
}

void PlayListControler::playerPlaying()
{
    mPlayerState = PlayListControler::PlayerState::Playing;
    mPlayListModel->setData(mCurrentTrack, true, mIsPlayingRole);
    Q_EMIT musicPlayingChanged();
    Q_EMIT musicPlayerStoppedChanged();
}

void PlayListControler::playerStopped()
{
    mPlayListModel->setData(mCurrentTrack, false, mIsPlayingRole);
    mPlayerState = PlayListControler::PlayerState::Stopped;
    Q_EMIT musicPlayingChanged();
    Q_EMIT musicPlayerStoppedChanged();
}

void PlayListControler::skipNextTrack()
{
    bool wasPlaying = mIsInPlayingState;
    stopPlayer();

    if (!mPlayListModel) {
        return;
    }

    if (!mCurrentTrack.isValid()) {
        return;
    }

    if (mCurrentTrack.row() >= mPlayListModel->rowCount(mCurrentTrack.parent()) - 1) {
        return;
    }

    mCurrentTrack = mPlayListModel->index(mCurrentTrack.row() + 1, mCurrentTrack.column(), mCurrentTrack.parent());
    signaTrackChange();
    mIsInPlayingState = wasPlaying;
    if (mIsInPlayingState) {
        startPlayer();
    }
}

void PlayListControler::skipPreviousTrack()
{
    bool wasPlaying = mIsInPlayingState;
    stopPlayer();

    if (!mPlayListModel) {
        return;
    }

    if (!mCurrentTrack.isValid()) {
        return;
    }

    if (mCurrentTrack.row() <= 0) {
        return;
    }

    mCurrentTrack = mPlayListModel->index(mCurrentTrack.row() - 1, mCurrentTrack.column(), mCurrentTrack.parent());
    signaTrackChange();
    mIsInPlayingState = wasPlaying;
    if (mIsInPlayingState) {
        startPlayer();
    }
}

void PlayListControler::playPause()
{
    switch(mPlayerState)
    {
    case PlayerState::Stopped:
        startPlayer();
        break;
    case PlayerState::Playing:
        pausePlayer();
        break;
    case PlayerState::Paused:
        startPlayer();
        break;
    }
}

void PlayListControler::playerSeek(int position)
{
    mAudioPosition = position;
    Q_EMIT audioPositionChanged();
    mPlayControlPosition = position;
    Q_EMIT playControlPositionChanged();
}

void PlayListControler::audioPlayerPositionChanged(int position)
{
    mRealAudioPosition = position;
    mAudioPosition = position;
    mPlayControlPosition = mRealAudioPosition;
    Q_EMIT playControlPositionChanged();
}

void PlayListControler::audioPlayerFinished(bool finished)
{
    if (finished) {
        gotoNextTrack();
    }
}

void PlayListControler::skipToTrack(int position)
{
    if (!mPlayListModel) {
        return;
    }

    if (!mCurrentTrack.isValid()) {
        return;
    }

    mCurrentTrack = mPlayListModel->index(position, mCurrentTrack.column(), mCurrentTrack.parent());

    signaTrackChange();
    if (mIsInPlayingState) {
        startPlayer();
    }
}

void PlayListControler::startPlayer()
{
    mIsInPlayingState = true;
    Q_EMIT skipForwardControlEnabledChanged();
    Q_EMIT skipBackwardControlEnabledChanged();

    if (!mCurrentTrack.isValid()) {
        resetCurrentTrack();
    }

    Q_EMIT playMusic();
}

void PlayListControler::pausePlayer()
{
    Q_EMIT pauseMusic();
}

void PlayListControler::stopPlayer()
{
    mIsInPlayingState = false;

    Q_EMIT skipForwardControlEnabledChanged();
    Q_EMIT skipBackwardControlEnabledChanged();
    Q_EMIT stopMusic();
}

void PlayListControler::gotoNextTrack()
{
    if (!mPlayListModel) {
        return;
    }

    if (!mCurrentTrack.isValid()) {
        return;
    }

    if (!mRandomPlay && (mCurrentTrack.row() >= mPlayListModel->rowCount(mCurrentTrack.parent()) - 1)) {
        resetCurrentTrack();
        if (mRepeatPlay) {
            startPlayer();
        } else {
            stopPlayer();
        }
        return;
    }

    if (mRandomPlay) {
        static bool firstTime = true;
        if (firstTime) {
            qsrand(QTime::currentTime().msec());
            firstTime = false;
        }
        int randomValue = qrand();
        randomValue = randomValue % (mPlayListModel->rowCount(mCurrentTrack.parent()) + 1);
        mCurrentTrack = mPlayListModel->index(randomValue, mCurrentTrack.column(), mCurrentTrack.parent());
    } else {
        mCurrentTrack = mPlayListModel->index(mCurrentTrack.row() + 1, mCurrentTrack.column(), mCurrentTrack.parent());
    }
    signaTrackChange();
    if (mIsInPlayingState) {
        startPlayer();
    }
}

void PlayListControler::signaTrackChange()
{
    Q_EMIT playerSourceChanged();
    Q_EMIT currentTrackPositionChanged();
    Q_EMIT artistChanged();
    Q_EMIT titleChanged();
    Q_EMIT albumChanged();
    Q_EMIT imageChanged();
    Q_EMIT remainingTracksChanged();
    Q_EMIT skipBackwardControlEnabledChanged();
    Q_EMIT skipForwardControlEnabledChanged();
}

void PlayListControler::resetCurrentTrack()
{
    mCurrentTrack = mPlayListModel->index(0, 0);
    signaTrackChange();
}


#include "moc_playlistcontroler.cpp"
