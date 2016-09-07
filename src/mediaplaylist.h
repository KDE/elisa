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
class DatabaseInterface;

class MediaPlayListEntry
{
public:

    MediaPlayListEntry() {
    }

    explicit MediaPlayListEntry(qulonglong id) : mId(id), mIsValid(true) {
    }

    MediaPlayListEntry(QString title, QString album, QString artist) : mTitle(title), mAlbum(album), mArtist(artist), mIsValid(false) {
    }

    QString mTitle;

    QString mAlbum;

    QString mArtist;

    qulonglong mId = 0;

    bool mIsValid = false;

    bool mIsPlaying = false;

};

class MediaPlayList : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(DatabaseInterface* databaseInterface
               READ databaseInterface
               WRITE setDatabaseInterface
               NOTIFY databaseInterfaceChanged)

    Q_PROPERTY(int trackCount
               READ trackCount
               NOTIFY trackCountChanged)

    Q_PROPERTY(QList<QVariant> persistentState
               READ persistentState
               WRITE setPersistentState
               NOTIFY persistentStateChanged)

public:

    enum ColumnsRoles {
        IsValidRole = Qt::UserRole + 1,
        TitleRole = IsValidRole + 1,
        DurationRole = TitleRole + 1,
        MilliSecondsDurationRole = DurationRole + 1,
        CreatorRole = MilliSecondsDurationRole + 1,
        ArtistRole = CreatorRole + 1,
        AlbumRole = ArtistRole + 1,
        TrackNumberRole = AlbumRole + 1,
        RatingRole = TrackNumberRole + 1,
        ImageRole = RatingRole + 1,
        ResourceRole = ImageRole + 1,
        CountRole = ResourceRole + 1,
        IsPlayingRole = CountRole + 1,
    };

#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
    Q_ENUM(ColumnsRoles)
#else
    Q_ENUMS(ColumnsRoles)
#endif

    MediaPlayList(QObject *parent = 0);

    ~MediaPlayList();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    int trackCount() const;

    Q_INVOKABLE void enqueue(qulonglong newTrackId);

    Q_INVOKABLE void enqueue(MediaPlayListEntry newEntry);

    Q_INVOKABLE void move(int from, int to, int n);

    DatabaseInterface* databaseInterface() const;

    QList<QVariant> persistentState() const;

Q_SIGNALS:

    void trackCountChanged();

    void trackHasBeenAdded(const QString &title, const QUrl &image);

    void databaseInterfaceChanged();

    void persistentStateChanged();

public Q_SLOTS:

    void setDatabaseInterface(DatabaseInterface* musicDatabase);

    void setPersistentState(QList<QVariant> persistentState);

private Q_SLOTS:

    void endTrackAdded(QVector<qulonglong> newTracks);

private:

    MediaPlayListPrivate *d;

};



#endif // MEDIAPLAYLIST_H
