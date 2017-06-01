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

#include "abstractalbummodel.h"
#include "musicstatistics.h"

#include <QtCore/QUrl>
#include <QtCore/QTimer>
#include <QtCore/QPointer>
#include <QtCore/QVector>

#include <QtCore/QDebug>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>

class AbstractAlbumModelPrivate
{
public:

    bool mUseLocalIcons = false;

    MusicStatistics *mMusicDatabase = nullptr;

    QVector<MusicAlbum> mAlbumsData;

    QMap<quintptr, int> mAlbumsIndex;

    QMap<quintptr, quintptr> mTracksInAlbums;

    QSqlDatabase mTracksDatabase;

};

AbstractAlbumModel::AbstractAlbumModel(QObject *parent) : QAbstractItemModel(parent), d(new AbstractAlbumModelPrivate)
{
    static int databaseCounter = 1;
    d->mTracksDatabase = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), QStringLiteral("album%1").arg(databaseCounter));
    ++databaseCounter;
    d->mTracksDatabase.setDatabaseName(QStringLiteral(":memory:"));
    d->mTracksDatabase.setConnectOptions(QStringLiteral("foreign_keys = ON"));
    auto result = d->mTracksDatabase.open();
    if (result) {
        qDebug() << "database open";
    } else {
        qDebug() << "database not open";
    }

    Q_EMIT refreshContent();
}

AbstractAlbumModel::~AbstractAlbumModel()
{
    delete d;
}

int AbstractAlbumModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return d->mAlbumsData.size();
    }

    if (parent.row() < 0 || parent.row() >= d->mAlbumsData.size()) {
        return 0;
    }

    return d->mAlbumsData[parent.row()].tracksCount();
}

QHash<int, QByteArray> AbstractAlbumModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[static_cast<int>(ColumnsRoles::TitleRole)] = "title";
    roles[static_cast<int>(ColumnsRoles::DurationRole)] = "duration";
    roles[static_cast<int>(ColumnsRoles::ArtistRole)] = "artist";
    roles[static_cast<int>(ColumnsRoles::AlbumRole)] = "album";
    roles[static_cast<int>(ColumnsRoles::TrackNumberRole)] = "trackNumber";
    roles[static_cast<int>(ColumnsRoles::RatingRole)] = "rating";
    roles[static_cast<int>(ColumnsRoles::ImageRole)] = "image";
    roles[static_cast<int>(ColumnsRoles::ItemClassRole)] = "itemClass";
    roles[static_cast<int>(ColumnsRoles::CountRole)] = "count";
    roles[static_cast<int>(ColumnsRoles::IsPlayingRole)] = "isPlaying";

    return roles;
}

Qt::ItemFlags AbstractAlbumModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant AbstractAlbumModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    if (index.column() != 0) {
        return {};
    }

    if (index.row() < 0) {
        return {};
    }

    if (!index.parent().isValid()) {
        if (index.internalId() == 0) {
            if (index.row() < 0 || index.row() >= d->mAlbumsData.size()) {
                return {};
            }

            const auto &itAlbum = d->mAlbumsData[index.row()];
            return internalDataAlbum(itAlbum, role);
        }
    }

    auto itTrack = d->mTracksInAlbums.find(index.internalId());
    if (itTrack == d->mTracksInAlbums.end()) {
        return {};
    }

    auto albumId = itTrack.value();
    if (albumId >= quintptr(d->mAlbumsData.size())) {
        return {};
    }

    const auto &currentAlbum = d->mAlbumsData[albumId];

    if (index.row() < 0 || index.row() >= currentAlbum.tracksCount()) {
        return {};
    }

    return internalDataTrack(currentAlbum.trackFromIndex(index.row()), index, role);
}

QVariant AbstractAlbumModel::internalDataAlbum(const MusicAlbum &albumData, int role) const
{
    ColumnsRoles convertedRole = static_cast<ColumnsRoles>(role);

    switch(convertedRole)
    {
    case ColumnsRoles::TitleRole:
        return albumData.title();
    case ColumnsRoles::DurationRole:
        return {};
    case ColumnsRoles::CreatorRole:
        return {};
    case ColumnsRoles::ArtistRole:
        return albumData.artist();
    case ColumnsRoles::AlbumRole:
        return {};
    case ColumnsRoles::TrackNumberRole:
        return {};
    case ColumnsRoles::RatingRole:
        return {};
    case ColumnsRoles::ImageRole:
        if (albumData.albumArtURI().isValid()) {
            return albumData.albumArtURI();
        } else {
            if (d->mUseLocalIcons) {
                return QUrl(QStringLiteral("qrc:/media-optical-audio.svg"));
            } else {
                return QUrl(QStringLiteral("image://icon/media-optical-audio"));
            }
        }
    case ColumnsRoles::ResourceRole:
        return {};
    case ColumnsRoles::ItemClassRole:
        return {};
    case ColumnsRoles::CountRole:
        return albumData.tracksCount();
    case ColumnsRoles::IdRole:
        return albumData.title();
    case ColumnsRoles::IsPlayingRole:
        return {};
    }

    return {};
}

QVariant AbstractAlbumModel::internalDataTrack(const MusicAudioTrack &track, const QModelIndex &index, int role) const
{
    ColumnsRoles convertedRole = static_cast<ColumnsRoles>(role);

    switch(convertedRole)
    {
    case ColumnsRoles::TitleRole:
        return track.title();
    case ColumnsRoles::DurationRole:
    {
        QTime trackDuration = track.duration();
        if (trackDuration.hour() == 0) {
            return trackDuration.toString(QStringLiteral("mm:ss"));
        } else {
            return trackDuration.toString();
        }
    }
    case ColumnsRoles::CreatorRole:
        return track.artist();
    case ColumnsRoles::ArtistRole:
        return track.artist();
    case ColumnsRoles::AlbumRole:
        return track.albumName();
    case ColumnsRoles::TrackNumberRole:
        return track.trackNumber();
    case ColumnsRoles::RatingRole:
        return 0;
    case ColumnsRoles::ImageRole:
        return data(index.parent(), role);
    case ColumnsRoles::ResourceRole:
        return track.resourceURI();
    case ColumnsRoles::ItemClassRole:
        return {};
    case ColumnsRoles::CountRole:
        return {};
    case ColumnsRoles::IdRole:
        return track.title();
    case ColumnsRoles::IsPlayingRole:
        return false;
    }

    return {};
}

void AbstractAlbumModel::initDatabase()
{
    if (!d->mTracksDatabase.tables().contains(QStringLiteral("Albums"))) {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        createSchemaQuery.exec(QStringLiteral("CREATE TABLE `Albums` (`ID` INTEGER PRIMARY KEY NOT NULL, "
                                              "`Title` TEXT NOT NULL, "
                                              "`Artist` TEXT NOT NULL, "
                                              "`CoverFileName` TEXT NOT NULL, "
                                              "`TracksCount` INTEGER NOT NULL, "
                                              "UNIQUE (`Title`, `Artist`))"));

        qDebug() << "AbstractAlbumModel::initDatabase" << createSchemaQuery.lastError();
    }

    if (!d->mTracksDatabase.tables().contains(QStringLiteral("Tracks"))) {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        createSchemaQuery.exec(QStringLiteral("CREATE TABLE `Tracks` (`ID` INTEGER PRIMARY KEY NOT NULL, "
                                              "`Title` TEXT NOT NULL, "
                                              "`Album` TEXT NOT NULL, "
                                              "`Artist` TEXT NOT NULL, "
                                              "`FileName` TEXT NOT NULL UNIQUE, "
                                              "UNIQUE (`Title`, `Album`, `Artist`), "
                                              "CONSTRAINT fk_album FOREIGN KEY (`Album`, `Artist`) REFERENCES `Albums`(`Title`, `Artist`))"));

        qDebug() << "AbstractAlbumModel::initDatabase" << createSchemaQuery.lastError();
    }
}

QModelIndex AbstractAlbumModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column != 0) {
        return {};
    }

    if (!parent.isValid()) {
        return createIndex(row, column, nullptr);
    }

    if (parent.row() < 0 || parent.row() >= d->mAlbumsData.size()) {
        return {};
    }

    const auto &currentAlbum = d->mAlbumsData[parent.row()];

    if (row >= currentAlbum.tracksCount()) {
        return {};
    }

    return createIndex(row, column, currentAlbum.trackIdFromIndex(row));
}

QModelIndex AbstractAlbumModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()) {
        return {};
    }

    if (child.internalId() == 0) {
        return {};
    }

    auto itTrack = d->mTracksInAlbums.find(child.internalId());
    if (itTrack == d->mTracksInAlbums.end()) {
        return {};
    }

    auto albumId = itTrack.value();

    return index(albumId, 0);
}

int AbstractAlbumModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 1;
}

MusicStatistics *AbstractAlbumModel::musicDatabase() const
{
    return d->mMusicDatabase;
}

void AbstractAlbumModel::setMusicDatabase(MusicStatistics *musicDatabase)
{
    if (d->mMusicDatabase == musicDatabase)
        return;

    if (d->mMusicDatabase) {
        disconnect(this, &AbstractAlbumModel::newAlbum, d->mMusicDatabase, &MusicStatistics::newAlbum);
        disconnect(this, &AbstractAlbumModel::newAudioTrack, d->mMusicDatabase, &MusicStatistics::newAudioTrack);
    }

    d->mMusicDatabase = musicDatabase;

    if (d->mMusicDatabase) {
        connect(this, &AbstractAlbumModel::newAlbum, d->mMusicDatabase, &MusicStatistics::newAlbum);
        connect(this, &AbstractAlbumModel::newAudioTrack, d->mMusicDatabase, &MusicStatistics::newAudioTrack);
    }

    emit musicDatabaseChanged();
}

void AbstractAlbumModel::albumsList(const QVector<MusicAlbum> &allAlbums)
{
    beginResetModel();

    initDatabase();

    d->mAlbumsData.clear();

    auto selectAlbumQueryText = QStringLiteral("SELECT ID FROM `Albums` "
                                          "WHERE "
                                          "`Title` = :title AND "
                                          "`Artist` = :artist");

    QSqlQuery selectAlbumQuery(d->mTracksDatabase);
    selectAlbumQuery.prepare(selectAlbumQueryText);

    auto insertAlbumQueryText = QStringLiteral("INSERT INTO Albums (`Title`, `Artist`, `CoverFileName`, `TracksCount`)"
                                          "VALUES (:title, :artist, :coverFileName, :tracksCount)");

    QSqlQuery insertAlbumQuery(d->mTracksDatabase);
    insertAlbumQuery.prepare(insertAlbumQueryText);

    for(const auto &album : allAlbums) {
        quintptr albumId = 0;

        selectAlbumQuery.bindValue(QStringLiteral(":title"), album.title());
        selectAlbumQuery.bindValue(QStringLiteral(":artist"), album.artist());

        selectAlbumQuery.exec();

        if (!selectAlbumQuery.isSelect() || !selectAlbumQuery.isActive()) {
            qDebug() << "AbstractAlbumModel::albumsList" << "not select" << selectAlbumQuery.lastQuery();
            qDebug() << "AbstractAlbumModel::albumsList" << selectAlbumQuery.lastError();
        }

        if (selectAlbumQuery.next()) {
            albumId = selectAlbumQuery.record().value(0).toInt() - 1;
        } else {
            insertAlbumQuery.bindValue(QStringLiteral(":title"), album.title());
            insertAlbumQuery.bindValue(QStringLiteral(":artist"), album.artist());
            insertAlbumQuery.bindValue(QStringLiteral(":coverFileName"), album.albumArtURI());
            insertAlbumQuery.bindValue(QStringLiteral(":tracksCount"), 0);

            insertAlbumQuery.exec();

            if (!insertAlbumQuery.isActive()) {
                qDebug() << "AbstractAlbumModel::albumsList" << "not select" << insertAlbumQuery.lastQuery();
                qDebug() << "AbstractAlbumModel::albumsList" << insertAlbumQuery.lastError();

                continue;
            }

            selectAlbumQuery.exec();

            if (!selectAlbumQuery.isSelect() || !selectAlbumQuery.isActive()) {
                qDebug() << "AbstractAlbumModel::albumsList" << "not select" << selectAlbumQuery.lastQuery();
                qDebug() << "AbstractAlbumModel::albumsList" << selectAlbumQuery.lastError();

                continue;
            }

            if (selectAlbumQuery.next()) {
                albumId = selectAlbumQuery.record().value(0).toInt() - 1;
            }
        }

        d->mAlbumsData.push_back(album);
        d->mAlbumsIndex[albumId] = d->mAlbumsData.size() - 1;
    }

    endResetModel();
}

void AbstractAlbumModel::tracksList(const QHash<QString, QVector<MusicAudioTrack> > &tracks, const QHash<QString, QString> &covers)
{
    const bool insertManyAlbums = (tracks.size() > 10);

    if (insertManyAlbums) {
        beginResetModel();
    }

    initDatabase();

    //d->mAlbumsData.clear();

    auto selectTrackQueryText = QStringLiteral("SELECT ID FROM `Tracks` "
                                          "WHERE "
                                          "`Title` = :title AND "
                                          "`Album` = :album AND "
                                          "`Artist` = :artist");

    QSqlQuery selectTrackQuery(d->mTracksDatabase);
    selectTrackQuery.prepare(selectTrackQueryText);

    auto insertTrackQueryText = QStringLiteral("INSERT INTO Tracks (`Title`, `Album`, `Artist`, `FileName`)"
                                          "VALUES (:title, :album, :artist, :fileName)");

    QSqlQuery insertTrackQuery(d->mTracksDatabase);
    insertTrackQuery.prepare(insertTrackQueryText);

    auto selectAlbumQueryText = QStringLiteral("SELECT ID FROM `Albums` "
                                          "WHERE "
                                          "`Title` = :title AND "
                                          "`Artist` = :artist");

    QSqlQuery selectAlbumQuery(d->mTracksDatabase);
    selectAlbumQuery.prepare(selectAlbumQueryText);

    auto insertAlbumQueryText = QStringLiteral("INSERT INTO Albums (`Title`, `Artist`, `CoverFileName`, `TracksCount`)"
                                          "VALUES (:title, :artist, :coverFileName, :tracksCount)");

    QSqlQuery insertAlbumQuery(d->mTracksDatabase);
    insertAlbumQuery.prepare(insertAlbumQueryText);

    quintptr albumId = 0;
    for (const auto &album : tracks) {
        MusicAlbum newAlbum;

        for(const auto &track : album) {
            if (newAlbum.artist().isNull()) {
                newAlbum.setArtist(track.artist());
            }

            if (newAlbum.title().isNull()) {
                newAlbum.setTitle(track.albumName());
            }

            if (newAlbum.albumArtURI().isEmpty()) {
                newAlbum.setAlbumArtURI(QUrl::fromLocalFile(covers[track.albumName()]));
            }

            if (!newAlbum.artist().isNull() && !newAlbum.title().isNull() && !newAlbum.albumArtURI().isEmpty()) {
                break;
            }
        }

        selectAlbumQuery.bindValue(QStringLiteral(":title"), newAlbum.title());
        selectAlbumQuery.bindValue(QStringLiteral(":artist"), newAlbum.artist());

        selectAlbumQuery.exec();

        if (!selectAlbumQuery.isSelect() || !selectAlbumQuery.isActive()) {
            qDebug() << "AbstractAlbumModel::tracksList" << "not select" << selectAlbumQuery.lastQuery();
            qDebug() << "AbstractAlbumModel::tracksList" << selectAlbumQuery.lastError();
        }

        if (selectAlbumQuery.next()) {
            albumId = selectAlbumQuery.record().value(0).toInt() - 1;
        } else {
            if (!insertManyAlbums) {
                beginInsertRows({}, d->mAlbumsData.size(), d->mAlbumsData.size());
            }

            insertAlbumQuery.bindValue(QStringLiteral(":title"), newAlbum.title());
            insertAlbumQuery.bindValue(QStringLiteral(":artist"), newAlbum.artist());
            insertAlbumQuery.bindValue(QStringLiteral(":coverFileName"), newAlbum.albumArtURI());
            insertAlbumQuery.bindValue(QStringLiteral(":tracksCount"), 0);

            insertAlbumQuery.exec();

            if (!insertAlbumQuery.isActive()) {
                qDebug() << "AbstractAlbumModel::tracksList" << "not select" << insertAlbumQuery.lastQuery();
                qDebug() << "AbstractAlbumModel::tracksList" << insertAlbumQuery.lastError();

                continue;
            }

            selectAlbumQuery.exec();

            if (!selectAlbumQuery.isSelect() || !selectAlbumQuery.isActive()) {
                qDebug() << "AbstractAlbumModel::tracksList" << "not select" << selectAlbumQuery.lastQuery();
                qDebug() << "AbstractAlbumModel::tracksList" << selectAlbumQuery.lastError();

                continue;
            }

            if (selectAlbumQuery.next()) {
                albumId = selectAlbumQuery.record().value(0).toInt() - 1;
            }

            d->mAlbumsData.push_back(newAlbum);
            d->mAlbumsIndex[albumId] = d->mAlbumsData.size() - 1;

            if (!insertManyAlbums) {
                endInsertRows();
            }
        }

        auto &currentAlbum = d->mAlbumsData[d->mAlbumsIndex[albumId]];

        if (!currentAlbum.isEmpty()) {
            beginRemoveRows(index(d->mAlbumsIndex[albumId], 0), 0, currentAlbum.tracksCount() - 1);
            currentAlbum.clear();
            endRemoveRows();
        }

        if (!insertManyAlbums) {
            beginInsertRows(index(d->mAlbumsIndex[albumId], 0), 0, album.size() - 1);
        }

        for(const auto &track : album) {
            quintptr currentElementId = 0;
            QString artistName = track.artist();

            if (artistName.isEmpty()) {
                artistName = currentAlbum.artist();
            }

            selectTrackQuery.bindValue(QStringLiteral(":title"), track.title());
            selectTrackQuery.bindValue(QStringLiteral(":album"), track.albumName());
            selectTrackQuery.bindValue(QStringLiteral(":artist"), artistName);

            selectTrackQuery.exec();

            if (!selectTrackQuery.isSelect() || !selectTrackQuery.isActive()) {
                qDebug() << "AbstractAlbumModel::tracksList" << "not select" << selectTrackQuery.lastQuery();
                qDebug() << "AbstractAlbumModel::tracksList" << selectTrackQuery.lastError();
            }

            if (selectTrackQuery.next()) {
                currentElementId = selectTrackQuery.record().value(0).toInt();
                continue;
            } else {
                insertTrackQuery.bindValue(QStringLiteral(":title"), track.title());
                insertTrackQuery.bindValue(QStringLiteral(":album"), track.albumName());
                insertTrackQuery.bindValue(QStringLiteral(":artist"), artistName);
                insertTrackQuery.bindValue(QStringLiteral(":fileName"), track.resourceURI());

                insertTrackQuery.exec();

                if (!insertTrackQuery.isActive()) {
                    qDebug() << "AbstractAlbumModel::tracksList" << "not select" << insertTrackQuery.lastQuery();
                    qDebug() << "AbstractAlbumModel::tracksList" << insertTrackQuery.lastError();
                }

                selectTrackQuery.exec();

                if (!selectTrackQuery.isSelect() || !selectTrackQuery.isActive()) {
                    qDebug() << "AbstractAlbumModel::tracksList" << "not select" << selectTrackQuery.lastQuery();
                    qDebug() << "AbstractAlbumModel::tracksList" << selectTrackQuery.lastError();
                }

                if (selectTrackQuery.next()) {
                    currentElementId = selectTrackQuery.record().value(0).toInt();
                }
            }

            currentAlbum.insertTrack(track, currentElementId);
            d->mTracksInAlbums[currentElementId] = albumId;
        }

        if (!insertManyAlbums) {
            endInsertRows();
        }
    }

    if (insertManyAlbums) {
        endResetModel();
    }
}

#include "moc_abstractalbummodel.cpp"
