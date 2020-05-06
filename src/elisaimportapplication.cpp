/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "elisaimportapplication.h"

#include <QCoreApplication>

ElisaImportApplication::ElisaImportApplication(QObject *parent) : QObject(parent)
{
}

void ElisaImportApplication::indexingChanged()
{
    static bool firstCall = true;

    if (firstCall) {
        firstCall = false;
    } else {
        QCoreApplication::quit();
    }
}


#include "moc_elisaimportapplication.cpp"
