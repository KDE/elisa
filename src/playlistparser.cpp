/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2019 (c) Alexander Stippich <a.stippich@gmx.net>
   SPDX-FileCopyrightText: 2020 (c) Carson Black <uhhadd@gmail.com>
   SPDX-FileCopyrightText: 2026 (c) Dina Sakharova <d.sakharova@notquiteloona.dev>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "playlistparser.h"
#include "mediaplaylist.h"
#include <QFile>
#include <qdebug.h>
#include <qfiledevice.h>
#include <qmimetype.h>

namespace
{

std::optional<PlaylistModel> readPlaylist(PlaylistParser::Format format, QTextStream *stream)
{
    switch (format) {
    case PlaylistParser::Format::Pls:
        return PlsPlaylistLoader{}.read(stream);
    case PlaylistParser::Format::M3u:
        return M3uPlaylistLoader{}.read(stream);
    }
    return {};
}

bool writePlaylist(PlaylistParser::Format format, QTextStream *stream, const PlaylistModel *playlist)
{
    switch (format) {
    case PlaylistParser::Format::Pls:
        return PlsPlaylistLoader{}.write(stream, playlist);
    case PlaylistParser::Format::M3u:
        return M3uPlaylistLoader{}.write(stream, playlist);
    }
    return false;
}

}

std::optional<PlaylistParser::Format> PlaylistParser::FormatForType(const QMimeType &type)
{
    return ElisaUtils::playlistFormatForType(type);
}

// TODO: return something that allows to also return (as a string for example) exact full/partial error and errored tracks
std::optional<PlaylistModel> PlaylistParser::Load(const QUrl &path)
{
    const auto type = mimeDb.mimeTypeForFile(path.toLocalFile());
    const auto format = FormatForType(type);
    if (!format) {
        return {};
    }

    QFile file(path.toLocalFile());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {};
    }

    QTextStream stream(&file);
    return readPlaylist(*format, &stream);
}

bool PlaylistParser::Save(const QUrl &path, const PlaylistModel &playlist)
{
    const auto type = mimeDb.mimeTypeForFile(path.toLocalFile());
    const auto format = FormatForType(type);
    if (!format) {
        return false;
    }

    QFile file(path.toLocalFile());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream stream(&file);
    return writePlaylist(*format, &stream, &playlist);
}

std::optional<PlaylistModel> M3uPlaylistLoader::read(QTextStream *stream)
{
    stream->setEncoding(QStringConverter::System);

    QList<MediaPlayListEntry> result;

    while (!stream->atEnd()) {
        QString line = stream->readLine();
        if (line.isEmpty() || line.startsWith(QStringLiteral("#"))) {
            continue;
        }

        const QUrl url = line.contains(QStringLiteral("://")) ? QUrl(line) : QUrl::fromLocalFile(line);
        result.append(MediaPlayListEntry(url));
    }

    return PlaylistModel(result);
}

bool M3uPlaylistLoader::write(QTextStream *stream, const PlaylistModel *playlist)
{
    for (const auto &line : playlist->tracks) {
        *stream << line.mTrackUrl.toString() << QStringLiteral("\n");
    }

    return true;
}

std::optional<PlaylistModel> PlsPlaylistLoader::read(QTextStream *stream)
{
    QList<MediaPlayListEntry> result;

    while (!stream->atEnd()) {
        if (auto line = stream->readLine(); !line.isEmpty() && line.startsWith(QStringLiteral("File")) && line.contains(QStringLiteral("="))) {
            const int indexOfEquals = line.indexOf(QStringLiteral("="));
            QString urlAsString = line.mid(indexOfEquals + QStringLiteral("=").length());
            const QUrl &url = urlAsString.contains(QStringLiteral("://")) ? QUrl(urlAsString) : QUrl::fromLocalFile(urlAsString);
            result.append(MediaPlayListEntry(url));
        }
    }

    return PlaylistModel(result);
}

bool PlsPlaylistLoader::write(QTextStream *stream, const PlaylistModel *playlist)
{
    const auto listOfTracks = playlist->tracks;

    *stream << QStringLiteral(R"--([playlist]

Version=2
NumberOfEntries=%1
)--")
                   .arg(listOfTracks.count());

    // Sample:
    /*
        [playlist]

        Version=2
        NumberOfEntries=2

        File1=http://example.example:8068

        File2=example2.mp3
    */

    // PLS requires to denote track numbers starting with 1
    int counter = 1;
    for (const MediaPlayListEntry &entry : listOfTracks) {
        if (auto url = entry.mTrackUrl.toString(); !url.isEmpty()) {
            *stream << QStringLiteral("\nFile%1=%2\n").arg(counter).arg(url);
        }
        counter++;
    }

    return true;
}
