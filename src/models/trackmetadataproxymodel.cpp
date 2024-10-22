/*
   SPDX-FileCopyrightText: 2024 (c) Jack Hill <jackhill3103@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "trackmetadataproxymodel.h"

#include "trackmetadatamodel.h"

TrackMetadataProxyModel::TrackMetadataProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setSortRole(Qt::DisplayRole);
    setSortCaseSensitivity(Qt::CaseInsensitive);
}

TrackMetadataProxyModel::~TrackMetadataProxyModel() = default;

int TrackMetadataProxyModel::mapRowToSource(const int proxyRow) const
{
    return mapToSource(index(proxyRow, 0)).row();
}

bool TrackMetadataProxyModel::showTagsWithNoData() const
{
    return mShowTagsWithNoData;
}

void TrackMetadataProxyModel::setShowTagsWithNoData(bool showTagsWithNoData)
{
    if (mShowTagsWithNoData == showTagsWithNoData) {
        return;
    }

    mShowTagsWithNoData = showTagsWithNoData;
    Q_EMIT showTagsWithNoDataChanged();
    invalidate();
}

bool TrackMetadataProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if (mShowTagsWithNoData) {
        return true;
    }

    const auto currentIndex = sourceModel()->index(sourceRow, 0, sourceParent);

    return sourceModel()->data(currentIndex, TrackMetadataModel::HasDataRole).toBool();
}

#include "moc_trackmetadataproxymodel.cpp"
