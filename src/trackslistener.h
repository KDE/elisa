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

#include <QtCore/QObject>

#include "musicaudiotrack.h"

class TracksListenerPrivate;
class DatabaseInterface;

class TracksListener : public QObject
{
    Q_OBJECT

public:

    explicit TracksListener(DatabaseInterface *database, QObject *parent = 0);

Q_SIGNALS:

    void trackChanged(MusicAudioTrack audioTrack);

    void albumAdded(const QVector<MusicAudioTrack> &tracks);

public Q_SLOTS:

    void trackAdded(MusicAudioTrack newTrack);

    void trackByNameInList(QString title, QString artist, QString album);

    void trackByIdInList(qulonglong newTrackId);

    void newArtistInList(QString artist);

private:

    TracksListenerPrivate *d = nullptr;

};

#endif // TRACKSLISTENER_H
