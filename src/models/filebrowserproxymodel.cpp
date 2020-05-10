/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2018 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "filebrowserproxymodel.h"

#include "filebrowsermodel.h"
#include "mediaplaylistproxymodel.h"

#include <QReadLocker>
#include <QDir>
#include <QtConcurrent>

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

void FileBrowserProxyModel::genericEnqueueToPlayList(ElisaUtils::PlayListEnqueueMode enqueueMode, ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay)
{
    QtConcurrent::run(&mThreadPool, [=] () {
        QReadLocker locker(&mDataLock);
        auto allTrackUrls = DataTypes::EntryDataList{};
        for (int rowIndex = 0, maxRowCount = rowCount(); rowIndex < maxRowCount; ++rowIndex) {
            auto currentIndex = index(rowIndex, 0);
            if (!data(currentIndex, FileBrowserModel::IsDirectoryRole).toBool()) {
                allTrackUrls.push_back({{{DataTypes::ElementTypeRole, ElisaUtils::Track},
                                         {DataTypes::ResourceRole, data(currentIndex, FileBrowserModel::FileUrlRole).toUrl()}}, {}, {}});
            }
        }
        Q_EMIT entriesToEnqueue(allTrackUrls, enqueueMode, triggerPlay);
    });
}

void FileBrowserProxyModel::enqueueToPlayList()
{
    genericEnqueueToPlayList(ElisaUtils::AppendPlayList,
                             ElisaUtils::DoNotTriggerPlay);
}

void FileBrowserProxyModel::replaceAndPlayOfPlayList()
{
    genericEnqueueToPlayList(ElisaUtils::ReplacePlayList,
                              ElisaUtils::TriggerPlay);
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

void FileBrowserProxyModel::disconnectPlayList()
{
    if (mPlayList) {
        disconnect(this, &FileBrowserProxyModel::entriesToEnqueue,
                   mPlayList, static_cast<void(MediaPlayListProxyModel::*)(const DataTypes::EntryDataList&, ElisaUtils::PlayListEnqueueMode, ElisaUtils::PlayListEnqueueTriggerPlay)>(&MediaPlayListProxyModel::enqueue));
    }
}

void FileBrowserProxyModel::connectPlayList()
{
    if (mPlayList) {
        connect(this, &FileBrowserProxyModel::entriesToEnqueue,
                mPlayList, static_cast<void(MediaPlayListProxyModel::*)(const DataTypes::EntryDataList&, ElisaUtils::PlayListEnqueueMode, ElisaUtils::PlayListEnqueueTriggerPlay)>(&MediaPlayListProxyModel::enqueue));
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

MediaPlayListProxyModel *FileBrowserProxyModel::playList() const
{
    return mPlayList;
}

int FileBrowserProxyModel::filterRating() const
{
    return mFilterRating;
}

void FileBrowserProxyModel::sortModel(Qt::SortOrder order)
{
    this->sort(0,order);
    Q_EMIT sortedAscendingChanged();
}

void FileBrowserProxyModel::setPlayList(MediaPlayListProxyModel *playList)
{
    disconnectPlayList();

    if (mPlayList == playList) {
        return;
    }

    mPlayList = playList;
    Q_EMIT playListChanged();

    connectPlayList();
}

void FileBrowserProxyModel::setFilterRating(int filterRating)
{
    if (mFilterRating == filterRating) {
        return;
    }

    mFilterRating = filterRating;
    Q_EMIT filterRatingChanged();
}

#include "moc_filebrowserproxymodel.cpp"
