/*
   SPDX-FileCopyrightText: 2025 (c) Pedro Nishiyama <nishiyama.v3@gmail.com>

   SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef ANDROIDPLAYER_H
#define ANDROIDPLAYER_H

#include "androidplayerjni.h"
#include "elisaLib_export.h"
#include "manageaudioplayer.h"

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

    [[nodiscard]] ManageAudioPlayer* audioPlayerManager() const;

public Q_SLOTS:

    void setAudioPlayerManager(ManageAudioPlayer* audioPlayerManager);

    void initialize();

Q_SIGNALS:

    void audioPlayerManagerChanged();

private Q_SLOTS:

    void playerStatusChanged();

    void playerPlaybackStateChanged();

    void playerPositionChanged(qint64 position);

    void next();

    void previous();

    void playPause();

    void stop();

    void seek(qlonglong offset);

private:

    AndroidPlayerJni* m_androidplayerjni = &AndroidPlayerJni::GetInstance();

    ManageAudioPlayer* m_manageAudioPlayer = nullptr;

};

#endif // ANDROIDPLAYER_H
