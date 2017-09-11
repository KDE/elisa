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

#include "musicartist.h"

#include <QtGlobal>
#include <QString>
#include <QDebug>

class MusicArtistPrivate
{
public:

    QString mName;

    qulonglong mDatabaseId;

    int mAlbumsCount = 0;

    bool mIsValid = false;

};

MusicArtist::MusicArtist() : d(std::make_unique<MusicArtistPrivate>())
{
}

MusicArtist::MusicArtist(MusicArtist &&other)
{
    d.swap(other.d);
}

MusicArtist::MusicArtist(const MusicArtist &other) : d(std::make_unique<MusicArtistPrivate>(*other.d))
{
}

MusicArtist& MusicArtist::operator=(MusicArtist &&other)
{
    if (&other != this) {
        d.reset();
        d.swap(other.d);
    }

    return *this;
}

MusicArtist &MusicArtist::operator=(const MusicArtist &other)
{
    if (&other != this) {
        (*d) = (*other.d);
    }

    return *this;
}

MusicArtist::~MusicArtist()
= default;

void MusicArtist::setValid(bool value)
{
    d->mIsValid = value;
}

bool MusicArtist::isValid() const
{
    return d->mIsValid;
}

void MusicArtist::setName(const QString &aName)
{
    d->mName = aName;
}

QString MusicArtist::name() const
{
    return d->mName;
}

void MusicArtist::setDatabaseId(qulonglong value)
{
    d->mDatabaseId = value;
}

qulonglong MusicArtist::databaseId() const
{
    return d->mDatabaseId;
}

void MusicArtist::setAlbumsCount(int value)
{
    d->mAlbumsCount = value;
}

int MusicArtist::albumsCount() const
{
    return d->mAlbumsCount;
}

QDebug& operator<<(QDebug &stream, const MusicArtist &data)
{
    stream << data.name() << data.databaseId() << data.albumsCount() << (data.isValid() ? "is valid" : "is invalid");

    return stream;
}

bool operator==(const MusicArtist &artist1, const MusicArtist &artist2)
{
    return artist1.name() == artist2.name() && artist1.albumsCount() == artist2.albumsCount();
}
