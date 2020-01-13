/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

    bool playControlEnabled() const;

    bool skipBackwardControlEnabled() const;

    bool skipForwardControlEnabled() const;

    bool musicPlaying() const;

    QPersistentModelIndex previousTrack() const;

    QPersistentModelIndex currentTrack() const;

    QPersistentModelIndex nextTrack() const;

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
