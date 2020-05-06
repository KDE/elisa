/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef ELISAQMLPLUGIN_H
#define ELISAQMLPLUGIN_H

#include <QQmlExtensionPlugin>

class QQmlEngine;

class ElisaQmlTestPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    explicit ElisaQmlTestPlugin(QObject *aParent = nullptr);

    void initializeEngine(QQmlEngine *engine, const char *uri) override;
    void registerTypes(const char *uri) override;
};

#endif // ELISAQMLPLUGIN_H
