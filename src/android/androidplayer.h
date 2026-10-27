/*
   SPDX-FileCopyrightText: 2025 (c) Pedro Nishiyama <nishiyama.v3@gmail.com>
   SPDX-FileCopyrightText: 2026 Ian Monroe <imonroe@kde.org>

   SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef ANDROIDPLAYER_H
#define ANDROIDPLAYER_H

#include "androidplayerjni.h"
#include "elisaLib_export.h"
#include "manageaudioplayer.h"

#include <QElapsedTimer>
#include <QObject>

class ELISALIB_EXPORT AndroidPlayer : public QObject
{
    Q_OBJECT
    
    QML_ELEMENT

    Q_PROPERTY(ManageAudioPlayer* audioPlayerManager
               READ audioPlayerManager
               WRITE setAudioPlayerManager
               NOTIFY audioPlayerManagerChanged)

public:
    explicit AndroidPlayer(QObject* parent = nullptr);
    ~AndroidPlayer() override;

    [[nodiscard]] ManageAudioPlayer *audioPlayerManager() const;

public Q_SLOTS:

    void setAudioPlayerManager(ManageAudioPlayer *audioPlayerManager);

    void initialize();

Q_SIGNALS:

    void audioPlayerManagerChanged();

private Q_SLOTS:

    void playerStatusChanged();

    void playerPlaybackStateChanged();

    void playerPositionChanged(qint64 position);

    void reportPlayerPosition();

    void next();

    void previous();

    void playPause();

    void play();

    void pause();

    void stop();

    void seek(qlonglong offset);

private:
    void pushMetadata();

    AndroidPlayerJni* m_androidplayerjni = &AndroidPlayerJni::GetInstance();

    ManageAudioPlayer *m_manageAudioPlayer = nullptr;

    QElapsedTimer m_positionReportTimer;
    qint64 m_reportedPosition = 0;
    bool m_metadataPushScheduled = false;
};

#endif // ANDROIDPLAYER_H
