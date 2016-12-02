#include "musicartist.h"

#include <QtCore/QtGlobal>
#include <QtCore/QString>

class MusicArtistPrivate
{
public:

    QString mName;

    qulonglong mDatabaseId;

    int mAlbumsCount = 0;

    bool mIsValid = false;

};

MusicArtist::MusicArtist() : d(new MusicArtistPrivate)
{
}

MusicArtist::MusicArtist(MusicArtist &&other) : d(other.d)
{
    other.d = nullptr;
}

MusicArtist::MusicArtist(const MusicArtist &other) : d(new MusicArtistPrivate(*other.d))
{
}

MusicArtist& MusicArtist::operator=(MusicArtist &&other)
{
    if (&other != this) {
        delete d;
        d = other.d;
        other.d = nullptr;
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
{
    delete d;
}

void MusicArtist::setValid(bool value)
{
    d->mIsValid = value;
}

bool MusicArtist::isValid() const
{
    return d->mIsValid;
}

void MusicArtist::setName(QString aName)
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
