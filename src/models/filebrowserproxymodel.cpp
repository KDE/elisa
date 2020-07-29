/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2018 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "filebrowserproxymodel.h"

#include "elisautils.h"
#include "filebrowsermodel.h"
#include "mediaplaylistproxymodel.h"

#include "models/modelLogging.h"

#include <stack>

FileBrowserProxyModel::FileBrowserProxyModel(QObject *parent)
    : KDirSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
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
    if (mFilterText == filterText)
        return;

    mFilterText = filterText;

    mFilterExpression.setPattern(mFilterText);
    mFilterExpression.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    mFilterExpression.optimize();

    setFilterRegularExpression(mFilterExpression);
    setFilterRole(Qt::DisplayRole);

    Q_EMIT filterTextChanged(mFilterText);
}

void FileBrowserProxyModel::listRecursiveResult(KJob *)
{
    if (mPendingEntries.empty()) {
        mEnqueueInProgress = false;
        Q_EMIT entriesToEnqueue(mAllData, mEnqueueMode, mTriggerPlay);
        return;
    }

    recursiveEnqueue();
}

void FileBrowserProxyModel::listRecursiveNewEntries(KIO::Job *job, const KIO::UDSEntryList &list)
{
    Q_UNUSED(job)

    for (const auto &oneEntry : list) {
        if (oneEntry.isDir()) {
            continue;
        }

        auto returnedPath = oneEntry.stringValue(KIO::UDSEntry::UDS_NAME);
        auto fullPath = QStringLiteral("%0/%1").arg(mCuurentUrl.toString(), returnedPath);
        auto fullPathUrl = QUrl { fullPath };

        auto mimeType = mMimeDatabase.mimeTypeForUrl(fullPathUrl);

        if (!mimeType.name().startsWith(QLatin1String("audio/"))) {
            continue;
        }

        mAllData.push_back(DataTypes::EntryData { { { DataTypes::ElementTypeRole, ElisaUtils::FileName } }, fullPath, fullPathUrl });
    }
}

void FileBrowserProxyModel::genericEnqueueToPlayList(QModelIndex rootIndex,
                                                     ElisaUtils::PlayListEnqueueMode enqueueMode,
                                                     ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay)
{
    if (mEnqueueInProgress) {
        // display error
        return;
    }

    mPendingEntries = {};
    mAllData.clear();

    for (int rowIndex = 0, maxRowCount = rowCount(); rowIndex < maxRowCount; ++rowIndex) {
        auto currentIndex = index(rowIndex, 0, rootIndex);
        mPendingEntries.emplace(currentIndex.data(DataTypes::FilePathRole).toUrl(),
                               currentIndex.data(DataTypes::ElementTypeRole).value<ElisaUtils::PlayListEntryType>() == ElisaUtils::Container);
    }

    mEnqueueInProgress = true;
    mEnqueueMode = enqueueMode;
    mTriggerPlay = triggerPlay;
    recursiveEnqueue();
}

void FileBrowserProxyModel::enqueueToPlayList(QModelIndex rootIndex)
{
    genericEnqueueToPlayList(rootIndex,
                             ElisaUtils::AppendPlayList,
                             ElisaUtils::DoNotTriggerPlay);
}

void FileBrowserProxyModel::enqueue(const DataTypes::MusicDataType &newEntry,
                                    const QString &newEntryTitle,
                                    ElisaUtils::PlayListEnqueueMode enqueueMode,
                                    ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay)
{
    Q_UNUSED(newEntryTitle)

    if (mEnqueueInProgress) {
        // display error
        return;
    }

    mPendingEntries = {};
    mAllData.clear();

    mPendingEntries.emplace(newEntry[DataTypes::FilePathRole].toUrl(),
                           newEntry.elementType() == ElisaUtils::Container);

    mEnqueueInProgress = true;
    mEnqueueMode = enqueueMode;
    mTriggerPlay = triggerPlay;
    recursiveEnqueue();
}

void FileBrowserProxyModel::replaceAndPlayOfPlayList(QModelIndex rootIndex)
{
    genericEnqueueToPlayList(rootIndex,
                             ElisaUtils::ReplacePlayList,
                             ElisaUtils::TriggerPlay);
}

void FileBrowserProxyModel::disconnectPlayList()
{
    if (mPlayList) {
        disconnect(this, &FileBrowserProxyModel::entriesToEnqueue,
                   mPlayList, static_cast<void (MediaPlayListProxyModel::*)(const DataTypes::EntryDataList &, ElisaUtils::PlayListEnqueueMode, ElisaUtils::PlayListEnqueueTriggerPlay)>(&MediaPlayListProxyModel::enqueue));
    }
}

void FileBrowserProxyModel::connectPlayList()
{
    if (mPlayList) {
        connect(this, &FileBrowserProxyModel::entriesToEnqueue,
                mPlayList, static_cast<void (MediaPlayListProxyModel::*)(const DataTypes::EntryDataList &, ElisaUtils::PlayListEnqueueMode, ElisaUtils::PlayListEnqueueTriggerPlay)>(&MediaPlayListProxyModel::enqueue));
    }
}

void FileBrowserProxyModel::recursiveEnqueue()
{
    auto [rootUrl, isDirectory] = mPendingEntries.front();
    mPendingEntries.pop();
    if (isDirectory) {
        mCuurentUrl = rootUrl;
        mCurrentJob = KIO::listRecursive(rootUrl, { KIO::HideProgressInfo });

        connect(mCurrentJob, &KJob::result, this, &FileBrowserProxyModel::listRecursiveResult);

        connect(dynamic_cast<KIO::ListJob*>(mCurrentJob), &KIO::ListJob::entries,
                this, &FileBrowserProxyModel::listRecursiveNewEntries);
    } else {
        if (mPlayList) {
            mAllData.push_back(DataTypes::EntryData { { { DataTypes::ElementTypeRole, ElisaUtils::FileName },
                                                        { DataTypes::ResourceRole, rootUrl}},
                                                      rootUrl.toString(),
                                                      rootUrl });

            if (mPendingEntries.empty()) {
                mEnqueueInProgress = false;
                Q_EMIT entriesToEnqueue(mAllData, mEnqueueMode, mTriggerPlay);
                return;
            }

            recursiveEnqueue();
        }
    }
}

void FileBrowserProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    KDirSortFilterProxyModel::setSourceModel(sourceModel);

    auto fileBrowserModel = dynamic_cast<FileBrowserModel *>(sourceModel);

    if (!fileBrowserModel) {
        return;
    }
}

MediaPlayListProxyModel *FileBrowserProxyModel::playList() const
{
    return mPlayList;
}

int FileBrowserProxyModel::filterRating() const
{
    return mFilterRating;
}

bool FileBrowserProxyModel::sortedAscending() const
{
    return sortOrder() ? false : true;
}

void FileBrowserProxyModel::sortModel(Qt::SortOrder order)
{
    sort(0, order);
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
