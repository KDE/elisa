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
#include "datatypes.h"
#include "elisautils.h"

#include <QObject>

#include <memory>

class TracksListenerPrivate;

class ELISALIB_EXPORT TracksListener : public QObject
{
    Q_OBJECT

public:

    using ListTrackDataType = DataTypes::ListTrackDataType;

    using TrackDataType = DataTypes::TrackDataType;

    explicit TracksListener(DatabaseInterface *database, QObject *parent = nullptr);

    ~TracksListener() override;

Q_SIGNALS:

    void trackHasChanged(const TracksListener::TrackDataType &audioTrack);

    void trackHasBeenRemoved(qulonglong id);

    void tracksListAdded(qulonglong newDatabaseId,
                         const QString &entryTitle,
                         ElisaUtils::PlayListEntryType databaseIdType,
                         const TracksListener::ListTrackDataType &tracks);

public Q_SLOTS:

    void tracksAdded(const TracksListener::ListTrackDataType &allTracks);

    void trackRemoved(qulonglong id);

    void trackModified(const TracksListener::TrackDataType &modifiedTrack);

    void trackByNameInList(const QVariant &title, const QVariant &artist, const QVariant &album, const QVariant &trackNumber, const QVariant &discNumber);

    void newEntryInList(qulonglong newDatabaseId,
                        const QString &entryTitle,
                        ElisaUtils::PlayListEntryType databaseIdType);

    void trackByFileNameInList(ElisaUtils::PlayListEntryType type, const QUrl &fileName);

    void newUrlInList(const QUrl &entryUrl,
                      ElisaUtils::PlayListEntryType databaseIdType);

    void updateSingleFileMetaData(const QUrl &url, DataTypes::ColumnsRoles role, const QVariant &data);

private:

    void newArtistInList(qulonglong newDatabaseId, const QString &artist);

    void newGenreInList(qulonglong newDatabaseId, const QString &entryTitle);

    void newAlbumInList(qulonglong newDatabaseId,
                        const QString &entryTitle);

    std::unique_ptr<TracksListenerPrivate> d;

};

#endif // TRACKSLISTENER_H
