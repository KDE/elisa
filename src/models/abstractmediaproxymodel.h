/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * Copyright 2017 Alexander Stippich <a.stippich@gmx.net>
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

#ifndef ABSTRACTMEDIAPROXYMODEL_H
#define ABSTRACTMEDIAPROXYMODEL_H

#include "elisaLib_export.h"

#include "elisautils.h"

#include <QSortFilterProxyModel>
#include <QRegularExpression>
#include <QReadWriteLock>
#include <QThreadPool>

class MediaPlayListProxyModel;

class ELISALIB_EXPORT AbstractMediaProxyModel : public QSortFilterProxyModel
{

    Q_OBJECT

    Q_PROPERTY(QString filterText
               READ filterText
               WRITE setFilterText
               NOTIFY filterTextChanged)

    Q_PROPERTY(int filterRating
               READ filterRating
               WRITE setFilterRating
               NOTIFY filterRatingChanged)

    Q_PROPERTY(bool sortedAscending
               READ sortedAscending
               NOTIFY sortedAscendingChanged)

    Q_PROPERTY(MediaPlayListProxyModel* playList READ playList WRITE setPlayList NOTIFY playListChanged)

public:

    explicit AbstractMediaProxyModel(QObject *parent = nullptr);

    ~AbstractMediaProxyModel() override;

    QString filterText() const;

    int filterRating() const;

    bool sortedAscending() const;

    MediaPlayListProxyModel* playList() const;

public Q_SLOTS:

    void setFilterText(const QString &filterText);

    void setFilterRating(int filterRating);

    void sortModel(Qt::SortOrder order);

    void setPlayList(MediaPlayListProxyModel* playList);

Q_SIGNALS:

    void filterTextChanged(const QString &filterText);

    void filterRatingChanged(int filterRating);

    void sortedAscendingChanged();

    void playListChanged();

    void entriesToEnqueue(const ElisaUtils::EntryDataList &newEntries,
                          ElisaUtils::PlayListEntryType databaseIdType,
                          ElisaUtils::PlayListEnqueueMode enqueueMode,
                          ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

protected:

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override = 0;

    void disconnectPlayList();

    void connectPlayList();

    QString mFilterText;

    int mFilterRating = 0;

    QRegularExpression mFilterExpression;

    QReadWriteLock mDataLock;

    QThreadPool mThreadPool;

    MediaPlayListProxyModel* mPlayList = nullptr;

};

#endif // ABSTRACTMEDIAPROXYMODEL_H
