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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef MEDIAPLAYLISTPROXYMODEL_H
#define MEDIAPLAYLISTPROXYMODEL_H

#include "elisaLib_export.h"
#include "elisautils.h"

#include <QAbstractProxyModel>
#include <QMediaPlayer>

#include <memory>

class MediaPlayList;
class MediaPlayListProxyModelPrivate;

class ELISALIB_EXPORT MediaPlayListProxyModel : public QAbstractProxyModel
{

    Q_OBJECT

    Q_PROPERTY(QVariantMap persistentState
               READ persistentState
               WRITE setPersistentState
               NOTIFY persistentStateChanged)

    Q_PROPERTY(QPersistentModelIndex previousTrack
               READ previousTrack
               NOTIFY previousTrackChanged)

    Q_PROPERTY(QPersistentModelIndex currentTrack
               READ currentTrack
               NOTIFY currentTrackChanged)

    Q_PROPERTY(QPersistentModelIndex nextTrack
               READ nextTrack
               NOTIFY nextTrackChanged)

    Q_PROPERTY(bool repeatPlay
               READ repeatPlay
               WRITE setRepeatPlay
               NOTIFY repeatPlayChanged)

    Q_PROPERTY(bool shufflePlayList
               READ shufflePlayList
               WRITE setShufflePlayList
               NOTIFY shufflePlayListChanged)

    Q_PROPERTY(int remainingTracks
               READ remainingTracks
               NOTIFY remainingTracksChanged)

    Q_PROPERTY(int currentTrackRow
               READ currentTrackRow
               NOTIFY currentTrackRowChanged)

    Q_PROPERTY(int tracksCount
               READ tracksCount
               NOTIFY tracksCountChanged)

public:

    explicit MediaPlayListProxyModel(QObject *parent = nullptr);

    ~MediaPlayListProxyModel() override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;

    QItemSelection mapSelectionFromSource(const QItemSelection &sourceSelection) const override;

    QItemSelection mapSelectionToSource(const QItemSelection &proxySelection) const override;

    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;

    int mapRowFromSource(const int sourceRow) const;

    int mapRowToSource(const int proxyRow) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    int columnCount(const QModelIndex &parent) const override;

    QModelIndex parent(const QModelIndex &child) const override;

    bool hasChildren(const QModelIndex &parent) const override;

    void setPlayListModel(MediaPlayList* playListModel);

    QPersistentModelIndex previousTrack() const;

    QPersistentModelIndex currentTrack() const;

    QPersistentModelIndex nextTrack() const;

    bool repeatPlay() const;

    bool shufflePlayList() const;

    int remainingTracks() const;

    int currentTrackRow() const;

    int tracksCount() const;

    QVariantMap persistentState() const;

public Q_SLOTS:

    void enqueue(const ElisaUtils::EntryData &newEntry, ElisaUtils::PlayListEntryType databaseIdType);

    void enqueue(const ElisaUtils::EntryDataList &newEntries, ElisaUtils::PlayListEntryType databaseIdType);

    void enqueue(qulonglong newEntryDatabaseId, const QString &newEntryTitle,
                 ElisaUtils::PlayListEntryType databaseIdType,
                 ElisaUtils::PlayListEnqueueMode enqueueMode,
                 ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

    void enqueue(const QUrl &entryUrl,
                 ElisaUtils::PlayListEntryType databaseIdType,
                 ElisaUtils::PlayListEnqueueMode enqueueMode,
                 ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

    void enqueue(const ElisaUtils::EntryData &newEntry,
                 ElisaUtils::PlayListEntryType databaseIdType,
                 ElisaUtils::PlayListEnqueueMode enqueueMode,
                 ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

    void enqueue(const ElisaUtils::EntryDataList &newEntries,
                 ElisaUtils::PlayListEntryType databaseIdType,
                 ElisaUtils::PlayListEnqueueMode enqueueMode,
                 ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

    void setRepeatPlay(const bool value);

    void setShufflePlayList(const bool value);

    void trackInError(const QUrl &sourceInError, QMediaPlayer::Error playerError);

    void skipNextTrack();

    void skipPreviousTrack();

    void switchTo(int row);

    void removeSelection(QList<int> selection);

    void removeRow(int row);

    void moveRow(int from, int to);

    void clearPlayList();

    void undoClearPlayList();

    bool savePlayList(const QUrl &fileName);

    void loadPlayList(const QUrl &fileName);

    void setPersistentState(const QVariantMap &persistentState);

Q_SIGNALS:

    void previousTrackChanged(const QPersistentModelIndex &previousTrack);

    void currentTrackChanged(const QPersistentModelIndex &currentTrack);

    void nextTrackChanged(const QPersistentModelIndex &nextTrack);

    void previousTrackDataChanged();

    void currentTrackDataChanged();

    void nextTrackDataChanged();

    void repeatPlayChanged();

    void shufflePlayListChanged();

    void remainingTracksChanged();

    void ensurePlay();

    void currentTrackRowChanged();

    void tracksCountChanged();

    void playListFinished();

    void playListLoaded();

    void playListLoadFailed();

    void persistentStateChanged();

    void clearPlayListPlayer();

    void undoClearPlayListPlayer();

    void displayUndoNotification();

    void hideUndoNotification();

private Q_SLOTS:

    void sourceRowsAboutToBeInserted(const QModelIndex &parent, int start, int end);

    void sourceRowsInserted(const QModelIndex &parent, int start, int end);

    void sourceRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);

    void sourceRowsRemoved(const QModelIndex &parent, int start, int end);

    void sourceRowsAboutToBeMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destParent, int destRow);

    void sourceRowsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destParent, int destRow);

    void sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);

    void sourceHeaderDataChanged(Qt::Orientation orientation, int first, int last);

    void sourceLayoutAboutToBeChanged();

    void sourceLayoutChanged();

    void sourceModelAboutToBeReset();

    void sourceModelReset();

private Q_SLOTS:

    void loadPlayListLoaded();

    void loadPlayListLoadFailed();

private:

    void setSourceModel(QAbstractItemModel *sourceModel) override;

    void determineTracks();

    void notifyCurrentTrackChanged();

    void determineAndNotifyPreviousAndNextTracks();

    std::unique_ptr<MediaPlayListProxyModelPrivate> d;
};

#endif // MEDIAPLAYLISTPROXYMODEL_H
