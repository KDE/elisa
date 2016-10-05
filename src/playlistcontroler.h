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

#ifndef PLAYLISTCONTROLER_H
#define PLAYLISTCONTROLER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QPersistentModelIndex>
#include <QtCore/QAbstractItemModel>
#include <QtCore/QModelIndex>
#include <QtCore/QUrl>

class QDataStream;

class PlayListControler : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QPersistentModelIndex currentTrack
               READ currentTrack
               NOTIFY currentTrackChanged)

    Q_PROPERTY(QAbstractItemModel* playListModel
               READ playListModel
               WRITE setPlayListModel
               NOTIFY playListModelChanged)

    Q_PROPERTY(int isPlayingRole
               READ isPlayingRole
               WRITE setIsPlayingRole
               NOTIFY isPlayingRoleChanged)

    Q_PROPERTY(int isValidRole
               READ isValidRole
               WRITE setIsValidRole
               NOTIFY isValidRoleChanged)

    Q_PROPERTY(bool randomPlay
               READ randomPlay
               WRITE setRandomPlay
               NOTIFY randomPlayChanged)

    Q_PROPERTY(bool randomPlayControl
               READ randomPlayControl
               WRITE setRandomPlayControl
               NOTIFY randomPlayControlChanged)

    Q_PROPERTY(bool repeatPlay
               READ repeatPlay
               WRITE setRepeatPlay
               NOTIFY repeatPlayChanged)

    Q_PROPERTY(bool repeatPlayControl
               READ repeatPlayControl
               WRITE setRepeatPlayControl
               NOTIFY repeatPlayControlChanged)

    Q_PROPERTY(QVariantMap persistentState
               READ persistentState
               WRITE setPersistentState
               NOTIFY persistentStateChanged)

public:

    explicit PlayListControler(QObject *parent = 0);

    QPersistentModelIndex currentTrack() const;

    QAbstractItemModel* playListModel() const;

    int isPlayingRole() const;

    int isValidRole() const;

    int remainingTracks() const;

    bool randomPlay() const;

    bool randomPlayControl() const;

    bool repeatPlay() const;

    bool repeatPlayControl() const;

    QVariantMap persistentState() const;

Q_SIGNALS:

    void currentTrackChanged();

    void playListModelChanged();

    void isPlayingRoleChanged();

    void isValidRoleChanged();

    void randomPlayChanged();

    void randomPlayControlChanged();

    void repeatPlayChanged();

    void repeatPlayControlChanged();

    void persistentStateChanged();

public Q_SLOTS:

    void setPlayListModel(QAbstractItemModel* aPlayListModel);

    void setIsPlayingRole(int value);

    void setIsValidRole(int isValidRole);

    void setRandomPlay(bool value);

    void setRandomPlayControl(bool randomPlayControl);

    void setRepeatPlay(bool value);

    void setRepeatPlayControl(bool repeatPlayControl);

    void setPersistentState(QVariantMap persistentStateValue);

    void playListReset();

    void playListLayoutChanged(const QList<QPersistentModelIndex> &parents = QList<QPersistentModelIndex>(),
                               QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::NoLayoutChangeHint);

    void tracksInserted(const QModelIndex &parent, int first, int last);

    void tracksMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);

    void tracksRemoved(const QModelIndex & parent, int first, int last);

    void tracksDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int> ());

    void skipNextTrack();

    void skipPreviousTrack();

private:

    void gotoNextTrack();

    void resetCurrentTrack();

    void restorePlayListPosition();

    void restoreRandomPlay();

    void restoreRepeatPlay();

    QPersistentModelIndex mCurrentTrack;

    bool mCurrentTrackIsValid = false;

    QAbstractItemModel *mPlayListModel = nullptr;

    int mIsPlayingRole = Qt::DisplayRole;

    int mIsValidRole = Qt::DisplayRole;

    bool mRandomPlay = false;

    bool mRandomPlayControl = false;

    bool mRepeatPlay = false;

    bool mRepeatPlayControl = false;

    QVariantMap mPersistentState;

};

#endif // PLAYLISTCONTROLER_H
