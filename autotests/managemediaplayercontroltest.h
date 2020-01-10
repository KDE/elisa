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

#ifndef MANAGEMEDIAPLAYERCONTROLTEST_H
#define MANAGEMEDIAPLAYERCONTROLTEST_H

#include <QObject>

#include "databasetestdata.h"

#include <QHash>

class ManageMediaPlayerControlTest : public QObject, public DatabaseTestData
{

    Q_OBJECT

public:

    explicit ManageMediaPlayerControlTest(QObject *parent = nullptr);

Q_SIGNALS:

private Q_SLOTS:

    void initTestCase();

    void simpleInitialCase();

    void testPlayingCase();

    void testTracksCase();
};

#endif // MANAGEMEDIAPLAYERCONTROLTEST_H
