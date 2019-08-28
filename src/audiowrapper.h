/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef AUDIOWRAPPER_H
#define AUDIOWRAPPER_H

#include "elisaLib_export.h"

#include <QObject>
#include <QUrl>
#include <QMediaPlayer>
#include <QString>

#include <memory>

class AudioWrapperPrivate;

class ELISALIB_EXPORT AudioWrapper : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool muted
               READ muted
               WRITE setMuted
               NOTIFY mutedChanged)

    Q_PROPERTY(qreal volume
               READ volume
               WRITE setVolume
               NOTIFY volumeChanged)

    Q_PROPERTY(QUrl source
               READ source
               WRITE setSource
               NOTIFY sourceChanged)

    Q_PROPERTY(QMediaPlayer::MediaStatus status
               READ status
               NOTIFY statusChanged)

    Q_PROPERTY(QMediaPlayer::State playbackState
               READ playbackState
               NOTIFY playbackStateChanged)

    Q_PROPERTY(QMediaPlayer::Error error
               READ error
               NOTIFY errorChanged)

    Q_PROPERTY(qint64 duration
               READ duration
               NOTIFY durationChanged)

    Q_PROPERTY(qint64 position
               READ position
               WRITE setPosition
               NOTIFY positionChanged)

    Q_PROPERTY(bool seekable
               READ seekable
               NOTIFY seekableChanged)

    Q_PROPERTY(QAudio::Role audioRole
               READ audioRole
               WRITE setAudioRole
               NOTIFY audioRoleChanged)

public:

    explicit AudioWrapper(QObject *parent = nullptr);

    ~AudioWrapper() override;

    bool muted() const;

    qreal volume() const;

    QUrl source() const;

    QMediaPlayer::MediaStatus status() const;

    QMediaPlayer::State playbackState() const;

    QMediaPlayer::Error error() const;

    qint64 duration() const;

    qint64 position() const;

    bool seekable() const;

    QAudio::Role audioRole() const;

Q_SIGNALS:

    void mutedChanged(bool muted);

    void volumeChanged();

    void sourceChanged();

    void statusChanged(QMediaPlayer::MediaStatus status);

    void playbackStateChanged(QMediaPlayer::State state);

    void errorChanged(QMediaPlayer::Error error);

    void durationChanged(qint64 duration);

    void positionChanged(qint64 position);

    void currentPlayingForRadiosChanged(const QString &title, const QString &nowPlaying);

    void seekableChanged(bool seekable);

    void playing();

    void paused();

    void stopped();

    void audioRoleChanged();

public Q_SLOTS:

    void setMuted(bool muted);

    void setVolume(qreal volume);

    void setSource(const QUrl &source);

    void setPosition(qint64 position);

    void saveUndoPosition(qint64 position);

    void restoreUndoPosition();

    void play();

    void pause();

    void stop();

    void seek(qint64 position);

    void setAudioRole(QAudio::Role audioRole);

private Q_SLOTS:

    void mediaStatusChanged();

    void playerStateChanged();

    void playerMutedChanged();

    void playerVolumeChanged();

private:
    void savePosition(qint64 position);

    void playerStateSignalChanges(QMediaPlayer::State newState);

    void mediaStatusSignalChanges(QMediaPlayer::MediaStatus newStatus);

    void playerErrorSignalChanges(QMediaPlayer::Error error);

    void playerDurationSignalChanges(qint64 newDuration);

    void playerPositionSignalChanges(qint64 newPosition);

    void playerVolumeSignalChanges();

    void playerMutedSignalChanges(bool isMuted);

    void playerSeekableSignalChanges(bool isSeekable);

    friend class AudioWrapperPrivate;

    std::unique_ptr<AudioWrapperPrivate> d;

};

#endif // AUDIOWRAPPER_H
