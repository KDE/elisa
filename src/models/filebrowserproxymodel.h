/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2018 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef FILEBROWSERPROXYMODEL_H
#define FILEBROWSERPROXYMODEL_H

#include "elisaLib_export.h"

#include "filescanner.h"
#include "elisautils.h"

#include <KDirSortFilterProxyModel>

#include <QMimeDatabase>
#include <QRegularExpression>

#include <memory>

class MediaPlayListProxyModel;

class ELISALIB_EXPORT FileBrowserProxyModel : public KDirSortFilterProxyModel
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

    explicit FileBrowserProxyModel(QObject *parent = nullptr);

    ~FileBrowserProxyModel() override;

    QString filterText() const;

    int filterRating() const;

    bool sortedAscending() const;

    MediaPlayListProxyModel* playList() const;

    void setSourceModel(QAbstractItemModel *sourceModel) override;

public Q_SLOTS:

    void enqueueToPlayList(QModelIndex rootIndex);

    void replaceAndPlayOfPlayList(QModelIndex rootIndex);

    void setFilterText(const QString &filterText);

    void setFilterRating(int filterRating);

    void setPlayList(MediaPlayListProxyModel* playList);

    void sortModel(Qt::SortOrder order);

Q_SIGNALS:

    void entriesToEnqueue(const DataTypes::EntryDataList &newEntries,
                          ElisaUtils::PlayListEnqueueMode enqueueMode,
                          ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

    void filterTextChanged(const QString &filterText);

    void filterRatingChanged();

    void sortedAscendingChanged();

    void playListChanged();

protected:

private:

    void genericEnqueueToPlayList(QModelIndex rootIndex,
                                  ElisaUtils::PlayListEnqueueMode enqueueMode,
                                  ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

    void disconnectPlayList();

    void connectPlayList();

    QString mFilterText;

    QRegularExpression mFilterExpression;

    MediaPlayListProxyModel* mPlayList = nullptr;

    bool mFilterRating = false;

    QMimeDatabase mMimeDatabase;

};

#endif // FILEBROWSERPROXYMODEL_H
