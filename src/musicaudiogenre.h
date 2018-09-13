/*
 * Copyright 2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef MUSICAUDIOGENRE_H
#define MUSICAUDIOGENRE_H

#include "elisaLib_export.h"

#include <QString>
#include <QMetaType>

#include <memory>

class MusicAudioGenrePrivate;
class QDebug;

class ELISALIB_EXPORT MusicAudioGenre
{

public:

    MusicAudioGenre();

    MusicAudioGenre(MusicAudioGenre &&other);

    MusicAudioGenre(const MusicAudioGenre &other);

    MusicAudioGenre& operator=(MusicAudioGenre &&other);

    MusicAudioGenre& operator=(const MusicAudioGenre &other);

    ~MusicAudioGenre();

    void setName(const QString &aName);

    QString name() const;

    void setDatabaseId(qulonglong value);

    qulonglong databaseId() const;

private:

    std::unique_ptr<MusicAudioGenrePrivate> d;

};

QDebug& operator<<(QDebug &stream, const MusicAudioGenre &data);

bool operator==(const MusicAudioGenre &genre1, const MusicAudioGenre &genre2);

Q_DECLARE_METATYPE(MusicAudioGenre)

#endif // MUSICAUDIOGENRE_H
