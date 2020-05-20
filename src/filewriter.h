/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2019 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef FILEWRITER_H
#define FILEWRITER_H

#include "elisaLib_export.h"
#include "datatypes.h"

#include <memory>

class FileWriterPrivate;

class ELISALIB_EXPORT FileWriter
{
public:

    FileWriter();

    ~FileWriter();

    bool writeSingleMetaDataToFile(const QUrl &url, const DataTypes::ColumnsRoles role, const QVariant &data);

    bool writeAllMetaDataToFile(const QUrl &url, const DataTypes::TrackDataType &data);

private:

    std::unique_ptr<FileWriterPrivate> d;

};

#endif // FILEWRITER_H
