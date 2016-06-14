/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef DATABASEINTERFACE_H
#define DATABASEINTERFACE_H

#include "musicalbum.h"
#include "musicaudiotrack.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QHash>
#include <QtCore/QVector>

class DatabaseInterfacePrivate;

class DatabaseInterface : public QObject
{
    Q_OBJECT

public:

    explicit DatabaseInterface(QObject *parent = 0);

    virtual ~DatabaseInterface();

    MusicAlbum albumFromIndex(int albumIndex) const;

    MusicAlbum albumFromId(quint64 albumId) const;

    int albumIdFromTrackId(quintptr trackId) const;

    int albumCount() const;

Q_SIGNALS:

public Q_SLOTS:

    void insertAlbumsList(const QVector<MusicAlbum> &allAlbums);

    void insertTracksList(QHash<QString, QVector<MusicAudioTrack> > tracks, QHash<QString, QString> covers);

private:

    void initDatabase() const;

    DatabaseInterfacePrivate *d;

};

#endif // DATABASEINTERFACE_H
