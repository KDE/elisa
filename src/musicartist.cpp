/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "musicartist.h"

#include <QtGlobal>
#include <QString>
#include <QStringList>
#include <QSharedData>

#include <QDebug>

class MusicArtistPrivate : public QSharedData
{
public:

    QString mName;

    QStringList mGenres;

    qulonglong mDatabaseId = 0;

    int mAlbumsCount = 0;

    bool mIsValid = false;

};

MusicArtist::MusicArtist() : d(new MusicArtistPrivate())
{
}

MusicArtist::MusicArtist(MusicArtist &&other) noexcept = default;

MusicArtist::MusicArtist(const MusicArtist &other) = default;

MusicArtist& MusicArtist::operator=(MusicArtist &&other) noexcept = default;

MusicArtist &MusicArtist::operator=(const MusicArtist &other) = default;

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

void MusicArtist::setGenres(const QStringList &aGenres)
{
    d->mGenres = aGenres;
}

QStringList MusicArtist::genres() const
{
    return d->mGenres;
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
    stream << data.name() << data.genres() << data.databaseId() << data.albumsCount() << (data.isValid() ? "is valid" : "is invalid");

    return stream;
}

bool operator==(const MusicArtist &artist1, const MusicArtist &artist2)
{
    return artist1.name() == artist2.name() && artist1.genres() == artist2.genres() && artist1.albumsCount() == artist2.albumsCount();
}
