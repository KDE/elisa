/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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
