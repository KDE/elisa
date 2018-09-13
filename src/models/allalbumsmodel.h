/*
 * Copyright 2015-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef ALLALBUMSMODEL_H
#define ALLALBUMSMODEL_H

#include "elisaLib_export.h"

#include <QAbstractItemModel>
#include <QVector>
#include <QHash>
#include <QString>

#include "musicalbum.h"
#include "musicaudiotrack.h"

#include <memory>

class AllAlbumsModelPrivate;
class AllArtistsModel;

class ELISALIB_EXPORT AllAlbumsModel : public QAbstractItemModel
{
    Q_OBJECT

    Q_PROPERTY(int albumCount
               READ albumCount
               NOTIFY albumCountChanged)

    Q_PROPERTY(AllArtistsModel* allArtists
               READ allArtists
               WRITE setAllArtists
               NOTIFY allArtistsChanged)

public:

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

    AllArtistsModel *allArtists() const;

public Q_SLOTS:

    void albumsAdded(const QList<MusicAlbum> &newAlbums);

    void albumRemoved(const MusicAlbum &removedAlbum);

    void albumModified(const MusicAlbum &modifiedAlbum);

    void setAllArtists(AllArtistsModel *model);

Q_SIGNALS:

    void albumCountChanged();

    void allArtistsChanged();

private:

    QVariant internalDataAlbum(int albumIndex, int role) const;

    std::unique_ptr<AllAlbumsModelPrivate> d;

};

#endif // ALLALBUMSMODEL_H
