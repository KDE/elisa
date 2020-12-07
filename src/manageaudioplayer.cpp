/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "manageaudioplayer.h"

#include "mediaplaylist.h"

#include "elisa_settings.h"

#include "playerLogging.h"

#include <QTimer>
#include <QDateTime>

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

QMediaPlayer::State ManageAudioPlayer::playerPlaybackState() const
{
    return mPlayerPlaybackState;
}

QMediaPlayer::Error ManageAudioPlayer::playerError() const
{
    return mPlayerError;
}

qint64 ManageAudioPlayer::audioDuration() const
{
    return mAudioDuration;
}

bool ManageAudioPlayer::playerIsSeekable() const
{
    return mPlayerIsSeekable;
}

qint64 ManageAudioPlayer::playerPosition() const
{
    return mPlayerPosition;
}

qint64 ManageAudioPlayer::playControlPosition() const
{
    return mPlayerPosition;
}

QVariantMap ManageAudioPlayer::persistentState() const
{
    auto persistentStateValue = QVariantMap();

    persistentStateValue[QStringLiteral("isPlaying")] = mPlayingState;

    persistentStateValue[QStringLiteral("playerPosition")] = mPlayerPosition;
    persistentStateValue[QStringLiteral("playerDuration")] = mAudioDuration;

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
    mOldCurrentTrack = mCurrentTrack;

    mCurrentTrack = currentTrack;

    if (mCurrentTrack.isValid()) {
        restorePreviousState();
    }

    mPlayerError = QMediaPlayer::NoError;

    if (mOldCurrentTrack != mCurrentTrack || mPlayingState) {
        Q_EMIT currentTrackChanged();
    }

    switch (mPlayerPlaybackState) {
    case QMediaPlayer::StoppedState:
        Q_EMIT playerSourceChanged(mCurrentTrack.data(mUrlRole).toUrl());
        break;
    case QMediaPlayer::PlayingState:
    case QMediaPlayer::PausedState:
        triggerStop();
        if (mPlayingState && !mCurrentTrack.isValid()) {
            mPlayingState = false;
        }
        mSkippingCurrentTrack = true;
        break;
    }
}

void ManageAudioPlayer::saveForUndoClearPlaylist(){
    mUndoPlayingState = mPlayingState;

    mUndoPlayerPosition = mPlayerPosition;
    Q_EMIT saveUndoPositionInAudioWrapper(mUndoPlayerPosition);
}

void ManageAudioPlayer::restoreForUndoClearPlaylist(){
    mPlayerPosition = mUndoPlayerPosition;
    Q_EMIT seek(mPlayerPosition);

    mPlayingState = mUndoPlayingState;
    Q_EMIT restoreUndoPositionInAudioWrapper();
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

    mPlayerStatus = playerStatus;
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

void ManageAudioPlayer::setPlayerPlaybackState(QMediaPlayer::State playerPlaybackState)
{
    if (mPlayerPlaybackState == playerPlaybackState) {
        return;
    }

    mPlayerPlaybackState = playerPlaybackState;
    Q_EMIT playerPlaybackStateChanged();

    if (!mSkippingCurrentTrack) {
        switch(mPlayerPlaybackState) {
        case QMediaPlayer::StoppedState:
            if (mPlayerStatus == QMediaPlayer::EndOfMedia || mPlayerStatus == QMediaPlayer::InvalidMedia) {
                triggerSkipNextTrack();
            }
            if (mPlayListModel && mCurrentTrack.isValid()) {
                mPlayListModel->setData(mCurrentTrack, MediaPlayList::NotPlaying, mIsPlayingRole);
            }
            break;
        case QMediaPlayer::PlayingState:
            if (mPlayListModel && mCurrentTrack.isValid()) {
                mPlayListModel->setData(mCurrentTrack, MediaPlayList::IsPlaying, mIsPlayingRole);
                Q_EMIT startedPlayingTrack(mCurrentTrack.data(mUrlRole).toUrl(), QDateTime::currentDateTime());
            }
            break;
        case QMediaPlayer::PausedState:
            if (mPlayListModel && mCurrentTrack.isValid()) {
                mPlayListModel->setData(mCurrentTrack, MediaPlayList::IsPaused, mIsPlayingRole);
            }
            break;
        }
    } else {
        switch(mPlayerPlaybackState) {
        case QMediaPlayer::StoppedState:
            notifyPlayerSourceProperty();
            mSkippingCurrentTrack = false;
            if (mPlayListModel && mOldCurrentTrack.isValid()) {
                mPlayListModel->setData(mOldCurrentTrack, MediaPlayList::NotPlaying, mIsPlayingRole);
            }
            break;
        case QMediaPlayer::PlayingState:
            if (mPlayListModel && mCurrentTrack.isValid()) {
                mPlayListModel->setData(mCurrentTrack, MediaPlayList::IsPlaying, mIsPlayingRole);
                Q_EMIT startedPlayingTrack(mCurrentTrack.data(mUrlRole).toUrl(), QDateTime::currentDateTime());
            }
            break;
        case QMediaPlayer::PausedState:
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
        auto currentSource = playerSource();

        Q_EMIT sourceInError(currentSource, mPlayerError);

        if (currentSource.isLocalFile()) {
            Q_EMIT displayTrackError(currentSource.toLocalFile());
        } else {
            Q_EMIT displayTrackError(currentSource.toString());
        }
    }
}

void ManageAudioPlayer::ensurePause()
{
    if (mPlayingState) {
        mPlayingState = false;
        triggerPause();
    }
}

void ManageAudioPlayer::ensurePlay()
{
    if (!mPlayingState) {
        mPlayingState = true;
        triggerPlay();
    }
}

void ManageAudioPlayer::stop()
{
    mPlayingState = false;
    triggerStop();
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
        if (mPlayerPlaybackState == QMediaPlayer::PlayingState && !mPlayingState) {
            triggerPause();
        } else if (mPlayerPlaybackState == QMediaPlayer::PausedState && mPlayingState) {
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

void ManageAudioPlayer::setAudioDuration(qint64 audioDuration)
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

void ManageAudioPlayer::setPlayerPosition(qint64 playerPosition)
{
    if (mPlayerPosition == playerPosition) {
        return;
    }

    mPlayerPosition = playerPosition;
    Q_EMIT playerPositionChanged();
    QTimer::singleShot(0, this, [this]() {Q_EMIT playControlPositionChanged();});
}

void ManageAudioPlayer::setCurrentPlayingForRadios(const QString &title, const QString &nowPlaying)
{
    if (mPlayListModel && mCurrentTrack.isValid()) {
        Q_EMIT updateData(mCurrentTrack, title, MediaPlayList::TitleRole);
        Q_EMIT updateData(mCurrentTrack, nowPlaying, MediaPlayList::ArtistRole);
    }
}

void ManageAudioPlayer::setPlayControlPosition(int playerPosition)
{
    qCDebug(orgKdeElisaPlayer()) << "ManageAudioPlayer::setPlayControlPosition" << playerPosition;
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
    qCDebug(orgKdeElisaPlayer()) << "ManageAudioPlayer::playerSeek" << position;
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
    if (mSkippingCurrentTrack || mOldPlayerSource != newUrlValue) {
        Q_EMIT playerSourceChanged(mCurrentTrack.data(mUrlRole).toUrl());

        mOldPlayerSource = newUrlValue;
    }
}

void ManageAudioPlayer::triggerPlay()
{
    QTimer::singleShot(0, this, [this]() {Q_EMIT playerPlay();});
}

void ManageAudioPlayer::triggerPause()
{
    QTimer::singleShot(0, this, [this]() {Q_EMIT playerPause();});
}

void ManageAudioPlayer::triggerStop()
{
    QTimer::singleShot(0, this, [this]() {Q_EMIT playerStop();});
}

void ManageAudioPlayer::triggerSkipNextTrack()
{
    QTimer::singleShot(0, this, [this]() {Q_EMIT skipNextTrack();});
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

    if (*itTitle != mCurrentTrack.data(mTitleRole) ||
            (itArtistName->isValid() && *itArtistName != mCurrentTrack.data(mArtistNameRole)) ||
            (itAlbumName->isValid() && *itAlbumName != mCurrentTrack.data(mAlbumNameRole))) {
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
    auto currentConfiguration = Elisa::ElisaConfiguration::self();
    bool autoPlay = currentConfiguration->playAtStartup();
    if (autoPlay && isPlaying != mPersistentState.end() && mPlayingState != isPlaying->toBool()) {
        mPlayingState = isPlaying->toBool();
    }

    auto playerPosition = mPersistentState.find(QStringLiteral("playerPosition"));
    if (playerPosition != mPersistentState.end()) {
        setPlayerPosition(playerPosition->toLongLong());
        Q_EMIT seek(mPlayerPosition);
    }
    auto playerDuration = mPersistentState.find(QStringLiteral("playerDuration"));
    if (playerDuration != mPersistentState.end()) {
        setAudioDuration(playerDuration->toInt());
    }

    mPersistentState.clear();
}


#include "moc_manageaudioplayer.cpp"
