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

#include "playlistcontroler.h"

#include <QTime>
#include <QTimer>
#include <QDebug>

#include <cstdlib>

PlayListControler::PlayListControler(QObject *parent)
    : QObject(parent)
{
}

void PlayListControler::setPlayListModel(QAbstractItemModel *aPlayListModel)
{
    if (mPlayListModel) {
        disconnect(mPlayListModel, &QAbstractItemModel::rowsInserted, this, &PlayListControler::tracksInserted);
        disconnect(mPlayListModel, &QAbstractItemModel::rowsRemoved, this, &PlayListControler::tracksRemoved);
        disconnect(mPlayListModel, &QAbstractItemModel::dataChanged, this, &PlayListControler::tracksDataChanged);
        disconnect(mPlayListModel, &QAbstractItemModel::modelReset, this, &PlayListControler::playListReset);
        disconnect(mPlayListModel, &QAbstractItemModel::layoutChanged, this, &PlayListControler::playListLayoutChanged);
    }

    mPlayListModel = aPlayListModel;

    connect(mPlayListModel, &QAbstractItemModel::rowsInserted, this, &PlayListControler::tracksInserted);
    connect(mPlayListModel, &QAbstractItemModel::rowsRemoved, this, &PlayListControler::tracksRemoved);
    connect(mPlayListModel, &QAbstractItemModel::dataChanged, this, &PlayListControler::tracksDataChanged);
    connect(mPlayListModel, &QAbstractItemModel::modelReset, this, &PlayListControler::playListReset);
    connect(mPlayListModel, &QAbstractItemModel::layoutChanged, this, &PlayListControler::playListLayoutChanged);

    Q_EMIT playListModelChanged();
    playListReset();
}

QAbstractItemModel *PlayListControler::playListModel() const
{
    return mPlayListModel;
}

int PlayListControler::remainingTracks() const
{
    if (!mCurrentTrack.isValid()) {
        return -1;
    }

    return mPlayListModel->rowCount(mCurrentTrack.parent()) - mCurrentTrack.row() - 1;
}

void PlayListControler::setRandomPlay(bool value)
{
    mRandomPlay = value;
    Q_EMIT randomPlayChanged();
    setRandomPlayControl(mRandomPlay);
}

bool PlayListControler::randomPlay() const
{
    return mRandomPlay;
}

void PlayListControler::setRepeatPlay(bool value)
{
    mRepeatPlay = value;
    Q_EMIT repeatPlayChanged();
    setRepeatPlayControl(mRepeatPlay);
}

bool PlayListControler::repeatPlay() const
{
    return mRepeatPlay;
}

int PlayListControler::isValidRole() const
{
    return mIsValidRole;
}

QVariantMap PlayListControler::persistentState() const
{
    auto persistentStateValue = QVariantMap();

    persistentStateValue[QStringLiteral("currentTrack")] = mCurrentTrack.row();
    persistentStateValue[QStringLiteral("randomPlay")] = mRandomPlay;
    persistentStateValue[QStringLiteral("repeatPlay")] = mRepeatPlay;

    return persistentStateValue;
}

bool PlayListControler::randomPlayControl() const
{
    return mRandomPlayControl;
}

bool PlayListControler::repeatPlayControl() const
{
    return mRepeatPlayControl;
}

QPersistentModelIndex PlayListControler::currentTrack() const
{
    return mCurrentTrack;
}

int PlayListControler::currentTrackRow() const
{
    return mCurrentTrack.row();
}

void PlayListControler::playListReset()
{
    if (!mCurrentTrack.isValid()) {
        resetCurrentTrack();
        return;
    }
}

void PlayListControler::playListLayoutChanged(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint)
{
    Q_UNUSED(parents);
    Q_UNUSED(hint);

    qDebug() << "PlayListControler::playListLayoutChanged" << "not implemented";
}

void PlayListControler::tracksInserted(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent);
    Q_UNUSED(first);
    Q_UNUSED(last);

    restorePlayListPosition();
    if (!mCurrentTrack.isValid()) {
        resetCurrentTrack();
    }
}

void PlayListControler::tracksDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    Q_UNUSED(topLeft);
    Q_UNUSED(bottomRight);
    Q_UNUSED(roles);

    if (!mCurrentTrack.isValid()) {
        resetCurrentTrack();
    }
}

void PlayListControler::tracksRemoved(const QModelIndex &parent, int first, int last)
{
    if (mCurrentTrack.parent() != parent) {
        return;
    }

    if (!mCurrentTrack.isValid()) {
        if (mCurrentTrackIsValid) {
            mCurrentTrack = mPlayListModel->index(mCurrentPlayListPosition, 0);
            notifyCurrentTrackChanged();
        }
        if (!mCurrentTrack.isValid()) {
            Q_EMIT playListFinished();
        }

        return;
    }

    if (mCurrentTrack.row() < first || mCurrentTrack.row() > last) {
        return;
    }

    if (mPlayListModel->rowCount(parent) <= first) {
        resetCurrentTrack();
        return;
    }

    mCurrentTrack = mPlayListModel->index(first, 0);
    notifyCurrentTrackChanged();
}

void PlayListControler::skipNextTrack()
{
    if (!mPlayListModel) {
        return;
    }

    if (!mCurrentTrack.isValid()) {
        return;
    }

    if (!mRandomPlay && (mCurrentTrack.row() >= (mPlayListModel->rowCount() - 1))) {
        if (!mRepeatPlay) {
            Q_EMIT playListFinished();
        }

        if (mPlayListModel->rowCount() == 1) {
            mCurrentTrack = QPersistentModelIndex{};
            notifyCurrentTrackChanged();
        }

        resetCurrentTrack();

        return;
    }

    if (mRandomPlay) {
        int randomValue = qrand();
        randomValue = randomValue % (mPlayListModel->rowCount());
        mCurrentTrack = mPlayListModel->index(randomValue, 0);
    } else {
        mCurrentTrack = mPlayListModel->index(mCurrentTrack.row() + 1, 0);
    }

    notifyCurrentTrackChanged();
}

void PlayListControler::skipPreviousTrack()
{
    if (!mPlayListModel) {
        return;
    }

    if (!mCurrentTrack.isValid()) {
        return;
    }

    if (!mRandomPlay && !mRepeatPlay && mCurrentTrack.row() <= 0) {
        return;
    }

    if (mRandomPlay) {
        int randomValue = qrand();
        randomValue = randomValue % (mPlayListModel->rowCount());
        mCurrentTrack = mPlayListModel->index(randomValue, 0);
    } else {
        if (mRepeatPlay) {
            mCurrentTrack = mPlayListModel->index(mPlayListModel->rowCount() - 1, 0);
        } else {
            mCurrentTrack = mPlayListModel->index(mCurrentTrack.row() - 1, mCurrentTrack.column(), mCurrentTrack.parent());
        }
    }
    notifyCurrentTrackChanged();
}

void PlayListControler::seedRandomGenerator(uint seed)
{
    qsrand(seed);
}

void PlayListControler::switchTo(int row)
{
    if (!mPlayListModel) {
        return;
    }

    if (!mCurrentTrack.isValid()) {
        return;
    }

    mCurrentTrack = mPlayListModel->index(row, 0);

    notifyCurrentTrackChanged();
}

void PlayListControler::setIsValidRole(int isValidRole)
{
    mIsValidRole = isValidRole;
    emit isValidRoleChanged();
}

void PlayListControler::restorePlayListPosition()
{
    auto playerCurrentTrack = mPersistentState.find(QStringLiteral("currentTrack"));
    if (playerCurrentTrack != mPersistentState.end()) {
        if (mPlayListModel) {
            auto newIndex = mPlayListModel->index(playerCurrentTrack->toInt(), 0);
            if (newIndex.isValid() && (newIndex != mCurrentTrack)) {
                mCurrentTrack = newIndex;
                notifyCurrentTrackChanged();

                if (mCurrentTrack.isValid()) {
                    mPersistentState.erase(playerCurrentTrack);
                }
            }
        }
    }
}

void PlayListControler::restoreRandomPlay()
{
    auto randomPlayStoredValue = mPersistentState.find(QStringLiteral("randomPlay"));
    if (randomPlayStoredValue != mPersistentState.end()) {
        setRandomPlayControl(randomPlayStoredValue->toBool());
        mPersistentState.erase(randomPlayStoredValue);
    }
}

void PlayListControler::restoreRepeatPlay()
{
    auto repeatPlayStoredValue = mPersistentState.find(QStringLiteral("repeatPlay"));
    if (repeatPlayStoredValue != mPersistentState.end()) {
        setRepeatPlayControl(repeatPlayStoredValue->toBool());
        mPersistentState.erase(repeatPlayStoredValue);
    }
}

void PlayListControler::notifyCurrentTrackChanged()
{
    Q_EMIT currentTrackChanged();
    Q_EMIT currentTrackRowChanged();
    mCurrentTrackIsValid = mCurrentTrack.isValid();
    if (mCurrentTrackIsValid) {
        mCurrentPlayListPosition = mCurrentTrack.row();
    }
}

void PlayListControler::setPersistentState(const QVariantMap &persistentStateValue)
{
    if (mPersistentState == persistentStateValue) {
        return;
    }

    qDebug() << "PlayListControler::setPersistentState" << persistentStateValue;

    mPersistentState = persistentStateValue;

    restorePlayListPosition();
    restoreRandomPlay();
    restoreRepeatPlay();

    Q_EMIT persistentStateChanged();
}

void PlayListControler::setRandomPlayControl(bool randomPlayControl)
{
    if (mRandomPlayControl == randomPlayControl) {
        return;
    }

    mRandomPlayControl = randomPlayControl;
    Q_EMIT randomPlayControlChanged();
}

void PlayListControler::setRepeatPlayControl(bool repeatPlayControl)
{
    if (mRepeatPlayControl == repeatPlayControl) {
        return;
    }

    mRepeatPlayControl = repeatPlayControl;
    Q_EMIT repeatPlayControlChanged();
}

void PlayListControler::resetCurrentTrack()
{
    for(int row = 0; row < mPlayListModel->rowCount(); ++row) {
        auto candidateTrack = mPlayListModel->index(row, 0);

        if (candidateTrack.isValid() && candidateTrack.data(mIsValidRole).toBool()) {
            mCurrentTrack = candidateTrack;
            notifyCurrentTrackChanged();
            break;
        }
    }
}


#include "moc_playlistcontroler.cpp"
