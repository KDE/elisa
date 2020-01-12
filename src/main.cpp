/*
 * Copyright 2015-2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "config-upnp-qt.h"
#include "elisa-version.h"

#include "elisaapplication.h"
#include "elisa_settings.h"

#include "localFileConfiguration/elisaconfigurationdialog.h"

//#define QT_QML_DEBUG

#if defined KF5Declarative_FOUND && KF5Declarative_FOUND
#include <KDeclarative/KDeclarative>
#include <KQuickAddons/QtQuickSettings>
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


#include <QIcon>

#include <QApplication>
#include <QCommandLineParser>
#include <QStandardPaths>

#include <QQmlApplicationEngine>
#include <QQmlFileSelector>
#include <QQuickStyle>
#include <QQmlContext>

#if defined Qt5AndroidExtras_FOUND && Qt5AndroidExtras_FOUND
#include <QAndroidService>
#endif

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
#if defined Q_OS_ANDROID
    if(argc > 1 && strcmp(argv[1], "-service") == 0){
        QAndroidService app(argc, argv);
        qInfo() << "Service starting...";

        // My service stuff

        return app.exec();
    }

    qInfo() << "Application starting...";
#endif

    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    qputenv("QT_GSTREAMER_USE_PLAYBIN_VOLUME", "true");

    QApplication app(argc, argv);

#if defined KF5Declarative_FOUND && KF5Declarative_FOUND
    KQuickAddons::QtQuickSettings::init();
#endif

    KLocalizedString::setApplicationDomain("elisa");

#if defined Qt5AndroidExtras_FOUND && Qt5AndroidExtras_FOUND
    qInfo() << QCoreApplication::arguments();

    QAndroidJniObject::callStaticMethod<void>("org/kde/elisa/ElisaService",
                                              "startMyService",
                                              "(Landroid/content/Context;)V",
                                              QtAndroid::androidContext().object());
#endif

#if defined KF5Crash_FOUND && KF5Crash_FOUND
    KCrash::initialize();
#endif

    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("elisa")));

    KAboutData aboutData( QStringLiteral("elisa"),
                          i18n("Elisa"),
                          QStringLiteral(ELISA_VERSION_STRING),
                          i18n("A Simple Music Player written with KDE Frameworks"),
                          KAboutLicense::LGPL_V3,
                          i18n("(c) 2015-2019, Elisa contributors"));

    aboutData.addAuthor(QStringLiteral("Matthieu Gallien"),i18n("Creator"), QStringLiteral("mgallien@mgallien.fr"));
    aboutData.addAuthor(QStringLiteral("Alexander Stippich"), i18n("Author"), QStringLiteral("a.stippich@gmx.net"));

    aboutData.addCredit(QStringLiteral("Andrew Lake"), i18n("Concept and design work"), QStringLiteral("jamboarder@gmail.com"));
    aboutData.addCredit(QStringLiteral("Luigi Toscano"), i18n("Localization support"), QStringLiteral("luigi.toscano@tiscali.it"));
    aboutData.addCredit(QStringLiteral("Safa Alfulaij"), i18n("Right to left support in interface"), QStringLiteral("safa1996alfulaij@gmail.com"));
    aboutData.addCredit(QStringLiteral("Diego Gangl"), i18n("Various improvements to the interface"), QStringLiteral("diego@sinestesia.co"));

    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    QQuickStyle::setFallbackStyle(QStringLiteral("Fusion"));

    QQmlApplicationEngine engine;
    engine.addImportPath(QStringLiteral("qrc:/imports"));
    QQmlFileSelector selector(&engine);

#if defined KF5Declarative_FOUND && KF5Declarative_FOUND
    KDeclarative::KDeclarative decl;
    decl.setDeclarativeEngine(&engine);
    decl.setupEngine(&engine);
    decl.setupContext();
#endif

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));

#if defined KF5DBusAddons_FOUND && KF5DBusAddons_FOUND
    KDBusService elisaService(KDBusService::Unique);
#endif

    std::unique_ptr<ElisaApplication> myApp = std::make_unique<ElisaApplication>();

    myApp->setQmlEngine(&engine);

#if defined KF5DBusAddons_FOUND && KF5DBusAddons_FOUND
    QObject::connect(&elisaService, &KDBusService::activateActionRequested, myApp.get(), &ElisaApplication::activateActionRequested);
    QObject::connect(&elisaService, &KDBusService::activateRequested, myApp.get(), &ElisaApplication::activateRequested);
    QObject::connect(&elisaService, &KDBusService::openRequested, myApp.get(), &ElisaApplication::openRequested);
#endif

    auto arguments = ElisaUtils::EntryDataList{};
    auto realArgumentsList = parser.positionalArguments();

    for (const auto &oneArgument : realArgumentsList) {
        arguments.push_back(ElisaUtils::EntryData{{}, {}, QUrl(oneArgument)});
    }

    myApp->setArguments(arguments);

    engine.rootContext()->setContextProperty(QStringLiteral("elisa"), myApp.release());
    engine.rootContext()->setContextProperty(QStringLiteral("config"), new ElisaConfigurationDialog);

    engine.load(QUrl(QStringLiteral("qrc:/qml/ElisaMainWindow.qml")));

    return app.exec();
}
