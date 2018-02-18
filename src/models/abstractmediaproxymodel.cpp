/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * Copyright 2017 Alexander Stippich <a.stippich@gmx.net>
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

#include "abstractmediaproxymodel.h"

AbstractMediaProxyModel::AbstractMediaProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
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
    if (mFilterRating == filterRating) {
        return;
    }

    mFilterRating = filterRating;

    invalidate();

    Q_EMIT filterRatingChanged(filterRating);
}

MediaPlayList *AbstractMediaProxyModel::mediaPlayList() const
{
    return mMediaPlayList;
}

void AbstractMediaProxyModel::setMediaPlayList(MediaPlayList *playList)
{
    mMediaPlayList = playList;
    Q_EMIT mediaPlayListChanged(mMediaPlayList);
}

void AbstractMediaProxyModel::replaceAndPlayOfPlayList()
{
    this->mediaPlayList()->clearPlayList();
    this->enqueueToPlayList();
    Q_EMIT this->mediaPlayList()->ensurePlay();
}

#include "moc_abstractmediaproxymodel.cpp"
