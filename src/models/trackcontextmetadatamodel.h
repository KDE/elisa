/*
 * Copyright 2019 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef TRACKCONTEXTMETADATAMODEL_H
#define TRACKCONTEXTMETADATAMODEL_H

#include "elisaLib_export.h"

#include "trackmetadatamodel.h"

class ELISALIB_EXPORT TrackContextMetaDataModel : public TrackMetadataModel
{

    Q_OBJECT

public:

    TrackContextMetaDataModel(QObject *parent = nullptr);

protected:

    void filterDataFromTrackData() override;

};

#endif // TRACKCONTEXTMETADATAMODEL_H
