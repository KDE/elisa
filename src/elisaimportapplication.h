/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef ELISAIMPORTAPPLICATION_H
#define ELISAIMPORTAPPLICATION_H

#include <QObject>

class ElisaImportApplication : public QObject
{
    Q_OBJECT
public:
    explicit ElisaImportApplication(QObject *parent = nullptr);

Q_SIGNALS:

public Q_SLOTS:

    void indexingChanged();

};

#endif // ELISAIMPORTAPPLICATION_H
