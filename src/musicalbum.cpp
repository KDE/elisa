/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "musicalbum.h"

#include <algorithm>

#include <QList>
#include <QString>
#include <QUrl>
#include <QMap>

#include <QDebug>

class MusicAlbumPrivate
{
public:

    qulonglong mDatabaseId = 0;

    QString mId;

    QString mParentId;

    QString mTitle;

    QString mArtist;

    QUrl mAlbumArtURI;

    QUrl mResourceURI;

    QMap<qulonglong, MusicAudioTrack> mTracks;

    QList<qulonglong> mTrackIds;

    int mTracksCount = 0;

    bool mIsValid = false;

    bool mIsSingleDiscAlbum = true;

};

MusicAlbum::MusicAlbum() : d(new MusicAlbumPrivate)
{
}

MusicAlbum::MusicAlbum(MusicAlbum &&other) : d(other.d)
{
    other.d = nullptr;
}

MusicAlbum::MusicAlbum(const MusicAlbum &other) : d(new MusicAlbumPrivate(*other.d))
{
}

MusicAlbum& MusicAlbum::operator=(MusicAlbum &&other)
{
    if (&other != this) {
        delete d;
        d = other.d;
        other.d = nullptr;
    }

    return *this;
}

MusicAlbum& MusicAlbum::operator=(const MusicAlbum &other)
{
    if (&other != this) {
        (*d) = (*other.d);
    }

    return *this;
}

MusicAlbum::~MusicAlbum()
{
    delete d;
}

void MusicAlbum::setValid(bool value)
{
    d->mIsValid = value;
}

bool MusicAlbum::isValid() const
{
    return d->mIsValid;
}

void MusicAlbum::setDatabaseId(qulonglong value)
{
    d->mDatabaseId = value;
}

qulonglong MusicAlbum::databaseId() const
{
    return d->mDatabaseId;
}

void MusicAlbum::setId(const QString &value)
{
    d->mId = value;
}

QString MusicAlbum::id() const
{
    return d->mId;
}

void MusicAlbum::setParentId(const QString &value)
{
    d->mParentId = value;
}

QString MusicAlbum::parentId() const
{
    return d->mParentId;
}

void MusicAlbum::setTracksCount(int count)
{
    d->mTracksCount = count;
}

int MusicAlbum::tracksCount() const
{
    const auto realTracksCount = d->mTracks.size();
    return (realTracksCount == 0 ? d->mTracksCount : realTracksCount);
}

void MusicAlbum::setTitle(const QString &value)
{
    d->mTitle = value;
}

QString MusicAlbum::title() const
{
    return d->mTitle;
}

void MusicAlbum::setArtist(const QString &value)
{
    d->mArtist = value;
}

const QString &MusicAlbum::artist() const
{
    return d->mArtist;
}

void MusicAlbum::setAlbumArtURI(const QUrl &value)
{
    d->mAlbumArtURI = value;
}

QUrl MusicAlbum::albumArtURI() const
{
    return d->mAlbumArtURI;
}

void MusicAlbum::setResourceURI(const QUrl &value)
{
    d->mResourceURI = value;
}

QUrl MusicAlbum::resourceURI() const
{
    return d->mResourceURI;
}

void MusicAlbum::setIsSingleDiscAlbum(bool value)
{
    d->mIsSingleDiscAlbum = value;
}

bool MusicAlbum::isSingleDiscAlbum() const
{
    return d->mIsSingleDiscAlbum;
}

void MusicAlbum::setTracks(const QMap<qulonglong, MusicAudioTrack> &allTracks)
{
    d->mTracks = allTracks;
}

QList<qulonglong> MusicAlbum::tracksKeys() const
{
    return d->mTracks.keys();
}

MusicAudioTrack MusicAlbum::trackFromIndex(int index) const
{
    return d->mTracks[d->mTrackIds[index]];
}

void MusicAlbum::setTrackIds(const QList<qulonglong> &allTracksIds)
{
    d->mTrackIds = allTracksIds;
}

qulonglong MusicAlbum::trackIdFromIndex(int index) const
{
    return d->mTrackIds[index];
}

int MusicAlbum::trackIndexFromId(qulonglong id) const
{
    int result = -1;

    auto itTrack = std::find(d->mTrackIds.begin(), d->mTrackIds.end(), id);

    if (itTrack == d->mTrackIds.end()) {
        return result;
    }

    result = itTrack - d->mTrackIds.begin();

    return result;
}

QStringList MusicAlbum::allArtists() const
{
    auto result = QList<QString>();

    for (const auto &oneTrack : d->mTracks) {
        result.push_back(oneTrack.artist());
    }

    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());

    return result;
}

QStringList MusicAlbum::allTracksTitle() const
{
    auto result = QList<QString>();

    for (const auto &oneTrack : d->mTracks) {
        result.push_back(oneTrack.title());
    }

    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());

    return result;
}

bool MusicAlbum::isEmpty() const
{
    return d->mTrackIds.isEmpty();
}

void MusicAlbum::removeTrackFromIndex(int index)
{
    if (index < 0 || index >= tracksCount()) {
        return;
    }

    --d->mTracksCount;
    d->mTracks.remove(d->mTrackIds.at(index));
    d->mTrackIds.removeAt(index);
}

void MusicAlbum::insertTrack(MusicAudioTrack newTrack, int index)
{
    d->mTrackIds.insert(index, newTrack.databaseId());
    d->mTracks[newTrack.databaseId()] = newTrack;
    ++d->mTracksCount;
}

void MusicAlbum::updateTrack(MusicAudioTrack modifiedTrack, int index)
{
    d->mTracks[d->mTrackIds[index]] = modifiedTrack;
}

QDebug& operator<<(QDebug &stream, const MusicAlbum &data)
{
    stream << data.title() << " " << data.artist();

    return stream;
}

bool operator==(const MusicAlbum &album1, const MusicAlbum &album2)
{
    return album1.artist() == album2.artist() && album1.title() == album2.title();
}
