/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef ELISAAPPLICATION_H
#define ELISAAPPLICATION_H

#include "config-upnp-qt.h"

#if defined KF5XmlGui_FOUND && KF5XmlGui_FOUND
#include <KXmlGui/KActionCollection>
#endif

#include <KCMultiDialog>

#include <QObject>
#include <QString>

class QIcon;
class QAction;

class ElisaApplication : public QObject
{
    Q_OBJECT
public:
    explicit ElisaApplication(QObject *parent = 0);

    void setupActions();

    Q_INVOKABLE QAction* action(const QString& name);

    Q_INVOKABLE QString iconName(const QIcon& icon);

Q_SIGNALS:

public Q_SLOTS:

    void appHelpActivated();

    void aboutApplication();

    void reportBug();

    void configureShortcuts();

    void configureElisa();

private:

#if defined KF5XmlGui_FOUND && KF5XmlGui_FOUND
    KActionCollection mCollection;
#endif

    KCMultiDialog mConfigurationDialog;

};

#endif // ELISAAPPLICATION_H
