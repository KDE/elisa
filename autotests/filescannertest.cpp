/*
 * Copyright 2018 Alexander Stippich <a.stippich@gmx.net>
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

#include "musicaudiotrack.h"
#include "filescanner.h"
#include "config-upnp-qt.h"

#include <QObject>
#include <QList>
#include <QUrl>
#include <QDateTime>
#include <QTime>

#include <QDebug>

#include <QtTest>

class FileScannerTest: public QObject
{
    Q_OBJECT

public:

    QString createTrackUrl(QString subpath) {
        return QStringLiteral(LOCAL_FILE_TESTS_SAMPLE_FILES_PATH) + QStringLiteral("/cover_art") + subpath;
    }

    QList<QString> mTestTracksForDirectory = {
        createTrackUrl(QStringLiteral("/artist1/album1/not_existing.ogg")),
        createTrackUrl(QStringLiteral("/artist1/album2/not_existing.ogg")),
        createTrackUrl(QStringLiteral("/artist1/album3/not_existing.ogg")),
        createTrackUrl(QStringLiteral("/artist2/album1/not_existing.ogg")),
        createTrackUrl(QStringLiteral("/artist2/album2/not_existing.ogg")),
        createTrackUrl(QStringLiteral("/artist2/album3/not_existing.ogg")),
        createTrackUrl(QStringLiteral("/artist3/album1/not_existing.ogg")),
        createTrackUrl(QStringLiteral("/artist3/album2/not_existing.ogg")),
        createTrackUrl(QStringLiteral("/artist3/album3/not_existing.ogg")),
    };

    QList<QString> mTestTracksForMetaData = {
        createTrackUrl(QStringLiteral("/artist4/test.ogg")),
        createTrackUrl(QStringLiteral("/artist4/test.flac")),
        createTrackUrl(QStringLiteral("/artist4/test.mp3")),
    };

private Q_SLOTS:

    void initTestCase()
    {

    }

    void testFindCoverInDirectory()
    {
        FileScanner fileScanner;
        QVERIFY(!fileScanner.searchForCoverFile(mTestTracksForDirectory.at(0)).isEmpty());
        QVERIFY(!fileScanner.searchForCoverFile(mTestTracksForDirectory.at(1)).isEmpty());
        QVERIFY(!fileScanner.searchForCoverFile(mTestTracksForDirectory.at(2)).isEmpty());
        QVERIFY(!fileScanner.searchForCoverFile(mTestTracksForDirectory.at(3)).isEmpty());
        QVERIFY(!fileScanner.searchForCoverFile(mTestTracksForDirectory.at(4)).isEmpty());
        QVERIFY(!fileScanner.searchForCoverFile(mTestTracksForDirectory.at(5)).isEmpty());
        QVERIFY(!fileScanner.searchForCoverFile(mTestTracksForDirectory.at(6)).isEmpty());
        QVERIFY(!fileScanner.searchForCoverFile(mTestTracksForDirectory.at(7)).isEmpty());
        QVERIFY(!fileScanner.searchForCoverFile(mTestTracksForDirectory.at(8)).isEmpty());
    }

    void loadCoverFromMetaData()
    {
        FileScanner fileScanner;
        QVERIFY(fileScanner.checkEmbeddedCoverImage(mTestTracksForMetaData.at(0)));
        QVERIFY(fileScanner.checkEmbeddedCoverImage(mTestTracksForMetaData.at(1)));
        QVERIFY(fileScanner.checkEmbeddedCoverImage(mTestTracksForMetaData.at(2)));
    }

    void benchmarkCoverInDirectory()
    {
        FileScanner fileScanner;
        QBENCHMARK {
            fileScanner.searchForCoverFile(mTestTracksForDirectory.at(0));
            fileScanner.searchForCoverFile(mTestTracksForDirectory.at(1));
            fileScanner.searchForCoverFile(mTestTracksForDirectory.at(2));
            fileScanner.searchForCoverFile(mTestTracksForDirectory.at(3));
            fileScanner.searchForCoverFile(mTestTracksForDirectory.at(4));
            fileScanner.searchForCoverFile(mTestTracksForDirectory.at(5));
            fileScanner.searchForCoverFile(mTestTracksForDirectory.at(6));
            fileScanner.searchForCoverFile(mTestTracksForDirectory.at(7));
            fileScanner.searchForCoverFile(mTestTracksForDirectory.at(8));
        }
    }
    void benchmarkCoverFromMetadata()
    {
        FileScanner fileScanner;
        QBENCHMARK {
            fileScanner.checkEmbeddedCoverImage(mTestTracksForMetaData.at(0));
            fileScanner.checkEmbeddedCoverImage(mTestTracksForMetaData.at(1));
            fileScanner.checkEmbeddedCoverImage(mTestTracksForMetaData.at(2));
        }
    }

};

QTEST_GUILESS_MAIN(FileScannerTest)


#include "filescannertest.moc"
