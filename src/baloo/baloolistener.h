/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef BALOOLISTENER_H
#define BALOOLISTENER_H

#include "../abstractfile/abstractfilelistener.h"

#include <QObject>

#include <memory>

class BalooListenerPrivate;

class BalooListener : public AbstractFileListener
{
    Q_OBJECT

public:
    explicit BalooListener(QObject *parent = nullptr);

    ~BalooListener() override;

Q_SIGNALS:

public Q_SLOTS:

private:

    std::unique_ptr<BalooListenerPrivate> d;

};

#endif // BALOOLISTENER_H
