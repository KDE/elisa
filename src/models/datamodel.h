/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef DATAMODEL_H
#define DATAMODEL_H

#include "elisaLib_export.h"

#include "elisautils.h"
#include "datatypes.h"

#include <QAbstractListModel>
#include <QHash>
#include <QString>

#include <memory>

class DataModelPrivate;
class MusicListenersManager;
class DatabaseInterface;

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

    using ListRadioDataType = DataTypes::ListRadioDataType;

    using ListTrackDataType = DataTypes::ListTrackDataType;

    using TrackDataType = DataTypes::TrackDataType;

    using ListAlbumDataType = DataTypes::ListAlbumDataType;

    using AlbumDataType = DataTypes::AlbumDataType;

    using ListArtistDataType = DataTypes::ListArtistDataType;

    using ArtistDataType = DataTypes::ArtistDataType;

    using ListGenreDataType = DataTypes::ListGenreDataType;

    using GenreDataType = DataTypes::GenreDataType;

    using FilterType = ElisaUtils::FilterType;

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

    void needDataById(ElisaUtils::PlayListEntryType dataType, qulonglong databaseId);

    void needDataByGenre(ElisaUtils::PlayListEntryType dataType, const QString &genre);

    void needDataByArtist(ElisaUtils::PlayListEntryType dataType, const QString &artist);

    void needDataByGenreAndArtist(ElisaUtils::PlayListEntryType dataType,
                                  const QString &genre, const QString &artist);

    void needRecentlyPlayedData(ElisaUtils::PlayListEntryType dataType);

    void needFrequentlyPlayedData(ElisaUtils::PlayListEntryType dataType);

    void isBusyChanged();

public Q_SLOTS:

    void tracksAdded(DataModel::ListTrackDataType newData);

    void radiosAdded(DataModel::ListRadioDataType newData);

    void trackModified(const DataModel::TrackDataType &modifiedTrack);

    void trackRemoved(qulonglong removedTrackId);

    void radioRemoved(qulonglong removedRadioId);

    void genresAdded(DataModel::ListGenreDataType newData);

    void artistsAdded(DataModel::ListArtistDataType newData);

    void artistRemoved(qulonglong removedDatabaseId);

    void albumsAdded(DataModel::ListAlbumDataType newData);

    void albumRemoved(qulonglong removedDatabaseId);

    void albumModified(const DataModel::AlbumDataType &modifiedAlbum);

    void initialize(MusicListenersManager *manager, DatabaseInterface *database,
                    ElisaUtils::PlayListEntryType modelType, ElisaUtils::FilterType filter,
                    const QString &genre, const QString &artist, qulonglong databaseId);

private Q_SLOTS:

    void cleanedDatabase();

private:

    void radioAdded(const TrackDataType &radiosData);

    void radioModified(const DataModel::TrackDataType &modifiedRadio);

    int indexFromId(qulonglong id) const;

    void connectModel(DatabaseInterface *database);

    void setBusy(bool value);

    void initializeModel(MusicListenersManager *manager, DatabaseInterface *database,
                         ElisaUtils::PlayListEntryType modelType, ElisaUtils::FilterType type);

    void askModelData();

    void removeRadios();

    std::unique_ptr<DataModelPrivate> d;

};

#endif // DATAMODEL_H
