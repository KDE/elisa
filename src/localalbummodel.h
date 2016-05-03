/*
 * Copyright 2015-2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef LOCALALBUMMODEL_H
#define LOCALALBUMMODEL_H

#include "abstractalbummodel.h"

class LocalAlbumModelPrivate;
class MusicStatistics;
class LocalBalooTrack;
class LocalBalooAlbum;

class LocalAlbumModel : public AbstractAlbumModel
{
    Q_OBJECT

public:

    explicit LocalAlbumModel(QObject *parent = 0);

    virtual ~LocalAlbumModel();

Q_SIGNALS:

public Q_SLOTS:

private Q_SLOTS:

private:

    LocalAlbumModelPrivate *d;
};

#endif // LOCALALBUMMODEL_H
