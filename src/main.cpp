/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "config-upnp-qt.h"
#include "elisa-version.h"

#include "elisaapplication.h"
#include "elisa_settings.h"

#include "localFileConfiguration/elisaconfigurationdialog.h"

//#define QT_QML_DEBUG

#if KF5Declarative_FOUND
#include <KDeclarative/KDeclarative>
#include <KQuickAddons/QtQuickSettings>
#endif

#include <KLocalizedString>
#include <KLocalizedContext>

#include <KAboutData>

#if KF5Crash_FOUND
#include <KCrash>
#endif


#include <QIcon>

#include <QApplication>
#include <QCommandLineParser>
#include <QStandardPaths>
#include <QDir>

#include <QQmlApplicationEngine>
#include <QJSEngine>
#include <QQmlFileSelector>
#include <QQuickStyle>
#include <QQmlContext>

#if Qt5AndroidExtras_FOUND
#include <QAndroidService>
#endif

#include <memory>

#if Qt5AndroidExtras_FOUND
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

    qputenv("QT_LOGGING_RULES", "org.kde.elisa*=true");
#endif
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    qputenv("QT_GSTREAMER_USE_PLAYBIN_VOLUME", "true");

    QApplication app(argc, argv);

#if defined Q_OS_ANDROID || defined Q_OS_MAC
    QApplication::setStyle(QStringLiteral("breeze"));
#endif

#if KF5Declarative_FOUND
    KQuickAddons::QtQuickSettings::init();
#endif

    KLocalizedString::setApplicationDomain("elisa");

#if Qt5AndroidExtras_FOUND
    qInfo() << QCoreApplication::arguments();

    QAndroidJniObject::callStaticMethod<void>("org/kde/elisa/ElisaService",
                                              "startMyService",
                                              "(Landroid/content/Context;)V",
                                              QtAndroid::androidContext().object());
#endif

#if KF5Crash_FOUND
    KCrash::initialize();
#endif

    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("elisa")));

    KAboutData aboutData( QStringLiteral("elisa"),
                          i18n("Elisa"),
                          QStringLiteral(ELISA_VERSION_STRING),
                          i18n("A Simple Music Player made with love by the KDE community"),
                          KAboutLicense::LGPL_V3,
                          i18n("(c) 2015-2020, Elisa contributors"));

    aboutData.addAuthor(QStringLiteral("Matthieu Gallien"),i18n("Creator"), QStringLiteral("mgallien@mgallien.fr"));
    aboutData.addAuthor(QStringLiteral("Nate Graham"), i18n("Author"), QStringLiteral("nate@kde.org"));

    aboutData.addCredit(QStringLiteral("Andrew Lake"), i18n("Concept and design work"), QStringLiteral("jamboarder@gmail.com"));
    aboutData.addCredit(QStringLiteral("Luigi Toscano"), i18n("Localization support"), QStringLiteral("luigi.toscano@tiscali.it"));
    aboutData.addCredit(QStringLiteral("Safa Alfulaij"), i18n("Right to left support in interface"), QStringLiteral("safa1996alfulaij@gmail.com"));
    aboutData.addCredit(QStringLiteral("Diego Gangl"), i18n("Various improvements to the interface"), QStringLiteral("diego@sinestesia.co"));
    aboutData.addCredit(QStringLiteral("Jérôme Guidon"), i18n("Support for online radios"), QStringLiteral("guidon@live.fr"));

    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

#if defined Q_OS_ANDROID
    QQuickStyle::setStyle(QStringLiteral("Material"));
#else
    QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    QQuickStyle::setFallbackStyle(QStringLiteral("Fusion"));
#endif

    QQmlApplicationEngine engine;
    engine.addImportPath(QStringLiteral("qrc:/imports"));
    QQmlFileSelector selector(&engine);

#if KF5Declarative_FOUND
    KDeclarative::KDeclarative decl;
    decl.setDeclarativeEngine(&engine);
    decl.setupEngine(&engine);
    decl.setupContext();
#endif

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));

    QList<QUrl> urls;
    for (const auto &oneArgument : parser.positionalArguments()) {
        urls.push_back(QUrl::fromUserInput(oneArgument, QDir::currentPath()));
    }

    engine.rootContext()->setContextProperty(QStringLiteral("elisaStartupArguments"), QVariant::fromValue(urls));

    engine.load(QUrl(QStringLiteral("qrc:/qml/ElisaMainWindow.qml")));

    return app.exec();
}
