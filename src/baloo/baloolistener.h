/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
