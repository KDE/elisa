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

#ifndef MEDIAPLAYER2TRACKLIST_H
#define MEDIAPLAYER2TRACKLIST_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>
#include <QSharedPointer>
#include <QDBusMetaType>
#include <QStringList>
#include <QVariantMap>

Q_DECLARE_METATYPE(QList<QVariantMap>)

class QAbstractItemModel;

class MediaPlayer2Tracklist : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2.TrackList") // Docs: http://specifications.freedesktop.org/mpris-spec/latest/Track_List_Interface.html

    Q_PROPERTY(QList<QDBusObjectPath> Tracks READ Tracks)
    Q_PROPERTY(bool CanEditTracks READ CanEditTracks)

public:
    explicit MediaPlayer2Tracklist(QAbstractItemModel *playListModel, QObject* parent = 0);
    ~MediaPlayer2Tracklist();

    bool CanEditTracks() const;
    QList<QDBusObjectPath> Tracks() const;

    int currentIndex() const;
    QDBusObjectPath currentTrackId() const;

private Q_SLOTS:
    void rowsInsertedInModel(const QModelIndex &parentModel, int start, int end);
    void rowsRemovedFromModel(const QModelIndex &parentModel, int start, int end);
    void rowsMovedInModel(const QModelIndex &sourceParent, int sourceStart, int sourceEnd,
                          const QModelIndex &destinationParent, int movedBeforeRow);
    void resetTrackIds();

public Q_SLOTS:
    QList<QVariantMap> GetTracksMetadata(const QList<QDBusObjectPath>& trackIds);
    void AddTrack(const QString& uri, const QDBusObjectPath& afterTrack, bool setAsCurrent);
    void RemoveTrack(const QDBusObjectPath& trackId);
    void GoTo(const QDBusObjectPath& trackId);

Q_SIGNALS:
    void TrackListReplaced(QList<QDBusObjectPath> newTracks, QDBusObjectPath currentTrack) const;
    void TrackAdded(QVariantMap metadata, QDBusObjectPath afterTrack);
    void TrackRemoved(QDBusObjectPath trackId);
    void TrackMetadataChanged(QDBusObjectPath trackId, QVariantMap metadata);

private:
    QString urlOfIndex(int index) const;

    QList<QDBusObjectPath> m_orderedTrackIds;

    int tidCounter = 0;

    QAbstractItemModel* m_playListModel;
};

#endif // MEDIAPLAYER2TRACKLIST_H
