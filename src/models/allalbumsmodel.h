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

#include "elisautils.h"
#include "databaseinterface.h"

#include <QAbstractItemModel>
#include <QVector>
#include <QHash>
#include <QString>

#include <memory>

class AllAlbumsModelPrivate;
class MusicListenersManager;

class ELISALIB_EXPORT AllAlbumsModel : public QAbstractItemModel
{
    Q_OBJECT

    Q_PROPERTY(int albumCount
               READ albumCount
               NOTIFY albumCountChanged)

public:

    using ListAlbumDataType = DatabaseInterface::ListAlbumDataType;

    using AlbumDataType = DatabaseInterface::AlbumDataType;

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

    void albumsAdded(AllAlbumsModel::ListAlbumDataType newAlbums);

    void albumRemoved(qulonglong removedAlbumId);

    void albumModified(const AllAlbumsModel::AlbumDataType &modifiedAlbum);

    void initialize(MusicListenersManager *manager);

    void initializeByArtist(MusicListenersManager *manager, const QString &artist);

    void initializeByGenreAndArtist(MusicListenersManager *manager,
                                    const QString &genre, const QString &artist);

Q_SIGNALS:

    void albumCountChanged();

    void needData(ElisaUtils::PlayListEntryType dataType);

    void needDataByArtist(ElisaUtils::PlayListEntryType dataType, const QString &artist);

    void needDataByGenreAndArtist(ElisaUtils::PlayListEntryType dataType,
                                  const QString &genre, const QString &artist);

private:

    QVariant internalDataAlbum(int albumIndex, int role) const;

    std::unique_ptr<AllAlbumsModelPrivate> d;

};

#endif // ALLALBUMSMODEL_H
