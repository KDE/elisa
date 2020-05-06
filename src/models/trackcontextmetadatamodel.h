/*
   SPDX-FileCopyrightText: 2019 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef TRACKCONTEXTMETADATAMODEL_H
#define TRACKCONTEXTMETADATAMODEL_H

#include "elisaLib_export.h"

#include "trackmetadatamodel.h"

class ELISALIB_EXPORT TrackContextMetaDataModel : public TrackMetadataModel
{

    Q_OBJECT

public:

    explicit TrackContextMetaDataModel(QObject *parent = nullptr);

protected:

    void filterDataFromTrackData() override;

    void fillLyricsDataFromTrack() override;

};

#endif // TRACKCONTEXTMETADATAMODEL_H
