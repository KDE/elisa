/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
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
