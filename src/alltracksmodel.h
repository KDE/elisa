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

#ifndef ALLTRACKSMODEL_H
#define ALLTRACKSMODEL_H

#include <QAbstractItemModel>

#include "musicaudiotrack.h"

#include <memory>

class AllTracksModelPrivate;

class AllTracksModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    enum ColumnsRoles {
        TitleRole = Qt::UserRole + 1,
        DurationRole,
        MilliSecondsDurationRole,
        ArtistRole,
        AlbumRole,
        AlbumArtistRole,
        TrackNumberRole,
        DiscNumberRole,
        RatingRole,
        GenreRole,
        LyricistRole,
        ComposerRole,
        CommentRole,
        YearRole,
        ChannelsRole,
        BitRateRole,
        SampleRateRole,
        ImageRole,
        ResourceRole,
        IdRole,
        DatabaseIdRole,
        IsSingleDiscAlbumRole,
        ContainerDataRole,
        SecondaryTextRole,
        ImageUrlRole,
        ShadowForImageRole,
    };

    Q_ENUM(ColumnsRoles)

    explicit AllTracksModel(QObject *parent = nullptr);

    ~AllTracksModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex &child) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

public Q_SLOTS:

    void tracksAdded(const QList<MusicAudioTrack> &allTracks);

    void trackRemoved(qulonglong removedTrackId);

    void trackModified(const MusicAudioTrack &modifiedTrack);

private:

    std::unique_ptr<AllTracksModelPrivate> d;

};

#endif // ALLTRACKSMODEL_H
