/*
 * Copyright 2015-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "config-upnp-qt.h"

#include <QObject>
#include <QUrl>
#include <QString>
#include <QDir>
#include <QFileInfo>

#include <QDebug>

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
    }

    void setArgumentsTest()
    {
        ElisaApplication myApp;

        QSignalSpy argumentsChangedSpy(&myApp, &ElisaApplication::argumentsChanged);
        QSignalSpy enqueueSpy(&myApp, &ElisaApplication::enqueue);

        auto myTestFile = QFileInfo(QStringLiteral(LOCAL_FILE_TESTS_SAMPLE_FILES_PATH) + QStringLiteral("/music/test.ogg"));
        auto myDirectory = QDir::current();
        auto relativePath = myDirectory.relativeFilePath(myTestFile.canonicalFilePath());

        myApp.setArguments({relativePath});

        QCOMPARE(argumentsChangedSpy.count(), 1);
        QCOMPARE(enqueueSpy.count(), 1);

        QCOMPARE(myApp.arguments().count(), 1);
        QCOMPARE(myApp.arguments().at(0), myTestFile.canonicalFilePath());
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

        QCOMPARE(enqueueSpy.at(0).count(), 1);
        QCOMPARE(enqueueSpy.at(0).at(0).toString(), myTestFile.canonicalFilePath());
    }
};

QTEST_MAIN(ElisaApplicationTests)


#include "elisaapplicationtest.moc"
