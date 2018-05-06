/*
 * Copyright 2015-2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "config-upnp-qt.h"

#include "elisaapplication.h"
#include "elisa_settings.h"

//#define QT_QML_DEBUG

#if defined KF5Declarative_FOUND && KF5Declarative_FOUND
#include <KDeclarative/KDeclarative>
#endif

#include <KI18n/KLocalizedString>
#include <KI18n/KLocalizedContext>

#include <KCoreAddons/KAboutData>

#if defined KF5Crash_FOUND && KF5Crash_FOUND
#include <KCrash>
#endif

#if defined KF5DBusAddons_FOUND && KF5DBusAddons_FOUND
#include <KDBusService>
#endif

#include <QSortFilterProxyModel>

#include <QIcon>

#include <QApplication>
#include <QCommandLineParser>
#include <QtGlobal>
#include <QStandardPaths>

#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlFileSelector>
#include <QQmlDebuggingEnabler>
#include <QQmlContext>
#include <QScreen>

#include <memory>

#if defined Qt5AndroidExtras_FOUND && Qt5AndroidExtras_FOUND
#include <QAndroidJniObject>
#include <QtAndroid>
#endif

#if defined Q_OS_ANDROID
int __attribute__((visibility("default"))) main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    qputenv("QT_GSTREAMER_USE_PLAYBIN_VOLUME", "true");

    QApplication app(argc, argv);

#if defined KF5Crash_FOUND && KF5Crash_FOUND
    KCrash::initialize();
#endif

    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("elisa")));

    KLocalizedString::setApplicationDomain("elisa");

    KAboutData aboutData( QStringLiteral("elisa"),
                          i18n("Elisa"),
                          QStringLiteral("0.1.80"),
                          i18n("A Simple Music Player written with KDE Frameworks"),
                          KAboutLicense::LGPL_V3,
                          i18n("(c) 2015-2018, Elisa contributors"));

    aboutData.addAuthor(QStringLiteral("Matthieu Gallien"),i18n("Creator"), QStringLiteral("mgallien@mgallien.fr"));
    aboutData.addAuthor(QStringLiteral("Alexander Stippich"), i18n("Author"), QStringLiteral("a.stippich@gmx.net"));

    aboutData.addCredit(QStringLiteral("Andrew Lake"), i18n("Concept and design work"), QStringLiteral("jamboarder@gmail.com"));
    aboutData.addCredit(QStringLiteral("Luigi Toscano"), i18n("Localization support"), QStringLiteral("luigi.toscano@tiscali.it"));
    aboutData.addCredit(QStringLiteral("Safa Alfulaij"), i18n("Right to left support in interface"), QStringLiteral("safa1996alfulaij@gmail.com"));
    aboutData.addCredit(QStringLiteral("Diego Gangl"), i18n("Various improvements to the interface"), QStringLiteral("diego@sinestesia.co"));

    KAboutData::setApplicationData(aboutData);

    KLocalizedString::setApplicationDomain("elisa");

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

#if defined Qt5AndroidExtras_FOUND && Qt5AndroidExtras_FOUND
    qDebug() << QCoreApplication::arguments();

    QAndroidJniObject::callStaticMethod<void>("com/kde/elisa/ElisaService",
                                              "startMyService",
                                              "(Landroid/content/Context;)V",
                                              QtAndroid::androidContext().object());
#endif

    QQmlApplicationEngine engine;
    engine.addImportPath(QStringLiteral("qrc:/imports"));
    QQmlFileSelector selector(&engine);

#if defined KF5Declarative_FOUND && KF5Declarative_FOUND
    KDeclarative::KDeclarative decl;
    decl.setDeclarativeEngine(&engine);
    decl.setupBindings();
#endif

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.rootContext()->setContextProperty(QStringLiteral("logicalDpi"), QGuiApplication::primaryScreen()->logicalDotsPerInch());

#if defined KF5DBusAddons_FOUND && KF5DBusAddons_FOUND
    KDBusService elisaService(KDBusService::Unique);
#endif

    std::unique_ptr<ElisaApplication> myApp = std::make_unique<ElisaApplication>();

#if defined KF5DBusAddons_FOUND && KF5DBusAddons_FOUND
    QObject::connect(&elisaService, &KDBusService::activateActionRequested, myApp.get(), &ElisaApplication::activateActionRequested);
    QObject::connect(&elisaService, &KDBusService::activateRequested, myApp.get(), &ElisaApplication::activateRequested);
    QObject::connect(&elisaService, &KDBusService::openRequested, myApp.get(), &ElisaApplication::openRequested);
#endif

    myApp->setArguments(parser.positionalArguments());

    engine.rootContext()->setContextProperty(QStringLiteral("elisa"), myApp.release());

    engine.load(QUrl(QStringLiteral("qrc:/qml/ElisaMainWindow.qml")));

    return app.exec();
}
