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

#include "abstractmediaproxymodel.h"

#include <QWriteLocker>

AbstractMediaProxyModel::AbstractMediaProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    mThreadPool.setMaxThreadCount(1);
}

AbstractMediaProxyModel::~AbstractMediaProxyModel()
= default;

QString AbstractMediaProxyModel::filterText() const
{
    return mFilterText;
}

int AbstractMediaProxyModel::filterRating() const
{
    return mFilterRating;
}

void AbstractMediaProxyModel::setFilterText(const QString &filterText)
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

void AbstractMediaProxyModel::setFilterRating(int filterRating)
{
    QWriteLocker writeLocker(&mDataLock);

    if (mFilterRating == filterRating) {
        return;
    }

    mFilterRating = filterRating;

    invalidate();

    Q_EMIT filterRatingChanged(filterRating);
}

bool AbstractMediaProxyModel::sortedAscending() const
{
    return sortOrder() ? false : true;
}

void AbstractMediaProxyModel::sortModel(Qt::SortOrder order)
{
    this->sort(0,order);
    Q_EMIT sortedAscendingChanged();
}

#include "moc_abstractmediaproxymodel.cpp"
