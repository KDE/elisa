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

#ifndef MANAGEAUDIOPLAYER_H
#define MANAGEAUDIOPLAYER_H

#include <QtCore/QObject>
#include <QtCore/QPersistentModelIndex>
#include <QtCore/QAbstractItemModel>
#include <QtCore/QUrl>

class ManageAudioPlayer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QPersistentModelIndex currentTrack
               READ currentTrack
               WRITE setCurrentTrack
               NOTIFY currentTrackChanged)

    Q_PROPERTY(QUrl playerSource
               READ playerSource
               NOTIFY playerSourceChanged)

    Q_PROPERTY(int urlRole
               READ urlRole
               WRITE setUrlRole
               NOTIFY urlRoleChanged)

public:

    explicit ManageAudioPlayer(QObject *parent = 0);

    QPersistentModelIndex currentTrack() const;

    int urlRole() const;

    QUrl playerSource() const;

Q_SIGNALS:

    void currentTrackChanged();

    void playerSourceChanged();

    void urlRoleChanged();

public Q_SLOTS:

    void setCurrentTrack(QPersistentModelIndex currentTrack);

    void setUrlRole(int value);

private:

    void notifyPlayerSourceProperty();

    QPersistentModelIndex mCurrentTrack;

    QAbstractItemModel *mPlayListModel = nullptr;

    int mUrlRole = Qt::DisplayRole;

    QVariant mOldPlayerSource;

};

#endif // MANAGEAUDIOPLAYER_H
