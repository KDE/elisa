/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2019 (c) Alexander Stippich <a.stippich@gmx.net>
   SPDX-FileCopyrightText: 2020 (c) Carson Black <uhhadd@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
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
#include <QDir>
#include <QMimeDatabase>

#include <algorithm>

class MediaPlayListProxyModelPrivate
{
public:

    MediaPlayList* mPlayListModel;

    QPersistentModelIndex mPreviousTrack;

    QPersistentModelIndex mCurrentTrack;

    bool mCurrentTrackWasValid = false;

    QPersistentModelIndex mNextTrack;

    QMediaPlaylist mLoadPlaylist;

    QList<int> mRandomMapping;

    QVariantMap mPersistentSettingsForUndo;

    QRandomGenerator mRandomGenerator;

    QMimeDatabase mMimeDb;

    ElisaUtils::PlayListEnqueueTriggerPlay mTriggerPlay = ElisaUtils::DoNotTriggerPlay;

    int mCurrentPlayListPosition = -1;

    MediaPlayListProxyModel::Repeat mRepeatMode = MediaPlayListProxyModel::Repeat::None;

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

void MediaPlayListProxyModel::setRepeatMode(Repeat value)
{
    if (d->mRepeatMode != value) {
        d->mRepeatMode = value;
        Q_EMIT repeatModeChanged();
        Q_EMIT remainingTracksChanged();
        Q_EMIT persistentStateChanged();
        determineAndNotifyPreviousAndNextTracks();
    }
}

MediaPlayListProxyModel::Repeat MediaPlayListProxyModel::repeatMode() const
{
    return d->mRepeatMode;
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
        d->mCurrentTrackWasValid = d->mCurrentTrack.isValid();
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
        if (d->mCurrentTrack.isValid() && d->mCurrentTrackWasValid) {
            notifyCurrentTrackChanged();
        } else {
            if (!d->mCurrentTrack.isValid()) {
                Q_EMIT playListFinished();
                determineTracks();
                if (!d->mCurrentTrack.isValid() && d->mCurrentTrackWasValid) {
                    notifyCurrentTrackChanged();
                }
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
    if (!d->mCurrentTrack.isValid() || (d->mRepeatMode == Repeat::One) || (d->mRepeatMode == Repeat::Playlist)) {
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

void MediaPlayListProxyModel::enqueue(const DataTypes::MusicDataType &newEntry, const QString &newEntryTitle,
                                      ElisaUtils::PlayListEnqueueMode enqueueMode, ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay)
{
    enqueue({{newEntry, newEntryTitle, {}}}, enqueueMode, triggerPlay);
}

void MediaPlayListProxyModel::enqueue(const QUrl &entryUrl,
                            ElisaUtils::PlayListEnqueueMode enqueueMode,
                            ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay)
{
    enqueue({{{{DataTypes::ElementTypeRole, ElisaUtils::Track}}, {}, entryUrl}}, enqueueMode, triggerPlay);
}

void MediaPlayListProxyModel::enqueue(const DataTypes::EntryDataList &newEntries,
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

    d->mPlayListModel->enqueueMultipleEntries(newEntries);
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
        switch (d->mRepeatMode) {
        case Repeat::One:
            d->mCurrentTrack = index(d->mCurrentTrack.row(), 0);
            break;
        case Repeat::Playlist:
            d->mCurrentTrack = index(0, 0);
            break;
        case Repeat::None:
            d->mCurrentTrack = index(0, 0);
            Q_EMIT playListFinished();
            break;
        }
    } else {
        if (d->mRepeatMode == Repeat::One) {
            d->mCurrentTrack = index(d->mCurrentTrack.row(), 0);
        } else {
            d->mCurrentTrack = index(d->mCurrentTrack.row() + 1, 0);
        }
    }

    notifyCurrentTrackChanged();
}

void MediaPlayListProxyModel::skipPreviousTrack(qint64 position)
{
    if (!d->mCurrentTrack.isValid()) {
        return;
    }

    if (position > mSeekToBeginningDelay) {
        Q_EMIT seek(0);
        return;
    }

    if (d->mCurrentTrack.row() == 0) {
        if (d->mRepeatMode == Repeat::One || d->mRepeatMode == Repeat::Playlist) {
            d->mCurrentTrack = index(rowCount() - 1, 0);
        } else {
            return;
        }
    } else {
        if (d->mRepeatMode == Repeat::One) {
            d->mCurrentTrack = index(d->mCurrentTrack.row(), 0);
        } else {
            d->mCurrentTrack = index(d->mCurrentTrack.row() - 1, 0);
        }
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
    switch (d->mRepeatMode) {
    case Repeat::None:
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
        break;
    case Repeat::Playlist:
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
        break;
    case Repeat::One:
        d->mPreviousTrack = index(d->mCurrentTrack.row(), 0);
        d->mNextTrack = index(d->mCurrentTrack.row(), 0);
        break;
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
    notifyCurrentTrackChanged();
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

    return savePlaylist.save(fileName, "m3u8");
}

void MediaPlayListProxyModel::loadPlayList(const QUrl &fileName)
{
    d->mLoadPlaylist.clear();
    d->mLoadPlaylist.load(fileName, "m3u8");
}

void MediaPlayListProxyModel::loadPlayListLoaded()
{
    clearPlayList();

    auto newTracks = DataTypes::EntryDataList{};
    for (int i = 0; i < d->mLoadPlaylist.mediaCount(); ++i) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        newTracks.push_back({{{{DataTypes::ElementTypeRole, ElisaUtils::FileName},
                               {DataTypes::ResourceRole, d->mLoadPlaylist.media(i).request().url()}}}, {}, {}});
#else
        newTracks.push_back({{{{DataTypes::ElementTypeRole, ElisaUtils::FileName},
                               {DataTypes::ResourceRole, d->mLoadPlaylist.media(i).canonicalUrl()}}}, {}, {}});
#endif
    }

    enqueue(newTracks, ElisaUtils::ReplacePlayList, ElisaUtils::DoNotTriggerPlay);

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
    currentState[QStringLiteral("repeatMode")] = d->mRepeatMode;

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
    if (repeatPlayStoredValue != persistentStateValue.end() && repeatPlayStoredValue->value<bool>()) {
        setRepeatMode(Repeat::Playlist);
    }

    auto repeatModeStoredValue = persistentStateValue.find(QStringLiteral("repeatMode"));
    if (repeatModeStoredValue != persistentStateValue.end()) {
        setRepeatMode(repeatModeStoredValue->value<Repeat>());
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
    auto newFiles = DataTypes::EntryDataList();
    for (const auto &file : files)
    {
        auto fileUrl = QUrl::fromLocalFile(file.filePath());
        if (file.isFile() && d->mMimeDb.mimeTypeForUrl(fileUrl).name().startsWith(QLatin1String("audio/")))
        {
            newFiles.push_back({{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                 {DataTypes::ResourceRole, fileUrl}},{}, {}});
        }
        else if (file.isDir() && depth > 1)
        {
            // recurse through directory
            enqueueDirectory(fileUrl, databaseIdType, ElisaUtils::AppendPlayList, triggerPlay, depth-1);
        }
    }
    if (newFiles.size() != 0) enqueue(newFiles, ElisaUtils::AppendPlayList, triggerPlay);
}

#include "moc_mediaplaylistproxymodel.cpp"
