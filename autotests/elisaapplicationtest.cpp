/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "elisaapplication.h"

#include "config-upnp-qt.h"

#include <QObject>
#include <QUrl>
#include <QString>
#include <QDir>
#include <QFileInfo>


#include <QtTest>

class ElisaApplicationTests: public QObject
{
    Q_OBJECT

public:

    ElisaApplicationTests(QObject *parent = nullptr) : QObject(parent)
    {
    }

private:

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<QHash<qulonglong,int>>("QHash<qulonglong,int>");
        qRegisterMetaType<QHash<QString,QUrl>>("QHash<QString,QUrl>");
        qRegisterMetaType<QVector<qlonglong>>("QVector<qlonglong>");
        qRegisterMetaType<QHash<qlonglong,int>>("QHash<qlonglong,int>");
        qRegisterMetaType<QList<QUrl>>("QList<QUrl>");
        qRegisterMetaType<DataTypes::EntryDataList>("DataTypes::EntryDataList");
        qRegisterMetaType<ElisaUtils::PlayListEntryType>("ElisaUtils::PlayListEntryType");
        qRegisterMetaType<ElisaUtils::PlayListEnqueueMode>("ElisaUtils::PlayListEnqueueMode");
        qRegisterMetaType<ElisaUtils::PlayListEnqueueTriggerPlay>("ElisaUtils::PlayListEnqueueTriggerPlay");
    }

    void setArgumentsTest()
    {
        ElisaApplication myApp;

        QSignalSpy argumentsChangedSpy(&myApp, &ElisaApplication::argumentsChanged);
        QSignalSpy enqueueSpy(&myApp, &ElisaApplication::enqueue);

        auto myTestFile = QFileInfo(QStringLiteral(LOCAL_FILE_TESTS_SAMPLE_FILES_PATH) + QStringLiteral("/music/test.ogg"));
        auto myDirectory = QDir::current();
        auto relativePath = myDirectory.relativeFilePath(myTestFile.canonicalFilePath());

        myApp.setArguments({{{}, {}, QUrl::fromLocalFile(relativePath)}});

        QCOMPARE(argumentsChangedSpy.count(), 1);
        QCOMPARE(enqueueSpy.count(), 1);

        QCOMPARE(myApp.arguments().count(), 1);
        QCOMPARE(std::get<2>(myApp.arguments().at(0)), QUrl::fromLocalFile(myTestFile.canonicalFilePath()));
    }

    void activateRequestedTest()
    {
        ElisaApplication myApp;

        QSignalSpy argumentsChangedSpy(&myApp, &ElisaApplication::argumentsChanged);
        QSignalSpy enqueueSpy(&myApp, &ElisaApplication::enqueue);

        auto myTestFile = QFileInfo(QStringLiteral(LOCAL_FILE_TESTS_SAMPLE_FILES_PATH) + QStringLiteral("/music/test.ogg"));
        auto myDirectory = QDir::current();
        auto relativePath = myDirectory.relativeFilePath(myTestFile.canonicalFilePath());

        myApp.activateRequested({QString(), relativePath}, QDir::currentPath());

        QCOMPARE(argumentsChangedSpy.count(), 0);
        QCOMPARE(enqueueSpy.count(), 1);

        QCOMPARE(enqueueSpy.at(0).count(), 3);
        QCOMPARE(std::get<2>(enqueueSpy.at(0).at(0).value<DataTypes::EntryDataList>().at(0)), QUrl::fromLocalFile(myTestFile.canonicalFilePath()));
    }
};

QTEST_MAIN(ElisaApplicationTests)


#include "elisaapplicationtest.moc"
