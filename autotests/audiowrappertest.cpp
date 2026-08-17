/*
 * SPDX-FileCopyrightText: 2026 Ian Monroe <imonroe@kde.org>
 * SPDX-License-Identifier: LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "config-upnp-qt.h"

#include "audiowrapper.h"

#include <chrono>

#include <KLocalizedString>

#include <QMediaPlayer>
#include <QSignalSpy>
#include <QStandardPaths>
#include <QTest>
#include <QUrl>

using namespace Qt::Literals::StringLiterals;

namespace
{
constexpr auto kTimeout = std::chrono::seconds(10);
constexpr auto kEndOfMediaTimeout = std::chrono::seconds(15);
constexpr auto kSettleWait = std::chrono::milliseconds(300);
constexpr auto kSeekTolerance = std::chrono::milliseconds(200);
constexpr auto kUndoRestoreTolerance = std::chrono::milliseconds(500);

void seekToAndVerify(AudioWrapper &wrapper, qint64 target)
{
    wrapper.seek(target);
    QTRY_COMPARE_LE_WITH_TIMEOUT(std::chrono::milliseconds(qAbs(wrapper.position() - target)), kSeekTolerance, kTimeout);
}
}

class AudioWrapperTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        QStandardPaths::setTestModeEnabled(true);
        KLocalizedString::setApplicationDomain(QByteArrayLiteral("elisa"));
    }

    void init()
    {
        mFixtureDir = QStringLiteral(LOCAL_FILE_TESTS_SAMPLE_FILES_PATH) + u"/music"_s;
    }

    void constructAndDefaults()
    {
        AudioWrapper wrapper;

        QVERIFY(!wrapper.muted());
        QVERIFY(wrapper.source().isEmpty());
        QCOMPARE(wrapper.status(), QMediaPlayer::NoMedia);
        QCOMPARE(wrapper.playbackState(), QMediaPlayer::StoppedState);
        QCOMPARE(wrapper.error(), QMediaPlayer::NoError);
        QVERIFY(!wrapper.seekable());

        // No source loaded: playback controls must be safe no-ops
        wrapper.play();
        wrapper.pause();
        wrapper.stop();
        wrapper.seek(0);
    }

    void playPauseStopCycle()
    {
        AudioWrapper wrapper;
        QSignalSpy playingSpy(&wrapper, &AudioWrapper::playing);
        QSignalSpy pausedSpy(&wrapper, &AudioWrapper::paused);
        QSignalSpy stoppedSpy(&wrapper, &AudioWrapper::stopped);
        QSignalSpy playbackStateChangedSpy(&wrapper, &AudioWrapper::playbackStateChanged);

        wrapper.setSource(QUrl::fromLocalFile(mFixtureDir + u"/test.ogg"_s));

        wrapper.play();
        QTRY_COMPARE_EQ_WITH_TIMEOUT(wrapper.playbackState(), QMediaPlayer::PlayingState, kTimeout);
        QTRY_COMPARE_EQ_WITH_TIMEOUT(playingSpy.count(), 1, kTimeout);

        wrapper.pause();
        QTRY_COMPARE_EQ_WITH_TIMEOUT(wrapper.playbackState(), QMediaPlayer::PausedState, kTimeout);
        QTRY_COMPARE_EQ_WITH_TIMEOUT(pausedSpy.count(), 1, kTimeout);

        wrapper.play();
        QTRY_COMPARE_EQ_WITH_TIMEOUT(wrapper.playbackState(), QMediaPlayer::PlayingState, kTimeout);

        wrapper.stop();
        QTRY_COMPARE_EQ_WITH_TIMEOUT(wrapper.playbackState(), QMediaPlayer::StoppedState, kTimeout);
        QTRY_COMPARE_EQ_WITH_TIMEOUT(stoppedSpy.count(), 1, kTimeout);

        QCOMPARE(playbackStateChangedSpy.count(), 4);
    }

    void seekDuringPlayback()
    {
        AudioWrapper wrapper;
        wrapper.setSource(QUrl::fromLocalFile(mFixtureDir + u"/test.ogg"_s));

        wrapper.play();
        QTRY_COMPARE_GT_WITH_TIMEOUT(wrapper.position(), 0, kTimeout);
        QTRY_COMPARE_GT_WITH_TIMEOUT(wrapper.duration(), 0, kTimeout);

        seekToAndVerify(wrapper, wrapper.duration() / 2);

        wrapper.stop();
    }

    void setPositionProperty()
    {
        AudioWrapper wrapper;
        wrapper.setSource(QUrl::fromLocalFile(mFixtureDir + u"/test.ogg"_s));

        wrapper.play();
        QTRY_COMPARE_EQ_WITH_TIMEOUT(wrapper.playbackState(), QMediaPlayer::PlayingState, kTimeout);
        QTRY_COMPARE_GT_WITH_TIMEOUT(wrapper.duration(), 0, kTimeout);

        const qint64 target = wrapper.duration() / 3;
        wrapper.setPosition(target);
        QTRY_COMPARE_LE_WITH_TIMEOUT(std::chrono::milliseconds(qAbs(wrapper.position() - target)), kSeekTolerance, kTimeout);

        wrapper.stop();
    }

    void loadSignals()
    {
        AudioWrapper wrapper;
        QSignalSpy statusChangedSpy(&wrapper, &AudioWrapper::statusChanged);
        QSignalSpy durationChangedSpy(&wrapper, &AudioWrapper::durationChanged);
        QSignalSpy seekableChangedSpy(&wrapper, &AudioWrapper::seekableChanged);

        wrapper.setSource(QUrl::fromLocalFile(mFixtureDir + u"/test.ogg"_s));
        wrapper.play();
        QTRY_COMPARE_EQ_WITH_TIMEOUT(wrapper.playbackState(), QMediaPlayer::PlayingState, kTimeout);

        QTRY_COMPARE_GE_WITH_TIMEOUT(durationChangedSpy.count(), 1, kTimeout);
        QVERIFY(wrapper.duration() > 0);

        QTRY_COMPARE_GE_WITH_TIMEOUT(statusChangedSpy.count(), 1, kTimeout);
        QTRY_VERIFY_WITH_TIMEOUT(wrapper.seekable(), kTimeout);
        QTRY_COMPARE_GE_WITH_TIMEOUT(seekableChangedSpy.count(), 1, kTimeout);

        wrapper.stop();
    }

    void positionUpdates()
    {
        AudioWrapper wrapper;
        QSignalSpy positionChangedSpy(&wrapper, &AudioWrapper::positionChanged);
        wrapper.setSource(QUrl::fromLocalFile(mFixtureDir + u"/test.ogg"_s));

        wrapper.play();
        QTRY_COMPARE_EQ_WITH_TIMEOUT(wrapper.playbackState(), QMediaPlayer::PlayingState, kTimeout);
        QTRY_COMPARE_GE_WITH_TIMEOUT(positionChangedSpy.count(), 2, kTimeout);

        wrapper.stop();
    }

    void endOfMedia()
    {
        AudioWrapper wrapper;
        const QUrl trackUrl = QUrl::fromLocalFile(mFixtureDir + u"/test.ogg"_s);
        wrapper.setSource(trackUrl);

        wrapper.play();
        QTRY_COMPARE_EQ_WITH_TIMEOUT(wrapper.status(), QMediaPlayer::EndOfMedia, kEndOfMediaTimeout);

        wrapper.setSource(trackUrl);
        wrapper.play();
        QTRY_COMPARE_EQ_WITH_TIMEOUT(wrapper.playbackState(), QMediaPlayer::PlayingState, kTimeout);

        wrapper.stop();
    }

    void invalidSource()
    {
        AudioWrapper wrapper;
        QSignalSpy errorChangedSpy(&wrapper, &AudioWrapper::errorChanged);
        QSignalSpy statusChangedSpy(&wrapper, &AudioWrapper::statusChanged);

        wrapper.setSource(QUrl::fromLocalFile(mFixtureDir + u"/does-not-exist.ogg"_s));
        wrapper.play();

        // Backends differ: libvlc reports both an error and InvalidMedia,
        // QtMultimedia may only report one of them. Accept either outcome.
        QTRY_VERIFY_WITH_TIMEOUT(wrapper.error() != QMediaPlayer::NoError || wrapper.status() == QMediaPlayer::InvalidMedia, kTimeout);
        QVERIFY(errorChangedSpy.count() >= 1 || statusChangedSpy.count() >= 1);
    }

    void multiFormat_data()
    {
        QTest::addColumn<QString>("fileName");

        QTest::newRow("ogg") << u"test.ogg"_s;
        QTest::newRow("mp3") << u"test.mp3"_s;
        QTest::newRow("m4a") << u"test.m4a"_s;
    }

    void multiFormat()
    {
        QFETCH(QString, fileName);

        AudioWrapper wrapper;
        wrapper.setSource(QUrl::fromLocalFile(mFixtureDir + u"/"_s + fileName));

        wrapper.play();
        QTRY_COMPARE_EQ_WITH_TIMEOUT(wrapper.playbackState(), QMediaPlayer::PlayingState, kTimeout);
        QTRY_COMPARE_GT_WITH_TIMEOUT(wrapper.duration(), 0, kTimeout);
        QCOMPARE(wrapper.error(), QMediaPlayer::NoError);
        QTRY_COMPARE_GT_WITH_TIMEOUT(wrapper.position(), 0, kTimeout);

        wrapper.stop();
    }

    void volumeAndMute()
    {
        AudioWrapper wrapper;

        wrapper.setSource(QUrl::fromLocalFile(mFixtureDir + u"/test.ogg"_s));
        wrapper.play();
        QTRY_COMPARE_EQ_WITH_TIMEOUT(wrapper.playbackState(), QMediaPlayer::PlayingState, kTimeout);

        wrapper.setVolume(50);
        QTRY_COMPARE_LE_WITH_TIMEOUT(qAbs(wrapper.volume() - 50.0), 10.0, kTimeout);

        wrapper.setMuted(true);
        QTRY_VERIFY_WITH_TIMEOUT(wrapper.muted(), kTimeout);

        wrapper.setMuted(false);
        QTRY_VERIFY_WITH_TIMEOUT(!wrapper.muted(), kTimeout);

        wrapper.stop();
    }

    void volumeBoundaryValues()
    {
        AudioWrapper wrapper;
        wrapper.setSource(QUrl::fromLocalFile(mFixtureDir + u"/test.ogg"_s));
        wrapper.play();
        QTRY_COMPARE_EQ_WITH_TIMEOUT(wrapper.playbackState(), QMediaPlayer::PlayingState, kTimeout);

        wrapper.setVolume(0);
        QTRY_COMPARE_LE_WITH_TIMEOUT(wrapper.volume(), 1.0, kTimeout);

        wrapper.setVolume(100);
        QTRY_COMPARE_GE_WITH_TIMEOUT(wrapper.volume(), 99.0, kTimeout);

        wrapper.stop();
    }

    void undoPositionRestore()
    {
        AudioWrapper wrapper;
        wrapper.setSource(QUrl::fromLocalFile(mFixtureDir + u"/test.ogg"_s));

        wrapper.play();
        QTRY_COMPARE_GT_WITH_TIMEOUT(wrapper.position(), 0, kTimeout);
        QTRY_COMPARE_GT_WITH_TIMEOUT(wrapper.duration(), 0, kTimeout);

        seekToAndVerify(wrapper, wrapper.duration() / 2);

        const qint64 savedPosition = wrapper.position();
        wrapper.saveUndoPosition(savedPosition);
        wrapper.stop();
        QTRY_COMPARE_EQ_WITH_TIMEOUT(wrapper.playbackState(), QMediaPlayer::StoppedState, kTimeout);

        wrapper.restoreUndoPosition();
        wrapper.play();
        QTRY_COMPARE_EQ_WITH_TIMEOUT(wrapper.playbackState(), QMediaPlayer::PlayingState, kTimeout);
        QTRY_COMPARE_LE_WITH_TIMEOUT(std::chrono::milliseconds(qAbs(wrapper.position() - savedPosition)), kUndoRestoreTolerance, kTimeout);

        wrapper.stop();
    }

    void clearSource()
    {
        AudioWrapper wrapper;
        wrapper.setSource(QUrl::fromLocalFile(mFixtureDir + u"/test.ogg"_s));

        wrapper.play();
        QTRY_COMPARE_EQ_WITH_TIMEOUT(wrapper.playbackState(), QMediaPlayer::PlayingState, kTimeout);

        // Clearing the source must at minimum stop playback.
        wrapper.setSource(QUrl());
        QTRY_COMPARE_EQ_WITH_TIMEOUT(wrapper.playbackState(), QMediaPlayer::StoppedState, kTimeout);
    }

    void repeatedControls()
    {
        AudioWrapper wrapper;
        QSignalSpy playingSpy(&wrapper, &AudioWrapper::playing);
        QSignalSpy stoppedSpy(&wrapper, &AudioWrapper::stopped);
        wrapper.setSource(QUrl::fromLocalFile(mFixtureDir + u"/test.ogg"_s));

        wrapper.play();
        QTRY_COMPARE_EQ_WITH_TIMEOUT(wrapper.playbackState(), QMediaPlayer::PlayingState, kTimeout);

        // Repeated play while playing must be a no-op.
        wrapper.play();
        QTest::qWait(kSettleWait);
        QCOMPARE(playingSpy.count(), 1);
        QCOMPARE(wrapper.playbackState(), QMediaPlayer::PlayingState);

        wrapper.stop();
        wrapper.stop();
        QTRY_COMPARE_EQ_WITH_TIMEOUT(wrapper.playbackState(), QMediaPlayer::StoppedState, kTimeout);
        QTest::qWait(kSettleWait);
        QCOMPARE(stoppedSpy.count(), 1);
    }

    void stopImmediatelyAfterPlay()
    {
        AudioWrapper wrapper;
        wrapper.setSource(QUrl::fromLocalFile(mFixtureDir + u"/test.ogg"_s));

        // Stop before the backend has reported PlayingState. The player must
        // settle on StoppedState no matter how quickly the stop follows.
        wrapper.play();
        wrapper.stop();
        QTRY_COMPARE_EQ_WITH_TIMEOUT(wrapper.playbackState(), QMediaPlayer::StoppedState, kTimeout);

        QTest::qWait(kSettleWait);
        QCOMPARE(wrapper.playbackState(), QMediaPlayer::StoppedState);
    }

private:
    QString mFixtureDir;
};

QTEST_GUILESS_MAIN(AudioWrapperTest)

#include "audiowrappertest.moc"
