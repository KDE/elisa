/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef ANDROIDMUSICLISTENER_H
#define ANDROIDMUSICLISTENER_H

#include "../abstractfile/abstractfilelistener.h"

#include <QObject>
#include <QVector>
#include <QString>

#include <memory>

class AndroidMusicListenerPrivate;

class AndroidMusicListener : public AbstractFileListener
{

    Q_OBJECT

public:

    explicit AndroidMusicListener(QObject *parent = nullptr);

    ~AndroidMusicListener() override;

Q_SIGNALS:

public Q_SLOTS:

private:

    std::unique_ptr<AndroidMusicListenerPrivate> d;

};

#endif // ANDROIDMUSICLISTENER_H
