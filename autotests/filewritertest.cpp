/*
   SPDX-FileCopyrightText: 2019 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
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
