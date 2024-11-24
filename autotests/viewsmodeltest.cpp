/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "databasetestdata.h"

#include "databaseinterface.h"
#include "models/viewsmodel.h"
#include "models/viewsproxymodel.h"
#include "viewslistdata.h"

#include <QAbstractItemModelTester>
#include <QHash>
#include <QObject>
#include <QStandardPaths>
#include <QString>
#include <QThread>
#include <QUrl>
#include <QList>

#include <QSignalSpy>
#include <QTest>

class ViewsModelTests : public QObject, public DatabaseTestData
{
    Q_OBJECT

private:
    const int mNumOfBaseViews = 9;

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<QHash<qulonglong, int>>("QHash<qulonglong,int>");
        qRegisterMetaType<QHash<QString, QUrl>>("QHash<QString,QUrl>");
        qRegisterMetaType<QList<qlonglong>>("QList<qlonglong>");
        qRegisterMetaType<QHash<qlonglong, int>>("QHash<qlonglong,int>");
    }

    void switchToEmbeddedGenreCategory()
    {
        DatabaseInterface musicDb;
        ViewsModel viewsModel;
        ViewsProxyModel viewsProxyModel;
        ViewsListData viewsPagesData;
        viewsModel.setViewsData(&viewsPagesData);
        viewsProxyModel.setSourceModel(&viewsModel);

        QAbstractItemModelTester testModel(&viewsModel);
        QAbstractItemModelTester testProxyModel(&viewsProxyModel);

        musicDb.init(QStringLiteral("testDb"));

        viewsModel.viewsData()->setDatabase(&musicDb);

        QSignalSpy beginInsertRowsSpy(&viewsModel, &ViewsModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&viewsModel, &ViewsModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&viewsModel, &ViewsModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&viewsModel, &ViewsModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&viewsModel, &ViewsModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews);

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Genre);
        viewsProxyModel.setEmbeddedCategory(ElisaUtils::Genre);

        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews + 4);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews - 1 + 4);

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Unknown);
        viewsProxyModel.setEmbeddedCategory(ElisaUtils::Unknown);

        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 1);
        QCOMPARE(endRemoveRowsSpy.count(), 1);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews);
    }

    void switchToEmbeddedGenreCategoryWithInsert()
    {
        DatabaseInterface musicDb;
        ViewsModel viewsModel;
        ViewsProxyModel viewsProxyModel;
        ViewsListData viewsPagesData;
        viewsModel.setViewsData(&viewsPagesData);
        viewsProxyModel.setSourceModel(&viewsModel);

        QAbstractItemModelTester testModel(&viewsModel);
        QAbstractItemModelTester testProxyModel(&viewsProxyModel);

        musicDb.init(QStringLiteral("testDb"));

        viewsModel.viewsData()->setDatabase(&musicDb);

        QSignalSpy beginInsertRowsSpy(&viewsModel, &ViewsModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&viewsModel, &ViewsModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&viewsModel, &ViewsModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&viewsModel, &ViewsModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&viewsModel, &ViewsModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews);

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Genre);
        viewsProxyModel.setEmbeddedCategory(ElisaUtils::Genre);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews - 1);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(beginInsertRowsSpy.count(), 4);
        QCOMPARE(endInsertRowsSpy.count(), 4);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews + 4);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews - 1 + 4);

        auto newTracks = DataTypes::ListTrackDataType{
        {true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album3"), {},
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/test/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
                QUrl::fromLocalFile(QStringLiteral("album3")), 5, true,
                QStringLiteral("genre7"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false},
        {true, QStringLiteral("$24"), QStringLiteral("0"), QStringLiteral("track7"),
                QStringLiteral("artist3"), QStringLiteral("album3"), {},
                7, 1, QTime::fromMSecsSinceStartOfDay(24), {QUrl::fromLocalFile(QStringLiteral("/test/$24"))},
                QDateTime::fromMSecsSinceEpoch(24),
                QUrl::fromLocalFile(QStringLiteral("album3")), 5, true,
                QStringLiteral("genre6"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false},
        {true, QStringLiteral("$25"), QStringLiteral("0"), QStringLiteral("track8"),
                QStringLiteral("artist4"), QStringLiteral("album3"), {},
                8, 1, QTime::fromMSecsSinceStartOfDay(25), {QUrl::fromLocalFile(QStringLiteral("/test/$25"))},
                QDateTime::fromMSecsSinceEpoch(25),
                QUrl::fromLocalFile(QStringLiteral("album3")), 5, true,
                QStringLiteral("genre5"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false}};

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("/test/$23")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        newCovers[QStringLiteral("/test/$24")] = QUrl::fromLocalFile(QStringLiteral("album3"));
        newCovers[QStringLiteral("/test/$25")] = QUrl::fromLocalFile(QStringLiteral("album3"));

        musicDb.insertTracksList(newTracks, newCovers);

        QCOMPARE(beginInsertRowsSpy.count(), 7);
        QCOMPARE(endInsertRowsSpy.count(), 7);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews + 7);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews - 1 + 7);

        const auto newTrack = DataTypes::TrackDataType {
            true, QStringLiteral("$26"), QStringLiteral("0"), QStringLiteral("track6"),
            QStringLiteral("artist2"), QStringLiteral("album3"), {},
            6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/test/$23"))},
            QDateTime::fromMSecsSinceEpoch(23),
            QUrl::fromLocalFile(QStringLiteral("album3")), 5, true,
            QStringLiteral("a genre"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false};

        const auto newCover = QHash<QString, QUrl>{{QStringLiteral("/test/$26"), QUrl::fromLocalFile(QStringLiteral("album3"))}};

        musicDb.insertTracksList({newTrack}, newCover);

        QCOMPARE(beginInsertRowsSpy.count(), 8);
        QCOMPARE(endInsertRowsSpy.count(), 8);
        QCOMPARE(beginRemoveRowsSpy.count(), 1);
        QCOMPARE(endRemoveRowsSpy.count(), 1);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews + 7);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews - 1 + 7);

        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(0, 0), Qt::DisplayRole), QStringLiteral("Now Playing"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(1, 0), Qt::DisplayRole), QStringLiteral("Recently Played"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(2, 0), Qt::DisplayRole), QStringLiteral("Frequently Played"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(3, 0), Qt::DisplayRole), QStringLiteral("Albums"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(4, 0), Qt::DisplayRole), QStringLiteral("Artists"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(5, 0), Qt::DisplayRole), QStringLiteral("Tracks"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(6, 0), Qt::DisplayRole), QStringLiteral("Files"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(7, 0), Qt::DisplayRole), QStringLiteral("Radio Stations"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(8, 0), Qt::DisplayRole), QStringLiteral("a genre"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(9, 0), Qt::DisplayRole), QStringLiteral("genre1"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(10, 0), Qt::DisplayRole), QStringLiteral("genre2"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(11, 0), Qt::DisplayRole), QStringLiteral("genre3"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(12, 0), Qt::DisplayRole), QStringLiteral("genre4"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(13, 0), Qt::DisplayRole), QStringLiteral("genre5"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(14, 0), Qt::DisplayRole), QStringLiteral("genre6"));

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Unknown);
        viewsProxyModel.setEmbeddedCategory(ElisaUtils::Unknown);

        QCOMPARE(beginInsertRowsSpy.count(), 8);
        QCOMPARE(endInsertRowsSpy.count(), 8);
        QCOMPARE(beginRemoveRowsSpy.count(), 2);
        QCOMPARE(endRemoveRowsSpy.count(), 2);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews);
    }

    void switchToEmbeddedAlbumCategoryWithInsertAndRemove()
    {
        DatabaseInterface musicDb;
        ViewsModel viewsModel;
        ViewsProxyModel viewsProxyModel;
        ViewsListData viewsPagesData;
        viewsModel.setViewsData(&viewsPagesData);
        viewsProxyModel.setSourceModel(&viewsModel);

        QAbstractItemModelTester testModel(&viewsModel);
        QAbstractItemModelTester testProxyModel(&viewsProxyModel);

        musicDb.init(QStringLiteral("testDb"));

        viewsModel.viewsData()->setDatabase(&musicDb);

        QSignalSpy beginInsertRowsSpy(&viewsModel, &ViewsModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&viewsModel, &ViewsModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&viewsModel, &ViewsModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&viewsModel, &ViewsModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&viewsModel, &ViewsModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews);

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Album);
        viewsProxyModel.setEmbeddedCategory(ElisaUtils::Album);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews - 1);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews + 5);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews - 1 + 5);

        auto newTracks = DataTypes::ListTrackDataType{
        {true, QStringLiteral("$23"), QStringLiteral("0"), QStringLiteral("track6"),
                QStringLiteral("artist2"), QStringLiteral("album8"), {},
                6, 1, QTime::fromMSecsSinceStartOfDay(23), {QUrl::fromLocalFile(QStringLiteral("/test/$23"))},
                QDateTime::fromMSecsSinceEpoch(23),
                QUrl::fromLocalFile(QStringLiteral("album8")), 5, true,
                QStringLiteral("genre7"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false},
        {true, QStringLiteral("$24"), QStringLiteral("0"), QStringLiteral("track7"),
                QStringLiteral("artist3"), QStringLiteral("album9"), {},
                7, 1, QTime::fromMSecsSinceStartOfDay(24), {QUrl::fromLocalFile(QStringLiteral("/test/$24"))},
                QDateTime::fromMSecsSinceEpoch(24),
                QUrl::fromLocalFile(QStringLiteral("album9")), 5, true,
                QStringLiteral("genre6"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false},
        {true, QStringLiteral("$25"), QStringLiteral("0"), QStringLiteral("track8"),
                QStringLiteral("artist4"), QStringLiteral("album10"), {},
                8, 1, QTime::fromMSecsSinceStartOfDay(25), {QUrl::fromLocalFile(QStringLiteral("/test/$25"))},
                QDateTime::fromMSecsSinceEpoch(25),
                QUrl::fromLocalFile(QStringLiteral("album10")), 5, true,
                QStringLiteral("genre5"), QStringLiteral("composer1"), QStringLiteral("lyricist1"), false}};

        auto newCovers = mNewCovers;
        newCovers[QStringLiteral("/test/$23")] = QUrl::fromLocalFile(QStringLiteral("album8"));
        newCovers[QStringLiteral("/test/$24")] = QUrl::fromLocalFile(QStringLiteral("album9"));
        newCovers[QStringLiteral("/test/$25")] = QUrl::fromLocalFile(QStringLiteral("album10"));

        musicDb.insertTracksList(newTracks, newCovers);

        QCOMPARE(beginInsertRowsSpy.count(), 2);
        QCOMPARE(endInsertRowsSpy.count(), 2);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews + 8);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews - 1 + 8);

        musicDb.removeTracksList({{QUrl::fromLocalFile(QStringLiteral("/test/$23"))},
                                  {QUrl::fromLocalFile(QStringLiteral("/test/$24"))},
                                  {QUrl::fromLocalFile(QStringLiteral("/test/$25"))}});

        QCOMPARE(beginInsertRowsSpy.count(), 2);
        QCOMPARE(endInsertRowsSpy.count(), 2);
        QCOMPARE(beginRemoveRowsSpy.count(), 3);
        QCOMPARE(endRemoveRowsSpy.count(), 3);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews + 5);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews - 1 + 5);

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Unknown);
        viewsProxyModel.setEmbeddedCategory(ElisaUtils::Unknown);

        QCOMPARE(beginInsertRowsSpy.count(), 2);
        QCOMPARE(endInsertRowsSpy.count(), 2);
        QCOMPARE(beginRemoveRowsSpy.count(), 4);
        QCOMPARE(endRemoveRowsSpy.count(), 4);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews);
    }

    void simpleSwitchToEmbeddedAlbumCategory()
    {
        DatabaseInterface musicDb;
        ViewsModel viewsModel;
        ViewsProxyModel viewsProxyModel;
        ViewsListData viewsPagesData;
        viewsModel.setViewsData(&viewsPagesData);
        viewsProxyModel.setSourceModel(&viewsModel);

        QAbstractItemModelTester testModel(&viewsModel);
        QAbstractItemModelTester testProxyModel(&viewsProxyModel);

        musicDb.init(QStringLiteral("testDb"));

        viewsModel.viewsData()->setDatabase(&musicDb);

        QSignalSpy beginInsertRowsSpy(&viewsModel, &ViewsModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&viewsModel, &ViewsModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&viewsModel, &ViewsModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&viewsModel, &ViewsModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&viewsModel, &ViewsModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews);

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Album);
        viewsProxyModel.setEmbeddedCategory(ElisaUtils::Album);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews - 1);

        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(0, 0), Qt::DisplayRole), QStringLiteral("Now Playing"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(1, 0), Qt::DisplayRole), QStringLiteral("Recently Played"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(2, 0), Qt::DisplayRole), QStringLiteral("Frequently Played"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(3, 0), Qt::DisplayRole), QStringLiteral("Artists"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(4, 0), Qt::DisplayRole), QStringLiteral("Tracks"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(5, 0), Qt::DisplayRole), QStringLiteral("Genres"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(6, 0), Qt::DisplayRole), QStringLiteral("Files"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(7, 0), Qt::DisplayRole), QStringLiteral("Radio Stations"));
    }

    void simpleSwitchToEmbeddedArtistCategory()
    {
        DatabaseInterface musicDb;
        ViewsModel viewsModel;
        ViewsProxyModel viewsProxyModel;
        ViewsListData viewsPagesData;
        viewsModel.setViewsData(&viewsPagesData);
        viewsProxyModel.setSourceModel(&viewsModel);

        QAbstractItemModelTester testModel(&viewsModel);
        QAbstractItemModelTester testProxyModel(&viewsProxyModel);

        musicDb.init(QStringLiteral("testDb"));

        viewsModel.viewsData()->setDatabase(&musicDb);

        QSignalSpy beginInsertRowsSpy(&viewsModel, &ViewsModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&viewsModel, &ViewsModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&viewsModel, &ViewsModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&viewsModel, &ViewsModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&viewsModel, &ViewsModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews);

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Artist);
        viewsProxyModel.setEmbeddedCategory(ElisaUtils::Artist);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews - 1);

        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(0, 0), Qt::DisplayRole), QStringLiteral("Now Playing"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(1, 0), Qt::DisplayRole), QStringLiteral("Recently Played"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(2, 0), Qt::DisplayRole), QStringLiteral("Frequently Played"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(3, 0), Qt::DisplayRole), QStringLiteral("Albums"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(4, 0), Qt::DisplayRole), QStringLiteral("Tracks"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(5, 0), Qt::DisplayRole), QStringLiteral("Genres"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(6, 0), Qt::DisplayRole), QStringLiteral("Files"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(7, 0), Qt::DisplayRole), QStringLiteral("Radio Stations"));
    }

    void simpleSwitchToEmbeddedGenreCategory()
    {
        DatabaseInterface musicDb;
        ViewsModel viewsModel;
        ViewsProxyModel viewsProxyModel;
        ViewsListData viewsPagesData;
        viewsModel.setViewsData(&viewsPagesData);
        viewsProxyModel.setSourceModel(&viewsModel);

        QAbstractItemModelTester testModel(&viewsModel);
        QAbstractItemModelTester testProxyModel(&viewsProxyModel);

        musicDb.init(QStringLiteral("testDb"));

        viewsModel.viewsData()->setDatabase(&musicDb);

        QSignalSpy beginInsertRowsSpy(&viewsModel, &ViewsModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&viewsModel, &ViewsModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&viewsModel, &ViewsModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&viewsModel, &ViewsModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&viewsModel, &ViewsModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews);

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Genre);
        viewsProxyModel.setEmbeddedCategory(ElisaUtils::Genre);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews - 1);

        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(0, 0), Qt::DisplayRole), QStringLiteral("Now Playing"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(1, 0), Qt::DisplayRole), QStringLiteral("Recently Played"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(2, 0), Qt::DisplayRole), QStringLiteral("Frequently Played"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(3, 0), Qt::DisplayRole), QStringLiteral("Albums"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(4, 0), Qt::DisplayRole), QStringLiteral("Artists"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(5, 0), Qt::DisplayRole), QStringLiteral("Tracks"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(6, 0), Qt::DisplayRole), QStringLiteral("Files"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(7, 0), Qt::DisplayRole), QStringLiteral("Radio Stations"));
    }

    void simpleSwitchToEmbeddedAlbumCategoryWithoutDatabase()
    {
        ViewsModel viewsModel;
        ViewsProxyModel viewsProxyModel;
        ViewsListData viewsPagesData;
        viewsModel.setViewsData(&viewsPagesData);
        viewsProxyModel.setSourceModel(&viewsModel);

        QAbstractItemModelTester testModel(&viewsModel);
        QAbstractItemModelTester testProxyModel(&viewsProxyModel);

        QSignalSpy beginInsertRowsSpy(&viewsModel, &ViewsModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&viewsModel, &ViewsModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&viewsModel, &ViewsModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&viewsModel, &ViewsModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&viewsModel, &ViewsModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews);

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Album);
        viewsProxyModel.setEmbeddedCategory(ElisaUtils::Album);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews - 1);

        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(0, 0), Qt::DisplayRole), QStringLiteral("Now Playing"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(1, 0), Qt::DisplayRole), QStringLiteral("Recently Played"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(2, 0), Qt::DisplayRole), QStringLiteral("Frequently Played"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(3, 0), Qt::DisplayRole), QStringLiteral("Artists"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(4, 0), Qt::DisplayRole), QStringLiteral("Tracks"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(5, 0), Qt::DisplayRole), QStringLiteral("Genres"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(6, 0), Qt::DisplayRole), QStringLiteral("Files"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(7, 0), Qt::DisplayRole), QStringLiteral("Radio Stations"));
    }

    void simpleSwitchToEmbeddedArtistCategoryWithoutDatabase()
    {
        ViewsModel viewsModel;
        ViewsProxyModel viewsProxyModel;
        ViewsListData viewsPagesData;
        viewsModel.setViewsData(&viewsPagesData);
        viewsProxyModel.setSourceModel(&viewsModel);

        QAbstractItemModelTester testModel(&viewsModel);
        QAbstractItemModelTester testProxyModel(&viewsProxyModel);

        QSignalSpy beginInsertRowsSpy(&viewsModel, &ViewsModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&viewsModel, &ViewsModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&viewsModel, &ViewsModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&viewsModel, &ViewsModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&viewsModel, &ViewsModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews);

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Artist);
        viewsProxyModel.setEmbeddedCategory(ElisaUtils::Artist);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews - 1);

        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(0, 0), Qt::DisplayRole), QStringLiteral("Now Playing"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(1, 0), Qt::DisplayRole), QStringLiteral("Recently Played"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(2, 0), Qt::DisplayRole), QStringLiteral("Frequently Played"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(3, 0), Qt::DisplayRole), QStringLiteral("Albums"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(4, 0), Qt::DisplayRole), QStringLiteral("Tracks"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(5, 0), Qt::DisplayRole), QStringLiteral("Genres"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(6, 0), Qt::DisplayRole), QStringLiteral("Files"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(7, 0), Qt::DisplayRole), QStringLiteral("Radio Stations"));
    }

    void simpleSwitchToEmbeddedGenreCategoryWithoutDatabase()
    {
        ViewsModel viewsModel;
        ViewsProxyModel viewsProxyModel;
        ViewsListData viewsPagesData;
        viewsModel.setViewsData(&viewsPagesData);
        viewsProxyModel.setSourceModel(&viewsModel);

        QAbstractItemModelTester testModel(&viewsModel);
        QAbstractItemModelTester testProxyModel(&viewsProxyModel);

        QSignalSpy beginInsertRowsSpy(&viewsModel, &ViewsModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&viewsModel, &ViewsModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&viewsModel, &ViewsModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&viewsModel, &ViewsModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&viewsModel, &ViewsModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews);

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Genre);
        viewsProxyModel.setEmbeddedCategory(ElisaUtils::Genre);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews - 1);

        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(0, 0), Qt::DisplayRole), QStringLiteral("Now Playing"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(1, 0), Qt::DisplayRole), QStringLiteral("Recently Played"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(2, 0), Qt::DisplayRole), QStringLiteral("Frequently Played"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(3, 0), Qt::DisplayRole), QStringLiteral("Albums"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(4, 0), Qt::DisplayRole), QStringLiteral("Artists"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(5, 0), Qt::DisplayRole), QStringLiteral("Tracks"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(6, 0), Qt::DisplayRole), QStringLiteral("Files"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(7, 0), Qt::DisplayRole), QStringLiteral("Radio Stations"));
    }

    void switchToEmbeddedAlbumCategoryAndToEmbeddedArtistCategory()
    {
        DatabaseInterface musicDb;
        ViewsModel viewsModel;
        ViewsProxyModel viewsProxyModel;
        ViewsListData viewsPagesData;
        viewsModel.setViewsData(&viewsPagesData);
        viewsProxyModel.setSourceModel(&viewsModel);

        QAbstractItemModelTester testModel(&viewsModel);
        QAbstractItemModelTester testProxyModel(&viewsProxyModel);

        musicDb.init(QStringLiteral("testDb"));

        viewsModel.viewsData()->setDatabase(&musicDb);

        QSignalSpy beginInsertRowsSpy(&viewsModel, &ViewsModel::rowsAboutToBeInserted);
        QSignalSpy endInsertRowsSpy(&viewsModel, &ViewsModel::rowsInserted);
        QSignalSpy beginRemoveRowsSpy(&viewsModel, &ViewsModel::rowsAboutToBeRemoved);
        QSignalSpy endRemoveRowsSpy(&viewsModel, &ViewsModel::rowsRemoved);
        QSignalSpy dataChangedSpy(&viewsModel, &ViewsModel::dataChanged);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews);

        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(0, 0), Qt::DisplayRole), QStringLiteral("Now Playing"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(1, 0), Qt::DisplayRole), QStringLiteral("Recently Played"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(2, 0), Qt::DisplayRole), QStringLiteral("Frequently Played"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(3, 0), Qt::DisplayRole), QStringLiteral("Albums"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(4, 0), Qt::DisplayRole), QStringLiteral("Artists"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(5, 0), Qt::DisplayRole), QStringLiteral("Tracks"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(6, 0), Qt::DisplayRole), QStringLiteral("Genres"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(7, 0), Qt::DisplayRole), QStringLiteral("Files"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(8, 0), Qt::DisplayRole), QStringLiteral("Radio Stations"));

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Album);
        viewsProxyModel.setEmbeddedCategory(ElisaUtils::Album);

        beginInsertRowsSpy.wait(100);

        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews + 5);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews - 1 + 5);

        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(0, 0), Qt::DisplayRole), QStringLiteral("Now Playing"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(1, 0), Qt::DisplayRole), QStringLiteral("Recently Played"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(2, 0), Qt::DisplayRole), QStringLiteral("Frequently Played"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(3, 0), Qt::DisplayRole), QStringLiteral("Artists"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(4, 0), Qt::DisplayRole), QStringLiteral("Tracks"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(5, 0), Qt::DisplayRole), QStringLiteral("Genres"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(6, 0), Qt::DisplayRole), QStringLiteral("Files"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(7, 0), Qt::DisplayRole), QStringLiteral("Radio Stations"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(8, 0), Qt::DisplayRole), QStringLiteral("album1"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(9, 0), Qt::DisplayRole), QStringLiteral("album2"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(10, 0), Qt::DisplayRole), QStringLiteral("album3"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(11, 0), Qt::DisplayRole), QStringLiteral("album3"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(12, 0), Qt::DisplayRole), QStringLiteral("album4"));

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Artist);
        viewsProxyModel.setEmbeddedCategory(ElisaUtils::Artist);

        beginInsertRowsSpy.wait(100);

        QCOMPARE(beginInsertRowsSpy.count(), 2);
        QCOMPARE(endInsertRowsSpy.count(), 2);
        QCOMPARE(beginRemoveRowsSpy.count(), 1);
        QCOMPARE(endRemoveRowsSpy.count(), 1);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews + 7);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews - 1 + 7);

        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(0, 0), Qt::DisplayRole), QStringLiteral("Now Playing"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(1, 0), Qt::DisplayRole), QStringLiteral("Recently Played"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(2, 0), Qt::DisplayRole), QStringLiteral("Frequently Played"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(3, 0), Qt::DisplayRole), QStringLiteral("Albums"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(4, 0), Qt::DisplayRole), QStringLiteral("Tracks"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(5, 0), Qt::DisplayRole), QStringLiteral("Genres"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(6, 0), Qt::DisplayRole), QStringLiteral("Files"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(7, 0), Qt::DisplayRole), QStringLiteral("Radio Stations"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(8, 0), Qt::DisplayRole), QStringLiteral("artist1"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(9, 0), Qt::DisplayRole), QStringLiteral("artist1 and artist2"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(10, 0), Qt::DisplayRole), QStringLiteral("artist2"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(11, 0), Qt::DisplayRole), QStringLiteral("artist3"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(12, 0), Qt::DisplayRole), QStringLiteral("artist4"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(13, 0), Qt::DisplayRole), QStringLiteral("artist7"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(14, 0), Qt::DisplayRole), QStringLiteral("Various Artists"));

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Unknown);
        viewsProxyModel.setEmbeddedCategory(ElisaUtils::Unknown);

        beginInsertRowsSpy.wait(100);

        QCOMPARE(beginInsertRowsSpy.count(), 2);
        QCOMPARE(endInsertRowsSpy.count(), 2);
        QCOMPARE(beginRemoveRowsSpy.count(), 2);
        QCOMPARE(endRemoveRowsSpy.count(), 2);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), mNumOfBaseViews);
        QCOMPARE(viewsProxyModel.rowCount(), mNumOfBaseViews);

        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(0, 0), Qt::DisplayRole), QStringLiteral("Now Playing"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(1, 0), Qt::DisplayRole), QStringLiteral("Recently Played"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(2, 0), Qt::DisplayRole), QStringLiteral("Frequently Played"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(3, 0), Qt::DisplayRole), QStringLiteral("Albums"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(4, 0), Qt::DisplayRole), QStringLiteral("Artists"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(5, 0), Qt::DisplayRole), QStringLiteral("Tracks"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(6, 0), Qt::DisplayRole), QStringLiteral("Genres"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(7, 0), Qt::DisplayRole), QStringLiteral("Files"));
        QCOMPARE(viewsProxyModel.data(viewsProxyModel.index(8, 0), Qt::DisplayRole), QStringLiteral("Radio Stations"));
    }
};

QTEST_GUILESS_MAIN(ViewsModelTests)

#include "viewsmodeltest.moc"
