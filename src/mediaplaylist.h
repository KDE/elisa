/*
 * Copyright 2015 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef MEDIAPLAYLIST_H
#define MEDIAPLAYLIST_H

#include <QtCore/QAbstractListModel>

class MediaPlayListPrivate;

class MediaPlayList : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int trackCount
               READ trackCount
               NOTIFY trackCountChanged)

public:

    enum ColumnsRoles {
        TitleRole = Qt::UserRole + 1,
        DurationRole = TitleRole + 1,
        CreatorRole = DurationRole + 1,
        ArtistRole = CreatorRole + 1,
        AlbumRole = ArtistRole + 1,
        RatingRole = AlbumRole + 1,
        ImageRole = RatingRole + 1,
        ResourceRole = ImageRole + 1,
        ItemClassRole = ResourceRole + 1,
        CountRole = ItemClassRole + 1,
        IdRole = CountRole + 1,
        ParentIdRole = IdRole + 1,
        IsPlayingRole = ParentIdRole + 1,
    };
    Q_ENUM(ColumnsRoles)

    MediaPlayList(QObject *parent = 0);

    ~MediaPlayList();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    int trackCount() const;

    Q_INVOKABLE void enqueue(const QModelIndex &newTrack);

Q_SIGNALS:

    void trackCountChanged();

    void trackHasBeenAdded(const QString &title, const QUrl &image);

public Q_SLOTS:

private:

    MediaPlayListPrivate *d;

};

#endif // MEDIAPLAYLIST_H
