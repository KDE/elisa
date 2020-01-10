/*
 * Copyright 2015-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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
        qRegisterMetaType<ElisaUtils::EntryDataList>("ElisaUtils::EntryDataList");
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

        QCOMPARE(enqueueSpy.at(0).count(), 4);
        QCOMPARE(std::get<2>(enqueueSpy.at(0).at(0).value<ElisaUtils::EntryDataList>().at(0)), QUrl::fromLocalFile(myTestFile.canonicalFilePath()));
    }
};

QTEST_MAIN(ElisaApplicationTests)


#include "elisaapplicationtest.moc"
