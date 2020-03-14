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

#include <memory>

class QFileInfo;
class QUrl;
class FileScannerPrivate;

class ELISALIB_EXPORT FileScanner
{
public:

    FileScanner();

    ~FileScanner();

    bool shouldScanFile(const QString &scanFile);

    DataTypes::TrackDataType scanOneFile(const QUrl &scanFile);

    DataTypes::TrackDataType scanOneFile(const QUrl &scanFile, const QFileInfo &scanFileInfo);

    DataTypes::TrackDataType scanOneBalooFile(const QUrl &scanFile, const QFileInfo &scanFileInfo);

    QUrl searchForCoverFile(const QString &localFileName);

private:

    void scanProperties(const QString &localFileName, DataTypes::TrackDataType &trackData);

    bool checkEmbeddedCoverImage(const QString &localFileName);

    std::unique_ptr<FileScannerPrivate> d;

};

#endif // FILESCANNER_H
