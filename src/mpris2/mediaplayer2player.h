/***************************************************************************
 *   Copyright 2014 Sujith Haridasan <sujith.haridasan@kdemail.net>        *
 *   Copyright 2014 Ashish Madeti <ashishmadeti@gmail.com>                 *
 *   Copyright 2016 Matthieu Gallien <mgallien@mgallien.fr>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#ifndef MEDIAPLAYER2PLAYER_H
#define MEDIAPLAYER2PLAYER_H

#include "elisaLib_export.h"

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>
#include <QDBusMessage>

class MediaPlayList;
class ManageAudioPlayer;
class ManageMediaPlayerControl;
class ManageHeaderBar;
class AudioWrapper;

class ELISALIB_EXPORT MediaPlayer2Player : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2.Player") // Docs: https://specifications.freedesktop.org/mpris-spec/latest/Player_Interface.html

    Q_PROPERTY(QString PlaybackStatus READ PlaybackStatus NOTIFY playbackStatusChanged)
    Q_PROPERTY(double Rate READ Rate WRITE setRate NOTIFY rateChanged)
    Q_PROPERTY(QVariantMap Metadata READ Metadata NOTIFY playbackStatusChanged)
    Q_PROPERTY(double Volume READ Volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(qlonglong Position READ Position WRITE setPropertyPosition NOTIFY playbackStatusChanged)
    Q_PROPERTY(double MinimumRate READ MinimumRate CONSTANT)
    Q_PROPERTY(double MaximumRate READ MaximumRate CONSTANT)
    Q_PROPERTY(bool CanGoNext READ CanGoNext NOTIFY canGoNextChanged)
    Q_PROPERTY(bool CanGoPrevious READ CanGoPrevious NOTIFY canGoPreviousChanged)
    Q_PROPERTY(bool CanPlay READ CanPlay NOTIFY canPlayChanged)
    Q_PROPERTY(bool CanPause READ CanPause NOTIFY canPauseChanged)
    Q_PROPERTY(bool CanControl READ CanControl NOTIFY canControlChanged)
    Q_PROPERTY(bool CanSeek READ CanSeek NOTIFY canSeekChanged)
    Q_PROPERTY(int currentTrack READ currentTrack WRITE setCurrentTrack NOTIFY currentTrackChanged)
    Q_PROPERTY(int mediaPlayerPresent READ mediaPlayerPresent WRITE setMediaPlayerPresent NOTIFY mediaPlayerPresentChanged)

public:
    explicit MediaPlayer2Player(MediaPlayList *playListControler,
                                ManageAudioPlayer *manageAudioPlayer,
                                ManageMediaPlayerControl* manageMediaPlayerControl,
                                ManageHeaderBar * manageHeaderBar,
                                AudioWrapper *audioPlayer,
                                QObject* parent = nullptr);
    ~MediaPlayer2Player() override;

    QString PlaybackStatus() const;
    double Rate() const;
    QVariantMap Metadata() const;
    double Volume() const;
    qlonglong Position() const;
    double MinimumRate() const;
    double MaximumRate() const;
    bool CanGoNext() const;
    bool CanGoPrevious() const;
    bool CanPlay() const;
    bool CanPause() const;
    bool CanSeek() const;
    bool CanControl() const;
    int currentTrack() const;
    int mediaPlayerPresent() const;

Q_SIGNALS:
    void Seeked(qlonglong Position);

    void rateChanged(double newRate);
    void volumeChanged(double newVol);
    void playbackStatusChanged();
    void canGoNextChanged();
    void canGoPreviousChanged();
    void canPlayChanged();
    void canPauseChanged();
    void canControlChanged();
    void canSeekChanged();
    void currentTrackChanged();
    void mediaPlayerPresentChanged();
    void next();
    void previous();
    void playPause();
    void stop();

public Q_SLOTS:

    void emitSeeked(int pos);

    void Next();
    void Previous();
    void Pause();
    void PlayPause();
    void Stop();
    void Play();
    void Seek(qlonglong Offset);
    void SetPosition(const QDBusObjectPath &trackId, qlonglong pos);
    void OpenUri(const QString &uri);

private Q_SLOTS:

    void playerSourceChanged();

    void playControlEnabledChanged();

    void skipBackwardControlEnabledChanged();

    void skipForwardControlEnabledChanged();

    void playerPlaybackStateChanged();

    void playerIsSeekableChanged();

    void audioPositionChanged();

    void audioDurationChanged();

    void playerVolumeChanged();

private:
    void signalPropertiesChange(const QString &property, const QVariant &value);

    void setMediaPlayerPresent(int status);
    void setRate(double newRate);
    void setVolume(double volume);
    void setPropertyPosition(int newPositionInMs);
    void setCurrentTrack(int newTrackPosition);

    QVariantMap getMetadataOfCurrentTrack();

    QVariantMap m_metadata;
    QString m_currentTrack;
    QString m_currentTrackId;
    double m_rate = 1.0;
    double m_volume = 0.0;
    int m_mediaPlayerPresent = 0;
    bool m_canPlay = false;
    bool m_canGoNext = false;
    bool m_canGoPrevious = false;
    qlonglong m_position = 0;
    MediaPlayList *m_playListControler = nullptr;
    bool m_playerIsSeekableChanged = false;
    ManageAudioPlayer* m_manageAudioPlayer = nullptr;
    ManageMediaPlayerControl* m_manageMediaPlayerControl = nullptr;
    ManageHeaderBar * m_manageHeaderBar = nullptr;
    AudioWrapper *m_audioPlayer = nullptr;
    mutable QDBusMessage mProgressIndicatorSignal;
    int mPreviousProgressPosition = 0;
};

#endif // MEDIAPLAYER2PLAYER_H
