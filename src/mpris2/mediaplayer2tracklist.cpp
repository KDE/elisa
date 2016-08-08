/***************************************************************************
 *   Copyright 2014 Ashish Madeti <ashishmadeti@gmail.com>                 *
 *   Copyright 2016 Matthieu Gallien <mgallien@mgallien.fr>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
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

#include "mediaplaylist.h"

#include <QAbstractItemModel>

static const QString playlistTidPrefix(QStringLiteral("/org/kde/elisa/playlist/"));
static const QDBusObjectPath mprisNoTrack(QStringLiteral("/org/mpris/MediaPlayer2/TrackList/NoTrack"));

MediaPlayer2Tracklist::MediaPlayer2Tracklist(QAbstractItemModel *playListModel, QObject *parent)
    : QDBusAbstractAdaptor(parent), m_playListModel(playListModel)
{
    qDBusRegisterMetaType< QList<QVariantMap> >();

    QObject::connect(m_playListModel, &QAbstractItemModel::rowsInserted, this, &MediaPlayer2Tracklist::rowsInsertedInModel);
    QObject::connect(m_playListModel, &QAbstractItemModel::rowsRemoved, this, &MediaPlayer2Tracklist::rowsRemovedFromModel);
    QObject::connect(m_playListModel, SIGNAL(modelReset()), this, SLOT(resetTrackIds()));
    QObject::connect(m_playListModel, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(rowsMovedInModel(QModelIndex,int,int,QModelIndex,int)));

    for (int i = 0; i < m_playListModel->rowCount(); i++) {
        m_orderedTrackIds << QDBusObjectPath(playlistTidPrefix + QString::number(tidCounter++));
        m_trackIndexFromURL[m_playListModel->data(m_playListModel->index(i, 0), MediaPlayList::ResourceRole).toString()] = i;
    }
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

void MediaPlayer2Tracklist::setCurrentIndex(int newIndex)
{
    m_currentIndex = newIndex;
}

int MediaPlayer2Tracklist::currentIndex() const
{
    return m_currentIndex;
}

QDBusObjectPath MediaPlayer2Tracklist::currentDBusTrackId() const
{
    if (currentIndex() == -1) {
        return mprisNoTrack;
    }

    if (currentIndex() < 0 || currentIndex() >= m_orderedTrackIds.size()) {
        return mprisNoTrack;
    }

    return m_orderedTrackIds.at(currentIndex());
}

QString MediaPlayer2Tracklist::currentTrackId() const
{
    if (currentIndex() != -1) {
        return currentDBusTrackId().path();
    }

    return QString(QStringLiteral("/org/mpris/MediaPlayer2/TrackList/NoTrack"));
}

QVariantMap MediaPlayer2Tracklist::getMetadataOf(const QString &url)
{
    QVariantMap metadata = getMetadataOf(url, currentTrackId());

    return metadata;
}

QVariantMap MediaPlayer2Tracklist::getMetadataOf(const QString &url, const QString& trackId)
{
    QVariantMap metadata;

    auto trackRow = indexOfURL(url);
    if (trackRow == -1) {
        return metadata;
    }

    auto trackIndex = m_playListModel->index(trackRow, 0);
    if (!trackIndex.isValid()) {
        return metadata;
    }

    metadata[QStringLiteral("mpris:trackid")] = QVariant::fromValue<QDBusObjectPath>(QDBusObjectPath(trackId)).toString();
    metadata[QStringLiteral("mpris:length")] = qlonglong(m_playListModel->data(trackIndex, MediaPlayList::MilliSecondsDurationRole).toInt()) * 1000;
    //convert milli-seconds into micro-seconds
    metadata[QStringLiteral("xesam:title")] = m_playListModel->data(trackIndex, MediaPlayList::TitleRole);
    metadata[QStringLiteral("xesam:url")] = m_playListModel->data(trackIndex, MediaPlayList::ResourceRole).toString();
    metadata[QStringLiteral("xesam:album")] = m_playListModel->data(trackIndex, MediaPlayList::AlbumRole);
    metadata[QStringLiteral("xesam:artist")] = {m_playListModel->data(trackIndex, MediaPlayList::ArtistRole)};
    metadata[QStringLiteral("mpris:artUrl")] = m_playListModel->data(trackIndex, MediaPlayList::ImageRole).toString();

    return metadata;
}

void MediaPlayer2Tracklist::rowsInsertedInModel(const QModelIndex &parentModel, int start, int end)
{
    Q_UNUSED(parentModel);

    m_trackIndexFromURL.clear();
    for (int i = 0; i < m_playListModel->rowCount(); i++) {
        m_trackIndexFromURL[m_playListModel->data(m_playListModel->index(i, 0), MediaPlayList::ResourceRole).toString()] = i;
    }

    QVariantMap metadata;
    QDBusObjectPath afterTrack;
    for (int i = start; i <= end; i++) {
        m_orderedTrackIds.insert(i, QDBusObjectPath(playlistTidPrefix + QString::number(tidCounter++)));
        metadata = getMetadataOf(urlOfIndex(i), m_orderedTrackIds[i].path());
        afterTrack = (i > 0) ? m_orderedTrackIds.at(i-1) : mprisNoTrack;
        emit TrackAdded(metadata, afterTrack);
    }
}

void MediaPlayer2Tracklist::rowsRemovedFromModel(const QModelIndex &parentModel, int start, int end)
{
    Q_UNUSED(parentModel);

    m_trackIndexFromURL.clear();
    for (int i = 0; i < m_playListModel->rowCount(); i++) {
        m_trackIndexFromURL[m_playListModel->data(m_playListModel->index(i, 0), MediaPlayList::ResourceRole).toString()] = i;
    }

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

    m_trackIndexFromURL.clear();
    for (int i = 0; i < m_playListModel->rowCount(); i++) {
        m_trackIndexFromURL[m_playListModel->data(m_playListModel->index(i, 0), MediaPlayList::ResourceRole).toString()] = i;
    }
}

void MediaPlayer2Tracklist::resetTrackIds()
{
    m_orderedTrackIds.clear();
    m_trackIndexFromURL.clear();
    for (int i = 0; i < m_playListModel->rowCount(); i++) {
        m_orderedTrackIds << QDBusObjectPath(playlistTidPrefix + QString::number(tidCounter++));
        m_trackIndexFromURL[m_playListModel->data(m_playListModel->index(i, 0), MediaPlayList::ResourceRole).toString()] = i;
    }

    emit TrackListReplaced(Tracks(), currentDBusTrackId());
}

QList<QVariantMap> MediaPlayer2Tracklist::GetTracksMetadata(const QList<QDBusObjectPath> &trackIds)
{
    QList<QVariantMap> metadataList;
    int index;

    Q_FOREACH (const QDBusObjectPath& trackId, trackIds) {
        index = m_orderedTrackIds.indexOf(trackId);
        if (index != -1) {
            metadataList << getMetadataOf(urlOfIndex(index), trackId.path());
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
        //m_playListModel->setCurrentIndex(newIndex);
    }
}

QString MediaPlayer2Tracklist::urlOfIndex(int index) const
{
    return m_playListModel->data(m_playListModel->index(index, 0),
                                 MediaPlayList::ResourceRole).toString();
}

int MediaPlayer2Tracklist::indexOfURL(const QString &url)
{
    if (m_playListModel->rowCount() != m_trackIndexFromURL.size()) {
        resetTrackIds();
    }

    auto urlIndex = m_trackIndexFromURL.find(url);

    if (urlIndex == m_trackIndexFromURL.end()) {
        return -1;
    }

    return urlIndex.value();
}
