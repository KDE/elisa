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

#ifndef DATAMODEL_H
#define DATAMODEL_H

#include "elisaLib_export.h"

#include "elisautils.h"
#include "databaseinterface.h"

#include <QAbstractListModel>
#include <QVector>
#include <QHash>
#include <QString>

#include <memory>

class DataModelPrivate;
class MusicListenersManager;

class ELISALIB_EXPORT DataModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString title
               READ title
               NOTIFY titleChanged)

    Q_PROPERTY(QString author
               READ author
               NOTIFY authorChanged)

    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)

public:

    using ListTrackDataType = DatabaseInterface::ListTrackDataType;

    using TrackDataType = DatabaseInterface::TrackDataType;

    using ListAlbumDataType = DatabaseInterface::ListAlbumDataType;

    using AlbumDataType = DatabaseInterface::AlbumDataType;

    using ListArtistDataType = DatabaseInterface::ListArtistDataType;

    using ArtistDataType = DatabaseInterface::ArtistDataType;

    using ListGenreDataType = DatabaseInterface::ListGenreDataType;

    using GenreDataType = DatabaseInterface::GenreDataType;

    explicit DataModel(QObject *parent = nullptr);

    ~DataModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex &child) const override;

    QString title() const;

    QString author() const;

    bool isBusy() const;

Q_SIGNALS:

    void titleChanged();

    void authorChanged();

    void needData(ElisaUtils::PlayListEntryType dataType);

    void needDataByGenre(ElisaUtils::PlayListEntryType dataType, const QString &genre);

    void needDataByArtist(ElisaUtils::PlayListEntryType dataType, const QString &artist);

    void needDataByGenreAndArtist(ElisaUtils::PlayListEntryType dataType,
                                  const QString &genre, const QString &artist);

    void needRecentlyPlayedData(ElisaUtils::PlayListEntryType dataType);

    void needFrequentlyPlayedData(ElisaUtils::PlayListEntryType dataType);

    void isBusyChanged();

public Q_SLOTS:

    void tracksAdded(DataModel::ListTrackDataType newData);

    void trackModified(const DataModel::TrackDataType &modifiedTrack);

    void trackRemoved(qulonglong removedTrackId);

    void genresAdded(DataModel::ListGenreDataType newData);

    void artistsAdded(DataModel::ListArtistDataType newData);

    void artistRemoved(qulonglong removedDatabaseId);

    void albumsAdded(DataModel::ListAlbumDataType newData);

    void albumRemoved(qulonglong removedDatabaseId);

    void albumModified(const DataModel::AlbumDataType &modifiedAlbum);

    void initialize(MusicListenersManager *manager, ElisaUtils::PlayListEntryType modelType);

    void initializeByAlbumTitleAndArtist(MusicListenersManager *manager, ElisaUtils::PlayListEntryType modelType,
                                         const QString &albumTitle, const QString &albumArtist);

    void initializeByGenre(MusicListenersManager *manager, ElisaUtils::PlayListEntryType modelType,
                           const QString &genre);

    void initializeByArtist(MusicListenersManager *manager, ElisaUtils::PlayListEntryType modelType,
                            const QString &artist);

    void initializeByGenreAndArtist(MusicListenersManager *manager, ElisaUtils::PlayListEntryType modelType,
                                    const QString &genre, const QString &artist);

    void initializeRecentlyPlayed(MusicListenersManager *manager, ElisaUtils::PlayListEntryType modelType);

    void initializeFrequentlyPlayed(MusicListenersManager *manager, ElisaUtils::PlayListEntryType modelType);

private:

    int trackIndexFromId(qulonglong id) const;

    void connectModel(MusicListenersManager *manager);

    void setBusy(bool value);

    std::unique_ptr<DataModelPrivate> d;

};

#endif // DATAMODEL_H
