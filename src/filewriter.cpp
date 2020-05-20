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

#include "filewriter.h"

#include "config-upnp-qt.h"
#include <QMimeDatabase>

#if defined KF5FileMetaData_FOUND && KF5FileMetaData_FOUND

#include <KFileMetaData/WriterCollection>
#include <KFileMetaData/Writer>
#include <KFileMetaData/UserMetaData>
#include <KFileMetaData/Properties>
#include <KFileMetaData/WriteData>
#include <QHash>

#endif

class FileWriterPrivate
{
public:

#if defined KF5FileMetaData_FOUND && KF5FileMetaData_FOUND
    KFileMetaData::WriterCollection mAllWriters;

    const QHash<DataTypes::ColumnsRoles, KFileMetaData::Property::Property> mPropertyTranslation = {
        {DataTypes::ColumnsRoles::ArtistRole, KFileMetaData::Property::Artist},
        {DataTypes::ColumnsRoles::AlbumArtistRole, KFileMetaData::Property::AlbumArtist},
        {DataTypes::ColumnsRoles::GenreRole, KFileMetaData::Property::Genre},
        {DataTypes::ColumnsRoles::ComposerRole, KFileMetaData::Property::Composer},
        {DataTypes::ColumnsRoles::LyricistRole, KFileMetaData::Property::Lyricist},
        {DataTypes::ColumnsRoles::TitleRole, KFileMetaData::Property::Title},
        {DataTypes::ColumnsRoles::AlbumRole, KFileMetaData::Property::Album},
        {DataTypes::ColumnsRoles::TrackNumberRole, KFileMetaData::Property::TrackNumber},
        {DataTypes::ColumnsRoles::DiscNumberRole, KFileMetaData::Property::DiscNumber},
        {DataTypes::ColumnsRoles::YearRole, KFileMetaData::Property::ReleaseYear},
        {DataTypes::ColumnsRoles::LyricsRole, KFileMetaData::Property::Lyrics},
        {DataTypes::ColumnsRoles::CommentRole, KFileMetaData::Property::Comment},
        {DataTypes::ColumnsRoles::RatingRole, KFileMetaData::Property::Rating},
    };
#endif

    QMimeDatabase mMimeDb;

};

FileWriter::FileWriter() : d(std::make_unique<FileWriterPrivate>())
{
}

FileWriter::~FileWriter() = default;

bool FileWriter::writeSingleMetaDataToFile(const QUrl &url, const DataTypes::ColumnsRoles role, const QVariant &data)
{
#if defined KF5FileMetaData_FOUND && KF5FileMetaData_FOUND

    if (!url.isLocalFile()) {
        return false;
    }
    const auto &localFileName = url.toLocalFile();
    const auto &fileMimeType = d->mMimeDb.mimeTypeForFile(localFileName);
    if (!fileMimeType.name().startsWith(QStringLiteral("audio/"))) {
        return false;
    }

    const auto &mimetype = fileMimeType.name();
    const QList<KFileMetaData::Writer*> &writerList = d->mAllWriters.fetchWriters(mimetype);

    if (writerList.isEmpty()) {
        return false;
    }
    KFileMetaData::Writer *writer = writerList.first();
    KFileMetaData::WriteData writeData(localFileName, mimetype);
    auto translatedKey = d->mPropertyTranslation.find(role);
    if (translatedKey != d->mPropertyTranslation.end()) {
        writeData.add(translatedKey.value(), data);
    }
    writer->write(writeData);

#if !defined Q_OS_ANDROID && !defined Q_OS_WIN
    auto fileData = KFileMetaData::UserMetaData(localFileName);

    if (role == DataTypes::RatingRole) {
        fileData.setRating(data.toInt());
    }

    if (role == DataTypes::CommentRole) {
        fileData.setUserComment(data.toString());
    }
#endif

    return true;
#else
    Q_UNUSED(url)
    Q_UNUSED(role)
    Q_UNUSED(data)

    return false;
#endif
}

bool FileWriter::writeAllMetaDataToFile(const QUrl &url, const DataTypes::TrackDataType &data)
{
#if defined KF5FileMetaData_FOUND && KF5FileMetaData_FOUND

    if (!url.isLocalFile()) {
        return false;
    }
    const auto &localFileName = url.toLocalFile();
    const auto &fileMimeType = d->mMimeDb.mimeTypeForFile(localFileName);
    if (!fileMimeType.name().startsWith(QLatin1String("audio/"))) {
        return false;
    }

    KFileMetaData::UserMetaData md(localFileName);
    md.setUserComment(data.value(DataTypes::ColumnsRoles::CommentRole).toString());
    md.setRating(data.value(DataTypes::ColumnsRoles::RatingRole).toInt());

    const auto &mimetype = fileMimeType.name();
    const QList<KFileMetaData::Writer*> &writerList = d->mAllWriters.fetchWriters(mimetype);

    if (writerList.isEmpty()) {
        return false;
    }

    KFileMetaData::Writer *writer = writerList.first();
    KFileMetaData::WriteData writeData(localFileName, mimetype);
    auto rangeBegin = data.constKeyValueBegin();
    while (rangeBegin != data.constKeyValueEnd()) {
        auto key = (*rangeBegin).first;
        auto translatedKey = d->mPropertyTranslation.find(key);
        if (translatedKey != d->mPropertyTranslation.end()) {
            writeData.add(translatedKey.value(), (*rangeBegin).second);
        }
        rangeBegin++;
    }
    writer->write(writeData);

    return true;
#else
    Q_UNUSED(url)
    Q_UNUSED(data)

    return false;
#endif
}

