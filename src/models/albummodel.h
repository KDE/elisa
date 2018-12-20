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

#ifndef ALBUMMODEL_H
#define ALBUMMODEL_H

#include "elisaLib_export.h"

#include "elisautils.h"
#include "databaseinterface.h"

#include <QAbstractItemModel>
#include <QVector>
#include <QHash>
#include <QString>

#include <memory>

class AlbumModelPrivate;
class MusicListenersManager;

class ELISALIB_EXPORT AlbumModel : public QAbstractItemModel
{
    Q_OBJECT

    Q_PROPERTY(QString title
               READ title
               NOTIFY titleChanged)

    Q_PROPERTY(QString author
               READ author
               NOTIFY authorChanged)

    Q_PROPERTY(int tracksCount
               READ tracksCount
               NOTIFY tracksCountChanged)

public:

    using ListTrackDataType = DatabaseInterface::ListTrackDataType;

    using TrackDataType = DatabaseInterface::TrackDataType;

    using ListAlbumDataType = DatabaseInterface::ListAlbumDataType;

    using AlbumDataType = DatabaseInterface::AlbumDataType;

    explicit AlbumModel(QObject *parent = nullptr);

    ~AlbumModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex &child) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QString title() const;

    QString author() const;

    int tracksCount() const;

Q_SIGNALS:

    void titleChanged();

    void authorChanged();

    void tracksCountChanged();

    void needData(ElisaUtils::PlayListEntryType dataType);

public Q_SLOTS:

    void tracksAdded(const AlbumModel::ListTrackDataType &newTracks);

    void trackModified(const AlbumModel::TrackDataType &modifiedTrack);

    void trackRemoved(qulonglong trackId);

    void initialize(const QString &albumTitle, const QString &albumArtist, MusicListenersManager *manager);

private:

    int trackIndexFromId(qulonglong id) const;

    std::unique_ptr<AlbumModelPrivate> d;

};

#endif // ALBUMMODEL_H
