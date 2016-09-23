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

#include "manageheaderbar.h"

#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtCore/QDebug>

#include <cstdlib>

ManageHeaderBar::ManageHeaderBar(QObject *parent)
    : QObject(parent)
{
}

QUrl ManageHeaderBar::playerSource() const
{
    if (!mCurrentTrack.isValid()) {
        return QUrl();
    }

    return mCurrentTrack.data(mUrlRole).toUrl();
}

void ManageHeaderBar::setUrlRole(int value)
{
    mUrlRole = value;
    Q_EMIT urlRoleChanged();
}

int ManageHeaderBar::urlRole() const
{
    return mUrlRole;
}

void ManageHeaderBar::setArtistRole(int value)
{
    mArtistRole = value;
    Q_EMIT artistRoleChanged();
}

int ManageHeaderBar::artistRole() const
{
    return mArtistRole;
}

void ManageHeaderBar::setTitleRole(int value)
{
    mTitleRole = value;
    Q_EMIT titleRoleChanged();
}

int ManageHeaderBar::titleRole() const
{
    return mTitleRole;
}

void ManageHeaderBar::setAlbumRole(int value)
{
    mAlbumRole = value;
    Q_EMIT albumRoleChanged();
}

int ManageHeaderBar::albumRole() const
{
    return mAlbumRole;
}

void ManageHeaderBar::setImageRole(int value)
{
    mImageRole = value;
    Q_EMIT imageRoleChanged();
}

int ManageHeaderBar::imageRole() const
{
    return mImageRole;
}

QVariant ManageHeaderBar::album() const
{
    if (!mCurrentTrack.isValid()) {
        return QStringLiteral("");
    }

    return mCurrentTrack.data(mAlbumRole);
}

QVariant ManageHeaderBar::title() const
{
    if (!mCurrentTrack.isValid()) {
        return QStringLiteral("");
    }

    return mCurrentTrack.data(mTitleRole);
}

QVariant ManageHeaderBar::artist() const
{
    if (!mCurrentTrack.isValid()) {
        return QStringLiteral("");
    }

    return mCurrentTrack.data(mArtistRole);
}

QUrl ManageHeaderBar::image() const
{
    if (!mCurrentTrack.isValid()) {
        return {};
    }

    return mCurrentTrack.data(mImageRole).toUrl();
}

bool ManageHeaderBar::isValid() const
{
    if (!mCurrentTrack.isValid()) {
        return false;
    }

    return mCurrentTrack.data(mIsValidRole).toBool();
}

int ManageHeaderBar::remainingTracks() const
{
    if (!mCurrentTrack.isValid()) {
        return 0;
    }

    return mPlayListModel->rowCount(mCurrentTrack.parent()) - mCurrentTrack.row() - 1;
}

int ManageHeaderBar::isValidRole() const
{
    return mIsValidRole;
}

QPersistentModelIndex ManageHeaderBar::currentTrack() const
{
    return mCurrentTrack;
}

void ManageHeaderBar::playListLayoutChanged(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint)
{
    Q_UNUSED(parents);
    Q_UNUSED(hint);

    qDebug() << "ManageHeaderBar::playListLayoutChanged" << "not implemented";
}

void ManageHeaderBar::tracksInserted(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);
    Q_UNUSED(first);
    Q_UNUSED(last);

    if (!mCurrentTrack.isValid()) {
        return;
    }

    if (mCurrentTrack.row() >= first) {
        return;
    }

    Q_EMIT remainingTracksChanged();
}

void ManageHeaderBar::tracksDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    if (!mCurrentTrack.isValid()) {
        return;
    }

    if (mCurrentTrack.row() > bottomRight.row() || mCurrentTrack.row() < topLeft.row()) {
        return;
    }

    if (mCurrentTrack.column() > bottomRight.column() || mCurrentTrack.column() < topLeft.column()) {
        return;
    }

    if (roles.isEmpty()) {
        notifyPlayerSourceProperty();
        notifyArtistProperty();
        notifyTitleProperty();
        notifyAlbumProperty();
        notifyImageProperty();
        notifyIsValidProperty();
    } else {
        for(auto oneRole : roles) {
            if (oneRole == mUrlRole) {
                notifyPlayerSourceProperty();
            }
            if (oneRole == mArtistRole) {
                notifyArtistProperty();
            }
            if (oneRole == mTitleRole) {
                notifyTitleProperty();
            }
            if (oneRole == mAlbumRole) {
                notifyAlbumProperty();
            }
            if (oneRole == mImageRole) {
                notifyImageProperty();
            }
            if (oneRole == mIsValidRole) {
                notifyIsValidProperty();
            }
        }
    }
}

void ManageHeaderBar::tracksMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    Q_UNUSED(destination);
    Q_UNUSED(row);

    qDebug() << "ManageHeaderBar::tracksMoved" << "not implemented";
}

void ManageHeaderBar::tracksRemoved(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);
    Q_UNUSED(first);
    Q_UNUSED(last);

    if (!mCurrentTrack.isValid()) {
        notifyPlayerSourceProperty();
        notifyArtistProperty();
        notifyTitleProperty();
        notifyAlbumProperty();
        notifyImageProperty();
        notifyIsValidProperty();

        if (mOldIsValid) {
            if (mCurrentTrack.row() < last - first + mPlayListModel->rowCount()) {
                Q_EMIT remainingTracks();
            }
        } else {
            return;
        }
    }

    if (mCurrentTrack.row() < first) {
        Q_EMIT remainingTracksChanged();
        return;
    }

    if (mCurrentTrack.row() == first) {
        Q_EMIT remainingTracksChanged();
        return;
    }

    if (mCurrentTrack.row() <= last) {
        Q_EMIT remainingTracksChanged();
        return;
    }
}

void ManageHeaderBar::notifyPlayerSourceProperty()
{
    auto newUrlValue = mCurrentTrack.data(mUrlRole);
    if (mOldPlayerSource != newUrlValue) {
        Q_EMIT playerSourceChanged();

        mOldPlayerSource = newUrlValue;
    }
}

void ManageHeaderBar::notifyArtistProperty()
{
    auto newArtistValue = mCurrentTrack.data(mArtistRole);
    if (mOldArtist != newArtistValue) {
        Q_EMIT artistChanged();

        mOldArtist = newArtistValue;
    }
}

void ManageHeaderBar::notifyTitleProperty()
{
    auto newTitleValue = mCurrentTrack.data(mTitleRole);
    if (mOldTitle != newTitleValue) {
        Q_EMIT titleChanged();

        mOldTitle = newTitleValue;
    }
}

void ManageHeaderBar::notifyAlbumProperty()
{
    auto newAlbumValue = mCurrentTrack.data(mAlbumRole);
    if (mOldAlbum != newAlbumValue) {
        Q_EMIT albumChanged();

        mOldAlbum = newAlbumValue;
    }
}

void ManageHeaderBar::notifyImageProperty()
{
    auto newImageValue = mCurrentTrack.data(mImageRole);
    if (mOldImage != newImageValue) {
        Q_EMIT imageChanged();

        mOldImage = newImageValue;
    }
}

void ManageHeaderBar::notifyIsValidProperty()
{
    auto newIsValidValue = mCurrentTrack.data(mIsValidRole).toBool();
    if (mOldIsValid != newIsValidValue) {
        Q_EMIT isValidChanged();

        mOldIsValid = newIsValidValue;
    }
}

void ManageHeaderBar::setIsValidRole(int isValidRole)
{
    mIsValidRole = isValidRole;
    emit isValidRoleChanged();
}

void ManageHeaderBar::setCurrentTrack(QPersistentModelIndex currentTrack)
{
    if (mCurrentTrack == currentTrack) {
        return;
    }

    auto oldRemainingTracksCount = remainingTracks();

    mCurrentTrack = currentTrack;
    Q_EMIT currentTrackChanged();

    if (mCurrentTrack.isValid() && oldRemainingTracksCount != remainingTracks()) {
        Q_EMIT remainingTracksChanged();
    }

    notifyPlayerSourceProperty();
    notifyArtistProperty();
    notifyTitleProperty();
    notifyAlbumProperty();
    notifyImageProperty();
    notifyIsValidProperty();
}

void ManageHeaderBar::setPlayListModel(QAbstractItemModel *aPlayListModel)
{
    if (mPlayListModel) {
        disconnect(mPlayListModel, &QAbstractItemModel::rowsInserted, this, &ManageHeaderBar::tracksInserted);
        disconnect(mPlayListModel, &QAbstractItemModel::rowsMoved, this, &ManageHeaderBar::tracksMoved);
        disconnect(mPlayListModel, &QAbstractItemModel::rowsRemoved, this, &ManageHeaderBar::tracksRemoved);
        disconnect(mPlayListModel, &QAbstractItemModel::dataChanged, this, &ManageHeaderBar::tracksDataChanged);
        disconnect(mPlayListModel, &QAbstractItemModel::layoutChanged, this, &ManageHeaderBar::playListLayoutChanged);
    }

    mPlayListModel = aPlayListModel;

    connect(mPlayListModel, &QAbstractItemModel::rowsInserted, this, &ManageHeaderBar::tracksInserted);
    connect(mPlayListModel, &QAbstractItemModel::rowsMoved, this, &ManageHeaderBar::tracksMoved);
    connect(mPlayListModel, &QAbstractItemModel::rowsRemoved, this, &ManageHeaderBar::tracksRemoved);
    connect(mPlayListModel, &QAbstractItemModel::dataChanged, this, &ManageHeaderBar::tracksDataChanged);
    connect(mPlayListModel, &QAbstractItemModel::layoutChanged, this, &ManageHeaderBar::playListLayoutChanged);

    Q_EMIT playListModelChanged();
}

QAbstractItemModel *ManageHeaderBar::playListModel() const
{
    return mPlayListModel;
}


#include "moc_manageheaderbar.cpp"
