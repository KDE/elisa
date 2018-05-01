/***************************************************************************
 *   Copyright 2014 Sujith Haridasan <sujith.haridasan@kdemail.net>        *
 *   Copyright 2014 Ashish Madeti <ashishmadeti@gmail.com>                 *
 *   Copyright 2016 Matthieu Gallien <mgallien@mgallien.fr>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#ifndef MEDIACENTER_MPRIS2_H
#define MEDIACENTER_MPRIS2_H

#include "elisaLib_export.h"

#include <QObject>
#include <QSharedPointer>
#include <QVariantMap>
#include <memory>

class MediaPlayer2Player;
class MediaPlayer2;
class QAbstractItemModel;
class MediaPlayList;
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

    Q_PROPERTY(QAbstractItemModel* playListModel
               READ playListModel
               WRITE setPlayListModel
               NOTIFY playListModelChanged)

    Q_PROPERTY(MediaPlayList* playListControler
               READ playListControler
               WRITE setPlayListControler
               NOTIFY playListControlerChanged)

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

public:
    explicit Mpris2(QObject* parent = nullptr);
    ~Mpris2() override;

    QString playerName() const;

    QAbstractItemModel* playListModel() const;

    MediaPlayList* playListControler() const;

    ManageAudioPlayer* audioPlayerManager() const;

    ManageMediaPlayerControl* manageMediaPlayerControl() const;

    ManageHeaderBar* headerBarManager() const;

    AudioWrapper* audioPlayer() const;

public Q_SLOTS:

    void setPlayerName(const QString &playerName);

    void setPlayListModel(QAbstractItemModel* playListModel);

    void setPlayListControler(MediaPlayList* playListControler);

    void setAudioPlayerManager(ManageAudioPlayer* audioPlayerManager);

    void setManageMediaPlayerControl(ManageMediaPlayerControl* manageMediaPlayerControl);

    void setHeaderBarManager(ManageHeaderBar* headerBarManager);

    void setAudioPlayer(AudioWrapper* audioPlayer);

Q_SIGNALS:
    void raisePlayer();

    void playerNameChanged();

    void playListModelChanged();

    void playListControlerChanged();

    void audioPlayerManagerChanged();

    void manageMediaPlayerControlChanged();

    void headerBarManagerChanged();

    void audioPlayerChanged();

private:

    void initDBusService();

    std::unique_ptr<MediaPlayer2> m_mp2;
    std::unique_ptr<MediaPlayer2Player> m_mp2p;
    QString m_playerName;
    QAbstractItemModel* m_playListModel = nullptr;
    MediaPlayList* m_playListControler = nullptr;
    ManageAudioPlayer* m_manageAudioPlayer = nullptr;
    ManageMediaPlayerControl* m_manageMediaPlayerControl = nullptr;
    ManageHeaderBar* m_manageHeaderBar = nullptr;
    AudioWrapper* m_audioPlayer = nullptr;
};

#endif //MEDIACENTER_MPRIS2_H
