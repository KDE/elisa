/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "progressindicator.h"

#include <KLocalizedString>

using namespace Qt::StringLiterals;

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

    int totalNumberOfSeconds = mPosition / 1000;
    int seconds = totalNumberOfSeconds % 60;
    int minutes = (totalNumberOfSeconds / 60) % 60;
    int hours = (totalNumberOfSeconds / 60 / 60);

    QString hoursString = u"%1"_s.arg(hours, 2, 10, '0'_L1);
    QString minutesString = u"%1"_s.arg(minutes, 2, 10, '0'_L1);
    QString secondsString = u"%1"_s.arg(seconds, 2, 10, '0'_L1);

    QString duration;
    if (hours == 0) {
        duration = i18nc("duration formatted as minutes:seconds", "%1:%2", minutesString, secondsString);
    } else {
        duration = i18nc("duration formatted as hours:minutes:seconds", "%1:%2:%3", hoursString, minutesString, secondsString);
    }
    mProgressDuration = duration;

    Q_EMIT positionChanged();
    Q_EMIT progressDurationChanged();
}


#include "moc_progressindicator.cpp"
