/*
   SPDX-FileCopyrightText: 2024 (c) Jack Hill <jackhill3103@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "models/lyricsmodel.h"

#include <QTest>

#include <chrono>

using namespace Qt::Literals::StringLiterals;
using namespace std::literals::chrono_literals;

struct LyricsData {
    QString lyric;
    std::chrono::milliseconds timeStamp;
};

class LyricsModelTest : public QObject
{
    Q_OBJECT

public:
    explicit LyricsModelTest(QObject *aParent = nullptr)
        : QObject(aParent)
    {
    }

private Q_SLOTS:
    void testParse_data()
    {
        QTest::addColumn<QString>("lyrics");
        QTest::addColumn<QList<LyricsData>>("parsedLyrics");

        QTest::addRow("Simple") << u"[01:33.82]Lyric 1\n[02:06.81]Lyric 2\n[03:30.46]Lyric 3\n"_s
                                << QList<LyricsData>{{u"Lyric 1"_s, 1min + 33s + 820ms}, {u"Lyric 2"_s, 2min + 6s + 810ms}, {u"Lyric 3"_s, 3min + 30s + 460ms}};

        QTest::addRow("Timestamps out of order") << u"[02:06.81]Lyric 2\n[01:33.82]Lyric 1\n[03:30.46]Lyric 3\n"_s
                                                 << QList<LyricsData>{{u"Lyric 1"_s, 1min + 33s + 820ms},
                                                                      {u"Lyric 2"_s, 2min + 6s + 810ms},
                                                                      {u"Lyric 3"_s, 3min + 30s + 460ms}};

        QTest::addRow("Repeated lyric") << u"[01:33.82][03:30.46]Chorus\n[02:06.81]Lyric 2\n"_s
                                        << QList<LyricsData>{{u"Chorus"_s, 1min + 33s + 820ms},
                                                             {u"Lyric 2"_s, 2min + 6s + 810ms},
                                                             {u"Chorus"_s, 3min + 30s + 460ms}};

        QTest::addRow("Chinese") << u"[01:33.82][03:30.46]一如最初的模样\n[02:06.81]烟花的瞬间\n"_s
                                 << QList<LyricsData>{{u"一如最初的模样"_s, 1min + 33s + 820ms},
                                                      {u"烟花的瞬间"_s, 2min + 6s + 810ms},
                                                      {u"一如最初的模样"_s, 3min + 30s + 460ms}};
    }

    void testParse()
    {
        QFETCH(QString, lyrics);
        QFETCH(QList<LyricsData>, parsedLyrics);

        LyricsModel lyricsModel;
        lyricsModel.setLyric(lyrics);

        QCOMPARE(lyricsModel.rowCount(), parsedLyrics.size());
        for (int i = 0; i < parsedLyrics.size(); ++i) {
            const auto expectedData = parsedLyrics.at(i);
            const auto index = lyricsModel.index(i);
            QCOMPARE(lyricsModel.data(index, LyricsModel::Lyric), expectedData.lyric);
            QCOMPARE(lyricsModel.data(index, LyricsModel::TimeStamp).toLongLong(), expectedData.timeStamp.count());
        }
    }
};

QTEST_GUILESS_MAIN(LyricsModelTest)

#include "lyricsmodeltest.moc"
