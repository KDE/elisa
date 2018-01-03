/*
 * Copyright 2015-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef ALLALBUMSMODEL_H
#define ALLALBUMSMODEL_H

#include <QAbstractItemModel>
#include <QVector>
#include <QHash>
#include <QString>

#include "musicalbum.h"
#include "musicaudiotrack.h"

#include <memory>

class AllAlbumsModelPrivate;
class MusicStatistics;
class QMutex;

class AllAlbumsModel : public QAbstractItemModel
{
    Q_OBJECT

    Q_PROPERTY(int albumCount
               READ albumCount
               NOTIFY albumCountChanged)

public:

    enum ColumnsRoles {
        TitleRole = Qt::UserRole + 1,
        AllTracksTitleRole,
        ArtistRole,
        AllArtistsRole,
        ImageRole,
        CountRole,
        IdRole,
        IsSingleDiscAlbumRole,
        AlbumDataRole,
        HighestTrackRating,
        AlbumDatabaseIdRole,
    };

    Q_ENUM(ColumnsRoles)

    explicit AllAlbumsModel(QObject *parent = nullptr);

    ~AllAlbumsModel() override;

    Q_INVOKABLE int albumCount() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex &child) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

public Q_SLOTS:

    void albumAdded(const MusicAlbum &newAlbum);

    void albumRemoved(const MusicAlbum &removedAlbum);

    void albumModified(const MusicAlbum &modifiedAlbum);

Q_SIGNALS:

    void albumCountChanged();

private:

    QVariant internalDataAlbum(int albumIndex, int role) const;

    std::unique_ptr<AllAlbumsModelPrivate> d;

};

#endif // ALLALBUMSMODEL_H
