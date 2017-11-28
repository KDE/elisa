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

#ifndef TRACKSLISTENER_H
#define TRACKSLISTENER_H

#include <QObject>

#include "musicaudiotrack.h"

#include <memory>

class TracksListenerPrivate;
class DatabaseInterface;

class TracksListener : public QObject
{
    Q_OBJECT

public:

    explicit TracksListener(DatabaseInterface *database, QObject *parent = nullptr);

    ~TracksListener() override;

Q_SIGNALS:

    void trackHasChanged(const MusicAudioTrack &audioTrack);

    void trackHasBeenRemoved(qulonglong id);

    void albumAdded(const QList<MusicAudioTrack> &tracks);

public Q_SLOTS:

    void tracksAdded(const QList<MusicAudioTrack> &allTracks);

    void trackRemoved(qulonglong id);

    void trackModified(const MusicAudioTrack &modifiedTrack);

    void trackByNameInList(const QString &title, const QString &artist, const QString &album, int trackNumber, int discNumber);

    void trackByIdInList(qulonglong newTrackId);

    void newArtistInList(const QString &artist);

private:

    std::unique_ptr<TracksListenerPrivate> d;

};

#endif // TRACKSLISTENER_H
