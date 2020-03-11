/*
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

#include "filescanner.h"
#include "filewriter.h"
#include "config-upnp-qt.h"

#include <QObject>
#include <QList>
#include <QUrl>
#include <QFile>

#include <QtTest>

class FileWriterTest: public QObject
{
    Q_OBJECT

public:

    explicit FileWriterTest(QObject *aParent = nullptr) : QObject(aParent)
    {
    }

private Q_SLOTS:

    void initTestCase()
    {

    }

    void testFileAllMetaDataWrite()
    {
        const auto testFileName = QStringLiteral("writerTest.ogg");
        const auto testFileUrl = QUrl::fromLocalFile(testFileName);
        QFile::copy(QStringLiteral(LOCAL_FILE_TESTS_SAMPLE_FILES_PATH) + QStringLiteral("/music/test.ogg"), testFileName);

        FileWriter fileWriter;
        FileScanner fileScanner;
        auto scannedTrackBefore = fileScanner.scanOneFile(testFileUrl);
        QCOMPARE(scannedTrackBefore.title(), QStringLiteral("Title"));
        QCOMPARE(scannedTrackBefore.genre(), QStringLiteral("Genre"));
        QCOMPARE(scannedTrackBefore.album(), QStringLiteral("Test"));
        QCOMPARE(scannedTrackBefore.artist(), QStringLiteral("Artist"));
        QCOMPARE(scannedTrackBefore.year(), 2015);

        fileWriter.writeAllMetaDataToFile(testFileUrl, {{DataTypes::AlbumRole, QStringLiteral("testAlbum")},
                                                        {DataTypes::GenreRole, QStringLiteral("testGenre")},
                                                        {DataTypes::YearRole, 1999},
                                                        {DataTypes::TitleRole, QStringLiteral("testTitle")},
                                                        {DataTypes::ArtistRole, QStringLiteral("testArtist")}});
        auto scannedTrackAfter = fileScanner.scanOneFile(testFileUrl);
        QCOMPARE(scannedTrackAfter.title(), QStringLiteral("testTitle"));
        QCOMPARE(scannedTrackAfter.genre(), QStringLiteral("testGenre"));
        QCOMPARE(scannedTrackAfter.album(), QStringLiteral("testAlbum"));
        QCOMPARE(scannedTrackAfter.artist(), QStringLiteral("testArtist"));
        QCOMPARE(scannedTrackAfter.year(), 1999);

        QFile::remove(testFileName);
    }

    void testFileSingleMetaDataWrite()
    {
        const auto testFileName = QStringLiteral("writerTest.ogg");
        const auto testFileUrl = QUrl::fromLocalFile(testFileName);
        QFile::copy(QStringLiteral(LOCAL_FILE_TESTS_SAMPLE_FILES_PATH) + QStringLiteral("/music/test.ogg"), testFileName);

        FileWriter fileWriter;
        FileScanner fileScanner;
        auto scannedTrackBefore = fileScanner.scanOneFile(testFileUrl);
        QCOMPARE(scannedTrackBefore.title(), QStringLiteral("Title"));
        QCOMPARE(scannedTrackBefore.genre(), QStringLiteral("Genre"));
        QCOMPARE(scannedTrackBefore.album(), QStringLiteral("Test"));
        QCOMPARE(scannedTrackBefore.artist(), QStringLiteral("Artist"));
        QCOMPARE(scannedTrackBefore.year(), 2015);

        fileWriter.writeSingleMetaDataToFile(testFileUrl, DataTypes::AlbumRole, QStringLiteral("testAlbum"));
        auto scannedTrackAfter = fileScanner.scanOneFile(testFileUrl);
        QCOMPARE(scannedTrackAfter.title(), QStringLiteral("Title"));
        QCOMPARE(scannedTrackAfter.genre(), QStringLiteral("Genre"));
        QCOMPARE(scannedTrackAfter.album(), QStringLiteral("testAlbum"));
        QCOMPARE(scannedTrackAfter.artist(), QStringLiteral("Artist"));
        QCOMPARE(scannedTrackAfter.year(), 2015);

        QFile::remove(testFileName);
    }
};

QTEST_GUILESS_MAIN(FileWriterTest)


#include "filewritertest.moc"
