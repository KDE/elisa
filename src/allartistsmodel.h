/*
 * Copyright 2015-2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef ALLARTISTSMODEL_H
#define ALLARTISTSMODEL_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QVector>
#include <QtCore/QHash>
#include <QtCore/QString>

#include "musicartist.h"

class DatabaseInterface;
class AllArtistsModelPrivate;

class AllArtistsModel : public QAbstractItemModel
{
    Q_OBJECT

    Q_PROPERTY(DatabaseInterface* databaseInterface
               READ databaseInterface
               WRITE setDatabaseInterface
               NOTIFY databaseInterfaceChanged)

    Q_PROPERTY(QString filter
               READ filter
               WRITE setFilter
               NOTIFY filterChanged)

public:

    enum ColumnsRoles {
        NameRole = Qt::UserRole + 1,
        ArtistsCountRole = NameRole + 1,
        ImageRole = ArtistsCountRole + 1,
        IdRole = ImageRole + 1,
    };

    explicit AllArtistsModel(QObject *parent = 0);

    virtual ~AllArtistsModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex &child) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    DatabaseInterface* databaseInterface() const;

    QString filter() const;

Q_SIGNALS:

    void databaseInterfaceChanged();

    void filterChanged();

public Q_SLOTS:

    void setDatabaseInterface(DatabaseInterface* databaseInterface);

    void setFilter(const QString &filter);


private Q_SLOTS:

    void beginArtistAdded(QVector<qulonglong> newArtists);

    void endArtistAdded(QVector<qulonglong> newArtists);

private:

    AllArtistsModelPrivate *d;

};

#endif // ALLALBUMSMODEL_H
