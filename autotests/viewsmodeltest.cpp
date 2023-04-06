/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "databasetestdata.h"

#include "databaseinterface.h"
#include "models/viewsmodel.h"
#include "viewslistdata.h"

#include <QAbstractItemModelTester>
#include <QHash>
#include <QObject>
#include <QStandardPaths>
#include <QString>
#include <QThread>
#include <QUrl>
#include <QVector>


#include <QtTest>

class ViewsModelTests : public QObject, public DatabaseTestData
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<QHash<qulonglong, int>>("QHash<qulonglong,int>");
        qRegisterMetaType<QHash<QString, QUrl>>("QHash<QString,QUrl>");
        qRegisterMetaType<QVector<qlonglong>>("QVector<qlonglong>");
        qRegisterMetaType<QHash<qlonglong, int>>("QHash<qlonglong,int>");
    }

    void switchToEmbeddedGenreCategory()
    {
        DatabaseInterface musicDb;
        ViewsModel viewsModel;
        ViewsListData viewsPagesData;
        viewsModel.setViewsData(&viewsPagesData);

        QAbstractItemModelTester testModel(&viewsModel);

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
        QCOMPARE(viewsModel.rowCount(), 10);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), 10);

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Genre);

        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 1);
        QCOMPARE(endRemoveRowsSpy.count(), 1);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), 13);

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Unknown);

        QCOMPARE(beginInsertRowsSpy.count(), 2);
        QCOMPARE(endInsertRowsSpy.count(), 2);
        QCOMPARE(beginRemoveRowsSpy.count(), 5);
        QCOMPARE(endRemoveRowsSpy.count(), 5);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), 10);
    }

    void switchToEmbeddedGenreCategoryWithInsert()
    {
        DatabaseInterface musicDb;
        ViewsModel viewsModel;
        ViewsListData viewsPagesData;
        viewsModel.setViewsData(&viewsPagesData);

        QAbstractItemModelTester testModel(&viewsModel);

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
        QCOMPARE(viewsModel.rowCount(), 10);

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Genre);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 1);
        QCOMPARE(endRemoveRowsSpy.count(), 1);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), 9);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(beginInsertRowsSpy.count(), 4);
        QCOMPARE(endInsertRowsSpy.count(), 4);
        QCOMPARE(beginRemoveRowsSpy.count(), 1);
        QCOMPARE(endRemoveRowsSpy.count(), 1);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), 13);

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
        QCOMPARE(beginRemoveRowsSpy.count(), 1);
        QCOMPARE(endRemoveRowsSpy.count(), 1);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), 16);

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Unknown);

        QCOMPARE(beginInsertRowsSpy.count(), 8);
        QCOMPARE(endInsertRowsSpy.count(), 8);
        QCOMPARE(beginRemoveRowsSpy.count(), 8);
        QCOMPARE(endRemoveRowsSpy.count(), 8);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), 10);
    }

    void switchToEmbeddedAlbumCategoryWithInsertAndRemove()
    {
        DatabaseInterface musicDb;
        ViewsModel viewsModel;
        ViewsListData viewsPagesData;
        viewsModel.setViewsData(&viewsPagesData);

        QAbstractItemModelTester testModel(&viewsModel);

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
        QCOMPARE(viewsModel.rowCount(), 10);

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Album);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 1);
        QCOMPARE(endRemoveRowsSpy.count(), 1);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), 9);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 1);
        QCOMPARE(endRemoveRowsSpy.count(), 1);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), 14);

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
        QCOMPARE(beginRemoveRowsSpy.count(), 1);
        QCOMPARE(endRemoveRowsSpy.count(), 1);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), 17);

        musicDb.removeTracksList({{QUrl::fromLocalFile(QStringLiteral("/test/$23"))},
                                  {QUrl::fromLocalFile(QStringLiteral("/test/$24"))},
                                  {QUrl::fromLocalFile(QStringLiteral("/test/$25"))}});

        QCOMPARE(beginInsertRowsSpy.count(), 2);
        QCOMPARE(endInsertRowsSpy.count(), 2);
        QCOMPARE(beginRemoveRowsSpy.count(), 4);
        QCOMPARE(endRemoveRowsSpy.count(), 4);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), 14);

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Unknown);

        QCOMPARE(beginInsertRowsSpy.count(), 3);
        QCOMPARE(endInsertRowsSpy.count(), 3);
        QCOMPARE(beginRemoveRowsSpy.count(), 9);
        QCOMPARE(endRemoveRowsSpy.count(), 9);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), 10);
    }

    void simpleSwitchToEmbeddedAlbumCategory()
    {
        DatabaseInterface musicDb;
        ViewsModel viewsModel;
        ViewsListData viewsPagesData;
        viewsModel.setViewsData(&viewsPagesData);

        QAbstractItemModelTester testModel(&viewsModel);

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
        QCOMPARE(viewsModel.rowCount(), 10);

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Album);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 1);
        QCOMPARE(endRemoveRowsSpy.count(), 1);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), 9);

        QCOMPARE(viewsModel.data(viewsModel.index(0, 0), Qt::DisplayRole), QStringLiteral("Now Playing"));
        QCOMPARE(viewsModel.data(viewsModel.index(1, 0), Qt::DisplayRole), QStringLiteral("Recently Played"));
        QCOMPARE(viewsModel.data(viewsModel.index(2, 0), Qt::DisplayRole), QStringLiteral("Frequently Played"));
        QCOMPARE(viewsModel.data(viewsModel.index(3, 0), Qt::DisplayRole), QStringLiteral("Artists"));
        QCOMPARE(viewsModel.data(viewsModel.index(4, 0), Qt::DisplayRole), QStringLiteral("Tracks"));
        QCOMPARE(viewsModel.data(viewsModel.index(5, 0), Qt::DisplayRole), QStringLiteral("Genres"));
        QCOMPARE(viewsModel.data(viewsModel.index(6, 0), Qt::DisplayRole), QStringLiteral("Files"));
        QCOMPARE(viewsModel.data(viewsModel.index(7, 0), Qt::DisplayRole), QStringLiteral("Radio Stations"));
        QCOMPARE(viewsModel.data(viewsModel.index(8, 0), Qt::DisplayRole), QStringLiteral("UPnP Shares"));
    }

    void simpleSwitchToEmbeddedArtistCategory()
    {
        DatabaseInterface musicDb;
        ViewsModel viewsModel;
        ViewsListData viewsPagesData;
        viewsModel.setViewsData(&viewsPagesData);

        QAbstractItemModelTester testModel(&viewsModel);

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
        QCOMPARE(viewsModel.rowCount(), 10);

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Artist);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 1);
        QCOMPARE(endRemoveRowsSpy.count(), 1);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), 9);

        QCOMPARE(viewsModel.data(viewsModel.index(0, 0), Qt::DisplayRole), QStringLiteral("Now Playing"));
        QCOMPARE(viewsModel.data(viewsModel.index(1, 0), Qt::DisplayRole), QStringLiteral("Recently Played"));
        QCOMPARE(viewsModel.data(viewsModel.index(2, 0), Qt::DisplayRole), QStringLiteral("Frequently Played"));
        QCOMPARE(viewsModel.data(viewsModel.index(3, 0), Qt::DisplayRole), QStringLiteral("Albums"));
        QCOMPARE(viewsModel.data(viewsModel.index(4, 0), Qt::DisplayRole), QStringLiteral("Tracks"));
        QCOMPARE(viewsModel.data(viewsModel.index(5, 0), Qt::DisplayRole), QStringLiteral("Genres"));
        QCOMPARE(viewsModel.data(viewsModel.index(6, 0), Qt::DisplayRole), QStringLiteral("Files"));
        QCOMPARE(viewsModel.data(viewsModel.index(7, 0), Qt::DisplayRole), QStringLiteral("Radio Stations"));
        QCOMPARE(viewsModel.data(viewsModel.index(8, 0), Qt::DisplayRole), QStringLiteral("UPnP Shares"));
    }

    void simpleSwitchToEmbeddedGenreCategory()
    {
        DatabaseInterface musicDb;
        ViewsModel viewsModel;
        ViewsListData viewsPagesData;
        viewsModel.setViewsData(&viewsPagesData);

        QAbstractItemModelTester testModel(&viewsModel);

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
        QCOMPARE(viewsModel.rowCount(), 10);

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Genre);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 1);
        QCOMPARE(endRemoveRowsSpy.count(), 1);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), 9);

        QCOMPARE(viewsModel.data(viewsModel.index(0, 0), Qt::DisplayRole), QStringLiteral("Now Playing"));
        QCOMPARE(viewsModel.data(viewsModel.index(1, 0), Qt::DisplayRole), QStringLiteral("Recently Played"));
        QCOMPARE(viewsModel.data(viewsModel.index(2, 0), Qt::DisplayRole), QStringLiteral("Frequently Played"));
        QCOMPARE(viewsModel.data(viewsModel.index(3, 0), Qt::DisplayRole), QStringLiteral("Albums"));
        QCOMPARE(viewsModel.data(viewsModel.index(4, 0), Qt::DisplayRole), QStringLiteral("Artists"));
        QCOMPARE(viewsModel.data(viewsModel.index(5, 0), Qt::DisplayRole), QStringLiteral("Tracks"));
        QCOMPARE(viewsModel.data(viewsModel.index(6, 0), Qt::DisplayRole), QStringLiteral("Files"));
        QCOMPARE(viewsModel.data(viewsModel.index(7, 0), Qt::DisplayRole), QStringLiteral("Radio Stations"));
        QCOMPARE(viewsModel.data(viewsModel.index(8, 0), Qt::DisplayRole), QStringLiteral("UPnP Shares"));
    }

    void simpleSwitchToEmbeddedAlbumCategoryWithoutDatabase()
    {
        ViewsModel viewsModel;
        ViewsListData viewsPagesData;
        viewsModel.setViewsData(&viewsPagesData);

        QAbstractItemModelTester testModel(&viewsModel);

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
        QCOMPARE(viewsModel.rowCount(), 10);

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Album);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 1);
        QCOMPARE(endRemoveRowsSpy.count(), 1);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), 9);

        QCOMPARE(viewsModel.data(viewsModel.index(0, 0), Qt::DisplayRole), QStringLiteral("Now Playing"));
        QCOMPARE(viewsModel.data(viewsModel.index(1, 0), Qt::DisplayRole), QStringLiteral("Recently Played"));
        QCOMPARE(viewsModel.data(viewsModel.index(2, 0), Qt::DisplayRole), QStringLiteral("Frequently Played"));
        QCOMPARE(viewsModel.data(viewsModel.index(3, 0), Qt::DisplayRole), QStringLiteral("Artists"));
        QCOMPARE(viewsModel.data(viewsModel.index(4, 0), Qt::DisplayRole), QStringLiteral("Tracks"));
        QCOMPARE(viewsModel.data(viewsModel.index(5, 0), Qt::DisplayRole), QStringLiteral("Genres"));
        QCOMPARE(viewsModel.data(viewsModel.index(6, 0), Qt::DisplayRole), QStringLiteral("Files"));
        QCOMPARE(viewsModel.data(viewsModel.index(7, 0), Qt::DisplayRole), QStringLiteral("Radio Stations"));
        QCOMPARE(viewsModel.data(viewsModel.index(8, 0), Qt::DisplayRole), QStringLiteral("UPnP Shares"));
    }

    void simpleSwitchToEmbeddedArtistCategoryWithoutDatabase()
    {
        ViewsModel viewsModel;
        ViewsListData viewsPagesData;
        viewsModel.setViewsData(&viewsPagesData);

        QAbstractItemModelTester testModel(&viewsModel);

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
        QCOMPARE(viewsModel.rowCount(), 10);

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Artist);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 1);
        QCOMPARE(endRemoveRowsSpy.count(), 1);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), 9);

        QCOMPARE(viewsModel.data(viewsModel.index(0, 0), Qt::DisplayRole), QStringLiteral("Now Playing"));
        QCOMPARE(viewsModel.data(viewsModel.index(1, 0), Qt::DisplayRole), QStringLiteral("Recently Played"));
        QCOMPARE(viewsModel.data(viewsModel.index(2, 0), Qt::DisplayRole), QStringLiteral("Frequently Played"));
        QCOMPARE(viewsModel.data(viewsModel.index(3, 0), Qt::DisplayRole), QStringLiteral("Albums"));
        QCOMPARE(viewsModel.data(viewsModel.index(4, 0), Qt::DisplayRole), QStringLiteral("Tracks"));
        QCOMPARE(viewsModel.data(viewsModel.index(5, 0), Qt::DisplayRole), QStringLiteral("Genres"));
        QCOMPARE(viewsModel.data(viewsModel.index(6, 0), Qt::DisplayRole), QStringLiteral("Files"));
        QCOMPARE(viewsModel.data(viewsModel.index(7, 0), Qt::DisplayRole), QStringLiteral("Radio Stations"));
        QCOMPARE(viewsModel.data(viewsModel.index(8, 0), Qt::DisplayRole), QStringLiteral("UPnP Shares"));
    }

    void simpleSwitchToEmbeddedGenreCategoryWithoutDatabase()
    {
        ViewsModel viewsModel;
        ViewsListData viewsPagesData;
        viewsModel.setViewsData(&viewsPagesData);

        QAbstractItemModelTester testModel(&viewsModel);

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
        QCOMPARE(viewsModel.rowCount(), 10);

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Genre);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 1);
        QCOMPARE(endRemoveRowsSpy.count(), 1);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), 9);

        QCOMPARE(viewsModel.data(viewsModel.index(0, 0), Qt::DisplayRole), QStringLiteral("Now Playing"));
        QCOMPARE(viewsModel.data(viewsModel.index(1, 0), Qt::DisplayRole), QStringLiteral("Recently Played"));
        QCOMPARE(viewsModel.data(viewsModel.index(2, 0), Qt::DisplayRole), QStringLiteral("Frequently Played"));
        QCOMPARE(viewsModel.data(viewsModel.index(3, 0), Qt::DisplayRole), QStringLiteral("Albums"));
        QCOMPARE(viewsModel.data(viewsModel.index(4, 0), Qt::DisplayRole), QStringLiteral("Artists"));
        QCOMPARE(viewsModel.data(viewsModel.index(5, 0), Qt::DisplayRole), QStringLiteral("Tracks"));
        QCOMPARE(viewsModel.data(viewsModel.index(6, 0), Qt::DisplayRole), QStringLiteral("Files"));
        QCOMPARE(viewsModel.data(viewsModel.index(7, 0), Qt::DisplayRole), QStringLiteral("Radio Stations"));
        QCOMPARE(viewsModel.data(viewsModel.index(8, 0), Qt::DisplayRole), QStringLiteral("UPnP Shares"));
    }

    void switchToEmbeddedAlbumCategoryAndToEmbeddedArtistCategory()
    {
        DatabaseInterface musicDb;
        ViewsModel viewsModel;
        ViewsListData viewsPagesData;
        viewsModel.setViewsData(&viewsPagesData);

        QAbstractItemModelTester testModel(&viewsModel);

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
        QCOMPARE(viewsModel.rowCount(), 10);

        musicDb.insertTracksList(mNewTracks, mNewCovers);

        QCOMPARE(beginInsertRowsSpy.count(), 0);
        QCOMPARE(endInsertRowsSpy.count(), 0);
        QCOMPARE(beginRemoveRowsSpy.count(), 0);
        QCOMPARE(endRemoveRowsSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), 10);

        QCOMPARE(viewsModel.data(viewsModel.index(0, 0), Qt::DisplayRole), QStringLiteral("Now Playing"));
        QCOMPARE(viewsModel.data(viewsModel.index(1, 0), Qt::DisplayRole), QStringLiteral("Recently Played"));
        QCOMPARE(viewsModel.data(viewsModel.index(2, 0), Qt::DisplayRole), QStringLiteral("Frequently Played"));
        QCOMPARE(viewsModel.data(viewsModel.index(3, 0), Qt::DisplayRole), QStringLiteral("Albums"));
        QCOMPARE(viewsModel.data(viewsModel.index(4, 0), Qt::DisplayRole), QStringLiteral("Artists"));
        QCOMPARE(viewsModel.data(viewsModel.index(5, 0), Qt::DisplayRole), QStringLiteral("Tracks"));
        QCOMPARE(viewsModel.data(viewsModel.index(6, 0), Qt::DisplayRole), QStringLiteral("Genres"));
        QCOMPARE(viewsModel.data(viewsModel.index(7, 0), Qt::DisplayRole), QStringLiteral("Files"));
        QCOMPARE(viewsModel.data(viewsModel.index(8, 0), Qt::DisplayRole), QStringLiteral("Radio Stations"));
        QCOMPARE(viewsModel.data(viewsModel.index(9, 0), Qt::DisplayRole), QStringLiteral("UPnP Shares"));

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Album);

        beginInsertRowsSpy.wait(100);

        QCOMPARE(beginInsertRowsSpy.count(), 1);
        QCOMPARE(endInsertRowsSpy.count(), 1);
        QCOMPARE(beginRemoveRowsSpy.count(), 1);
        QCOMPARE(endRemoveRowsSpy.count(), 1);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), 14);

        QCOMPARE(viewsModel.data(viewsModel.index(0, 0), Qt::DisplayRole), QStringLiteral("Now Playing"));
        QCOMPARE(viewsModel.data(viewsModel.index(1, 0), Qt::DisplayRole), QStringLiteral("Recently Played"));
        QCOMPARE(viewsModel.data(viewsModel.index(2, 0), Qt::DisplayRole), QStringLiteral("Frequently Played"));
        QCOMPARE(viewsModel.data(viewsModel.index(3, 0), Qt::DisplayRole), QStringLiteral("Artists"));
        QCOMPARE(viewsModel.data(viewsModel.index(4, 0), Qt::DisplayRole), QStringLiteral("Tracks"));
        QCOMPARE(viewsModel.data(viewsModel.index(5, 0), Qt::DisplayRole), QStringLiteral("Genres"));
        QCOMPARE(viewsModel.data(viewsModel.index(6, 0), Qt::DisplayRole), QStringLiteral("Files"));
        QCOMPARE(viewsModel.data(viewsModel.index(7, 0), Qt::DisplayRole), QStringLiteral("Radio Stations"));
        QCOMPARE(viewsModel.data(viewsModel.index(8, 0), Qt::DisplayRole), QStringLiteral("UPnP Shares"));
        QCOMPARE(viewsModel.data(viewsModel.index(9, 0), Qt::DisplayRole), QStringLiteral("album1"));
        QCOMPARE(viewsModel.data(viewsModel.index(10, 0), Qt::DisplayRole), QStringLiteral("album2"));
        QCOMPARE(viewsModel.data(viewsModel.index(11, 0), Qt::DisplayRole), QStringLiteral("album3"));
        QCOMPARE(viewsModel.data(viewsModel.index(12, 0), Qt::DisplayRole), QStringLiteral("album3"));
        QCOMPARE(viewsModel.data(viewsModel.index(13, 0), Qt::DisplayRole), QStringLiteral("album4"));

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Artist);

        beginInsertRowsSpy.wait(100);

        QCOMPARE(beginInsertRowsSpy.count(), 3);
        QCOMPARE(endInsertRowsSpy.count(), 3);
        QCOMPARE(beginRemoveRowsSpy.count(), 7);
        QCOMPARE(endRemoveRowsSpy.count(), 7);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), 16);

        QCOMPARE(viewsModel.data(viewsModel.index(0, 0), Qt::DisplayRole), QStringLiteral("Now Playing"));
        QCOMPARE(viewsModel.data(viewsModel.index(1, 0), Qt::DisplayRole), QStringLiteral("Recently Played"));
        QCOMPARE(viewsModel.data(viewsModel.index(2, 0), Qt::DisplayRole), QStringLiteral("Frequently Played"));
        QCOMPARE(viewsModel.data(viewsModel.index(3, 0), Qt::DisplayRole), QStringLiteral("Albums"));
        QCOMPARE(viewsModel.data(viewsModel.index(4, 0), Qt::DisplayRole), QStringLiteral("Tracks"));
        QCOMPARE(viewsModel.data(viewsModel.index(5, 0), Qt::DisplayRole), QStringLiteral("Genres"));
        QCOMPARE(viewsModel.data(viewsModel.index(6, 0), Qt::DisplayRole), QStringLiteral("Files"));
        QCOMPARE(viewsModel.data(viewsModel.index(7, 0), Qt::DisplayRole), QStringLiteral("Radio Stations"));
        QCOMPARE(viewsModel.data(viewsModel.index(8, 0), Qt::DisplayRole), QStringLiteral("UPnP Shares"));
        QCOMPARE(viewsModel.data(viewsModel.index(9, 0), Qt::DisplayRole), QStringLiteral("artist1"));
        QCOMPARE(viewsModel.data(viewsModel.index(10, 0), Qt::DisplayRole), QStringLiteral("artist1 and artist2"));
        QCOMPARE(viewsModel.data(viewsModel.index(11, 0), Qt::DisplayRole), QStringLiteral("artist2"));
        QCOMPARE(viewsModel.data(viewsModel.index(12, 0), Qt::DisplayRole), QStringLiteral("artist3"));
        QCOMPARE(viewsModel.data(viewsModel.index(13, 0), Qt::DisplayRole), QStringLiteral("artist4"));
        QCOMPARE(viewsModel.data(viewsModel.index(14, 0), Qt::DisplayRole), QStringLiteral("artist7"));
        QCOMPARE(viewsModel.data(viewsModel.index(15, 0), Qt::DisplayRole), QStringLiteral("Various Artists"));

        viewsModel.viewsData()->setEmbeddedCategory(ElisaUtils::Unknown);

        beginInsertRowsSpy.wait(100);

        QCOMPARE(beginInsertRowsSpy.count(), 4);
        QCOMPARE(endInsertRowsSpy.count(), 4);
        QCOMPARE(beginRemoveRowsSpy.count(), 14);
        QCOMPARE(endRemoveRowsSpy.count(), 14);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(viewsModel.rowCount(), 10);

        QCOMPARE(viewsModel.data(viewsModel.index(0, 0), Qt::DisplayRole), QStringLiteral("Now Playing"));
        QCOMPARE(viewsModel.data(viewsModel.index(1, 0), Qt::DisplayRole), QStringLiteral("Recently Played"));
        QCOMPARE(viewsModel.data(viewsModel.index(2, 0), Qt::DisplayRole), QStringLiteral("Frequently Played"));
        QCOMPARE(viewsModel.data(viewsModel.index(3, 0), Qt::DisplayRole), QStringLiteral("Albums"));
        QCOMPARE(viewsModel.data(viewsModel.index(4, 0), Qt::DisplayRole), QStringLiteral("Artists"));
        QCOMPARE(viewsModel.data(viewsModel.index(5, 0), Qt::DisplayRole), QStringLiteral("Tracks"));
        QCOMPARE(viewsModel.data(viewsModel.index(6, 0), Qt::DisplayRole), QStringLiteral("Genres"));
        QCOMPARE(viewsModel.data(viewsModel.index(7, 0), Qt::DisplayRole), QStringLiteral("Files"));
        QCOMPARE(viewsModel.data(viewsModel.index(8, 0), Qt::DisplayRole), QStringLiteral("Radio Stations"));
        QCOMPARE(viewsModel.data(viewsModel.index(9, 0), Qt::DisplayRole), QStringLiteral("UPnP Shares"));
    }
};

QTEST_GUILESS_MAIN(ViewsModelTests)

#include "viewsmodeltest.moc"
