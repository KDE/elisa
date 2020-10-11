/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2017 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef ABSTRACTMEDIAPROXYMODEL_H
#define ABSTRACTMEDIAPROXYMODEL_H

#include "elisaLib_export.h"

#include "elisautils.h"
#include "datatypes.h"

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

    [[nodiscard]] QString filterText() const;

    [[nodiscard]] int filterRating() const;

    [[nodiscard]] bool sortedAscending() const;

    [[nodiscard]] MediaPlayListProxyModel* playList() const;

public Q_SLOTS:

    void setFilterText(const QString &filterText);

    void setFilterRating(int filterRating);

    void sortModel(Qt::SortOrder order);

    void setPlayList(MediaPlayListProxyModel* playList);

    void enqueueToPlayList(const QModelIndex &rootIndex);

    void replaceAndPlayOfPlayList(const QModelIndex &rootIndex);

    void enqueue(const DataTypes::MusicDataType &newEntry,
                 const QString &newEntryTitle,
                 ElisaUtils::PlayListEnqueueMode enqueueMode,
                 ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

Q_SIGNALS:

    void filterTextChanged(const QString &filterText);

    void filterRatingChanged(int filterRating);

    void sortedAscendingChanged();

    void playListChanged();

    void entriesToEnqueue(const DataTypes::EntryDataList &newEntries,
                          ElisaUtils::PlayListEnqueueMode enqueueMode,
                          ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

protected:

    [[nodiscard]] bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override = 0;

    void disconnectPlayList();

    void connectPlayList();

    QString mFilterText;

    int mFilterRating = 0;

    QRegularExpression mFilterExpression;

    QReadWriteLock mDataLock;

    QThreadPool mThreadPool;

    MediaPlayListProxyModel* mPlayList = nullptr;

private:

    void genericEnqueueToPlayList(const QModelIndex &rootIndex,
                                  ElisaUtils::PlayListEnqueueMode enqueueMode,
                                  ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

};

#endif // ABSTRACTMEDIAPROXYMODEL_H
