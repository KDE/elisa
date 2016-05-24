/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "databaseinterface.h"

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>

#include <QtCore/QVariant>
#include <QtCore/QMutexLocker>
#include <QtCore/QMutex>
#include <QtCore/QDebug>

class DatabaseInterfacePrivate
{
public:

    QMutex mModelMutex;

    QSqlDatabase mTracksDatabase;

};

DatabaseInterface::DatabaseInterface(QObject *parent) : QObject(parent), d(new DatabaseInterfacePrivate)
{
}

DatabaseInterface::~DatabaseInterface()
{
    delete d;
}

void DatabaseInterface::albumsList(const QVector<MusicAlbum> &allAlbums)
{
    QMutexLocker localLock(&d->mModelMutex);

    initDatabase();

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
        //quintptr albumId = 0;

        selectAlbumQuery.bindValue(QStringLiteral(":title"), album.mTitle);
        selectAlbumQuery.bindValue(QStringLiteral(":artist"), album.mArtist);

        selectAlbumQuery.exec();

        if (!selectAlbumQuery.isSelect() || !selectAlbumQuery.isActive()) {
            qDebug() << "AbstractAlbumModel::albumsList" << "not select" << selectAlbumQuery.lastQuery();
            qDebug() << "AbstractAlbumModel::albumsList" << selectAlbumQuery.lastError();
        }

        if (selectAlbumQuery.next()) {
            //albumId = selectAlbumQuery.record().value(0).toInt() - 1;
        } else {
            insertAlbumQuery.bindValue(QStringLiteral(":title"), album.mTitle);
            insertAlbumQuery.bindValue(QStringLiteral(":artist"), album.mArtist);
            insertAlbumQuery.bindValue(QStringLiteral(":coverFileName"), album.mAlbumArtURI);
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
                //albumId = selectAlbumQuery.record().value(0).toInt() - 1;
            }
        }
    }
}

void DatabaseInterface::tracksList(QHash<QString, QVector<MusicAudioTrack> > tracks, QHash<QString, QString> covers)
{
    QMutexLocker localLock(&d->mModelMutex);

    initDatabase();

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
            if (newAlbum.mArtist.isNull()) {
                newAlbum.mArtist = track.mArtist;
            }

            if (newAlbum.mTitle.isNull()) {
                newAlbum.mTitle = track.mAlbumName;
            }

            if (newAlbum.mAlbumArtURI.isEmpty()) {
                newAlbum.mAlbumArtURI = QUrl::fromLocalFile(covers[track.mAlbumName]);
            }

            if (!newAlbum.mArtist.isNull() && !newAlbum.mTitle.isNull() && !newAlbum.mAlbumArtURI.isEmpty()) {
                break;
            }
        }

        selectAlbumQuery.bindValue(QStringLiteral(":title"), newAlbum.mTitle);
        selectAlbumQuery.bindValue(QStringLiteral(":artist"), newAlbum.mArtist);

        selectAlbumQuery.exec();

        if (!selectAlbumQuery.isSelect() || !selectAlbumQuery.isActive()) {
            qDebug() << "AbstractAlbumModel::tracksList" << "not select" << selectAlbumQuery.lastQuery();
            qDebug() << "AbstractAlbumModel::tracksList" << selectAlbumQuery.lastError();
        }

        if (selectAlbumQuery.next()) {
            albumId = selectAlbumQuery.record().value(0).toInt() - 1;
        } else {
            insertAlbumQuery.bindValue(QStringLiteral(":title"), newAlbum.mTitle);
            insertAlbumQuery.bindValue(QStringLiteral(":artist"), newAlbum.mArtist);
            insertAlbumQuery.bindValue(QStringLiteral(":coverFileName"), newAlbum.mAlbumArtURI);
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
        }

        for(const auto &track : album) {
            quintptr currentElementId = 0;
            QString artistName = track.mArtist;

            if (artistName.isEmpty()) {
                artistName = newAlbum.mArtist;
            }

            selectTrackQuery.bindValue(QStringLiteral(":title"), track.mTitle);
            selectTrackQuery.bindValue(QStringLiteral(":albumID"), albumId);
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
                insertTrackQuery.bindValue(QStringLiteral(":title"), track.mTitle);
                insertTrackQuery.bindValue(QStringLiteral(":albumID"), albumId);
                insertTrackQuery.bindValue(QStringLiteral(":artist"), artistName);
                insertTrackQuery.bindValue(QStringLiteral(":fileName"), track.mResourceURI);

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
        }
    }
}

void DatabaseInterface::initDatabase()
{
    if (!d->mTracksDatabase.tables().contains(QStringLiteral("DiscoverSource"))) {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        createSchemaQuery.exec(QStringLiteral("CREATE TABLE `DiscoverSource` (`ID` INTEGER PRIMARY KEY NOT NULL, "
                                              "`UUID` TEXT NOT NULL, "
                                              "UNIQUE (`UUID`))"));

        qDebug() << "AbstractAlbumModel::initDatabase" << createSchemaQuery.lastError();
    }

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
                                              "`AlbumID` INTEGER NOT NULL, "
                                              "`Artist` TEXT NOT NULL, "
                                              "`FileName` TEXT NOT NULL UNIQUE, "
                                              "UNIQUE (`Title`, `Album`, `Artist`), "
                                              "CONSTRAINT fk_album FOREIGN KEY (`Album`, `Artist`) REFERENCES `Albums`(`Title`, `Artist`))"));

        qDebug() << "AbstractAlbumModel::initDatabase" << createSchemaQuery.lastError();
    }

    if (!d->mTracksDatabase.tables().contains(QStringLiteral("TracksMapping"))) {
        QSqlQuery createSchemaQuery(d->mTracksDatabase);

        createSchemaQuery.exec(QStringLiteral("CREATE TABLE `TracksMapping` (`TrackID` INTEGER NOT NULL, "
                                              "`DiscoverID` INTEGER NOT NULL, "
                                              "PRIMARY (`TrackID`, `DiscoverID`), "
                                              "CONSTRAINT fk_trackID FOREIGN KEY (`TrackID`) REFERENCES `Tracks`(`ID`)), "
                                              "CONSTRAINT fk_discoverID FOREIGN KEY (`DiscoverID`) REFERENCES `DiscoverSource`(`ID`))"));

        qDebug() << "AbstractAlbumModel::initDatabase" << createSchemaQuery.lastError();
    }
}


#include "moc_databaseinterface.cpp"
