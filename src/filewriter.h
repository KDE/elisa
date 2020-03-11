/*
 * Copyright 2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * Copyright 2019-2020 Alexander Stippich <a.stippich@gmx.net>
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

    bool writeSingleMetaDataToFile(const QUrl &url, const DataTypes::ColumnsRoles role, const QVariant data);

    bool writeAllMetaDataToFile(const QUrl &url, const DataTypes::TrackDataType &data);

private:

    std::unique_ptr<FileWriterPrivate> d;

};

#endif // FILEWRITER_H
