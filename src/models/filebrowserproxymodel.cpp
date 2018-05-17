/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "filebrowserproxymodel.h"

#include "filebrowsermodel.h"

#include <QReadLocker>
#include <QtConcurrentRun>
#include <QDir>
#include <KIOWidgets/KDirLister>
#include "elisautils.h"

FileBrowserProxyModel::FileBrowserProxyModel(QObject *parent) : KDirSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    mThreadPool.setMaxThreadCount(1);
    mFileModel = std::make_unique<FileBrowserModel>();
    setSourceModel(mFileModel.get());
    setSortFoldersFirst(true);
    sort(Qt::AscendingOrder);
    QObject::connect(mFileModel.get(), &FileBrowserModel::urlChanged,this, &FileBrowserProxyModel::urlChanged);
    mTopFolder = QDir::homePath();
    openFolder(mTopFolder, true);
}

FileBrowserProxyModel::~FileBrowserProxyModel()
{
}

QString FileBrowserProxyModel::filterText() const
{
    return mFilterText;
}

void FileBrowserProxyModel::setFilterText(const QString &filterText)
{
    QWriteLocker writeLocker(&mDataLock);

    if (mFilterText == filterText)
        return;

    mFilterText = filterText;

    mFilterExpression.setPattern(mFilterText);
    mFilterExpression.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    mFilterExpression.optimize();

    invalidate();

    Q_EMIT filterTextChanged(mFilterText);
}

bool FileBrowserProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    bool result = false;

    for (int column = 0, columnCount = sourceModel()->columnCount(source_parent); column < columnCount; ++column) {
        auto currentIndex = sourceModel()->index(source_row, column, source_parent);

        const auto &nameValue = sourceModel()->data(currentIndex, FileBrowserModel::NameRole).toString();

        if (mFilterExpression.match(nameValue).hasMatch()) {
            result = true;
            continue;
        }

        if (result) {
            continue;
        }

        if (!result) {
            break;
        }
    }

    return result;
}

void FileBrowserProxyModel::enqueueToPlayList()
{
    qDebug() << "enqueue";
    QtConcurrent::run(&mThreadPool, [=] () {
        QReadLocker locker(&mDataLock);
        auto allTrackUrls = QList<QUrl>();
        for (int rowIndex = 0, maxRowCount = rowCount(); rowIndex < maxRowCount; ++rowIndex) {
            auto currentIndex = index(rowIndex, 0);
            if (!data(currentIndex, FileBrowserModel::DirectoryRole).toBool()) {
                allTrackUrls.push_back(data(currentIndex, FileBrowserModel::ContainerDataRole).toUrl());
            }
        }
        Q_EMIT filesToEnqueue(allTrackUrls,
                              ElisaUtils::AppendPlayList,
                              ElisaUtils::DoNotTriggerPlay);
    });
}

void FileBrowserProxyModel::replaceAndPlayOfPlayList()
{
        qDebug() << "replace";
    QtConcurrent::run(&mThreadPool, [=] () {
        QReadLocker locker(&mDataLock);
        auto allTrackUrls = QList<QUrl>();
        for (int rowIndex = 0, maxRowCount = rowCount(); rowIndex < maxRowCount; ++rowIndex) {
            auto currentIndex = index(rowIndex, 0);
            if (!data(currentIndex, FileBrowserModel::DirectoryRole).toBool()) {
                allTrackUrls.push_back(data(currentIndex, FileBrowserModel::ContainerDataRole).toUrl());
            }
        }
        Q_EMIT filesToEnqueue(allTrackUrls,
                              ElisaUtils::ReplacePlayList,
                              ElisaUtils::TriggerPlay);
    });
}

QString FileBrowserProxyModel::parentFolder() const
{
    //return to the top folder if parent directory does not exist
    QDir dir(mFileModel->dirLister()->url().toLocalFile());
    if (dir.cdUp()) {
        return dir.path();
    } else {
        return mTopFolder;
    }
}

void FileBrowserProxyModel::openParentFolder()
{
    if (canGoBack()) {
        QString parent = parentFolder();
        mFileModel->setUrl(parent);
        if (parent == mTopFolder) {
            Q_EMIT canGoBackChanged();
        }
    }
}

bool FileBrowserProxyModel::canGoBack() const
{
    return mFileModel->dirLister()->url().toLocalFile() != mTopFolder;
}

void FileBrowserProxyModel::openFolder(const QString &folder, bool isDisplayRoot)
{
    if (folder.isEmpty()) {
        return;
    }
    mFileModel->setUrl(folder);
    if (!isDisplayRoot) {
        Q_EMIT canGoBackChanged();
    }
}

MusicAudioTrack FileBrowserProxyModel::loadMetaDataFromUrl(const QUrl &url)
{
    auto newTrack = ElisaUtils::scanOneFile(url,mMimeDb,mExtractors);
    qDebug() << "loaded metadata " << url << newTrack;
    return newTrack;
}

QString FileBrowserProxyModel::url() const
{
    return mFileModel->dirLister()->url().toLocalFile();
}

bool FileBrowserProxyModel::sortedAscending() const
{
    return sortOrder() ? false : true;
}

void FileBrowserProxyModel::sortModel(Qt::SortOrder order)
{
    this->sort(0,order);
    Q_EMIT sortedAscendingChanged();
}

#include "moc_filebrowserproxymodel.cpp"
