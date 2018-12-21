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

#ifndef ALLARTISTSMODEL_H
#define ALLARTISTSMODEL_H

#include "elisaLib_export.h"

#include "elisautils.h"
#include "databaseinterface.h"

#include <QAbstractListModel>
#include <QVector>
#include <QHash>
#include <QString>

#include <memory>

class AllArtistsModelPrivate;
class MusicListenersManager;

class ELISALIB_EXPORT AllArtistsModel : public QAbstractListModel
{
    Q_OBJECT

public:

    using ListArtistDataType = DatabaseInterface::ListArtistDataType;

    using ArtistDataType = DatabaseInterface::ArtistDataType;

    explicit AllArtistsModel(QObject *parent = nullptr);

    ~AllArtistsModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex &child) const override;

Q_SIGNALS:

    void needData(ElisaUtils::PlayListEntryType dataType);

    void needDataByGenre(ElisaUtils::PlayListEntryType dataType, const QString &genre);

public Q_SLOTS:

    void artistsAdded(AllArtistsModel::ListArtistDataType newData);

    void artistRemoved(qulonglong removedDatabaseId);

    void initialize(MusicListenersManager *manager);

    void initializeByGenre(MusicListenersManager *manager, const QString &genre);

private:

    std::unique_ptr<AllArtistsModelPrivate> d;

};



#endif // ALLARTISTSMODEL_H
