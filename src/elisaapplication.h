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

#include <QObject>
#include <QString>

#include <memory>

class QIcon;
class QAction;
class ElisaApplicationPrivate;

class ElisaApplication : public QObject
{

    Q_OBJECT

    Q_PROPERTY(QStringList arguments
               READ arguments
               WRITE setArguments
               NOTIFY argumentsChanged)

public:
    explicit ElisaApplication(QObject *parent = nullptr);

    ~ElisaApplication() override;

    void setupActions();

    Q_INVOKABLE QAction* action(const QString& name);

    Q_INVOKABLE QString iconName(const QIcon& icon);

    const QStringList &arguments() const;

Q_SIGNALS:

    void argumentsChanged();

    void enqueue(const QStringList &files);

public Q_SLOTS:

    void appHelpActivated();

    void aboutApplication();

    void reportBug();

    void configureShortcuts();

    void configureElisa();

    void setArguments(const QStringList &newArguments);

    void activateActionRequested(const QString &actionName, const QVariant &parameter);

    void activateRequested(const QStringList &arguments, const QString &workingDirectory);

    void openRequested(const QList< QUrl > &uris);


private Q_SLOTS:

    void goBack();

private:

    QStringList checkFileListAndMakeAbsolute(const QStringList &filesList, const QString &workingDirectory) const;

    std::unique_ptr<ElisaApplicationPrivate> d;

};

#endif // ELISAAPPLICATION_H
