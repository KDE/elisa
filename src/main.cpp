/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "config-upnp-qt.h"
#include "elisa-version.h"

#include "colorschemepreviewimageprovider.h"
#include "elisaapplication.h"
#include "elisa_settings.h"

#include "localFileConfiguration/elisaconfigurationdialog.h"

#if KFFileMetaData_FOUND
#include "embeddedcoverageimageprovider.h"
#endif

//#define QT_QML_DEBUG

#include <KLocalizedString>
#include <KLocalizedContext>

#include <KAboutData>

#if KFCrash_FOUND
#include <KCrash>
#endif

#include <KQuickIconProvider>
#include <QIcon>

#include <QQmlContext>
#include <QApplication>
#include <QCommandLineParser>
#include <QStandardPaths>
#include <QSurfaceFormat>
#include <QDir>

#include <QQmlApplicationEngine>
#include <QQmlFileSelector>
#include <QQuickStyle>

#include <memory>

#ifdef Q_OS_ANDROID
#include "android/androidcoverimageprovider.h"

#include <QCoreApplication>
#include <QJniObject>
#include <QtCore/private/qandroidextras_p.h>
#endif

#ifdef Q_OS_ANDROID
int __attribute__((visibility("default"))) main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    auto format = QSurfaceFormat::defaultFormat();
    format.setOption(QSurfaceFormat::ResetNotification);
    QSurfaceFormat::setDefaultFormat(format);

#ifdef Q_OS_ANDROID
    if(argc > 1 && strcmp(argv[1], "-service") == 0){
        QAndroidService app(argc, argv);
        qInfo() << "Service starting...";

        // My service stuff

        return app.exec();
    }

    qInfo() << "Application starting...";

    qputenv("QT_LOGGING_RULES", "org.kde.elisa*=true");
#endif
    qputenv("QT_GSTREAMER_USE_PLAYBIN_VOLUME", "true");

    QApplication app(argc, argv);

#if defined Q_OS_WIN || defined Q_OS_MAC
    QApplication::setStyle(QStringLiteral("breeze"));
#endif

    KLocalizedString::setApplicationDomain(QByteArrayLiteral("elisa"));

#ifdef Q_OS_ANDROID
    qInfo() << QCoreApplication::arguments();

#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
    QJniObject::callStaticMethod<void>("org/kde/elisa/ElisaService", "startMyService", "(Landroid/content/Context;)V", QNativeInterface::QAndroidApplication::context());
#else
    QJniObject::callStaticMethod<void>("org/kde/elisa/ElisaService", "startMyService", QNativeInterface::QAndroidApplication::context());
#endif
#endif

#if KFCrash_FOUND
    KCrash::initialize();
#endif

    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("elisa")));

    KAboutData aboutData( QStringLiteral("elisa"),
                          i18nc("@title", "Elisa"),
                          QStringLiteral(ELISA_VERSION_STRING),
                          i18nc("@info", "A Simple Music Player made with love by the KDE community"),
                          KAboutLicense::LGPL_V3,
                          i18nc("@info", "(c) 2015-2020, Elisa contributors"));

    aboutData.addAuthor(QStringLiteral("Matthieu Gallien"),i18nc("@label", "Creator"), QStringLiteral("mgallien@mgallien.fr"));
    aboutData.addAuthor(QStringLiteral("Nate Graham"), i18nc("@label", "Author"), QStringLiteral("nate@kde.org"));

    aboutData.addCredit(QStringLiteral("Andrew Lake"), i18nc("@label", "Concept and design work"), QStringLiteral("jamboarder@gmail.com"));
    aboutData.addCredit(QStringLiteral("Luigi Toscano"), i18nc("@label", "Localization support"), QStringLiteral("luigi.toscano@tiscali.it"));
    aboutData.addCredit(QStringLiteral("Safa Alfulaij"), i18nc("@label", "Right to left support in interface"), QStringLiteral("safa1996alfulaij@gmail.com"));
    aboutData.addCredit(QStringLiteral("Diego Gangl"), i18nc("@label", "Various improvements to the interface"), QStringLiteral("diego@sinestesia.co"));
    aboutData.addCredit(QStringLiteral("Jérôme Guidon"), i18nc("@label", "Support for online radios"), QStringLiteral("guidon@live.fr"));

    KAboutData::setApplicationData(aboutData);

    qmlRegisterSingletonType("org.kde.elisa.about", 1, 0, "About", [](QQmlEngine *engine, QJSEngine *) -> QJSValue {
        return engine->toScriptValue(KAboutData::applicationData());
    });

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);


    // If styling has not been set via QT_QUICK_CONTROLS_STYLE, default to some
    // styles that make sense for desktop and mobile form factors.
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
#ifdef Q_OS_ANDROID
        QQuickStyle::setStyle(QStringLiteral("org.kde.breeze"));
        QQuickStyle::setFallbackStyle(QStringLiteral("Material"));
#else
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
        QQuickStyle::setFallbackStyle(QStringLiteral("Fusion"));
#endif
    }

    QQmlApplicationEngine engine;
    engine.addImportPath(QStringLiteral("qrc:/imports"));
    QQmlFileSelector selector(&engine);

    // Allow image:// icon URLs to be loaded as images im QML
    engine.addImageProvider(QStringLiteral("icon"), new KQuickIconProvider);

#if KFFileMetaData_FOUND
    engine.addImageProvider(QStringLiteral("cover"), new EmbeddedCoverageImageProvider);
#endif

#ifdef Q_OS_ANDROID
    engine.addImageProvider(QStringLiteral("android"), new AndroidCoverImageProvider);
#endif

    engine.addImageProvider(QStringLiteral("colorScheme"), new ColorSchemePreviewImageProvider);

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));

    QList<QUrl> urls;
    for (const auto &oneArgument : parser.positionalArguments()) {
        urls.push_back(QUrl::fromUserInput(oneArgument, QDir::currentPath()));
    }

    engine.rootContext()->setContextProperty(QStringLiteral("elisaStartupArguments"), QVariant::fromValue(urls));

    engine.loadFromModule("org.kde.elisa", "ElisaMainWindow");

    return app.exec();
}
