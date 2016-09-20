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

#ifndef MANAGEHEADERBAR_H
#define MANAGEHEADERBAR_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QPersistentModelIndex>
#include <QtCore/QAbstractItemModel>
#include <QtCore/QModelIndex>
#include <QtCore/QUrl>

class ManageHeaderBar : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QPersistentModelIndex currentTrack
               READ currentTrack
               WRITE setCurrentTrack
               NOTIFY currentTrackChanged)

    Q_PROPERTY(QAbstractItemModel* playListModel
               READ playListModel
               WRITE setPlayListModel
               NOTIFY playListModelChanged)

    Q_PROPERTY(int urlRole
               READ urlRole
               WRITE setUrlRole
               NOTIFY urlRoleChanged)

    Q_PROPERTY(int artistRole
               READ artistRole
               WRITE setArtistRole
               NOTIFY artistRoleChanged)

    Q_PROPERTY(int titleRole
               READ titleRole
               WRITE setTitleRole
               NOTIFY titleRoleChanged)

    Q_PROPERTY(int albumRole
               READ albumRole
               WRITE setAlbumRole
               NOTIFY albumRoleChanged)

    Q_PROPERTY(int imageRole
               READ imageRole
               WRITE setImageRole
               NOTIFY imageRoleChanged)

    Q_PROPERTY(int isValidRole
               READ isValidRole
               WRITE setIsValidRole
               NOTIFY isValidRoleChanged)

    Q_PROPERTY(QUrl playerSource
               READ playerSource
               NOTIFY playerSourceChanged)

    Q_PROPERTY(QVariant artist
               READ artist
               NOTIFY artistChanged)

    Q_PROPERTY(QVariant title
               READ title
               NOTIFY titleChanged)

    Q_PROPERTY(QVariant album
               READ album
               NOTIFY albumChanged)

    Q_PROPERTY(QUrl image
               READ image
               NOTIFY imageChanged)

    Q_PROPERTY(bool isValid
               READ isValid
               NOTIFY isValidChanged)

    Q_PROPERTY(int remainingTracks
               READ remainingTracks
               NOTIFY remainingTracksChanged)

public:

    explicit ManageHeaderBar(QObject *parent = 0);

    QPersistentModelIndex currentTrack() const;

    QAbstractItemModel* playListModel() const;

    int urlRole() const;

    int artistRole() const;

    int titleRole() const;

    int albumRole() const;

    int imageRole() const;

    int isValidRole() const;

    QUrl playerSource() const;

    QVariant artist() const;

    QVariant title() const;

    QVariant album() const;

    QUrl image() const;

    bool isValid() const;

    int remainingTracks() const;

Q_SIGNALS:

    void currentTrackChanged();

    void playListModelChanged();

    void urlRoleChanged();

    void artistRoleChanged();

    void titleRoleChanged();

    void albumRoleChanged();

    void imageRoleChanged();

    void isValidRoleChanged();

    void playerSourceChanged();

    void artistChanged();

    void titleChanged();

    void albumChanged();

    void imageChanged();

    void remainingTracksChanged();

    void isValidChanged();

public Q_SLOTS:

    void setCurrentTrack(QPersistentModelIndex currentTrack);

    void setPlayListModel(QAbstractItemModel* aPlayListModel);

    void setUrlRole(int value);

    void setArtistRole(int value);

    void setTitleRole(int value);

    void setAlbumRole(int value);

    void setImageRole(int value);

    void setIsValidRole(int isValidRole);

    void playListLayoutChanged(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint);

    void tracksInserted(const QModelIndex &parent, int first, int last);

    void tracksDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);

    void tracksMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);

    void tracksRemoved(const QModelIndex &parent, int first, int last);

private:

    void notifyPlayerSourceProperty();

    void notifyArtistProperty();

    void notifyTitleProperty();

    void notifyAlbumProperty();

    void notifyImageProperty();

    void notifyIsValidProperty();

    QPersistentModelIndex mCurrentTrack;

    QAbstractItemModel *mPlayListModel = nullptr;

    int mUrlRole = Qt::DisplayRole;

    int mArtistRole = Qt::DisplayRole;

    int mTitleRole = Qt::DisplayRole;

    int mAlbumRole = Qt::DisplayRole;

    int mImageRole = Qt::DisplayRole;

    int mIsValidRole = Qt::DisplayRole;

    QVariant mOldPlayerSource;

    QVariant mOldArtist;

    QVariant mOldTitle;

    QVariant mOldAlbum;

    QVariant mOldImage;

    bool mOldIsValid = false;

};



#endif // MANAGEHEADERBAR_H
