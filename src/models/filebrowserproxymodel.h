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

#include <QRegularExpression>
#include <QReadWriteLock>
#include <QThreadPool>

#include <memory>

class MediaPlayListProxyModel;

class ELISALIB_EXPORT FileBrowserProxyModel : public KDirSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(QString filterText
               READ filterText
               WRITE setFilterText
               NOTIFY filterTextChanged)

    Q_PROPERTY(bool canGoBack
               READ canGoBack
               NOTIFY canGoBackChanged)

    Q_PROPERTY(QString url
               READ url
               NOTIFY urlChanged)

    Q_PROPERTY(bool sortedAscending
               READ sortedAscending
               NOTIFY sortedAscendingChanged)

    Q_PROPERTY(int filterRating
               READ filterRating
               WRITE setFilterRating
               NOTIFY filterRatingChanged)

    Q_PROPERTY(MediaPlayListProxyModel* playList READ playList WRITE setPlayList NOTIFY playListChanged)

public:

    explicit FileBrowserProxyModel(QObject *parent = nullptr);

    ~FileBrowserProxyModel() override;

    QString filterText() const;

    QString url() const;

    bool canGoBack() const;

    bool sortedAscending() const;

    void setSourceModel(QAbstractItemModel *sourceModel) override;

    MediaPlayListProxyModel* playList() const;

    int filterRating() const;

public Q_SLOTS:

    void enqueueToPlayList();

    void replaceAndPlayOfPlayList();

    void setFilterText(const QString &filterText);

    void openParentFolder();

    void openFolder(const QString &folder, bool isDisplayRoot = false);

    void sortModel(Qt::SortOrder order);

    void setPlayList(MediaPlayListProxyModel* playList);

    void setFilterRating(int filterRating);

Q_SIGNALS:

    void entriesToEnqueue(const DataTypes::EntryDataList &newEntries,
                          ElisaUtils::PlayListEnqueueMode enqueueMode,
                          ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

    void urlChanged();

    void canGoBackChanged();

    void filterTextChanged(const QString &filterText);

    void sortedAscendingChanged();

    void playListChanged();

    void filterRatingChanged();

protected:

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:

    void genericEnqueueToPlayList(ElisaUtils::PlayListEnqueueMode enqueueMode,
                                  ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

    QString parentFolder() const;

    void disconnectPlayList();

    void connectPlayList();

    QString mTopFolder;

    QString mFilterText;

    QRegularExpression mFilterExpression;

    QReadWriteLock mDataLock;

    QThreadPool mThreadPool;

    MediaPlayListProxyModel* mPlayList = nullptr;

    bool mFilterRating = false;

};

#endif // FILEBROWSERPROXYMODEL_H
