/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 * Copyright (C) 2012 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
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

#include "elisaapplication.h"

#if defined KF5XmlGui_FOUND && KF5XmlGui_FOUND
#include <KXmlGui/KAboutApplicationDialog>
#include <KXmlGui/KHelpMenu>
#include <KXmlGui/KBugReport>
#include <KXmlGui/KShortcutsDialog>
#endif

#if defined KF5ConfigWidgets_FOUND && KF5ConfigWidgets_FOUND
#include <KConfigWidgets/KStandardAction>
#endif

#if defined KF5Config_FOUND && KF5Config_FOUND
#include <KConfigCore/KAuthorized>
#endif

#if defined KF5CoreAddons_FOUND && KF5CoreAddons_FOUND
#include <KCoreAddons/KAboutData>
#endif

#include <QCoreApplication>
#include <QDesktopServices>
#include <QDialog>
#include <QPointer>
#include <QIcon>
#include <QAction>
#include <QUrl>

#include <QDebug>

ElisaApplication::ElisaApplication(QObject *parent) : QObject(parent)
#if defined KF5XmlGui_FOUND && KF5XmlGui_FOUND
  , mCollection(this)
#endif
{
    setupActions();
}

void ElisaApplication::setupActions()
{
#if defined KF5XmlGui_FOUND && KF5XmlGui_FOUND
    auto quitAction = KStandardAction::quit(QCoreApplication::instance(), &QCoreApplication::quit, &mCollection);
    mCollection.addAction(QStringLiteral("file_quit"), quitAction);

    if (KAuthorized::authorizeAction(QStringLiteral("help_contents"))) {
        auto mHandBookAction = KStandardAction::helpContents(this, &ElisaApplication::appHelpActivated, &mCollection);
        mCollection.addAction(mHandBookAction->objectName(), mHandBookAction);
    }

    if (KAuthorized::authorizeAction(QStringLiteral("help_report_bug")) && !KAboutData::applicationData().bugAddress().isEmpty()) {
        auto mReportBugAction = KStandardAction::reportBug(this, &ElisaApplication::reportBug, &mCollection);
        mCollection.addAction(mReportBugAction->objectName(), mReportBugAction);
    }

    if (KAuthorized::authorizeAction(QStringLiteral("help_about_app"))) {
        auto mAboutAppAction = KStandardAction::aboutApp(this, &ElisaApplication::aboutApplication, this);
        mCollection.addAction(mAboutAppAction->objectName(), mAboutAppAction);
    }

    auto mKeyBindignsAction = KStandardAction::keyBindings(this, &ElisaApplication::configureShortcuts, this);
    mCollection.addAction(mKeyBindignsAction->objectName(), mKeyBindignsAction);

    mCollection.readSettings();
#endif
}

void ElisaApplication::appHelpActivated()
{
    QDesktopServices::openUrl(QUrl(QStringLiteral("help:/")));
}

void ElisaApplication::aboutApplication()
{
#if defined KF5XmlGui_FOUND && KF5XmlGui_FOUND
    static QPointer<QDialog> dialog;
    if (!dialog) {
        dialog = new KAboutApplicationDialog(KAboutData::applicationData(), nullptr);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
    }
    dialog->show();
#endif
}

void ElisaApplication::reportBug()
{
#if defined KF5XmlGui_FOUND && KF5XmlGui_FOUND
    static QPointer<QDialog> dialog;
    if (!dialog) {
        dialog = new KBugReport(KAboutData::applicationData(), nullptr);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
    }
    dialog->show();
#endif
}

void ElisaApplication::configureShortcuts()
{
#if defined KF5XmlGui_FOUND && KF5XmlGui_FOUND
    KShortcutsDialog dlg(KShortcutsEditor::AllActions, KShortcutsEditor::LetterShortcutsAllowed, nullptr);
    dlg.setModal(true);
    dlg.addCollection(&mCollection);
    qDebug() << "saving shortcuts..." << dlg.configure(/*bSaveSettings*/);
#endif
}

QAction * ElisaApplication::action(const QString& name)
{
#if defined KF5XmlGui_FOUND && KF5XmlGui_FOUND
    return mCollection.action(name);
#else
    Q_UNUSED(name);

    return new QAction();
#endif
}

QString ElisaApplication::iconName(const QIcon& icon)
{
    return icon.name();
}


#include "moc_elisaapplication.cpp"
