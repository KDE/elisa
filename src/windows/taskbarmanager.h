/*
   SPDX-FileCopyrightText: 2024 (c) Jack Hill <jackhill3103@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef TASKBAR_H
#define TASKBAR_H

#include <QAbstractNativeEventFilter>
#include <QMediaPlayer>
#include <QQuickItem>
#include <QQmlEngine>

#include <memory>

class TaskBarManagerPrivate;

/**
 * Windows taskbar
 */
class TaskBarManager : public QQuickItem, public QAbstractNativeEventFilter
{

    Q_OBJECT

    QML_ELEMENT

    /**
     * Whether the media is playing, paused, or stopped
     */
    Q_PROPERTY(QMediaPlayer::PlaybackState playbackState
               READ playbackState
               WRITE setPlaybackState
               NOTIFY playbackStateChanged)

    /**
     * Whether to show track progress on the taskbar
     */
    Q_PROPERTY(bool showProgress
               READ showProgress
               WRITE setShowProgress
               NOTIFY showProgressChanged)

    /**
     * Maximum possible progress
     */
    Q_PROPERTY(qulonglong progressMaximum
               READ progressMaximum
               WRITE setProgressMaximum
               NOTIFY progressMaximumChanged)

    /**
     * Current progress; this is always clamped to be in the range [0, progressMaximum]
     */
    Q_PROPERTY(qulonglong progressValue
               READ progressValue
               WRITE setProgressValue
               NOTIFY progressValueChanged)

    /**
     * Whether the "Skip Backward" button is enabled
     */
    Q_PROPERTY(bool canSkipBackward
               READ canSkipBackward
               WRITE setCanSkipBackward
               NOTIFY canSkipBackwardChanged)

    /**
     * Whether the "Skip Forward" button is enabled
     */
    Q_PROPERTY(bool canSkipForward
               READ canSkipForward
               WRITE setCanSkipForward
               NOTIFY canSkipForwardChanged)

    /**
     * Whether the "Toggle Playback" button is enabled
     */
    Q_PROPERTY(bool canTogglePlayback
               READ canTogglePlayback
               WRITE setCanTogglePlayback
               NOTIFY canTogglePlaybackChanged)

public:

    explicit TaskBarManager(QQuickItem *parent = nullptr);

    ~TaskBarManager() override;

    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;

    [[nodiscard]] QMediaPlayer::PlaybackState playbackState() const;

    [[nodiscard]] bool showProgress() const;

    [[nodiscard]] qulonglong progressMaximum() const;

    [[nodiscard]] qulonglong progressValue() const;

    [[nodiscard]] bool canSkipBackward() const;

    [[nodiscard]] bool canSkipForward() const;

    [[nodiscard]] bool canTogglePlayback() const;

Q_SIGNALS:

    void playbackStateChanged();

    void showProgressChanged();

    void progressMaximumChanged();

    void progressValueChanged();

    void canSkipBackwardChanged();

    void canSkipForwardChanged();

    void canTogglePlaybackChanged();

    void skipBackward();

    void skipForward();

    void togglePlayback();

public Q_SLOTS:

    void setPlaybackState(QMediaPlayer::PlaybackState newPlaybackState);

    void setShowProgress(bool showProgress);

    void setProgressMaximum(qlonglong newMaximum);

    void setProgressValue(qlonglong newValue);

    void setCanSkipBackward(bool canSkip);

    void setCanSkipForward(bool canSkip);

    void setCanTogglePlayback(bool canToggle);

private:

    std::unique_ptr<TaskBarManagerPrivate> d;

};

#endif // TASKBAR_H
