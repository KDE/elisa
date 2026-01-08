/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2019 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "filewriter.h"

#include "trackmetadatamodel.h"

#include "config-upnp-qt.h"
#include <QMimeDatabase>

#if KFFileMetaData_FOUND

#include <KFileMetaData/MimeUtils>
#include <KFileMetaData/UserMetaData>
#include <KFileMetaData/WriteData>
#include <KFileMetaData/Writer>
#include <KFileMetaData/WriterCollection>
#include <QHash>

#endif

class FileWriterPrivate
{
public:

#if KFFileMetaData_FOUND
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
#if KFFileMetaData_FOUND

    if (!url.isLocalFile()) {
        return false;
    }
    const auto &localFileName = url.toLocalFile();
    const auto fileMimeType = KFileMetaData::MimeUtils::strictMimeType(localFileName, d->mMimeDb);
    const auto mimetype = fileMimeType.name();
    if (!mimetype.startsWith(QStringLiteral("audio/"))) {
        return false;
    }

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
#if KFFileMetaData_FOUND

    if (!url.isLocalFile()) {
        return false;
    }
    const auto &localFileName = url.toLocalFile();
    const auto fileMimeType = KFileMetaData::MimeUtils::strictMimeType(localFileName, d->mMimeDb);
    const auto mimetype = fileMimeType.name();
    if (!mimetype.startsWith(QLatin1String("audio/"))) {
        return false;
    }

    KFileMetaData::UserMetaData md(localFileName);
    md.setUserComment(data.value(DataTypes::ColumnsRoles::CommentRole).toString());
    md.setRating(data.value(DataTypes::ColumnsRoles::RatingRole).toInt());

    const QList<KFileMetaData::Writer*> &writerList = d->mAllWriters.fetchWriters(mimetype);

    if (writerList.isEmpty()) {
        return false;
    }

    KFileMetaData::Writer *writer = writerList.first();
    KFileMetaData::WriteData writeData(localFileName, mimetype);
    auto rangeBegin = data.constKeyValueBegin();
    while (rangeBegin != data.constKeyValueEnd()) {
        auto key = (*rangeBegin).first;
        auto value = (*rangeBegin).second;
        if (key == DataTypes::ColumnsRoles::LyricsLocationRole) {
            rangeBegin++;
            continue;
        }
        if (key == DataTypes::ColumnsRoles::LyricsRole) {
            if (!data.value(DataTypes::ColumnsRoles::LyricsLocationRole).toString().isEmpty()) {
                rangeBegin++;
                continue;
            }
        }
        auto translatedKey = d->mPropertyTranslation.find(key);
        if (translatedKey != d->mPropertyTranslation.end()) {
            writeData.add(translatedKey.value(), value);
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

