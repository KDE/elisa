/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
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
