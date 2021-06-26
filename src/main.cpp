/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "config-upnp-qt.h"
#include "elisa-version.h"

#include "elisaarguments.h"
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


#include <QIcon>

#include <QApplication>
#include <QCommandLineParser>
#include <QStandardPaths>

#include <QQmlApplicationEngine>
#include <QJSEngine>
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

    qputenv("QT_LOGGING_RULES", "org.kde.elisa*=true");
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

#if defined KF5Declarative_FOUND && KF5Declarative_FOUND
    KDeclarative::KDeclarative decl;
    decl.setDeclarativeEngine(&engine);
    decl.setupEngine(&engine);
    decl.setupContext();
#endif

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));

    auto arguments = DataTypes::EntryDataList{};
    auto realArgumentsList = parser.positionalArguments();

    for (const auto &oneArgument : realArgumentsList) {
        arguments.push_back(DataTypes::EntryData{{{DataTypes::ElementTypeRole, ElisaUtils::FileName},
                                                  {DataTypes::ResourceRole, QUrl::fromUserInput(oneArgument)}}, {}, {}});
    }

    int typeId = qmlRegisterSingletonType<ElisaArguments>("org.kde.elisa.host", 1, 0, "ElisaArguments", [](QQmlEngine *qmlEngine, QJSEngine *scriptEngine) -> QObject* {
        Q_UNUSED(qmlEngine)
        Q_UNUSED(scriptEngine)

        auto *result = new ElisaArguments;

        return result;
    });

    auto *argumentsSingleton = engine.singletonInstance<ElisaArguments*>(typeId);

    argumentsSingleton->setArguments(arguments);

    engine.load(QUrl(QStringLiteral("qrc:/qml/ElisaMainWindow.qml")));

    return app.exec();
}
