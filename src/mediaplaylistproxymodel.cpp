/*
 * Copyright 2015-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * Copyright 2019-2020 Alexander Stippich <a.stippich@gmx.net>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "mediaplaylistproxymodel.h"
#include "mediaplaylist.h"
#include "playListLogging.h"
#include <QItemSelection>
#include <QList>
#include <QMediaPlaylist>
#include <QRandomGenerator>
#include <QFile>
#include <QFileInfo>

#include <algorithm>

class MediaPlayListProxyModelPrivate
{
public:

    MediaPlayList* mPlayListModel;

    QPersistentModelIndex mPreviousTrack;

    QPersistentModelIndex mCurrentTrack;

    QPersistentModelIndex mNextTrack;

    QMediaPlaylist mLoadPlaylist;

    QList<int> mRandomMapping;

    QVariantMap mPersistentSettingsForUndo;

    QRandomGenerator mRandomGenerator;

    QMimeDatabase mMimeDb;

    ElisaUtils::PlayListEnqueueTriggerPlay mTriggerPlay = ElisaUtils::DoNotTriggerPlay;

    int mCurrentPlayListPosition = -1;

    bool mRepeatPlay = false;

    bool mShufflePlayList = false;

};

MediaPlayListProxyModel::MediaPlayListProxyModel(QObject *parent) : QAbstractProxyModel (parent),
    d(std::make_unique<MediaPlayListProxyModelPrivate>())
{
    connect(&d->mLoadPlaylist, &QMediaPlaylist::loaded, this, &MediaPlayListProxyModel::loadPlayListLoaded);
    connect(&d->mLoadPlaylist, &QMediaPlaylist::loadFailed, this, &MediaPlayListProxyModel::loadPlayListLoadFailed);
    d->mRandomGenerator.seed(static_cast<unsigned int>(QTime::currentTime().msec()));
}

MediaPlayListProxyModel::~MediaPlayListProxyModel()
=default;

QModelIndex MediaPlayListProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || column < 0 || row > rowCount() - 1) {
        return QModelIndex();
    }
    return createIndex(row, column);
    Q_UNUSED(parent);
}

QModelIndex MediaPlayListProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (!sourceIndex.isValid()) {
        return QModelIndex();
    }
    return d->mPlayListModel->index(mapRowFromSource(sourceIndex.row()), sourceIndex.column());
}

QItemSelection MediaPlayListProxyModel::mapSelectionFromSource(const QItemSelection &sourceSelection) const
{
    QItemSelection proxySelection;
    for (const QItemSelectionRange &range : sourceSelection) {
        QModelIndex proxyTopLeft = mapFromSource(range.topLeft());
        QModelIndex proxyBottomRight = mapFromSource(range.bottomRight());
        proxySelection.append(QItemSelectionRange(proxyTopLeft, proxyBottomRight));
    }
    return proxySelection;
}

QItemSelection MediaPlayListProxyModel::mapSelectionToSource(const QItemSelection &proxySelection) const
{
    QItemSelection sourceSelection;
    for (const QItemSelectionRange &range : proxySelection) {
        QModelIndex sourceTopLeft = mapToSource(range.topLeft());
        QModelIndex sourceBottomRight = mapToSource(range.bottomRight());
        sourceSelection.append(QItemSelectionRange(sourceTopLeft, sourceBottomRight));
    }
    return sourceSelection;
}

QModelIndex MediaPlayListProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!proxyIndex.isValid()) {
        return QModelIndex();
    }
    return d->mPlayListModel->index(mapRowToSource(proxyIndex.row()), proxyIndex.column());
}

int MediaPlayListProxyModel::mapRowToSource(const int proxyRow) const
{
    if (d->mShufflePlayList) {
        return d->mRandomMapping.at(proxyRow);
    } else {
        return proxyRow;
    }
}

int MediaPlayListProxyModel::mapRowFromSource(const int sourceRow) const
{
    if (d->mShufflePlayList) {
        return d->mRandomMapping.indexOf(sourceRow);
    } else {
        return sourceRow;
    }
}

int MediaPlayListProxyModel::rowCount(const QModelIndex &parent) const
{
    if (d->mShufflePlayList) {
        if (parent.isValid()) {
            return 0;
        }
        return d->mRandomMapping.count();
    } else {
        return d->mPlayListModel->rowCount(parent);
    }
}

int MediaPlayListProxyModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QModelIndex MediaPlayListProxyModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

bool MediaPlayListProxyModel::hasChildren(const QModelIndex &parent) const
{
    return (!parent.isValid()) ? false : (rowCount() > 0);
}

void MediaPlayListProxyModel::setPlayListModel(MediaPlayList *playListModel)
{
    if (d->mPlayListModel)
    {
        disconnect(playListModel, &QAbstractItemModel::rowsAboutToBeInserted, this, &MediaPlayListProxyModel::sourceRowsAboutToBeInserted);
        disconnect(playListModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, &MediaPlayListProxyModel::sourceRowsAboutToBeRemoved);
        disconnect(playListModel, &QAbstractItemModel::rowsAboutToBeMoved, this, &MediaPlayListProxyModel::sourceRowsAboutToBeMoved);
        disconnect(playListModel, &QAbstractItemModel::rowsInserted, this, &MediaPlayListProxyModel::sourceRowsInserted);
        disconnect(playListModel, &QAbstractItemModel::rowsRemoved, this, &MediaPlayListProxyModel::sourceRowsRemoved);
        disconnect(playListModel, &QAbstractItemModel::rowsMoved, this, &MediaPlayListProxyModel::sourceRowsMoved);
        disconnect(playListModel, &QAbstractItemModel::dataChanged, this, &MediaPlayListProxyModel::sourceDataChanged);
        disconnect(playListModel, &QAbstractItemModel::headerDataChanged, this, &MediaPlayListProxyModel::sourceHeaderDataChanged);
        disconnect(playListModel, &QAbstractItemModel::layoutAboutToBeChanged, this, &MediaPlayListProxyModel::sourceLayoutAboutToBeChanged);
        disconnect(playListModel, &QAbstractItemModel::layoutChanged, this, &MediaPlayListProxyModel::sourceLayoutChanged);
        disconnect(playListModel, &QAbstractItemModel::modelAboutToBeReset, this, &MediaPlayListProxyModel::sourceModelAboutToBeReset);
        disconnect(playListModel, &QAbstractItemModel::modelReset, this, &MediaPlayListProxyModel::sourceModelReset);
    }

    d->mPlayListModel = playListModel;
    setSourceModel(playListModel);

    if (d->mPlayListModel) {
        connect(playListModel, &QAbstractItemModel::rowsAboutToBeInserted, this, &MediaPlayListProxyModel::sourceRowsAboutToBeInserted);
        connect(playListModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, &MediaPlayListProxyModel::sourceRowsAboutToBeRemoved);
        connect(playListModel, &QAbstractItemModel::rowsAboutToBeMoved, this, &MediaPlayListProxyModel::sourceRowsAboutToBeMoved);
        connect(playListModel, &QAbstractItemModel::rowsInserted, this, &MediaPlayListProxyModel::sourceRowsInserted);
        connect(playListModel, &QAbstractItemModel::rowsRemoved, this, &MediaPlayListProxyModel::sourceRowsRemoved);
        connect(playListModel, &QAbstractItemModel::rowsMoved, this, &MediaPlayListProxyModel::sourceRowsMoved);
        connect(playListModel, &QAbstractItemModel::dataChanged, this, &MediaPlayListProxyModel::sourceDataChanged);
        connect(playListModel, &QAbstractItemModel::headerDataChanged, this, &MediaPlayListProxyModel::sourceHeaderDataChanged);
        connect(playListModel, &QAbstractItemModel::layoutAboutToBeChanged, this, &MediaPlayListProxyModel::sourceLayoutAboutToBeChanged);
        connect(playListModel, &QAbstractItemModel::layoutChanged, this, &MediaPlayListProxyModel::sourceLayoutChanged);
        connect(playListModel, &QAbstractItemModel::modelAboutToBeReset, this, &MediaPlayListProxyModel::sourceModelAboutToBeReset);
        connect(playListModel, &QAbstractItemModel::modelReset, this, &MediaPlayListProxyModel::sourceModelReset);
    }
}

void MediaPlayListProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    QAbstractProxyModel::setSourceModel(sourceModel);
}

QPersistentModelIndex MediaPlayListProxyModel::previousTrack() const
{
    return d->mPreviousTrack;
}

QPersistentModelIndex MediaPlayListProxyModel::currentTrack() const
{
    return d->mCurrentTrack;
}

QPersistentModelIndex MediaPlayListProxyModel::nextTrack() const
{
    return d->mNextTrack;
}

void MediaPlayListProxyModel::setRepeatPlay(const bool value)
{
    if (d->mRepeatPlay != value) {
        d->mRepeatPlay = value;
        Q_EMIT repeatPlayChanged();
        Q_EMIT remainingTracksChanged();
        Q_EMIT persistentStateChanged();
        determineAndNotifyPreviousAndNextTracks();
    }
}

bool MediaPlayListProxyModel::repeatPlay() const
{
    return d->mRepeatPlay;
}

void MediaPlayListProxyModel::setShufflePlayList(const bool value)
{
    if (d->mShufflePlayList != value) {
        Q_EMIT layoutAboutToBeChanged(QList<QPersistentModelIndex>(), QAbstractItemModel::VerticalSortHint);
        auto playListSize = d->mPlayListModel->rowCount();

        if (playListSize != 0) {
            if (value) {
                d->mRandomMapping.clear();
                d->mRandomMapping.reserve(playListSize);

                QModelIndexList to;
                to.reserve(playListSize);
                for (int i = 0; i < playListSize; ++i) {
                    to.append(index(i,0));
                    d->mRandomMapping.append(i);
                }

                QModelIndexList from;
                from.reserve(playListSize);
                // Fisher-Yates algorithm
                for (int i = 0;  i < playListSize - 1; ++i) {
                    const int swapIndex = d->mRandomGenerator.bounded(i, playListSize);
                    std::swap(d->mRandomMapping[i], d->mRandomMapping[swapIndex]);
                    from.append(index(d->mRandomMapping.at(i), 0));
                }
                from.append(index(d->mRandomMapping.at(playListSize - 1), 0));
                changePersistentIndexList(from, to);
            } else {
                QModelIndexList from;
                from.reserve(playListSize);
                QModelIndexList to;
                to.reserve(playListSize);
                for (int i = 0; i < playListSize; ++i) {
                    to.append(index(d->mRandomMapping.at(i), 0));
                    from.append(index(i, 0));
                }
                changePersistentIndexList(from, to);
                d->mRandomMapping.clear();
            }

            d->mCurrentPlayListPosition = d->mCurrentTrack.row();
            d->mShufflePlayList = value;
            Q_EMIT layoutChanged(QList<QPersistentModelIndex>(), QAbstractItemModel::VerticalSortHint);
            determineAndNotifyPreviousAndNextTracks();
        } else {
            d->mShufflePlayList = value;
        }
        Q_EMIT shufflePlayListChanged();
        Q_EMIT remainingTracksChanged();
        Q_EMIT persistentStateChanged();
    }
}

bool MediaPlayListProxyModel::shufflePlayList() const
{
    return d->mShufflePlayList;
}

void MediaPlayListProxyModel::sourceRowsAboutToBeInserted(const QModelIndex &parent, int start, int end)
{
    /*
     * When in random mode, rows are only inserted after
     * the source model is done inserting new items since
     * new items can be added at arbitrarily positions,
     * which requires a split of beginInsertRows
     */
    if (!d->mShufflePlayList) {
        beginInsertRows(parent, start, end);
    }
}

void MediaPlayListProxyModel::sourceRowsInserted(const QModelIndex &parent, int start, int end)
{
    if (d->mShufflePlayList) {
        const auto newItemsCount = end - start + 1;
        d->mRandomMapping.reserve(rowCount() + newItemsCount);
        if (rowCount() == 0 || newItemsCount == 1) {
            beginInsertRows(parent, start, end);
            for (int i = 0; i < newItemsCount; ++i) {
                //QRandomGenerator.bounded(int) is exclusive, thus + 1
                const auto random = d->mRandomGenerator.bounded(d->mRandomMapping.count()+1);
                d->mRandomMapping.insert(random, start + i);
            }
            endInsertRows();
        } else {
            for (int i = 0; i < newItemsCount; ++i) {
                //QRandomGenerator.bounded(int) is exclusive, thus + 1
                const auto random = d->mRandomGenerator.bounded(d->mRandomMapping.count()+1);
                beginInsertRows(parent, random, random);
                d->mRandomMapping.insert(random, start + i);
                endInsertRows();
            }
        }
    } else {
        endInsertRows();
    }
    determineTracks();
    Q_EMIT tracksCountChanged();
    Q_EMIT remainingTracksChanged();
    Q_EMIT persistentStateChanged();
}

void MediaPlayListProxyModel::sourceRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    if (d->mShufflePlayList) {
        if (end - start + 1 == rowCount()) {
            beginRemoveRows(parent, start, end);
            d->mRandomMapping.clear();
            endRemoveRows();
        }
        int row = 0;
        auto it = d->mRandomMapping.begin();
        while (it != d->mRandomMapping.end()) {
            if (*it >= start && *it <= end){
                beginRemoveRows(parent, row, row);
                it = d->mRandomMapping.erase(it);
                endRemoveRows();
            } else {
                if (*it > end) {
                    *it = *it - end + start - 1;
                }
                it++;
                row++;
            }
        }
    } else {
        beginRemoveRows(parent, start, end);
    }
}

void MediaPlayListProxyModel::sourceRowsRemoved(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    if (!d->mShufflePlayList) {
        endRemoveRows();
    }
    if (!d->mCurrentTrack.isValid()) {
        d->mCurrentTrack = index(d->mCurrentPlayListPosition, 0);

        if (d->mCurrentTrack.isValid()) {
            notifyCurrentTrackChanged();
        }

        if (!d->mCurrentTrack.isValid()) {
            Q_EMIT playListFinished();
            determineTracks();
            if (!d->mCurrentTrack.isValid()) {
                notifyCurrentTrackChanged();
            }
        }
    }
    if (!d->mNextTrack.isValid() || !d->mPreviousTrack.isValid()) {
        determineAndNotifyPreviousAndNextTracks();
    }
    Q_EMIT tracksCountChanged();
    Q_EMIT remainingTracksChanged();
    Q_EMIT persistentStateChanged();
}

void MediaPlayListProxyModel::sourceRowsAboutToBeMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destParent, int destRow)
{
    Q_ASSERT(!d->mShufflePlayList);
    beginMoveRows(parent, start, end, destParent, destRow);
}

void MediaPlayListProxyModel::sourceRowsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destParent, int destRow)
{
    Q_ASSERT(!d->mShufflePlayList);
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    Q_UNUSED(destParent);
    Q_UNUSED(destRow);
    endMoveRows();
    Q_EMIT remainingTracksChanged();
    Q_EMIT persistentStateChanged();
}

void MediaPlayListProxyModel::sourceModelAboutToBeReset()
{
    beginResetModel();
}
void MediaPlayListProxyModel::sourceModelReset()
{
    endResetModel();
}

void MediaPlayListProxyModel::sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    auto startSourceRow = topLeft.row();
    auto endSourceRow = bottomRight.row();
    for (int i = startSourceRow; i <= endSourceRow; i++) {
        Q_EMIT dataChanged(index(mapRowFromSource(i), 0), index(mapRowFromSource(i), 0), roles);       
        if (i == d->mCurrentTrack.row()) {
            Q_EMIT currentTrackDataChanged();
        } else if (i == d->mNextTrack.row()) {
            Q_EMIT nextTrackDataChanged();
        } else if (i == d->mPreviousTrack.row()) {
            Q_EMIT previousTrackDataChanged();
        }
        determineTracks();
    }
}

void MediaPlayListProxyModel::sourceLayoutAboutToBeChanged()
{
    Q_EMIT layoutAboutToBeChanged();
}

void MediaPlayListProxyModel::sourceLayoutChanged()
{
    Q_EMIT layoutChanged();
}

void MediaPlayListProxyModel::sourceHeaderDataChanged(Qt::Orientation orientation, int first, int last)
{
    Q_EMIT headerDataChanged(orientation, first, last);
}

int MediaPlayListProxyModel::remainingTracks() const
{
    if (!d->mCurrentTrack.isValid() || d->mRepeatPlay) {
        return -1;
    } else {
        return rowCount() - d->mCurrentTrack.row() - 1;
    }
}

int MediaPlayListProxyModel::tracksCount() const
{
    return rowCount();
}

int MediaPlayListProxyModel::currentTrackRow() const
{
    return d->mCurrentTrack.row();
}

void MediaPlayListProxyModel::enqueue(const ElisaUtils::EntryData &newEntry, ElisaUtils::PlayListEntryType databaseIdType)
{
    enqueue(newEntry, databaseIdType, ElisaUtils::PlayListEnqueueMode::AppendPlayList, ElisaUtils::PlayListEnqueueTriggerPlay::DoNotTriggerPlay);
}

void MediaPlayListProxyModel::enqueue(const ElisaUtils::EntryDataList &newEntries, ElisaUtils::PlayListEntryType databaseIdType)
{
    enqueue(newEntries, databaseIdType, ElisaUtils::PlayListEnqueueMode::AppendPlayList, ElisaUtils::PlayListEnqueueTriggerPlay::DoNotTriggerPlay);
}

void MediaPlayListProxyModel::enqueue(qulonglong newEntryDatabaseId,
                            const QString &newEntryTitle,
                            ElisaUtils::PlayListEntryType databaseIdType,
                            ElisaUtils::PlayListEnqueueMode enqueueMode,
                            ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay)
{
    enqueue(ElisaUtils::EntryData{{{DataTypes::DatabaseIdRole, newEntryDatabaseId}}, newEntryTitle, {}}, databaseIdType, enqueueMode, triggerPlay);
}

void MediaPlayListProxyModel::enqueue(const QUrl &entryUrl, ElisaUtils::PlayListEntryType databaseIdType,
                            ElisaUtils::PlayListEnqueueMode enqueueMode,
                            ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay)
{
    enqueue(ElisaUtils::EntryData{{}, {}, entryUrl}, databaseIdType, enqueueMode, triggerPlay);
}

void MediaPlayListProxyModel::enqueue(const ElisaUtils::EntryData &newEntry,
                            ElisaUtils::PlayListEntryType databaseIdType,
                            ElisaUtils::PlayListEnqueueMode enqueueMode,
                            ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay)
{
    d->mTriggerPlay = triggerPlay;

    if (enqueueMode == ElisaUtils::ReplacePlayList) {
        if (rowCount() == 0) {
            Q_EMIT hideUndoNotification();
        } else {
            clearPlayList();
        }
    }

    switch (databaseIdType)
    {
    case ElisaUtils::Album:
    case ElisaUtils::Artist:
    case ElisaUtils::Genre:
        d->mPlayListModel->enqueueOneEntry(newEntry, databaseIdType);
        break;
    case ElisaUtils::Track:
    case ElisaUtils::Radio:
        if (std::get<2>(newEntry).isValid()) {
            d->mPlayListModel->enqueueFilesList({newEntry}, databaseIdType);
        } else {
            d->mPlayListModel->enqueueOneEntry(newEntry, databaseIdType);
        }
        break;
    case ElisaUtils::FileName:
        d->mPlayListModel->enqueueFilesList({newEntry}, databaseIdType);
        break;
    case ElisaUtils::Lyricist:
    case ElisaUtils::Composer:
    case ElisaUtils::Unknown:
        break;
    }
}

void MediaPlayListProxyModel::enqueue(const ElisaUtils::EntryDataList &newEntries,
                            ElisaUtils::PlayListEntryType databaseIdType,
                            ElisaUtils::PlayListEnqueueMode enqueueMode,
                            ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay)
{
    if (newEntries.isEmpty()) {
        return;
    }

    d->mTriggerPlay = triggerPlay;

    if (enqueueMode == ElisaUtils::ReplacePlayList) {
        if (rowCount() == 0) {
            Q_EMIT hideUndoNotification();
        } else {
            clearPlayList();
        }
    }

    switch (databaseIdType)
    {
    case ElisaUtils::Track:
    case ElisaUtils::Radio:
        d->mPlayListModel->enqueueMultipleEntries(newEntries, databaseIdType);
        break;
    case ElisaUtils::FileName:
        d->mPlayListModel->enqueueFilesList(newEntries, databaseIdType);
        break;
    case ElisaUtils::Album:
    case ElisaUtils::Artist:
    case ElisaUtils::Genre:
        d->mPlayListModel->enqueueMultipleEntries(newEntries, databaseIdType);
        break;
    case ElisaUtils::Lyricist:
    case ElisaUtils::Composer:
    case ElisaUtils::Unknown:
        break;
    }
}

void MediaPlayListProxyModel::trackInError(const QUrl &sourceInError, QMediaPlayer::Error playerError)
{
    d->mPlayListModel->trackInError(sourceInError, playerError);
}

void MediaPlayListProxyModel::skipNextTrack()
{
    if (!d->mCurrentTrack.isValid()) {
        return;
    }

    if (d->mCurrentTrack.row() >= rowCount() - 1) {
        d->mCurrentTrack = index(0, 0);
        if (!d->mRepeatPlay) {
            Q_EMIT playListFinished();
        }
    } else {
        d->mCurrentTrack = index(d->mCurrentTrack.row() + 1, 0);
    }

    notifyCurrentTrackChanged();
}

void MediaPlayListProxyModel::skipPreviousTrack()
{
    if (!d->mCurrentTrack.isValid()) {
        return;
    }

    if (d->mCurrentTrack.row() == 0) {
        if (d->mRepeatPlay) {
            d->mCurrentTrack = index(rowCount() - 1, 0);
        } else {
            return;
        }
    } else {
        d->mCurrentTrack = index(d->mCurrentTrack.row() - 1, 0);
    }

    notifyCurrentTrackChanged();
}

void MediaPlayListProxyModel::switchTo(int row)
{
    if (!d->mCurrentTrack.isValid()) {
        return;
    }
    d->mCurrentTrack = index(row, 0);

    notifyCurrentTrackChanged();
}

void MediaPlayListProxyModel::removeSelection(QList<int> selection)
{
    std::sort(selection.begin(), selection.end());
    std::reverse(selection.begin(), selection.end());
    for (auto oneItem : selection) {
        removeRow(oneItem);
    }
}

void MediaPlayListProxyModel::removeRow(int row)
{
    d->mPlayListModel->removeRows(mapRowToSource(row), 1);
}

void MediaPlayListProxyModel::moveRow(int from, int to)
{
    if (d->mShufflePlayList) {
        beginMoveRows({}, from, from, {}, from < to ? to + 1 : to);
        d->mRandomMapping.move(from, to);
        endMoveRows();
    } else {
        d->mPlayListModel->moveRows({}, from, 1, {}, from < to ? to + 1 : to);
    }
}

void MediaPlayListProxyModel::notifyCurrentTrackChanged()
{
    if (d->mCurrentTrack.isValid()) {
        d->mCurrentPlayListPosition = d->mCurrentTrack.row();
    } else {
        d->mCurrentPlayListPosition = -1;
    }
    determineAndNotifyPreviousAndNextTracks();
    Q_EMIT currentTrackChanged(d->mCurrentTrack);
    Q_EMIT currentTrackRowChanged();
    Q_EMIT remainingTracksChanged();
}

void MediaPlayListProxyModel::determineAndNotifyPreviousAndNextTracks()
{
    if (!d->mCurrentTrack.isValid()) {
        d->mPreviousTrack = QPersistentModelIndex();
        d->mNextTrack = QPersistentModelIndex();
    }
    auto mOldPreviousTrack = d->mPreviousTrack;
    auto mOldNextTrack = d->mNextTrack;
    if (d->mRepeatPlay) {
        // forward to end or begin when repeating
        if (d->mCurrentTrack.row() == 0) {
            d->mPreviousTrack = index(rowCount() - 1, 0);
        } else {
            d->mPreviousTrack = index(d->mCurrentTrack.row() - 1, 0);
        }
        if (d->mCurrentTrack.row() == rowCount() - 1) {
            d->mNextTrack = index(0, 0);
        } else {
            d->mNextTrack = index(d->mCurrentTrack.row() + 1, 0);
        }
    } else {
        // return nothing if no tracks available
        if (d->mCurrentTrack.row() == 0) {
            d->mPreviousTrack = QPersistentModelIndex();
        } else {
            d->mPreviousTrack = index(d->mCurrentTrack.row() - 1, 0);
        }
        if (d->mCurrentTrack.row() == rowCount() - 1) {
            d->mNextTrack = QPersistentModelIndex();
        } else {
            d->mNextTrack = index(d->mCurrentTrack.row() + 1, 0);
        }
    }
    if (d->mPreviousTrack != mOldPreviousTrack) {
        Q_EMIT previousTrackChanged(d->mPreviousTrack);
    }
    if (d->mNextTrack != mOldNextTrack) {
        Q_EMIT nextTrackChanged(d->mNextTrack);
    }
}

void MediaPlayListProxyModel::clearPlayList()
{
    if (rowCount() == 0) {
        return;
    }
    d->mPersistentSettingsForUndo = persistentState();
    d->mCurrentPlayListPosition = -1;
    d->mCurrentTrack = QPersistentModelIndex{};
    d->mPlayListModel->clearPlayList();
    Q_EMIT clearPlayListPlayer();
    Q_EMIT displayUndoNotification();
}

void MediaPlayListProxyModel::undoClearPlayList()
{
    d->mPlayListModel->clearPlayList();

    setPersistentState(d->mPersistentSettingsForUndo);
    Q_EMIT hideUndoNotification();
    Q_EMIT undoClearPlayListPlayer();
}

void MediaPlayListProxyModel::determineTracks()
{
    if (!d->mCurrentTrack.isValid()) {
        for (int row = 0; row < rowCount(); ++row) {
            auto candidateTrack = index(row, 0);
            const auto type = candidateTrack.data(MediaPlayList::ElementTypeRole).value<ElisaUtils::PlayListEntryType>();
            if (candidateTrack.isValid() && candidateTrack.data(MediaPlayList::IsValidRole).toBool() &&
                    (type == ElisaUtils::Track || type == ElisaUtils::Radio || type == ElisaUtils::FileName)) {
                d->mCurrentTrack = candidateTrack;
                notifyCurrentTrackChanged();
                if (d->mTriggerPlay == ElisaUtils::TriggerPlay) {
                    d->mTriggerPlay = ElisaUtils::DoNotTriggerPlay;
                    Q_EMIT ensurePlay();
                }
                break;
            }
        }
    }
    if (!d->mNextTrack.isValid() || !d->mPreviousTrack.isValid()) {
        determineAndNotifyPreviousAndNextTracks();
    }
}

bool MediaPlayListProxyModel::savePlayList(const QUrl &fileName)
{
    QMediaPlaylist savePlaylist;

    for (int i = 0; i < rowCount(); ++i) {
        if (data(index(i,0), MediaPlayList::IsValidRole).toBool()) {
            data(index(i,0), MediaPlayList::ResourceRole).toUrl();
            savePlaylist.addMedia(data(index(i,0), MediaPlayList::ResourceRole).toUrl());
        }
    }

    return savePlaylist.save(fileName, "m3u");
}

void MediaPlayListProxyModel::loadPlayList(const QUrl &fileName)
{
    d->mLoadPlaylist.clear();
    d->mLoadPlaylist.load(fileName, "m3u");
}

void MediaPlayListProxyModel::loadPlayListLoaded()
{
    clearPlayList();
    for (int i = 0; i < d->mLoadPlaylist.mediaCount(); ++i) {
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
        enqueue(ElisaUtils::EntryData{{}, {}, d->mLoadPlaylist.media(i).canonicalUrl()}, ElisaUtils::FileName);
#else
        enqueue(ElisaUtils::EntryData{{}, {}, d->mLoadPlaylist.media(i).request().url()}, ElisaUtils::FileName);
#endif
    }

    Q_EMIT persistentStateChanged();

    d->mLoadPlaylist.clear();
    Q_EMIT playListLoaded();
}

void MediaPlayListProxyModel::loadPlayListLoadFailed()
{
    d->mLoadPlaylist.clear();
    Q_EMIT playListLoadFailed();
}

QVariantMap MediaPlayListProxyModel::persistentState() const
{
    QVariantMap currentState;

    currentState[QStringLiteral("playList")] = d->mPlayListModel->getEntriesForRestore();
    currentState[QStringLiteral("currentTrack")] = d->mCurrentPlayListPosition;
    currentState[QStringLiteral("shufflePlayList")] = d->mShufflePlayList;
    currentState[QStringLiteral("repeatPlay")] = d->mRepeatPlay;

    return currentState;
}

void MediaPlayListProxyModel::setPersistentState(const QVariantMap &persistentStateValue)
{
    qCDebug(orgKdeElisaPlayList()) << "MediaPlayListProxyModel::setPersistentState" << persistentStateValue;

    auto playListIt = persistentStateValue.find(QStringLiteral("playList"));
    if (playListIt != persistentStateValue.end()) {
        d->mPlayListModel->enqueueRestoredEntries(playListIt.value().toList());
    }

    auto playerCurrentTrack = persistentStateValue.find(QStringLiteral("currentTrack"));
    if (playerCurrentTrack != persistentStateValue.end()) {
        auto newIndex = index(playerCurrentTrack->toInt(), 0);
        if (newIndex.isValid() && (newIndex != d->mCurrentTrack)) {
            d->mCurrentTrack = newIndex;
            notifyCurrentTrackChanged();
        }
    }

    auto shufflePlayListStoredValue = persistentStateValue.find(QStringLiteral("shufflePlayList"));
    if (shufflePlayListStoredValue != persistentStateValue.end()) {
        setShufflePlayList(shufflePlayListStoredValue->toBool());
    }
    auto repeatPlayStoredValue = persistentStateValue.find(QStringLiteral("repeatPlay"));
    if (repeatPlayStoredValue != persistentStateValue.end()) {
        setRepeatPlay(repeatPlayStoredValue->toBool());
    }

    Q_EMIT persistentStateChanged();
}

void MediaPlayListProxyModel::enqueueDirectory(const QUrl &fileName, ElisaUtils::PlayListEntryType databaseIdType,
                                            ElisaUtils::PlayListEnqueueMode enqueueMode,
                                            ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay, int depth)
{
    if (!fileName.isLocalFile()) return;
    // clear playlist if required
    if (enqueueMode == ElisaUtils::ReplacePlayList) {
        if (rowCount() == 0) {
            Q_EMIT hideUndoNotification();
        } else {
            clearPlayList();
        }
    }
    // get contents of directory
    QDir dirInfo = QDir(fileName.toLocalFile());
    auto files = dirInfo.entryInfoList(QDir::NoDotAndDotDot | QDir::Readable | QDir::Files | QDir::Dirs, QDir::Name);
    auto newFiles = ElisaUtils::EntryDataList();
    for (auto file : files)
    {
        auto fileUrl = QUrl::fromLocalFile(file.filePath());
        if (file.isFile() && d->mMimeDb.mimeTypeForUrl(fileUrl).name().startsWith(QLatin1String("audio/")))
        {
            newFiles.append({ElisaUtils::EntryData{{},{},fileUrl}});
        }
        else if (file.isDir() && depth > 1)
        {
            // recurse through directory
            enqueueDirectory(fileUrl, databaseIdType, ElisaUtils::AppendPlayList, triggerPlay, depth-1);
        }
    }
    if (newFiles.size() != 0) enqueue(newFiles, databaseIdType, ElisaUtils::AppendPlayList, triggerPlay);
}

#include "moc_mediaplaylistproxymodel.cpp"
