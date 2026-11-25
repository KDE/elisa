/*
   SPDX-FileCopyrightText: 2026 (c) Asish Kumar <officialasishkumar@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "models/gridviewproxymodel.h"
#include "datatypes.h"

#include <QAbstractItemModelTester>
#include <QStandardItemModel>
#include <QTest>

#include <memory>

using namespace Qt::Literals::StringLiterals;

class GridViewProxyModelTest : public QObject
{
    Q_OBJECT

private:
    struct Entry {
        QString artist;
        qlonglong albumId;
        int discNumber;
        int trackNumber;
    };

    void addEntry(const Entry &entry)
    {
        auto item = std::make_unique<QStandardItem>(entry.artist);
        item->setData(entry.albumId, DataTypes::AlbumIdRole);
        item->setData(entry.discNumber, DataTypes::DiscNumberRole);
        item->setData(entry.trackNumber, DataTypes::TrackNumberRole);
        mSourceModel->appendRow(item.release());
    }

    [[nodiscard]] QList<Entry> sortedEntries() const
    {
        QList<Entry> result;
        for (int row = 0; row < mProxyModel->rowCount(); ++row) {
            const auto index = mProxyModel->index(row, 0);
            result.push_back({index.data(Qt::DisplayRole).toString(),
                              index.data(DataTypes::AlbumIdRole).toLongLong(),
                              index.data(DataTypes::DiscNumberRole).toInt(),
                              index.data(DataTypes::TrackNumberRole).toInt()});
        }
        return result;
    }

    [[nodiscard]] static QStringList describe(const QList<Entry> &entries)
    {
        QStringList result;
        for (const auto &entry : entries) {
            result.push_back(u"%1/%2/%3/%4"_s.arg(entry.artist).arg(entry.albumId).arg(entry.discNumber).arg(entry.trackNumber));
        }
        return result;
    }

private Q_SLOTS:

    void init()
    {
        mSourceModel = std::make_unique<QStandardItemModel>();
        mProxyModel = std::make_unique<GridViewProxyModel>();
        mProxyModel->setSourceModel(mSourceModel.get());
        mProxyModelTester = std::make_unique<QAbstractItemModelTester>(mProxyModel.get());
    }

    void cleanup()
    {
        mProxyModelTester.reset();
        mProxyModel.reset();
        mSourceModel.reset();
    }

    void equalKeysAreOrderedByAlbumDiscAndTrack_data()
    {
        QTest::addColumn<Qt::SortOrder>("sortOrder");

        QTest::addRow("ascending") << Qt::AscendingOrder;
        QTest::addRow("descending") << Qt::DescendingOrder;
    }

    void equalKeysAreOrderedByAlbumDiscAndTrack()
    {
        QFETCH(Qt::SortOrder, sortOrder);

        addEntry({u"artist"_s, 2, 1, 1});
        addEntry({u"artist"_s, 1, 2, 2});
        addEntry({u"artist"_s, 1, 1, 2});
        addEntry({u"artist"_s, 1, 2, 1});
        addEntry({u"artist"_s, 1, 1, 1});

        mProxyModel->sort(0, sortOrder);

        // the secondary keys always use their natural order, even when the primary
        // key is sorted in descending order
        QCOMPARE(describe(sortedEntries()), (QStringList{u"artist/1/1/1"_s, u"artist/1/1/2"_s, u"artist/1/2/1"_s, u"artist/1/2/2"_s, u"artist/2/1/1"_s}));
    }

    void differentPrimaryKeysTakePrecedence_data()
    {
        QTest::addColumn<Qt::SortOrder>("sortOrder");
        QTest::addColumn<QStringList>("expected");

        QTest::addRow("ascending") << Qt::AscendingOrder << QStringList{u"a/2/1/2"_s, u"a/2/1/3"_s, u"b/1/1/1"_s, u"b/1/2/1"_s};
        QTest::addRow("descending") << Qt::DescendingOrder << QStringList{u"b/1/1/1"_s, u"b/1/2/1"_s, u"a/2/1/2"_s, u"a/2/1/3"_s};
    }

    void differentPrimaryKeysTakePrecedence()
    {
        QFETCH(Qt::SortOrder, sortOrder);
        QFETCH(QStringList, expected);

        addEntry({u"b"_s, 1, 2, 1});
        addEntry({u"a"_s, 2, 1, 3});
        addEntry({u"b"_s, 1, 1, 1});
        addEntry({u"a"_s, 2, 1, 2});

        mProxyModel->sort(0, sortOrder);

        QCOMPARE(describe(sortedEntries()), expected);
    }

    void identicalEntriesAreAllKept_data()
    {
        QTest::addColumn<Qt::SortOrder>("sortOrder");

        QTest::addRow("ascending") << Qt::AscendingOrder;
        QTest::addRow("descending") << Qt::DescendingOrder;
    }

    void identicalEntriesAreAllKept()
    {
        QFETCH(Qt::SortOrder, sortOrder);

        for (int i = 0; i < 50; ++i) {
            addEntry({u"artist"_s, 1, 1, 1});
        }

        mProxyModel->sort(0, sortOrder);

        QCOMPARE(mProxyModel->rowCount(), 50);
    }

private:
    std::unique_ptr<QStandardItemModel> mSourceModel;
    std::unique_ptr<GridViewProxyModel> mProxyModel;
    std::unique_ptr<QAbstractItemModelTester> mProxyModelTester;
};

QTEST_GUILESS_MAIN(GridViewProxyModelTest)

#include "gridviewproxymodeltest.moc"
