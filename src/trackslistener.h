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

#ifndef TRACKSLISTENER_H
#define TRACKSLISTENER_H

#include "elisaLib_export.h"

#include "databaseinterface.h"
#include "musicaudiotrack.h"

#include <QObject>

#include <memory>

class TracksListenerPrivate;

class ELISALIB_EXPORT TracksListener : public QObject
{
    Q_OBJECT

public:

    using DataListType = DatabaseInterface::DataListType;

    using DataType = DatabaseInterface::DataType;

    explicit TracksListener(DatabaseInterface *database, QObject *parent = nullptr);

    ~TracksListener() override;

Q_SIGNALS:

    void trackHasChanged(const DataType &audioTrack);

    void trackHasBeenRemoved(qulonglong id);

    void albumAdded(const QList<MusicAudioTrack> &tracks);

public Q_SLOTS:

    void tracksAdded(const DatabaseInterface::DataListType &allTracks);

    void trackRemoved(qulonglong id);

    void trackModified(const DataType &modifiedTrack);

    void trackByNameInList(const QString &title, const QString &artist, const QString &album, int trackNumber, int discNumber);

    void trackByFileNameInList(const QUrl &fileName);

    void trackByIdInList(qulonglong newTrackId);

    void newArtistInList(const QString &artist);

private:

    MusicAudioTrack scanOneFile(const QUrl &scanFile);

    std::unique_ptr<TracksListenerPrivate> d;

};

#endif // TRACKSLISTENER_H
