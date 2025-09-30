/*
   SPDX-FileCopyrightText: 2024 (c) Jack Hill <jackhill3103@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "models/lyricsmodel.h"

#include <QSignalSpy>
#include <QTest>

#include <chrono>

using namespace Qt::Literals::StringLiterals;
using namespace std::literals::chrono_literals;

struct LyricsData {
    QString lyric;
    std::chrono::milliseconds timeStamp;
};

struct ExpectedHighlightedIndex {
    std::chrono::milliseconds timeStamp;
    int index;
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

        QTest::addRow("Timestamps with different length") << u"[01:23.4]Lyric 1\n[01:23.45]Lyric 2\n[01:23.456]Lyric 3"_s
                                                 << QList<LyricsData>{{u"Lyric 1"_s, 1min + 23s + 400ms},
                                                                      {u"Lyric 2"_s, 1min + 23s + 450ms},
                                                                      {u"Lyric 3"_s, 1min + 23s + 456ms}};

        QTest::addRow("Repeated lyric") << u"[01:33.82][03:30.46]Chorus\n[02:06.81]Lyric 2\n"_s
                                        << QList<LyricsData>{{u"Chorus"_s, 1min + 33s + 820ms},
                                                             {u"Lyric 2"_s, 2min + 6s + 810ms},
                                                             {u"Chorus"_s, 3min + 30s + 460ms}};

        QTest::addRow("Chinese") << u"[01:33.82][03:30.46]一如最初的模样\n[02:06.81]烟花的瞬间\n"_s
                                 << QList<LyricsData>{{u"一如最初的模样"_s, 1min + 33s + 820ms},
                                                      {u"烟花的瞬间"_s, 2min + 6s + 810ms},
                                                      {u"一如最初的模样"_s, 3min + 30s + 460ms}};

        QTest::addRow("No linebreak at the end") << u"[01:33.82]Lyric 1\n[02:06.81]Lyric 2\n[03:30.46]Lyric 3"_s
                                                 << QList<LyricsData>{{u"Lyric 1"_s, 1min + 33s + 820ms},
                                                                      {u"Lyric 2"_s, 2min + 6s + 810ms},
                                                                      {u"Lyric 3"_s, 3min + 30s + 460ms}};

        QTest::addRow("Empty line with timestamp") << u"[01:33.82]Lyric 1\n[02:06.81]\n[03:30.46]Lyric 3"_s
                                                   << QList<LyricsData>{{u"Lyric 1"_s, 1min + 33s + 820ms},
                                                                        {u""_s, 2min + 6s + 810ms},
                                                                        {u"Lyric 3"_s, 3min + 30s + 460ms}};

        QTest::addRow("Bilingual same timestamp")
            << u"[00:15.13]悴んだ心 ふるえる眼差し\n[00:15.13]『内心满是憔悴 眼神游动不止』\n[00:15.13]\"A heart grown numb with cold a trembling gaze\""_s
            << QList<LyricsData>{{u"悴んだ心 ふるえる眼差し"_s, 15s + 130ms},
                                 {u"『内心满是憔悴 眼神游动不止』"_s, 15s + 130ms},
                                 {u"\"A heart grown numb with cold a trembling gaze\""_s, 15s + 130ms}};
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

    void testHighlightedIndex_data()
    {
        QTest::addColumn<QString>("lyrics");
        QTest::addColumn<QList<ExpectedHighlightedIndex>>("expectedHighlightedIndexes");

        QList<ExpectedHighlightedIndex> expectedHighlightedIndexes = {{0s, -1},
                                                                      {1min + 40s, 0},
                                                                      {3min + 40s, 2},
                                                                      {2min + 6s + 810ms, 1},
                                                                      {2min + 6s + 800ms, 0},
                                                                      {1min + 33s + 810ms, -1},
                                                                      {1min + 33s + 820ms, 0},
                                                                      {3min + 30s + 460ms, 2},
                                                                      {3min + 30s + 450ms, 1}};

        QTest::addRow("Simple") << u"[01:33.82]Lyric 1\n[02:06.81]Lyric 2\n[03:30.46]Lyric 3\n"_s << expectedHighlightedIndexes;

        QTest::addRow("Timestamps out of order") << u"[02:06.81]Lyric 2\n[01:33.82]Lyric 1\n[03:30.46]Lyric 3\n"_s << expectedHighlightedIndexes;

        QTest::addRow("Repeated lyric") << u"[01:33.82][03:30.46]Chorus\n[02:06.81]Lyric 2\n"_s << expectedHighlightedIndexes;
    }

    void testHighlightedIndex()
    {
        QFETCH(QString, lyrics);
        QFETCH(QList<ExpectedHighlightedIndex>, expectedHighlightedIndexes);

        LyricsModel lyricsModel;
        QSignalSpy highlightedIndexChangedSpy{&lyricsModel, &LyricsModel::highlightedIndexChanged};

        lyricsModel.setLyric(lyrics);

        QCOMPARE(lyricsModel.highlightedIndex(), -1);
        QCOMPARE(highlightedIndexChangedSpy.count(), 1);
        highlightedIndexChangedSpy.clear();

        int previousIndex = -1;
        for (const auto positionIndex : expectedHighlightedIndexes) {
            lyricsModel.setPosition(positionIndex.timeStamp.count());
            QCOMPARE(lyricsModel.highlightedIndex(), positionIndex.index);

            if (positionIndex.index != previousIndex) {
                QCOMPARE(highlightedIndexChangedSpy.size(), 1);
                previousIndex = positionIndex.index;
                highlightedIndexChangedSpy.clear();
            }
        }
    }

    void testSameTimestampGroupHighlight()
    {
        // Two groups: [0..2] at 00:15.13 and [3..5] at 00:21.94
        const auto lyrics =
            u"[00:15.13]悴んだ心 ふるえる眼差し\n[00:15.13]『内心满是憔悴 眼神游动不止』\n[00:15.13]\"A heart grown numb with cold a trembling gaze\"\n[00:21.94]僕は ひとりぼっちだった\n[00:21.94]『我在这世界孤独一人』\n[00:21.94]\"In all the world, I was alone\""_s;

        LyricsModel lyricsModel;
        QSignalSpy indexSpy{&lyricsModel, &LyricsModel::highlightedIndexChanged};
        QSignalSpy dataSpy{&lyricsModel, &QAbstractItemModel::dataChanged};

        lyricsModel.setLyric(lyrics);

        // Reset state
        QCOMPARE(lyricsModel.highlightedIndex(), -1);
        QCOMPARE(indexSpy.count(), 1); // reset emits once
        indexSpy.clear();

        QCOMPARE(dataSpy.count(), 1);
        {
            const auto args = dataSpy.takeFirst();
            QCOMPARE(args.size(), 3);
            const auto topLeft = qvariant_cast<QModelIndex>(args.at(0));
            const auto bottomRight = qvariant_cast<QModelIndex>(args.at(1));
            const auto roles = args.at(2).value<QList<int>>();
            QCOMPARE(topLeft.row(), 0);
            QCOMPARE(bottomRight.row(), lyricsModel.rowCount() - 1);
            QCOMPARE(roles.size(), 1);
            QCOMPARE(roles.first(), (int)LyricsModel::IsHighlighted);
        }
        // Initially nothing highlighted
        for (int row = 0; row < lyricsModel.rowCount(); ++row) {
            const auto idx = lyricsModel.index(row);
            QCOMPARE(lyricsModel.data(idx, LyricsModel::IsHighlighted).toBool(), false);
        }

        // Before first group -> nothing highlighted, no dataChanged
        lyricsModel.setPosition((15s + 120ms).count());
        QCOMPARE(lyricsModel.highlightedIndex(), -1);
        QCOMPARE(indexSpy.count(), 0);
        QCOMPARE(dataSpy.count(), 0);

        // Enter first group (00:15.13) -> one dataChanged for [0,2]
        lyricsModel.setPosition((15s + 130ms).count());
        QCOMPARE(lyricsModel.highlightedIndex(), 0);
        QCOMPARE(indexSpy.count(), 1);
        indexSpy.clear();
        QCOMPARE(dataSpy.count(), 1);
        {
            const auto args = dataSpy.takeFirst();
            QCOMPARE(args.size(), 3);
            const auto topLeft = qvariant_cast<QModelIndex>(args.at(0));
            const auto bottomRight = qvariant_cast<QModelIndex>(args.at(1));
            const auto roles = args.at(2).value<QList<int>>();
            QCOMPARE(topLeft.row(), 0);
            QCOMPARE(bottomRight.row(), 2);
            QCOMPARE(roles.size(), 1);
            QCOMPARE(roles.first(), (int)LyricsModel::IsHighlighted);
        }
        for (int row = 0; row < lyricsModel.rowCount(); ++row) {
            const auto idx = lyricsModel.index(row);
            const bool expected = (row >= 0 && row <= 2);
            QCOMPARE(lyricsModel.data(idx, LyricsModel::IsHighlighted).toBool(), expected);
        }

        // Jump to second group (00:21.94) -> two dataChanged events: [0,2] and [3,5]
        lyricsModel.setPosition((21s + 940ms).count());
        QCOMPARE(lyricsModel.highlightedIndex(), 3);
        QCOMPARE(indexSpy.count(), 1);
        indexSpy.clear();
        QCOMPARE(dataSpy.count(), 2);

        QList<QPair<int, int>> ranges;
        for (int i = 0; i < 2; ++i) {
            const auto args = dataSpy.takeFirst();
            QCOMPARE(args.size(), 3);
            const auto topLeft = qvariant_cast<QModelIndex>(args.at(0));
            const auto bottomRight = qvariant_cast<QModelIndex>(args.at(1));
            const auto roles = args.at(2).value<QList<int>>();
            QCOMPARE(roles.size(), 1);
            QCOMPARE(roles.first(), (int)LyricsModel::IsHighlighted);
            ranges.append({topLeft.row(), bottomRight.row()});
        }
        auto hasRange = [&ranges](int a, int b) {
            for (const auto &r : ranges) {
                if (r.first == a && r.second == b)
                    return true;
            }
            return false;
        };
        QVERIFY(hasRange(0, 2));
        QVERIFY(hasRange(3, 5));

        for (int row = 0; row < lyricsModel.rowCount(); ++row) {
            const auto idx = lyricsModel.index(row);
            const bool expected = (row >= 3 && row <= 5);
            QCOMPARE(lyricsModel.data(idx, LyricsModel::IsHighlighted).toBool(), expected);
        }
    }
};

QTEST_GUILESS_MAIN(LyricsModelTest)

#include "lyricsmodeltest.moc"
