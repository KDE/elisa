/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "progressindicator.h"

#include <QTime>

ProgressIndicator::ProgressIndicator(QObject *parent) : QObject(parent)
{
}

ProgressIndicator::~ProgressIndicator()
= default;

int ProgressIndicator::position() const
{
    return mPosition;
}

QString ProgressIndicator::progressDuration() const
{
    return mProgressDuration;
}

void ProgressIndicator::setPosition(int position)
{
    if (mPosition == position)
        return;

    mPosition = position;

    QTime currentProgress = QTime::fromMSecsSinceStartOfDay(mPosition);
    if (currentProgress.hour() == 0) {
        mProgressDuration = currentProgress.toString(QStringLiteral("m:ss"));
    } else {
        mProgressDuration = currentProgress.toString(QStringLiteral("h:mm:ss"));
    }

    Q_EMIT positionChanged();
    Q_EMIT progressDurationChanged();
}


#include "moc_progressindicator.cpp"
