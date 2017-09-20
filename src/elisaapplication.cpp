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

#if defined KF5Config_FOUND && KF5Config_FOUND
#include "elisa_settings.h"
#include <KConfigWidgets/KStandardAction>
#include <KConfigCore/KAuthorized>
#endif

#if defined KF5XmlGui_FOUND && KF5XmlGui_FOUND
#include <KXmlGui/KActionCollection>
#include <KXmlGui/KAboutApplicationDialog>
#include <KXmlGui/KHelpMenu>
#include <KXmlGui/KBugReport>
#include <KXmlGui/KShortcutsDialog>
#endif

#if defined KF5KCMUtils_FOUND && KF5KCMUtils_FOUND
#include <KCMUtils/KCMultiDialog>
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

class ElisaApplicationPrivate
{
public:

    explicit ElisaApplicationPrivate(QObject *parent)
#if defined KF5XmlGui_FOUND && KF5XmlGui_FOUND
          : mCollection(parent)
#endif
    {
        Q_UNUSED(parent)
    }

#if defined KF5KCMUtils_FOUND && KF5KCMUtils_FOUND
    KCMultiDialog mConfigurationDialog;
#endif

#if defined KF5XmlGui_FOUND && KF5XmlGui_FOUND
    KActionCollection mCollection;
#endif

};

ElisaApplication::ElisaApplication(QObject *parent) : QObject(parent), d(std::make_unique<ElisaApplicationPrivate>(this))
{
    setupActions();

#if defined KF5KCMUtils_FOUND && KF5KCMUtils_FOUND
    d->mConfigurationDialog.addModule(QStringLiteral("kcm_elisa_local_file"));
#endif
}

ElisaApplication::~ElisaApplication()
= default;

void ElisaApplication::setupActions()
{
#if defined KF5XmlGui_FOUND && KF5XmlGui_FOUND
    auto quitAction = KStandardAction::quit(QCoreApplication::instance(), &QCoreApplication::quit, &d->mCollection);
    d->mCollection.addAction(QStringLiteral("file_quit"), quitAction);

    if (KAuthorized::authorizeAction(QStringLiteral("help_contents"))) {
        auto handBookAction = KStandardAction::helpContents(this, &ElisaApplication::appHelpActivated, &d->mCollection);
        d->mCollection.addAction(handBookAction->objectName(), handBookAction);
    }

    if (KAuthorized::authorizeAction(QStringLiteral("help_report_bug")) && !KAboutData::applicationData().bugAddress().isEmpty()) {
        auto reportBugAction = KStandardAction::reportBug(this, &ElisaApplication::reportBug, &d->mCollection);
        d->mCollection.addAction(reportBugAction->objectName(), reportBugAction);
    }

    if (KAuthorized::authorizeAction(QStringLiteral("help_about_app"))) {
        auto aboutAppAction = KStandardAction::aboutApp(this, &ElisaApplication::aboutApplication, this);
        d->mCollection.addAction(aboutAppAction->objectName(), aboutAppAction);
    }

    if (KAuthorized::authorizeAction(QStringLiteral("options_configure"))) {
        auto preferencesAction = KStandardAction::preferences(this, &ElisaApplication::configureElisa, this);
        d->mCollection.addAction(preferencesAction->objectName(), preferencesAction);
    }

    if (KAuthorized::authorizeAction(QStringLiteral("options_configure_keybinding"))) {
        auto keyBindignsAction = KStandardAction::keyBindings(this, &ElisaApplication::configureShortcuts, this);
        d->mCollection.addAction(keyBindignsAction->objectName(), keyBindignsAction);
    }

    d->mCollection.readSettings();
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
    dlg.addCollection(&d->mCollection);
    qDebug() << "saving shortcuts..." << dlg.configure(/*bSaveSettings*/);
#endif
}

void ElisaApplication::configureElisa()
{
#if defined KF5KCMUtils_FOUND && KF5KCMUtils_FOUND
    d->mConfigurationDialog.setModal(true);
    d->mConfigurationDialog.show();
#endif
}

QAction * ElisaApplication::action(const QString& name)
{
#if defined KF5XmlGui_FOUND && KF5XmlGui_FOUND
    return d->mCollection.action(name);
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
