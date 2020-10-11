/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef MANAGEMEDIAPLAYERCONTROL_H
#define MANAGEMEDIAPLAYERCONTROL_H

#include "elisaLib_export.h"
#include <QPersistentModelIndex>
#include <QObject>


class ELISALIB_EXPORT ManageMediaPlayerControl : public QObject
{

    Q_OBJECT

    Q_PROPERTY(bool playControlEnabled
               READ playControlEnabled
               NOTIFY playControlEnabledChanged)

    Q_PROPERTY(bool skipBackwardControlEnabled
               READ skipBackwardControlEnabled
               NOTIFY skipBackwardControlEnabledChanged)

    Q_PROPERTY(bool skipForwardControlEnabled
               READ skipForwardControlEnabled
               NOTIFY skipForwardControlEnabledChanged)

    Q_PROPERTY(bool musicPlaying
               READ musicPlaying
               NOTIFY musicPlayingChanged)

    Q_PROPERTY(QPersistentModelIndex previousTrack
               READ previousTrack
               WRITE setPreviousTrack
               NOTIFY previousTrackChanged)

    Q_PROPERTY(QPersistentModelIndex currentTrack
               READ currentTrack
               WRITE setCurrentTrack
               NOTIFY currentTrackChanged)

    Q_PROPERTY(QPersistentModelIndex nextTrack
               READ nextTrack
               WRITE setNextTrack
               NOTIFY nextTrackChanged)

public:

    explicit ManageMediaPlayerControl(QObject *parent = nullptr);

    [[nodiscard]] bool playControlEnabled() const;

    [[nodiscard]] bool skipBackwardControlEnabled() const;

    [[nodiscard]] bool skipForwardControlEnabled() const;

    [[nodiscard]] bool musicPlaying() const;

    [[nodiscard]] QPersistentModelIndex previousTrack() const;

    [[nodiscard]] QPersistentModelIndex currentTrack() const;

    [[nodiscard]] QPersistentModelIndex nextTrack() const;

Q_SIGNALS:

    void playControlEnabledChanged();

    void skipBackwardControlEnabledChanged();

    void skipForwardControlEnabledChanged();

    void musicPlayingChanged();

    void previousTrackChanged();

    void currentTrackChanged();

    void nextTrackChanged();

public Q_SLOTS:

    void playerPausedOrStopped();

    void playerPlaying();

    void setPreviousTrack(const QPersistentModelIndex &previousTrack);

    void setCurrentTrack(const QPersistentModelIndex &currentTrack);

    void setNextTrack(const QPersistentModelIndex &nextTrack);

private:

    QPersistentModelIndex mPreviousTrack;

    QPersistentModelIndex mCurrentTrack;

    QPersistentModelIndex mNextTrack;

    bool mIsInPlayingState = false;

};

#endif // MANAGEMEDIAPLAYERCONTROL_H
