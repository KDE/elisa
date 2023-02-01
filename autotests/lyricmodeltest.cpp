#include <QtTest>
#include "models/lyricsmodel.h"
class LyricsModelTests: public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;
private Q_SLOTS:
    void initTestCase() {
        setlocale(LC_ALL, "en_US.UTF-8");
        setenv("LANG", "en_US.UTF-8", 1);
    }
    void testLyricsParse() {
        const auto data = QStringLiteral(R"ESCAPE(
[ar:Chubby Checker oppure  Beatles, The]
[al:Hits Of The 60's - Vol. 2 – Oldies]
[ti:Let's Twist Again]
[au:Written by Kal Mann / Dave Appell, 1961]
[length: 2:23]
[00:01.00]作曲 : DAISHI DANCE/Cécile Corbel/Tomoharu Moriya
[00:59.06]In my dreams
[00:59.06]我的梦里
[01:01.01]I feel your light
[01:01.01]有你的光芒
[01:02.85]I feel love is born again
[01:02.85]爱再次绽放)ESCAPE");
        const std::array<QString, 8> expectedLyrics = {{
                                                           QStringLiteral("Artist: Chubby Checker oppure  Beatles, The\nAlbum: Hits Of The 60's - Vol. 2 \u2013 Oldies\nTitle: Let's Twist Again\nCreator: Written by Kal Mann / Dave Appell, 1961\nLength:  2:23\n"),
                                                           QStringLiteral("作曲 : DAISHI DANCE/Cécile Corbel/Tomoharu Moriya"),
                                                           QStringLiteral("In my dreams"),
                                                           QStringLiteral("我的梦里"),
                                                           QStringLiteral("I feel your light"),
                                                           QStringLiteral("有你的光芒"),
                                                           QStringLiteral("I feel love is born again"),
                                                           QStringLiteral("爱再次绽放")
                                                       }};
        model.setLyric(data);
        QCOMPARE(model.isLRC(), true);
        QCOMPARE(model.rowCount(), 8);
        int i = 0;
        for (const auto &line : expectedLyrics) {
            QCOMPARE(model.data(model.index(i++), LyricsModel::LyricsRole::Lyric).toString(), line);
        }
    }

    void testPlainLyrics() {
        const auto data = QStringLiteral("Not LRC format lyric");
        model.setLyric(data);
        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.data(model.index(0), LyricsModel::LyricsRole::Lyric).toString(), data);
    }

    void testInvalidLRC() {
        const auto data = QStringLiteral(R"ESCAPE(
[00:59.06]In my dreams
[01:01.01]I feel your light
[01:02.85I feel lov)ESCAPE");
        const std::array<QString, 2> expectedLyrics = {{
                                                           QStringLiteral("In my dreams"),
                                                           QStringLiteral("I feel your light")
                                                      }};
        model.setLyric(data);
        QCOMPARE(model.rowCount(), 2);
        int i = 0;
        for (const auto &line : expectedLyrics) {
            QCOMPARE(model.data(model.index(i++), LyricsModel::LyricsRole::Lyric).toString(), line);
        }
    }
private:
    LyricsModel model;
};

QTEST_GUILESS_MAIN(LyricsModelTests)

#include "lyricmodeltest.moc"
