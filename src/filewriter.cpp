/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2019 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "filewriter.h"

#include "config-upnp-qt.h"
#include <QMimeDatabase>

#if KFFileMetaData_FOUND

#include <KFileMetaData/WriterCollection>
#include <KFileMetaData/Writer>
#include <KFileMetaData/UserMetaData>
#include <KFileMetaData/WriteData>
#include <QHash>
#include <QFile>

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

bool FileWriter::writeLyricsToLyricsFile(const DataTypes::LyricsData lyrics)
{
    QFile file(lyrics.filePath());
    bool succeed = false;
    if (file.exists() && file.open(QFile::WriteOnly)) {
        QByteArray lyricsData = lyrics.lyrics().toUtf8();
        qint64 len = file.write(lyricsData);
        if (len == lyricsData.length()) {
            succeed = true;
        }
    }
    return succeed;
}

bool FileWriter::writeSingleMetaDataToFile(const QUrl &url, const DataTypes::ColumnsRoles role, const QVariant &data)
{
    QVariant dataCopy;
    // if lyrics is from local file, write changes into local file and return early, otherwise write to metadata
    if (role == DataTypes::LyricsRole) {
        DataTypes::LyricsData lyrics = data.value<DataTypes::LyricsData>();
        if (lyrics.fromMetaData()) {
            dataCopy.setValue(lyrics.lyrics());
        } else {
            return writeLyricsToLyricsFile(lyrics);
        }
    }
#if KFFileMetaData_FOUND

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
        writeData.add(translatedKey.value(), dataCopy);
    }
    writer->write(writeData);

#if !defined Q_OS_ANDROID && !defined Q_OS_WIN
    auto fileData = KFileMetaData::UserMetaData(localFileName);

    if (role == DataTypes::RatingRole) {
        fileData.setRating(dataCopy.toInt());
    }

    if (role == DataTypes::CommentRole) {
        fileData.setUserComment(dataCopy.toString());
    }
#endif

    return true;
#else
    Q_UNUSED(url)
    Q_UNUSED(role)
    Q_UNUSED(data)
    Q_UNUSED(dataCopy)

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
            if (key == DataTypes::LyricsRole) {
                DataTypes::LyricsData lyrics = (*rangeBegin).second.value<DataTypes::LyricsData>();
                if (lyrics.fromMetaData()) {
                    writeData.add(translatedKey.value(), lyrics.lyrics());
                } else {
                    writeLyricsToLyricsFile(lyrics);
                }
            } else {
                writeData.add(translatedKey.value(), (*rangeBegin).second);
            }
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

