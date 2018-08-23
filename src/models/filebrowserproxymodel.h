/*
 * Copyright 2016-2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * Copyright 2018 Alexander Stippich <a.stippich@gmx.net>
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

#ifndef FILEBROWSERPROXYMODEL_H
#define FILEBROWSERPROXYMODEL_H

#include "elisaLib_export.h"

#include "filebrowsermodel.h"
#include "musicaudiotrack.h"
#include "filescanner.h"
#include "elisautils.h"

#include <KIOFileWidgets/KDirSortFilterProxyModel>
#include <QRegularExpression>
#include <QReadWriteLock>
#include <QThreadPool>

#include <memory>

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

public:

    explicit FileBrowserProxyModel(QObject *parent = nullptr);

    ~FileBrowserProxyModel() override;

    QString filterText() const;

    QString url() const;

    bool canGoBack() const;

    Q_INVOKABLE MusicAudioTrack loadMetaDataFromUrl(const QUrl &url);

    bool sortedAscending() const;

public Q_SLOTS:

    void enqueueToPlayList();

    void replaceAndPlayOfPlayList();

    void setFilterText(const QString &filterText);

    void openParentFolder();

    void openFolder(const QString &folder, bool isDisplayRoot = false);

    void sortModel(Qt::SortOrder order);

Q_SIGNALS:

    void filesToEnqueue(QList<QUrl> newFiles,
                        ElisaUtils::PlayListEnqueueMode enqueueMode,
                        ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

    void urlChanged();

    void canGoBackChanged();

    void filterTextChanged(const QString &filterText);

    void sortedAscendingChanged();

protected:


    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:

    QString parentFolder() const;

    QString mTopFolder;

    FileScanner mFileScanner;

    QMimeDatabase mMimeDb;

    QString mFilterText;

    QRegularExpression mFilterExpression;

    QReadWriteLock mDataLock;

    QThreadPool mThreadPool;

    std::unique_ptr<FileBrowserModel> mFileModel;

};

#endif // FILEBROWSERPROXYMODEL_H
