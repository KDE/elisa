/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
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
