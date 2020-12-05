/***************************************************************************
   SPDX-FileCopyrightText: 2014 (c) Sujith Haridasan <sujith.haridasan@kdemail.net>
   SPDX-FileCopyrightText: 2014 (c) Ashish Madeti <ashishmadeti@gmail.com>
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: GPL-3.0-or-later
 ***************************************************************************/

#ifndef MEDIAPLAYER2PLAYER_H
#define MEDIAPLAYER2PLAYER_H

#include "elisaLib_export.h"

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>
#include <QDBusMessage>

class MediaPlayListProxyModel;
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
    explicit MediaPlayer2Player(MediaPlayListProxyModel *playListControler,
                                ManageAudioPlayer *manageAudioPlayer,
                                ManageMediaPlayerControl* manageMediaPlayerControl,
                                ManageHeaderBar * manageHeaderBar,
                                AudioWrapper *audioPlayer,
                                bool showProgressOnTaskBar,
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

    bool showProgressOnTaskBar() const;
    void setShowProgressOnTaskBar(bool value);

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

    void playerSeeked(qint64 position);

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
    MediaPlayListProxyModel *m_playListControler = nullptr;
    bool m_playerIsSeekableChanged = false;
    ManageAudioPlayer* m_manageAudioPlayer = nullptr;
    ManageMediaPlayerControl* m_manageMediaPlayerControl = nullptr;
    ManageHeaderBar * m_manageHeaderBar = nullptr;
    AudioWrapper *m_audioPlayer = nullptr;
    mutable QDBusMessage mProgressIndicatorSignal;
    int mPreviousProgressPosition = 0;
    bool mShowProgressOnTaskBar = true;
};

#endif // MEDIAPLAYER2PLAYER_H
