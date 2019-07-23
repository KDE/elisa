/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * Copyright 2018 Alexander Stippich <a.stippich@gmx.net>
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
    setSortFoldersFirst(true);
    sort(Qt::AscendingOrder);
}

FileBrowserProxyModel::~FileBrowserProxyModel() = default;

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
    QtConcurrent::run(&mThreadPool, [=] () {
        QReadLocker locker(&mDataLock);
        auto allTrackUrls = ElisaUtils::EntryDataList{};
        for (int rowIndex = 0, maxRowCount = rowCount(); rowIndex < maxRowCount; ++rowIndex) {
            auto currentIndex = index(rowIndex, 0);
            if (!data(currentIndex, FileBrowserModel::IsDirectoryRole).toBool()) {
                allTrackUrls.push_back({0, data(currentIndex, FileBrowserModel::FileUrlRole).toString()});
            }
        }
        Q_EMIT filesToEnqueue(allTrackUrls,
                              ElisaUtils::FileName,
                              ElisaUtils::AppendPlayList,
                              ElisaUtils::DoNotTriggerPlay);
    });
}

void FileBrowserProxyModel::replaceAndPlayOfPlayList()
{
    QtConcurrent::run(&mThreadPool, [=] () {
        QReadLocker locker(&mDataLock);
        auto allTrackUrls = ElisaUtils::EntryDataList{};
        for (int rowIndex = 0, maxRowCount = rowCount(); rowIndex < maxRowCount; ++rowIndex) {
            auto currentIndex = index(rowIndex, 0);
            if (!data(currentIndex, FileBrowserModel::IsDirectoryRole).toBool()) {
                allTrackUrls.push_back({0, data(currentIndex, FileBrowserModel::FileUrlRole).toString()});
            }
        }
        Q_EMIT filesToEnqueue(allTrackUrls,
                              ElisaUtils::FileName,
                              ElisaUtils::ReplacePlayList,
                              ElisaUtils::TriggerPlay);
    });
}

QString FileBrowserProxyModel::parentFolder() const
{
    auto fileBrowserModel = dynamic_cast<FileBrowserModel*>(sourceModel());

    if (!fileBrowserModel) {
        return {};
    }

    //return to the top folder if parent directory does not exist
    QDir dir(fileBrowserModel->dirLister()->url().toLocalFile());
    if (dir.cdUp()) {
        return dir.path();
    } else {
        return mTopFolder;
    }
}

void FileBrowserProxyModel::openParentFolder()
{
    auto fileBrowserModel = dynamic_cast<FileBrowserModel*>(sourceModel());

    if (!fileBrowserModel) {
        return;
    }

    if (canGoBack()) {
        QString parent = parentFolder();
        fileBrowserModel->setUrl(parent);
        if (parent == mTopFolder) {
            Q_EMIT canGoBackChanged();
        }
    }
}

bool FileBrowserProxyModel::canGoBack() const
{
    auto fileBrowserModel = dynamic_cast<FileBrowserModel*>(sourceModel());

    if (!fileBrowserModel) {
        return false;
    }

    return fileBrowserModel->dirLister()->url().toLocalFile() != mTopFolder;
}

void FileBrowserProxyModel::openFolder(const QString &folder, bool isDisplayRoot)
{
    auto fileBrowserModel = dynamic_cast<FileBrowserModel*>(sourceModel());

    if (!fileBrowserModel) {
        return;
    }

    if (folder.isEmpty()) {
        return;
    }

    fileBrowserModel->setUrl(folder);

    if (!isDisplayRoot) {
        Q_EMIT canGoBackChanged();
    }
}

QString FileBrowserProxyModel::url() const
{
    auto fileBrowserModel = dynamic_cast<FileBrowserModel*>(sourceModel());

    if (!fileBrowserModel) {
        return {};
    }

    return fileBrowserModel->dirLister()->url().toLocalFile();
}

bool FileBrowserProxyModel::sortedAscending() const
{
    return sortOrder() ? false : true;
}

void FileBrowserProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    KDirSortFilterProxyModel::setSourceModel(sourceModel);

    auto fileBrowserModel = dynamic_cast<FileBrowserModel*>(sourceModel);

    if (!fileBrowserModel) {
        return;
    }

    connect(fileBrowserModel, &FileBrowserModel::urlChanged,this, &FileBrowserProxyModel::urlChanged);
    mTopFolder = QDir::homePath();
    openFolder(mTopFolder, true);
}

void FileBrowserProxyModel::sortModel(Qt::SortOrder order)
{
    this->sort(0,order);
    Q_EMIT sortedAscendingChanged();
}

#include "moc_filebrowserproxymodel.cpp"
