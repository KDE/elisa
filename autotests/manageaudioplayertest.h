/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef MANAGEAUDIOPLAYERTEST_H
#define MANAGEAUDIOPLAYERTEST_H

#include <QObject>

class ManageAudioPlayerTest : public QObject
{

    Q_OBJECT

public:

    enum ColumnsRoles {
        IsValidRole = Qt::UserRole + 1,
        TitleRole = IsValidRole + 1,
        DurationRole = TitleRole + 1,
        CreatorRole = DurationRole + 1,
        ArtistRole = CreatorRole + 1,
        AlbumRole = ArtistRole + 1,
        TrackNumberRole = AlbumRole + 1,
        RatingRole = TrackNumberRole + 1,
        ImageRole = RatingRole + 1,
        ResourceRole = ImageRole + 1,
        CountRole = ResourceRole + 1,
        IsPlayingRole = CountRole + 1,
    };

    Q_ENUM(ColumnsRoles)

    explicit ManageAudioPlayerTest(QObject *parent = nullptr);

Q_SIGNALS:

private Q_SLOTS:

    void initTestCase();

    void simpleInitialCase();

    void noPlayCase();

    void skipNextTrack();

    void skipNextTrackWithRandomPlay();

    void skipPreviousTrack();

    void playTrackAndskipNextTrack();

    void playTrackAndskipPreviousTrack();

    void skipNextTrackAndPlayTrack();

    void skipPreviousTrackAndPlayTrack();

    void playLastCase();

    void playSingleTrack();

    void playRestoredTrack();

    void testRestoreSettingsAutomaticPlay();

    void testRestoreSettingsNoAutomaticPlay();

    void testRestoreSettingsAutomaticPlayAndPosition();

    void playTrackPauseAndSkipNextTrack();

    void testRestoreSettingsNoPlayWrongTrack();

    void testRestorePlayListAndSettingsAutomaticPlay();

    void playSingleAndClearPlayListTrack();

};

#endif // MANAGEAUDIOPLAYERTEST_H
