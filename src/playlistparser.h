/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2019 (c) Alexander Stippich <a.stippich@gmx.net>
   SPDX-FileCopyrightText: 2020 (c) Carson Black <uhhadd@gmail.com>
   SPDX-FileCopyrightText: 2026 (c) Dina Sakharova <d.sakharova@notquiteloona.dev>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#pragma once
#include "mediaplaylist.h"
#include <QList>
#include <QMimeDatabase>
#include <qmimetype.h>

class PlaylistModel
{
public:
    QString name()
    {
        return mName;
    }
    QString description()
    {
        return mDescription;
    }
    QString author()
    {
        return mAuthor;
    }
    QUrl imageUrl()
    {
        return mImageUrl;
    }
    QList<MediaPlayListEntry> tracks;

    explicit PlaylistModel(const QList<MediaPlayListEntry> &contents,
                           const QString &name = QString(),
                           const QString &description = QString(),
                           const QString &author = QString(),
                           const QUrl &imageUrl = QUrl())
    {
        mName = name;
        mDescription = description;
        mAuthor = author;
        mImageUrl = imageUrl;
        tracks = contents;
    }

private:
    QString mName;
    QString mDescription;
    QString mAuthor;
    QUrl mImageUrl;
};

class PlaylistParserBackend
{
public:
    virtual ~PlaylistParserBackend() = default;
    virtual std::optional<PlaylistModel> read(QTextStream *stream) = 0;
    virtual bool write(QTextStream *stream, const PlaylistModel *playlist) = 0;
};

class ELISALIB_EXPORT PlaylistParser
{
public:
    static std::optional<PlaylistModel> Load(const QUrl &path);
    static bool Save(const QUrl &path, const PlaylistModel &playlist);

private:
    inline static QMap<QString, PlaylistParserBackend *> backends;
    inline static QMimeDatabase mimeDb;
    static PlaylistParserBackend *GetBackendForType(const QMimeType &type);
};

class PlsPlaylistLoader : public PlaylistParserBackend
{
public:
    std::optional<PlaylistModel> read(QTextStream *stream) override;
    bool write(QTextStream *stream, const PlaylistModel *playlist) override;
};

class M3uPlaylistLoader : public PlaylistParserBackend
{
public:
    std::optional<PlaylistModel> read(QTextStream *stream) override;
    bool write(QTextStream *stream, const PlaylistModel *playlist) override;
};
