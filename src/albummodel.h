/*
 * Copyright 2015-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef ALBUMMODEL_H
#define ALBUMMODEL_H

#include <QAbstractItemModel>
#include <QVector>
#include <QHash>
#include <QString>

#include "musicalbum.h"
#include "musicaudiotrack.h"

class DatabaseInterface;
class AlbumModelPrivate;
class MusicStatistics;
class QMutex;

class AlbumModel : public QAbstractItemModel
{
    Q_OBJECT

    Q_PROPERTY(MusicAlbum albumData
               READ albumData
               WRITE setAlbumData
               NOTIFY albumDataChanged)

    Q_PROPERTY(QString title
               READ title
               WRITE setTitle
               NOTIFY titleChanged)

    Q_PROPERTY(QString author
               READ author
               WRITE setAuthor
               NOTIFY authorChanged)

public:

    enum ItemClass {
        Container = 0,
        Album = 1,
        Artist = 2,
        AudioTrack = 3,
    };

    enum ColumnsRoles {
        TitleRole = Qt::UserRole + 1,
        DurationRole = TitleRole + 1,
        MilliSecondsDurationRole = DurationRole + 1,
        CreatorRole = MilliSecondsDurationRole + 1,
        ArtistRole = CreatorRole + 1,
        AlbumRole = ArtistRole + 1,
        AlbumArtistRole = AlbumRole + 1,
        TrackNumberRole = AlbumArtistRole + 1,
        DiscNumberRole = TrackNumberRole + 1,
        RatingRole = DiscNumberRole + 1,
        ImageRole = RatingRole + 1,
        ResourceRole = ImageRole + 1,
        IdRole = ResourceRole + 1,
        DatabaseIdRole = IdRole + 1,
        DiscFirstTrackRole = DatabaseIdRole + 1,
        TrackDataRole = DiscFirstTrackRole + 1,
    };

    Q_ENUM(ColumnsRoles)

    explicit AlbumModel(QObject *parent = 0);

    ~AlbumModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex &child) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    MusicAlbum albumData() const;

    QString title() const;

    QString author() const;

Q_SIGNALS:

    void albumDataChanged();

    void titleChanged();

    void authorChanged();

public Q_SLOTS:

    void setAlbumData(const MusicAlbum &album);

    void setTitle(const QString &title);

    void setAuthor(const QString &author);

    void albumModified(const MusicAlbum &modifiedAlbum);

    void albumRemoved(const MusicAlbum &modifiedAlbum);

private:

    void trackAdded(const MusicAudioTrack &newTrack);

    void trackModified(const MusicAudioTrack &modifiedTrack);

    void trackRemoved(const MusicAudioTrack &removedTrack);

    QVariant internalDataTrack(const MusicAudioTrack &track, int role, int rowIndex) const;

    AlbumModelPrivate *d;

};

Q_DECLARE_METATYPE(AlbumModel::ItemClass)

#endif // ALBUMMODEL_H
