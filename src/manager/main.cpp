/*
   SPDX-FileCopyrightText: 2021 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "elisaapplication.h"

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

#include <QQmlApplicationEngine>
#include <QJSEngine>
#include <QQmlFileSelector>
#include <QQuickStyle>
#include <QQmlContext>

int main(int argc, char* argv[])
{
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    qputenv("QT_GSTREAMER_USE_PLAYBIN_VOLUME", "true");

    QApplication app(argc, argv);

#if KF5Declarative_FOUND
    KQuickAddons::QtQuickSettings::init();
#endif

    KLocalizedString::setApplicationDomain("elisa");

#if KF5Crash_FOUND
    KCrash::initialize();
#endif

    KAboutData aboutData( QStringLiteral("elisaManager"),
                          i18n("Elisa Manager"),
                          QStringLiteral("0.0.1"),
                          i18n("A Simple Music Manager made with love by the KDE community"),
                          KAboutLicense::LGPL_V3,
                          i18n("(c) 2021, Elisa Manager contributors"));

    aboutData.addAuthor(QStringLiteral("Matthieu Gallien"),i18n("Creator"), QStringLiteral("mgallien@mgallien.fr"));

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

#if KF5Declarative_FOUND
    KDeclarative::KDeclarative decl;
    decl.setDeclarativeEngine(&engine);
    decl.setupEngine(&engine);
    decl.setupContext();
#endif

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));

    engine.load(QUrl(QStringLiteral("qrc:/qml/MainWindow.qml")));

    return app.exec();
}
