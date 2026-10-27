/*
   SPDX-FileCopyrightText: 2025 (c) Pedro Nishiyama <nishiyama.v3@gmail.com>
   SPDX-FileCopyrightText: 2026 Ian Monroe <imonroe@kde.org>

   SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "androidplayer.h"
#include "mediaplaylist.h"
#include "playerLogging.h"

#include <QTimer>

namespace
{
constexpr QLatin1StringView playbackStateProperty{"PlaybackState"};
constexpr QLatin1StringView positionProperty{"Position"};
constexpr QLatin1StringView metadataProperty{"Metadata"};

constexpr qint64 positionReportIntervalMs = 1000;
}

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
        qCWarning(orgKdeElisaPlayer) << "AndroidPlayer::initialize() called without an audio player manager";
        return;
    }

    connect(m_manageAudioPlayer, &ManageAudioPlayer::seek, this, &AndroidPlayer::playerPositionChanged);
    connect(m_manageAudioPlayer, &ManageAudioPlayer::playerPositionChanged, this, &AndroidPlayer::reportPlayerPosition);
    connect(m_manageAudioPlayer, &ManageAudioPlayer::playerStatusChanged, this, &AndroidPlayer::playerStatusChanged);
    connect(m_manageAudioPlayer, &ManageAudioPlayer::playerPlaybackStateChanged, this, &AndroidPlayer::playerPlaybackStateChanged);
    connect(m_manageAudioPlayer, &ManageAudioPlayer::currentTrackChanged, this, &AndroidPlayer::playerStatusChanged);
    connect(m_manageAudioPlayer, &ManageAudioPlayer::audioDurationChanged, this, &AndroidPlayer::playerStatusChanged);

    connect(m_androidplayerjni, &AndroidPlayerJni::Next, this, &AndroidPlayer::next);
    connect(m_androidplayerjni, &AndroidPlayerJni::Previous, this, &AndroidPlayer::previous);
    connect(m_androidplayerjni, &AndroidPlayerJni::PlayPause, this, &AndroidPlayer::playPause);
    connect(m_androidplayerjni, &AndroidPlayerJni::Play, this, &AndroidPlayer::play);
    connect(m_androidplayerjni, &AndroidPlayerJni::Pause, this, &AndroidPlayer::pause);
    connect(m_androidplayerjni, &AndroidPlayerJni::Stop, this, &AndroidPlayer::stop);
    connect(m_androidplayerjni, &AndroidPlayerJni::Seek, this, &AndroidPlayer::seek);
}

void AndroidPlayer::playerStatusChanged()
{
    if (m_metadataPushScheduled) {
        return;
    }

    m_metadataPushScheduled = true;
    QTimer::singleShot(0, this, [this]() {
        m_metadataPushScheduled = false;
        pushMetadata();
    });
}

void AndroidPlayer::pushMetadata()
{
    const auto status = m_manageAudioPlayer->playerStatus();
    if (status != QMediaPlayer::LoadedMedia && status != QMediaPlayer::BufferedMedia) {
        return;
    }

    QVariantMap metadata;

    const auto currentTrack = m_manageAudioPlayer->currentTrack();

    metadata[QStringLiteral("title")] = currentTrack.data(MediaPlayList::TitleRole).toString();
    metadata[QStringLiteral("artist")] = currentTrack.data(MediaPlayList::ArtistRole).toString();
    metadata[QStringLiteral("albumName")] = currentTrack.data(MediaPlayList::AlbumRole).toString();
    metadata[QStringLiteral("albumCover")] = currentTrack.data(MediaPlayList::ImageUrlRole).toString();
    metadata[QStringLiteral("duration")] = qlonglong(currentTrack.data(MediaPlayList::DurationRole).toTime().msecsSinceStartOfDay());

    AndroidPlayerJni::propertyChanged(metadataProperty, metadata);
}

void AndroidPlayer::playerPlaybackStateChanged()
{
    AndroidPlayerJni::propertyChanged(playbackStateProperty, m_manageAudioPlayer->playerPlaybackState());
}

void AndroidPlayer::playerPositionChanged(qint64 position)
{
    m_reportedPosition = position;
    m_positionReportTimer.restart();
    AndroidPlayerJni::propertyChanged(positionProperty, position);
}

void AndroidPlayer::reportPlayerPosition()
{
    const qint64 position = m_manageAudioPlayer->playerPosition();

    if (m_positionReportTimer.isValid() && m_positionReportTimer.elapsed() < positionReportIntervalMs
        && qAbs(position - m_reportedPosition) < positionReportIntervalMs) {
        return;
    }

    playerPositionChanged(position);
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

void AndroidPlayer::play()
{
    m_manageAudioPlayer->ensurePlay();
    playerPositionChanged(m_manageAudioPlayer->playerPosition());
}

void AndroidPlayer::pause()
{
    m_manageAudioPlayer->ensurePause();
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
