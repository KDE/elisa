/***************************************************************************
 *   Copyright 2014 Ashish Madeti <ashishmadeti@gmail.com>                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#include "mediaplayer2tracklist.h"
#include "mpris2.h"

static const QString playlistTidPrefix(QStringLiteral("/org/kde/plasmamediacenter/playlist/"));
static const QDBusObjectPath mprisNoTrack(QStringLiteral("/org/mpris/MediaPlayer2/TrackList/NoTrack"));

MediaPlayer2Tracklist::MediaPlayer2Tracklist(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    qDBusRegisterMetaType< QList<QVariantMap> >();

    /*QObject::connect(m_playlistModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(rowsInsertedInModel(QModelIndex,int,int)));
    QObject::connect(m_playlistModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(rowsRemovedFromModel(QModelIndex,int,int)));
    QObject::connect(m_playlistModel, SIGNAL(modelReset()), this, SLOT(resetTrackIds()));
    QObject::connect(m_playlistModel, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(rowsMovedInModel(QModelIndex,int,int,QModelIndex,int)));*/

    /*for (int i = 0; i < m_playlistModel->rowCount(); i++) {
        m_orderedTrackIds << QDBusObjectPath(playlistTidPrefix + QString::number(tidCounter++));
    }*/
}

MediaPlayer2Tracklist::~MediaPlayer2Tracklist()
{
}

bool MediaPlayer2Tracklist::CanEditTracks() const
{
    //TODO: Allow adding/removing tracks from playlist via D-Bus
    return false;
}

QList<QDBusObjectPath> MediaPlayer2Tracklist::Tracks() const
{
    return m_orderedTrackIds;
}

int MediaPlayer2Tracklist::currentIndex() const
{
    return 0/*m_playlistModel->currentIndex()*/;
}

QDBusObjectPath MediaPlayer2Tracklist::currentTrackId() const
{
    if (currentIndex() == -1) {
        return mprisNoTrack;
    }
    return m_orderedTrackIds.at(currentIndex());
}

void MediaPlayer2Tracklist::rowsInsertedInModel(const QModelIndex &parentModel, int start, int end)
{
    Q_UNUSED(parentModel);

    QVariantMap metadata;
    QDBusObjectPath afterTrack;
    for (int i = start; i <= end; i++) {
        m_orderedTrackIds.insert(i, QDBusObjectPath(playlistTidPrefix + QString::number(tidCounter++)));
        metadata = static_cast<Mpris2*>(parent())->getMetadataOf(urlOfIndex(i), m_orderedTrackIds[i].path());
        afterTrack = (i > 0) ? m_orderedTrackIds.at(i-1) : mprisNoTrack;
        emit TrackAdded(metadata, afterTrack);
    }
}

void MediaPlayer2Tracklist::rowsRemovedFromModel(const QModelIndex &parentModel, int start, int end)
{
    Q_UNUSED(parentModel);

    for (int i = start; i <= end; i++) {
        emit TrackRemoved(m_orderedTrackIds.takeAt(i));
    }
}

void MediaPlayer2Tracklist::rowsMovedInModel(const QModelIndex &sourceParent, int sourceStart, int sourceEnd,
                                             const QModelIndex &destinationParent, int movedBeforeRow)
{
    Q_UNUSED(sourceParent);
    Q_UNUSED(destinationParent);

    int destinationRow = (movedBeforeRow > sourceStart) ? movedBeforeRow - 1 : movedBeforeRow;
    for (int i = 0, index = sourceStart; index <= sourceEnd; i++, index++) {
        m_orderedTrackIds.move(index, destinationRow + i);
    }
}

void MediaPlayer2Tracklist::resetTrackIds()
{
    m_orderedTrackIds.clear();
    /*for (int i = 0; i < m_playlistModel->rowCount(); i++) {
        m_orderedTrackIds << QDBusObjectPath(playlistTidPrefix + QString::number(tidCounter++));
    }*/

    emit TrackListReplaced(Tracks(), currentTrackId());
}

QList<QVariantMap> MediaPlayer2Tracklist::GetTracksMetadata(const QList<QDBusObjectPath> &trackIds)
{
    QList<QVariantMap> metadataList;
    int index;

    Q_FOREACH (const QDBusObjectPath& trackId, trackIds) {
        index = m_orderedTrackIds.indexOf(trackId);
        if (index != -1) {
            metadataList << static_cast<Mpris2*>(parent())->getMetadataOf(urlOfIndex(index), trackId.path());
        }
    }

    return metadataList;
}

void MediaPlayer2Tracklist::AddTrack(const QString &uri, const QDBusObjectPath &afterTrack, bool setAsCurrent)
{
    Q_UNUSED(uri)
    Q_UNUSED(afterTrack)
    Q_UNUSED(setAsCurrent)
    //As CanEditTracks is False, do nothing
}

void MediaPlayer2Tracklist::RemoveTrack(const QDBusObjectPath &trackId)
{
    Q_UNUSED(trackId)
    //As CanEditTracks is False, do nothing
}

void MediaPlayer2Tracklist::GoTo(const QDBusObjectPath &trackId)
{
    int newIndex = m_orderedTrackIds.indexOf(trackId);
    if (newIndex != -1) {
        /*m_playlistModel->setCurrentIndex(newIndex);*/
    }
}

QString MediaPlayer2Tracklist::urlOfIndex(int index) const
{
    return {}/*m_playlistModel->data(m_playlistModel->index(index, 0),
                                 MediaCenter::MediaUrlRole).toString()*/;
}
