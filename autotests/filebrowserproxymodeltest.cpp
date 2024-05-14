/*
   SPDX-FileCopyrightText: 2024 (c) Jack Hill <jackhill3103@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "config-upnp-qt.h"
#include "models/filebrowsermodel.h"
#include "models/filebrowserproxymodel.h"

#include <QAbstractItemModelTester>
#include <QSignalSpy>
#include <QTest>

#include <memory>

using namespace Qt::Literals::StringLiterals;

class FileBrowserProxyModelTest : public QObject
{

    Q_OBJECT

public:


    FileBrowserProxyModelTest(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private Q_SLOTS:

    void init()
    {
        mFileModel = std::make_unique<FileBrowserModel>();
        mFileProxyModel = std::make_unique<FileBrowserProxyModel>();
        mFileProxyModel->setSourceModel(mFileModel.get());
        mFileModelTester = std::make_unique<QAbstractItemModelTester>(mFileModel.get());
        mFileProxyModelTester = std::make_unique<QAbstractItemModelTester>(mFileProxyModel.get());

        mEntriesToEnqueueSpy = std::make_unique<QSignalSpy>(mFileProxyModel.get(), &FileBrowserProxyModel::entriesToEnqueue);
    }

    void enqueueFromEmptyDirectory()
    {
        const auto emptyDirectory = QUrl::fromLocalFile(QStringLiteral(LOCAL_FILE_TESTS_SAMPLE_FILES_PATH) + u"/empty"_s);
        mFileModel->initializeByData(nullptr, nullptr, ElisaUtils::FileName, ElisaUtils::FilterByPath, {{DataTypes::FilePathRole, emptyDirectory}});

        QCOMPARE(mFileModel->rowCount(), 0);
        QCOMPARE(mFileProxyModel->rowCount(), 0);
        QVERIFY(mEntriesToEnqueueSpy->isEmpty());

        mFileProxyModel->enqueueAll(ElisaUtils::AppendPlayList, ElisaUtils::DoNotTriggerPlay);

        QCOMPARE(mEntriesToEnqueueSpy->count(), 1);
        QVariantList arguments = mEntriesToEnqueueSpy->takeFirst();
        QCOMPARE(arguments.count(), 3);
        QVERIFY(arguments.at(0).toList().isEmpty());
        QCOMPARE(arguments.at(1).toInt(), static_cast<int>(ElisaUtils::AppendPlayList));
        QCOMPARE(arguments.at(2).toInt(), static_cast<int>(ElisaUtils::DoNotTriggerPlay));

        mFileProxyModel->enqueueAll(ElisaUtils::ReplacePlayList, ElisaUtils::TriggerPlay);

        QCOMPARE(mEntriesToEnqueueSpy->count(), 1);
        arguments = mEntriesToEnqueueSpy->takeFirst();
        QCOMPARE(arguments.count(), 3);
        QVERIFY(arguments.at(0).toList().isEmpty());
        QCOMPARE(arguments.at(1).toInt(), static_cast<int>(ElisaUtils::ReplacePlayList));
        QCOMPARE(arguments.at(2).toInt(), static_cast<int>(ElisaUtils::TriggerPlay));
    }

private:

    std::unique_ptr<FileBrowserModel> mFileModel;
    std::unique_ptr<FileBrowserProxyModel> mFileProxyModel;
    std::unique_ptr<QAbstractItemModelTester> mFileModelTester;
    std::unique_ptr<QAbstractItemModelTester> mFileProxyModelTester;

    std::unique_ptr<QSignalSpy> mEntriesToEnqueueSpy;

};

QTEST_GUILESS_MAIN(FileBrowserProxyModelTest)

#include "filebrowserproxymodeltest.moc"
