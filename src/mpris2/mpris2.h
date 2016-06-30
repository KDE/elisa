/***************************************************************************
 *   Copyright 2014 Sujith Haridasan <sujith.haridasan@kdemail.net>        *
 *   Copyright 2014 Ashish Madeti <ashishmadeti@gmail.com>                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
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

#include <QObject>
#include <QSharedPointer>
#include <QVariantMap>

class MediaPlayer2;
class MediaPlayer2Player;
class MediaPlayer2Tracklist;

class Mpris2 : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString playerName
               READ playerName
               WRITE setPlayerName
               NOTIFY playerNameChanged)

public:
    explicit Mpris2(QObject* parent = 0);
    ~Mpris2();

    MediaPlayer2Player* getMediaPlayer2Player();
    QString getCurrentTrackId();
    QVariantMap getMetadataOf(const QString& url);
    QVariantMap getMetadataOf(const QString& url, const QString& trackId);

    QString playerName() const;

public Q_SLOTS:

    void setPlayerName(QString playerName);

Q_SIGNALS:
    void raisePMC() const;

    void playerNameChanged();

private:

    void initDBusService();

    MediaPlayer2 *m_mp2 = nullptr;
    MediaPlayer2Player *m_mp2p = nullptr;
    MediaPlayer2Tracklist *m_mp2tl = nullptr;
    QString m_playerName;
};

#endif //MEDIACENTER_MPRIS2_H
