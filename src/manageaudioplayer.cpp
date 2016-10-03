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

#include "manageaudioplayer.h"

ManageAudioPlayer::ManageAudioPlayer(QObject *parent) : QObject(parent)
{

}

QPersistentModelIndex ManageAudioPlayer::currentTrack() const
{
    return mCurrentTrack;
}

int ManageAudioPlayer::urlRole() const
{
    return mUrlRole;
}

QUrl ManageAudioPlayer::playerSource() const
{
    if (!mCurrentTrack.isValid()) {
        return QUrl();
    }

    return mCurrentTrack.data(mUrlRole).toUrl();
}

void ManageAudioPlayer::setCurrentTrack(QPersistentModelIndex currentTrack)
{
    if (mCurrentTrack == currentTrack) {
        return;
    }

    mCurrentTrack = currentTrack;
    Q_EMIT currentTrackChanged();

    notifyPlayerSourceProperty();
}

void ManageAudioPlayer::setUrlRole(int value)
{
    mUrlRole = value;
    Q_EMIT urlRoleChanged();
}

void ManageAudioPlayer::notifyPlayerSourceProperty()
{
    auto newUrlValue = mCurrentTrack.data(mUrlRole);
    if (mOldPlayerSource != newUrlValue) {
        Q_EMIT playerSourceChanged();

        mOldPlayerSource = newUrlValue;
    }
}


#include "moc_manageaudioplayer.cpp"
