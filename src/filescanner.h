/*
 * Copyright 2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef FILESCANNER_H
#define FILESCANNER_H

#include "musicaudiotrack.h"

#include <QUrl>
#include <QMimeDatabase>
#include <QObject>

#include <memory>

namespace KFileMetaData {

class ExtractorCollection;

}

namespace Baloo {

class File;

}

class FileScannerPrivate;

class FileScanner
{
public:

    FileScanner();

    virtual ~FileScanner();

    MusicAudioTrack scanOneFile(const QUrl &scanFile, const QMimeDatabase &mimeDatabase);

    void scanProperties(const Baloo::File &match, MusicAudioTrack &trackData);

    void scanProperties(const QString &localFileName, MusicAudioTrack &trackData);

private:

    std::unique_ptr<FileScannerPrivate> d;

};

#endif // FILESCANNER_H
