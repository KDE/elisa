/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef AUDIOWRAPPER_H
#define AUDIOWRAPPER_H

#include <QObject>
#include <QUrl>
#include <QMediaPlayer>
#include <QString>

#include <memory>

class AudioWrapperPrivate;

class AudioWrapper : public QObject
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

    void mutedChanged();

    void volumeChanged();

    void sourceChanged();

    void statusChanged();

    void playbackStateChanged();

    void errorChanged();

    void durationChanged();

    void positionChanged();

    void seekableChanged();

    void playing();

    void paused();

    void stopped();

    void audioRoleChanged();

public Q_SLOTS:

    void setMuted(bool muted);

    void setVolume(qreal volume);

    void setSource(const QUrl &source);

    void setPosition(qint64 position);

    void play();

    void pause();

    void stop();

    void seek(int position);

    void setAudioRole(QAudio::Role audioRole);

private Q_SLOTS:

    void playerStateChanged();

    void playerVolumeChanged();

    void playerMutedChanged();

private:

    std::unique_ptr<AudioWrapperPrivate> d;

};

#endif // AUDIOWRAPPER_H
