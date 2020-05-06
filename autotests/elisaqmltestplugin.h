/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef ELISAQMLTESTPLUGIN_H
#define ELISAQMLTESTPLUGIN_H

#include <QQmlExtensionPlugin>

class QQmlEngine;

class ElisaQmlTestPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void initializeEngine(QQmlEngine *engine, const char *uri) Q_DECL_OVERRIDE;
    void registerTypes(const char *uri) Q_DECL_OVERRIDE;
};

#endif // ELISAQMLTESTPLUGIN_H
