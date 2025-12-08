/***************************************************************************
   SPDX-FileCopyrightText: 2014 (c) Sujith Haridasan <sujith.haridasan@kdemail.net>
   SPDX-FileCopyrightText: 2014 (c) Ashish Madeti <ashishmadeti@gmail.com>
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: GPL-3.0-or-later
 ***************************************************************************/

#include "config-upnp-qt.h"

#include "mediaplayer2player.h"
#include "mpris2.h"

#include "mediaplaylistproxymodel.h"
#include "manageaudioplayer.h"
#include "managemediaplayercontrol.h"
#include "manageheaderbar.h"
#include "audiowrapper.h"

#if KFFileMetaData_FOUND
#include <KFileMetaData/ExtractorCollection>
#include <KFileMetaData/Extractor>
#include <KFileMetaData/SimpleExtractionResult>
#include <KFileMetaData/EmbeddedImageData>
#include <QMimeDatabase>
#endif

#include <QCryptographicHash>
#include <QStringList>
#include <QDBusConnection>


static const double MAX_RATE = 1.0;
static const double MIN_RATE = 1.0;

MediaPlayer2Player::MediaPlayer2Player(MediaPlayListProxyModel *playListControler, ManageAudioPlayer *manageAudioPlayer,
                                       ManageMediaPlayerControl *manageMediaPlayerControl, ManageHeaderBar *manageHeaderBar,
                                       AudioWrapper *audioPlayer, bool showProgressOnTaskBar, QObject* parent)
    : QDBusAbstractAdaptor(parent), m_playListControler(playListControler), m_manageAudioPlayer(manageAudioPlayer),
      m_manageMediaPlayerControl(manageMediaPlayerControl), m_manageHeaderBar(manageHeaderBar), m_audioPlayer(audioPlayer),
      mProgressIndicatorSignal(QDBusMessage::createSignal(QStringLiteral("/org/kde/elisa"),
                                                          QStringLiteral("com.canonical.Unity.LauncherEntry"),
                                                          QStringLiteral("Update"))),
      mShowProgressOnTaskBar(showProgressOnTaskBar)
{
    if (!m_playListControler) {
        return;
    }

    connect(m_manageAudioPlayer, &ManageAudioPlayer::playerSourceChanged,
            this, &MediaPlayer2Player::playerSourceChanged, Qt::QueuedConnection);
    connect(m_manageMediaPlayerControl, &ManageMediaPlayerControl::playControlEnabledChanged,
            this, &MediaPlayer2Player::playControlEnabledChanged);
    connect(m_manageMediaPlayerControl, &ManageMediaPlayerControl::skipBackwardControlEnabledChanged,
            this, &MediaPlayer2Player::skipBackwardControlEnabledChanged);
    connect(m_manageMediaPlayerControl, &ManageMediaPlayerControl::skipForwardControlEnabledChanged,
            this, &MediaPlayer2Player::skipForwardControlEnabledChanged);
    connect(m_manageAudioPlayer, &ManageAudioPlayer::playerPlaybackStateChanged,
            this, &MediaPlayer2Player::playerPlaybackStateChanged);
    connect(m_manageAudioPlayer, &ManageAudioPlayer::playerIsSeekableChanged,
            this, &MediaPlayer2Player::playerIsSeekableChanged);
    connect(m_manageAudioPlayer, &ManageAudioPlayer::playerPositionChanged,
            this, &MediaPlayer2Player::audioPositionChanged);
    connect(m_manageAudioPlayer, &ManageAudioPlayer::seek,
            this, &MediaPlayer2Player::playerSeeked);
    connect(m_manageAudioPlayer, &ManageAudioPlayer::audioDurationChanged,
            this, &MediaPlayer2Player::audioDurationChanged);
    connect(m_audioPlayer, &AudioWrapper::volumeChanged,
            this, &MediaPlayer2Player::playerVolumeChanged);
    connect(m_playListControler, &MediaPlayListProxyModel::shuffleModeChanged,
            this, &MediaPlayer2Player::shuffleModeChanged);
    connect(m_playListControler, &MediaPlayListProxyModel::repeatModeChanged,
            this, &MediaPlayer2Player::repeatModeChanged);

    m_volume = m_audioPlayer->volume() / 100;
    m_canPlay = m_manageMediaPlayerControl->playControlEnabled();
    signalPropertiesChange(QStringLiteral("Volume"), Volume());

    m_mediaPlayerPresent = 1;
}

MediaPlayer2Player::~MediaPlayer2Player()
= default;

QString MediaPlayer2Player::PlaybackStatus() const
{
    QString result;

    if (!m_playListControler) {
        result = QStringLiteral("Stopped");
        return result;
    }

    if (m_manageAudioPlayer->playerPlaybackState() == QMediaPlayer::StoppedState) {
        result = QStringLiteral("Stopped");
    } else if (m_manageAudioPlayer->playerPlaybackState() == QMediaPlayer::PlayingState) {
        result = QStringLiteral("Playing");
    } else {
        result = QStringLiteral("Paused");
    }

    if (mShowProgressOnTaskBar) {
        QVariantMap parameters;

        if (m_manageAudioPlayer->playerPlaybackState() == QMediaPlayer::StoppedState || m_audioPlayer->duration() == 0) {
            parameters.insert(QStringLiteral("progress-visible"), false);
            parameters.insert(QStringLiteral("progress"), 0);
        } else {
            parameters.insert(QStringLiteral("progress-visible"), true);
            parameters.insert(QStringLiteral("progress"), qRound(static_cast<double>(m_position / m_audioPlayer->duration())) / 1000.0);
        }

        mProgressIndicatorSignal.setArguments({QStringLiteral("application://org.kde.elisa.desktop"), parameters});

        QDBusConnection::sessionBus().send(mProgressIndicatorSignal);
    }

    return result;
}

bool MediaPlayer2Player::CanGoNext() const
{
    return m_canGoNext;
}

void MediaPlayer2Player::Next()
{
    Q_EMIT next();

    if (m_playListControler) {
        m_playListControler->skipNextTrack(ElisaUtils::SkipReason::Manual);
    }
}

bool MediaPlayer2Player::CanGoPrevious() const
{
    return m_canGoPrevious;
}

void MediaPlayer2Player::Previous()
{
    Q_EMIT previous();

    if (m_playListControler) {
        m_playListControler->skipPreviousTrack(m_audioPlayer->position());
    }
}

bool MediaPlayer2Player::CanPause() const
{
    return m_canPlay;
}

void MediaPlayer2Player::Pause()
{
    if (m_playListControler) {
        m_manageAudioPlayer->ensurePause();
    }
}

void MediaPlayer2Player::PlayPause()
{
    Q_EMIT playPause();

    if (m_playListControler) {
        m_manageAudioPlayer->playPause();
    }
}

void MediaPlayer2Player::Stop()
{
    Q_EMIT stop();

    if (m_playListControler) {
        m_manageAudioPlayer->stop();
    }
}

bool MediaPlayer2Player::CanPlay() const
{
    return m_canPlay;
}

void MediaPlayer2Player::Play()
{
    if (m_playListControler) {
        m_manageAudioPlayer->ensurePlay();
    }
}

double MediaPlayer2Player::Volume() const
{
    return m_volume;
}

void MediaPlayer2Player::setVolume(double volume)
{
    m_volume= qBound(0.0, volume, 1.0);
    Q_EMIT volumeChanged(m_volume);

    m_audioPlayer->setVolume(100 * m_volume);

    signalPropertiesChange(QStringLiteral("Volume"), Volume());
}

QVariantMap MediaPlayer2Player::Metadata() const
{
    return m_metadata;
}

qlonglong MediaPlayer2Player::Position() const
{
    return m_position;
}

void MediaPlayer2Player::setPropertyPosition(int newPositionInMs)
{
    m_position = qlonglong(newPositionInMs) * 1000;

    /* only send new progress when it has advanced more than 1 %
     * to limit DBus traffic
     */
    const auto incrementalProgress = static_cast<double>(newPositionInMs - mPreviousProgressPosition) / m_audioPlayer->duration();
    if (mShowProgressOnTaskBar && (incrementalProgress > 0.01 || incrementalProgress < 0))
    {
        mPreviousProgressPosition = newPositionInMs;
        QVariantMap parameters;
        parameters.insert(QStringLiteral("progress-visible"), true);
        parameters.insert(QStringLiteral("progress"), static_cast<double>(newPositionInMs) / m_audioPlayer->duration());

        mProgressIndicatorSignal.setArguments({QStringLiteral("application://org.kde.elisa.desktop"), parameters});

        QDBusConnection::sessionBus().send(mProgressIndicatorSignal);
    }
}

double MediaPlayer2Player::Rate() const
{
    return m_rate;
}

void MediaPlayer2Player::setRate(double newRate)
{
    if (newRate <= 0.0001 && newRate >= -0.0001) {
        Pause();
    } else {
        m_rate = qBound(MinimumRate(), newRate, MaximumRate());
        Q_EMIT rateChanged(m_rate);

        signalPropertiesChange(QStringLiteral("Rate"), Rate());
    }
}

double MediaPlayer2Player::MinimumRate() const
{
    return MIN_RATE;
}

double MediaPlayer2Player::MaximumRate() const
{
    return MAX_RATE;
}

bool MediaPlayer2Player::CanSeek() const
{
    return m_playerIsSeekableChanged;
}

bool MediaPlayer2Player::CanControl() const
{
    return true;
}

void MediaPlayer2Player::Seek(qlonglong Offset)
{
    if (mediaPlayerPresent()) {
        auto offset = (m_position + Offset) / 1000;
        m_manageAudioPlayer->playerSeek(int(offset));
    }
}

void MediaPlayer2Player::SetPosition(const QDBusObjectPath &trackId, qlonglong pos)
{
    if (trackId.path() == m_currentTrackId) {
        m_manageAudioPlayer->playerSeek(int(pos / 1000));
    }
}

void MediaPlayer2Player::OpenUri(const QString &uri)
{
    Q_UNUSED(uri);
}

void MediaPlayer2Player::playerSourceChanged()
{
    if (!m_playListControler) {
        return;
    }

    setCurrentTrack(m_manageAudioPlayer->playListPosition());
}

void MediaPlayer2Player::playControlEnabledChanged()
{
    if (!m_playListControler) {
        return;
    }

    m_canPlay = m_manageMediaPlayerControl->playControlEnabled();

    signalPropertiesChange(QStringLiteral("CanPause"), CanPause());
    signalPropertiesChange(QStringLiteral("CanPlay"), CanPlay());

    Q_EMIT canPauseChanged();
    Q_EMIT canPlayChanged();
}

void MediaPlayer2Player::skipBackwardControlEnabledChanged()
{
    if (!m_playListControler) {
        return;
    }

    m_canGoPrevious = m_manageMediaPlayerControl->skipBackwardControlEnabled();

    signalPropertiesChange(QStringLiteral("CanGoPrevious"), CanGoPrevious());
    Q_EMIT canGoPreviousChanged();
}

void MediaPlayer2Player::skipForwardControlEnabledChanged()
{
    if (!m_playListControler) {
        return;
    }

    m_canGoNext = m_manageMediaPlayerControl->skipForwardControlEnabled();

    signalPropertiesChange(QStringLiteral("CanGoNext"), CanGoNext());
    Q_EMIT canGoNextChanged();
}

void MediaPlayer2Player::playerPlaybackStateChanged()
{
    signalPropertiesChange(QStringLiteral("PlaybackStatus"), PlaybackStatus());
    Q_EMIT playbackStatusChanged();

    playerIsSeekableChanged();
}

void MediaPlayer2Player::playerIsSeekableChanged()
{
    m_playerIsSeekableChanged = m_manageAudioPlayer->playerIsSeekable();

    signalPropertiesChange(QStringLiteral("CanSeek"), CanSeek());
    Q_EMIT canSeekChanged();
}

void MediaPlayer2Player::audioPositionChanged()
{
    setPropertyPosition(static_cast<int>(m_manageAudioPlayer->playerPosition()));
}

void MediaPlayer2Player::playerSeeked(qint64 position)
{
    Q_EMIT Seeked(position * 1000);
}

void MediaPlayer2Player::audioDurationChanged()
{
    m_metadata = getMetadataOfCurrentTrack();
    signalPropertiesChange(QStringLiteral("Metadata"), Metadata());

    skipBackwardControlEnabledChanged();
    skipForwardControlEnabledChanged();
    playerPlaybackStateChanged();
    playerIsSeekableChanged();
    setPropertyPosition(static_cast<int>(m_manageAudioPlayer->playerPosition()));
}

void MediaPlayer2Player::playerVolumeChanged()
{
    setVolume(m_audioPlayer->volume() / 100.0);
}

int MediaPlayer2Player::currentTrack() const
{
    return m_manageAudioPlayer->playListPosition();
}

void MediaPlayer2Player::setCurrentTrack(int newTrackPosition)
{
    m_currentTrack = m_manageAudioPlayer->playerSource().toString();
    m_currentTrackId = QDBusObjectPath(QLatin1String("/org/kde/elisa/playlist/") + QString::number(newTrackPosition)).path();

    Q_EMIT currentTrackChanged();

    m_metadata = getMetadataOfCurrentTrack();
    signalPropertiesChange(QStringLiteral("Metadata"), Metadata());
}

QVariantMap MediaPlayer2Player::getMetadataOfCurrentTrack()
{
    auto result = QVariantMap();

    if (m_currentTrackId.isEmpty()) {
        return {};
    }

    result[QStringLiteral("mpris:trackid")] = QVariant::fromValue<QDBusObjectPath>(QDBusObjectPath(m_currentTrackId));
    result[QStringLiteral("mpris:length")] = qlonglong(m_manageAudioPlayer->audioDuration()) * 1000;
    //convert milli-seconds into micro-seconds
    if (!m_manageHeaderBar->title().isNull() && !m_manageHeaderBar->title().toString().isEmpty()) {
        result[QStringLiteral("xesam:title")] = m_manageHeaderBar->title();
    }
    result[QStringLiteral("xesam:url")] = m_manageAudioPlayer->playerSource().toString();
    if (!m_manageHeaderBar->album().isNull() && !m_manageHeaderBar->album().toString().isEmpty()) {
        result[QStringLiteral("xesam:album")] = m_manageHeaderBar->album();
    }
    if (!m_manageHeaderBar->artist().isNull() && !m_manageHeaderBar->artist().toString().isEmpty()) {
        result[QStringLiteral("xesam:artist")] = QStringList{m_manageHeaderBar->artist().toString()};
    }
    if (!m_manageHeaderBar->albumArtist().isNull() && !m_manageHeaderBar->albumArtist().toString().isEmpty()) {
        result[QStringLiteral("xesam:albumArtist")] = QStringList{m_manageHeaderBar->albumArtist().toString()};
    }
    if (!m_manageHeaderBar->image().isEmpty() && !m_manageHeaderBar->image().toString().isEmpty()) {
        if (m_manageHeaderBar->image().scheme() == QStringLiteral("image")) {
            // adding a special case for image:// URLs that are only valid because Elisa installs a special handler for them
            // converting those URL to data URLs with embedded image data
#if KFFileMetaData_FOUND

            const auto &mimeType = QMimeDatabase().mimeTypeForFile(m_manageHeaderBar->image().toString().mid(14)).name();
            KFileMetaData::ExtractorCollection extractorCollection;
            const auto extractors = extractorCollection.fetchExtractors(mimeType);

            QMap<KFileMetaData::EmbeddedImageData::ImageType, QByteArray> imageData;
            for (const auto &extractor : extractors) {
                KFileMetaData::SimpleExtractionResult result(m_manageHeaderBar->image().toString().mid(14), mimeType, KFileMetaData::ExtractionResult::ExtractImageData);
                extractor->extract(&result);
                if (!result.imageData().isEmpty()) {
                    imageData = result.imageData();
                }
            }

            if (imageData.contains(KFileMetaData::EmbeddedImageData::FrontCover)) {
                result[QStringLiteral("mpris:artUrl")] = QString{QStringLiteral("data:image/png;base64,") + QLatin1String{imageData[KFileMetaData::EmbeddedImageData::FrontCover].toBase64()}};
            }
#endif
        } else {
            result[QStringLiteral("mpris:artUrl")] = m_manageHeaderBar->image().toString();
        }
    }

    return result;
}

int MediaPlayer2Player::mediaPlayerPresent() const
{
    return m_mediaPlayerPresent;
}

bool MediaPlayer2Player::showProgressOnTaskBar() const
{
    return mShowProgressOnTaskBar;
}

void MediaPlayer2Player::setShowProgressOnTaskBar(bool value)
{
    mShowProgressOnTaskBar = value;

    QVariantMap parameters;

    if (!mShowProgressOnTaskBar || m_manageAudioPlayer->playerPlaybackState() == QMediaPlayer::StoppedState || m_audioPlayer->duration() == 0) {
        parameters.insert(QStringLiteral("progress-visible"), false);
        parameters.insert(QStringLiteral("progress"), 0);
    } else {
        parameters.insert(QStringLiteral("progress-visible"), true);
        parameters.insert(QStringLiteral("progress"), qRound(static_cast<double>(m_position / m_audioPlayer->duration())) / 1000.0);
    }

    mProgressIndicatorSignal.setArguments({QStringLiteral("application://org.kde.elisa.desktop"), parameters});

    QDBusConnection::sessionBus().send(mProgressIndicatorSignal);
}

void MediaPlayer2Player::setMediaPlayerPresent(int status)
{
    if (m_mediaPlayerPresent != status) {
        m_mediaPlayerPresent = status;
        Q_EMIT mediaPlayerPresentChanged();

        signalPropertiesChange(QStringLiteral("CanGoNext"), CanGoNext());
        signalPropertiesChange(QStringLiteral("CanGoPrevious"), CanGoPrevious());
        signalPropertiesChange(QStringLiteral("CanPause"), CanPause());
        signalPropertiesChange(QStringLiteral("CanPlay"), CanPlay());
        Q_EMIT canGoNextChanged();
        Q_EMIT canGoPreviousChanged();
        Q_EMIT canPauseChanged();
        Q_EMIT canPlayChanged();
    }
}

void MediaPlayer2Player::setShuffle(bool shuffle)
{
    if (m_playListControler) {
        MediaPlayListProxyModel::Shuffle shuffleMode = shuffle ? MediaPlayListProxyModel::Shuffle::Track : MediaPlayListProxyModel::Shuffle::NoShuffle;
        m_playListControler->setShuffleMode(shuffleMode);
        signalPropertiesChange(QStringLiteral("Shuffle"), Shuffle());
    }
}

bool MediaPlayer2Player::Shuffle() const
{
    if (m_playListControler) {
        return (m_playListControler->shuffleMode() != MediaPlayListProxyModel::Shuffle::NoShuffle);
    }

    return false;
}

void MediaPlayer2Player::shuffleModeChanged()
{
    signalPropertiesChange(QStringLiteral("Shuffle"), Shuffle());
}

void MediaPlayer2Player::setLoopStatus(const QString& loopStatus)
{
    MediaPlayListProxyModel::Repeat repeatMode;

    if (m_playListControler) {
        if (loopStatus == QStringLiteral("Playlist")) {
            repeatMode = MediaPlayListProxyModel::Playlist;
        } else if (loopStatus == QStringLiteral("Track")) {
            repeatMode = MediaPlayListProxyModel::One;
        } else {
            repeatMode = MediaPlayListProxyModel::None;
        }

        m_playListControler->setRepeatMode(repeatMode);
        signalPropertiesChange(QStringLiteral("LoopStatus"), LoopStatus());
    }
}

QString MediaPlayer2Player::LoopStatus() const
{
    if (m_playListControler) {
        const auto repeatMode = m_playListControler->repeatMode();
        switch (repeatMode) {
            case MediaPlayListProxyModel::Playlist:
                return QStringLiteral("Playlist");
            case MediaPlayListProxyModel::One:
                return QStringLiteral("Track");
            case MediaPlayListProxyModel::None:
            default:
                return QStringLiteral("None");
        }
    }

    return QStringLiteral("None");
}

void MediaPlayer2Player::repeatModeChanged()
{
    signalPropertiesChange(QStringLiteral("LoopStatus"), LoopStatus());
}

void MediaPlayer2Player::signalPropertiesChange(const QString &property, const QVariant &value)
{
    QVariantMap properties;
    properties[property] = value;
    const int ifaceIndex = metaObject()->indexOfClassInfo("D-Bus Interface");
    QDBusMessage msg = QDBusMessage::createSignal(QStringLiteral("/org/mpris/MediaPlayer2"),
                                                  QStringLiteral("org.freedesktop.DBus.Properties"), QStringLiteral("PropertiesChanged"));

    msg << QLatin1String(metaObject()->classInfo(ifaceIndex).value());
    msg << properties;
    msg << QStringList();

    QDBusConnection::sessionBus().send(msg);
}

#include "moc_mediaplayer2player.cpp"
