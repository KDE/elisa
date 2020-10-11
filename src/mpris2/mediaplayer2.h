/***************************************************************************
   SPDX-FileCopyrightText: 2014 (c) Sujith Haridasan <sujith.haridasan@kdemail.net>
   SPDX-FileCopyrightText: 2014 (c) Ashish Madeti <ashishmadeti@gmail.com>
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: GPL-3.0-or-later
 ***************************************************************************/

#ifndef MEDIACENTER_MEDIAPLAYER2_H
#define MEDIACENTER_MEDIAPLAYER2_H

#include "elisaLib_export.h"

#include <QDBusAbstractAdaptor>
#include <QStringList>


class ELISALIB_EXPORT MediaPlayer2 : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2") // Docs: https://specifications.freedesktop.org/mpris-spec/latest/Media_Player.html

    Q_PROPERTY(bool CanQuit READ CanQuit CONSTANT)
    Q_PROPERTY(bool CanRaise READ CanRaise CONSTANT)
    Q_PROPERTY(bool HasTrackList READ HasTrackList CONSTANT)

    Q_PROPERTY(QString Identity READ Identity CONSTANT)
    Q_PROPERTY(QString DesktopEntry READ DesktopEntry CONSTANT)

    Q_PROPERTY(QStringList SupportedUriSchemes READ SupportedUriSchemes CONSTANT)
    Q_PROPERTY(QStringList SupportedMimeTypes READ SupportedMimeTypes CONSTANT)

public:
    explicit MediaPlayer2(QObject* parent = nullptr);
    ~MediaPlayer2() override;

    [[nodiscard]] bool CanQuit() const;
    [[nodiscard]] bool CanRaise() const;
    [[nodiscard]] bool HasTrackList() const;

    [[nodiscard]] QString Identity() const;
    [[nodiscard]] QString DesktopEntry() const;

    [[nodiscard]] QStringList SupportedUriSchemes() const;
    [[nodiscard]] QStringList SupportedMimeTypes() const;

public Q_SLOTS:
    void Quit();
    void Raise();

Q_SIGNALS:
    void raisePlayer();

};

#endif //MEDIACENTER_MEDIAPLAYER2_H
