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

#include <KDirLister>
#include <QFile>
#include <QTemporaryDir>

#include <memory>

using namespace Qt::Literals::StringLiterals;

class FileBrowserProxyModelTest : public QObject
{
    Q_OBJECT

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

    void playlistFilesAreVisible()
    {
        QTemporaryDir directory;

        createFile(directory.filePath(u"a.m3u"_s), u"#EXTM3U\nsong.mp3\n"_s);
        createFile(directory.filePath(u"b.m3u8"_s), u"#EXTM3U\nsong.mp3\n"_s);
        createFile(directory.filePath(u"c.pls"_s), u"[playlist]\nFile1=song.mp3\n"_s);
        createFile(directory.filePath(u"song.mp3"_s),
                   QByteArrayLiteral("ID3\x04\x00\x00\x00\x00\x00\x00") + QByteArrayLiteral("\xff\xfb\x90\x00") + QByteArray(400, 0));
        createFile(directory.filePath(u"notes.txt"_s), u"not audio"_s);

        mFileModel->initializeByData(nullptr,
                                     nullptr,
                                     ElisaUtils::FileName,
                                     ElisaUtils::FilterByPath,
                                     {{DataTypes::FilePathRole, QUrl::fromLocalFile(directory.path())}});

        QTRY_COMPARE(mFileModel->rowCount(), 4);
        QCOMPARE(mFileProxyModel->rowCount(), 4);

        for (int row = 0; row < mFileModel->rowCount(); ++row) {
            const QModelIndex index = mFileModel->index(row, 0);
            const auto fullData = index.data(DataTypes::ColumnsRoles::FullDataRole).value<DataTypes::MusicDataType>();

            const QString name = fullData[DataTypes::ColumnsRoles::TitleRole].toString();
            const bool shouldBePlaylist = name.endsWith(u".m3u"_s) || name.endsWith(u".m3u8"_s) || name.endsWith(u".pls"_s);
            const bool isPlaylist = fullData.elementType() == ElisaUtils::PlayList;

            QCOMPARE(isPlaylist, shouldBePlaylist);
            QCOMPARE(index.data(DataTypes::ColumnsRoles::IsPlayListRole).toBool(), shouldBePlaylist);
        }
    }

private:
    static void createFile(const QString &path, const QString &contents)
    {
        QFile file(path);
        QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
        file.write(contents.toUtf8());
    }

    static void createFile(const QString &path, const QByteArray &contents)
    {
        QFile file(path);
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write(contents);
    }

    std::unique_ptr<FileBrowserModel> mFileModel;
    std::unique_ptr<FileBrowserProxyModel> mFileProxyModel;
    std::unique_ptr<QAbstractItemModelTester> mFileModelTester;
    std::unique_ptr<QAbstractItemModelTester> mFileProxyModelTester;

    std::unique_ptr<QSignalSpy> mEntriesToEnqueueSpy;
};

QTEST_GUILESS_MAIN(FileBrowserProxyModelTest)

#include "filebrowserproxymodeltest.moc"
