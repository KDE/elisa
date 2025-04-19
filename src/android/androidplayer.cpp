/*
   SPDX-FileCopyrightText: 2025 (c) Pedro Nishiyama <nishiyama.v3@gmail.com>

   SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "androidplayer.h"
#include "mediaplaylist.h"

AndroidPlayer::AndroidPlayer(QObject *parent)
    : QObject(parent)
{
}

AndroidPlayer::~AndroidPlayer() = default;

ManageAudioPlayer *AndroidPlayer::audioPlayerManager() const
{
    return m_manageAudioPlayer;
}

void AndroidPlayer::setAudioPlayerManager(ManageAudioPlayer *manageAudioPlayer)
{
    if (m_manageAudioPlayer == manageAudioPlayer) {
        return;
    }

    m_manageAudioPlayer = manageAudioPlayer;

    Q_EMIT audioPlayerManagerChanged();
}

void AndroidPlayer::initialize()
{
    if (!m_manageAudioPlayer) {
        return;
    }

    connect(m_manageAudioPlayer, &ManageAudioPlayer::seek, this, &AndroidPlayer::playerPositionChanged);
    connect(m_manageAudioPlayer, &ManageAudioPlayer::playerStatusChanged, this, &AndroidPlayer::playerStatusChanged);
    connect(m_manageAudioPlayer, &ManageAudioPlayer::playerPlaybackStateChanged, this, &AndroidPlayer::playerPlaybackStateChanged);

    connect(m_androidplayerjni, &AndroidPlayerJni::Next, this, &AndroidPlayer::next);
    connect(m_androidplayerjni, &AndroidPlayerJni::Previous, this, &AndroidPlayer::previous);
    connect(m_androidplayerjni, &AndroidPlayerJni::PlayPause, this, &AndroidPlayer::playPause);
    connect(m_androidplayerjni, &AndroidPlayerJni::Stop, this, &AndroidPlayer::stop);
    connect(m_androidplayerjni, &AndroidPlayerJni::Seek, this, &AndroidPlayer::seek);
}

void AndroidPlayer::playerStatusChanged()
{   
    if (m_manageAudioPlayer->playerStatus() != QMediaPlayer::LoadedMedia) return;

    QVariantMap metadata;

    auto currentTrack = m_manageAudioPlayer->currentTrack();

    metadata[QStringLiteral("title")] = currentTrack.data(MediaPlayList::TitleRole).toString();
    metadata[QStringLiteral("artist")] = currentTrack.data(MediaPlayList::ArtistRole).toString();
    metadata[QStringLiteral("albumName")] = currentTrack.data(MediaPlayList::AlbumRole).toString();
    metadata[QStringLiteral("albumCover")] = currentTrack.data(MediaPlayList::ImageUrlRole).toString();
    metadata[QStringLiteral("duration")] = qlonglong(currentTrack.data(MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay());

    AndroidPlayerJni::propertyChanged(QStringLiteral("Metadata"), metadata);
}

void AndroidPlayer::playerPlaybackStateChanged()
{
    AndroidPlayerJni::propertyChanged(QStringLiteral("PlaybackState"), m_manageAudioPlayer->playerPlaybackState());
}

void AndroidPlayer::playerPositionChanged(qint64 position)
{
    AndroidPlayerJni::propertyChanged(QStringLiteral("Position"), position);
}

void AndroidPlayer::next()
{
    m_manageAudioPlayer->skipNextTrack(ElisaUtils::SkipReason::Manual);
}

void AndroidPlayer::previous()
{
    m_manageAudioPlayer->skipPreviousTrack(m_manageAudioPlayer->playerPosition());
    playerPositionChanged(m_manageAudioPlayer->playerPosition());
}

void AndroidPlayer::playPause()
{
    m_manageAudioPlayer->playPause();
    playerPositionChanged(m_manageAudioPlayer->playerPosition());
}

void AndroidPlayer::stop()
{
    m_manageAudioPlayer->stop();
}

void AndroidPlayer::seek(qlonglong offset)
{
    m_manageAudioPlayer->playerSeek(int(offset));
}

#include "moc_androidplayer.cpp"
