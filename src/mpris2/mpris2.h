/***************************************************************************
   SPDX-FileCopyrightText: 2014 (c) Sujith Haridasan <sujith.haridasan@kdemail.net>
   SPDX-FileCopyrightText: 2014 (c) Ashish Madeti <ashishmadeti@gmail.com>
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: GPL-3.0-or-later
 ***************************************************************************/

#ifndef MEDIACENTER_MPRIS2_H
#define MEDIACENTER_MPRIS2_H

#include "elisaLib_export.h"

#include <QObject>
#include <QSharedPointer>
#include <memory>

class MediaPlayer2Player;
class MediaPlayer2;
class MediaPlayListProxyModel;
class ManageAudioPlayer;
class ManageMediaPlayerControl;
class ManageHeaderBar;
class AudioWrapper;

class ELISALIB_EXPORT Mpris2 : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString playerName
               READ playerName
               WRITE setPlayerName
               NOTIFY playerNameChanged)

    Q_PROPERTY(MediaPlayListProxyModel* playListModel
               READ playListModel
               WRITE setPlayListModel
               NOTIFY playListModelChanged)

    Q_PROPERTY(ManageAudioPlayer* audioPlayerManager
               READ audioPlayerManager
               WRITE setAudioPlayerManager
               NOTIFY audioPlayerManagerChanged)

    Q_PROPERTY(ManageMediaPlayerControl* manageMediaPlayerControl
               READ manageMediaPlayerControl
               WRITE setManageMediaPlayerControl
               NOTIFY manageMediaPlayerControlChanged)

    Q_PROPERTY(ManageHeaderBar* headerBarManager
               READ headerBarManager
               WRITE setHeaderBarManager
               NOTIFY headerBarManagerChanged)

    Q_PROPERTY(AudioWrapper* audioPlayer
               READ audioPlayer
               WRITE setAudioPlayer
               NOTIFY audioPlayerChanged)

    Q_PROPERTY(bool showProgressOnTaskBar
               READ showProgressOnTaskBar
               WRITE setShowProgressOnTaskBar
               NOTIFY showProgressOnTaskBarChanged)

public:
    explicit Mpris2(QObject* parent = nullptr);
    ~Mpris2() override;

    [[nodiscard]] QString playerName() const;

    [[nodiscard]] MediaPlayListProxyModel* playListModel() const;

    [[nodiscard]] ManageAudioPlayer* audioPlayerManager() const;

    [[nodiscard]] ManageMediaPlayerControl* manageMediaPlayerControl() const;

    [[nodiscard]] ManageHeaderBar* headerBarManager() const;

    [[nodiscard]] AudioWrapper* audioPlayer() const;

    [[nodiscard]] bool showProgressOnTaskBar() const;

public Q_SLOTS:

    void setPlayerName(const QString &playerName);

    void setPlayListModel(MediaPlayListProxyModel* playListModel);

    void setAudioPlayerManager(ManageAudioPlayer* audioPlayerManager);

    void setManageMediaPlayerControl(ManageMediaPlayerControl* manageMediaPlayerControl);

    void setHeaderBarManager(ManageHeaderBar* headerBarManager);

    void setAudioPlayer(AudioWrapper* audioPlayer);

    void setShowProgressOnTaskBar(bool value);

Q_SIGNALS:
    void raisePlayer();

    void playerNameChanged();

    void playListModelChanged();

    void audioPlayerManagerChanged();

    void manageMediaPlayerControlChanged();

    void headerBarManagerChanged();

    void audioPlayerChanged();

    void showProgressOnTaskBarChanged();

private:

    void initDBusService();

    std::unique_ptr<MediaPlayer2> m_mp2;
    std::unique_ptr<MediaPlayer2Player> m_mp2p;
    QString m_playerName;
    MediaPlayListProxyModel* m_playListModel = nullptr;
    ManageAudioPlayer* m_manageAudioPlayer = nullptr;
    ManageMediaPlayerControl* m_manageMediaPlayerControl = nullptr;
    ManageHeaderBar* m_manageHeaderBar = nullptr;
    AudioWrapper* m_audioPlayer = nullptr;
    bool mShowProgressOnTaskBar = true;
};

#endif //MEDIACENTER_MPRIS2_H
