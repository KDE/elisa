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

#ifndef FILESCANNER_H
#define FILESCANNER_H

#include "elisaLib_export.h"

#include "datatypes.h"

#include <QUrl>

#include <memory>

namespace Baloo {

class File;

}

class FileScannerPrivate;

class ELISALIB_EXPORT FileScanner
{
public:

    FileScanner();

    virtual ~FileScanner();

    bool shouldScanFile(const QString &scanFile);

    DataTypes::TrackDataType scanOneFile(const QUrl &scanFile);

    void scanProperties(const Baloo::File &match, DataTypes::TrackDataType &trackData);

    void scanProperties(const QString &localFileName, DataTypes::TrackDataType &trackData);

    QUrl searchForCoverFile(const QString &localFileName);

    bool checkEmbeddedCoverImage(const QString &localFileName);

private:

    std::unique_ptr<FileScannerPrivate> d;

};

#endif // FILESCANNER_H
