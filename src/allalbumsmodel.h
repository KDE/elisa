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

#ifndef ALLALBUMSMODEL_H
#define ALLALBUMSMODEL_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QVector>
#include <QtCore/QHash>
#include <QtCore/QString>

#include "musicalbum.h"
#include "musicaudiotrack.h"

class DatabaseInterface;
class AllAlbumsModelPrivate;
class MusicStatistics;
class QMutex;

class AllAlbumsModel : public QAbstractItemModel
{
    Q_OBJECT

    Q_PROPERTY(DatabaseInterface* databaseInterface
               READ databaseInterface
               WRITE setDatabaseInterface
               NOTIFY databaseInterfaceChanged)

    Q_PROPERTY(QString artist
               READ artist
               WRITE setArtist
               NOTIFY artistChanged)

    Q_PROPERTY(bool exactMatch
               READ exactMatch
               WRITE setExactMatch
               NOTIFY exactMatchChanged)

public:

    enum ColumnsRoles {
        TitleRole = Qt::UserRole + 1,
        AllTracksTitleRole = TitleRole + 1,
        ArtistRole = AllTracksTitleRole + 1,
        AllArtistsRole = ArtistRole + 1,
        ImageRole = AllArtistsRole + 1,
        CountRole = ImageRole + 1,
        IdRole = CountRole + 1,
    };

    explicit AllAlbumsModel(QObject *parent = 0);

    virtual ~AllAlbumsModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex &child) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    DatabaseInterface* databaseInterface() const;

    QString artist() const;

    bool exactMatch() const;

Q_SIGNALS:

    void databaseInterfaceChanged();

    void artistChanged();

    void exactMatchChanged();

public Q_SLOTS:

    void setDatabaseInterface(DatabaseInterface* databaseInterface);

    void setArtist(QString artist);

    void setExactMatch(bool exactMatch);

private Q_SLOTS:

    void beginAlbumAdded(QVector<qulonglong> newAlbums);

    void endAlbumAdded(QVector<qulonglong> newAlbums);

private:

    QVariant internalDataAlbum(int albumIndex, int role) const;

    AllAlbumsModelPrivate *d;

};

#endif // ALLALBUMSMODEL_H
