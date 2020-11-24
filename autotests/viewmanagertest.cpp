/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "viewmanager.h"
#include "viewslistdata.h"
#include "viewconfigurationdata.h"

#include "elisa_settings.h"

#include <QtTest>

class ViewManagerTests: public QObject
{
    Q_OBJECT

public:

    explicit ViewManagerTests(QObject *aParent = nullptr) : QObject(aParent)
    {
        qRegisterMetaType<ViewConfigurationData*>("ViewConfigurationData*");
    }

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<ViewManager::RadioSpecificStyle>("ViewManager::RadioSpecificStyle");
        qRegisterMetaType<ViewManager::AlbumViewStyle>("ViewManager::AlbumViewStyle");
        qRegisterMetaType<ElisaUtils::PlayListEntryType>("ElisaUtils::PlayListEntryType");
        qRegisterMetaType<ElisaUtils::FilterType>("ElisaUtils::FilterType");
        qRegisterMetaType<ViewManager::ViewCanBeRated>("ViewManager::ViewCanBeRated");
        qRegisterMetaType<ViewManager::DelegateUseSecondaryText>("ViewManager::DelegateUseSecondaryText");
        qRegisterMetaType<ViewManager::AlbumCardinality>("ViewManager::AlbumCardinality");
        qRegisterMetaType<ViewManager::IsTreeModelType>("ViewManager::IsTreeModelType");
        qRegisterMetaType<Qt::SortOrder>("Qt::SortOrder");
        qRegisterMetaType<QAbstractItemModel*>("QAbstractItemModel*");
        qRegisterMetaType<QAbstractProxyModel*>("QAbstractProxyModel*");
        qRegisterMetaType<DataTypes::DataType>("DataTypes::DataType");
        QStandardPaths::setTestModeEnabled(true);
        Elisa::ElisaConfiguration::instance(QStringLiteral("testfoo"));
    }

    void openAlbumViewTest()
    {
        Elisa::ElisaConfiguration::self()->setDefaults();
        ViewManager viewManager;
        ViewsListData viewsData;
        viewManager.setViewsData(&viewsData);

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy openListViewSpy(&viewManager, &ViewManager::openListView);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.setInitialIndex(0);

        QCOMPARE(openGridViewSpy.count(), 0);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openChildView({{DataTypes::TitleRole, QStringLiteral("album1")},
                                   {DataTypes::ArtistRole, QStringLiteral("artist1")},
                                   {DataTypes::DatabaseIdRole, 12},
                                   {DataTypes::ElementTypeRole, ElisaUtils::Album}});

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(0).count(), 1);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::NoFilter);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Album);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 1);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openListViewSpy.at(0).count(), 1);
        QCOMPARE(openListViewSpy.at(0).at(0).value<ViewConfigurationData*>()->dataFilter()[DataTypes::TitleRole].toString(), QStringLiteral("album1"));
        QCOMPARE(openListViewSpy.at(0).at(0).value<ViewConfigurationData*>()->dataFilter()[DataTypes::ArtistRole].toString(), QStringLiteral("artist1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 1);
        QCOMPARE(popOneViewSpy.count(), 0);
    }

    void openAlbumView2Test()
    {
        Elisa::ElisaConfiguration::self()->setDefaults();
        ViewManager viewManager;
        ViewsListData viewsData;
        viewManager.setViewsData(&viewsData);

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy openListViewSpy(&viewManager, &ViewManager::openListView);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.setInitialIndex(0);

        QCOMPARE(openGridViewSpy.count(), 0);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openView(3);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(0).count(), 1);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::NoFilter);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Album);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openChildView({{DataTypes::TitleRole, QStringLiteral("album1")},
                                   {DataTypes::ArtistRole, QStringLiteral("artist1")},
                                   {DataTypes::DatabaseIdRole, 12},
                                   {DataTypes::ElementTypeRole, ElisaUtils::Album}});

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 1);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openListViewSpy.at(0).count(), 1);
        QCOMPARE(openListViewSpy.at(0).at(0).value<ViewConfigurationData*>()->dataFilter()[DataTypes::TitleRole].toString(), QStringLiteral("album1"));
        QCOMPARE(openListViewSpy.at(0).at(0).value<ViewConfigurationData*>()->dataFilter()[DataTypes::ArtistRole].toString(), QStringLiteral("artist1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 1);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openView(3);

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 1);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(1).count(), 1);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::NoFilter);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Album);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 1);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openChildView({{DataTypes::TitleRole, QStringLiteral("album1")},
                                   {DataTypes::ArtistRole, QStringLiteral("artist1")},
                                   {DataTypes::DatabaseIdRole, 12},
                                   {DataTypes::ElementTypeRole, ElisaUtils::Album}});

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 2);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openListViewSpy.at(1).count(), 1);
        QCOMPARE(openListViewSpy.at(1).at(0).value<ViewConfigurationData*>()->dataFilter()[DataTypes::TitleRole].toString(), QStringLiteral("album1"));
        QCOMPARE(openListViewSpy.at(1).at(0).value<ViewConfigurationData*>()->dataFilter()[DataTypes::ArtistRole].toString(), QStringLiteral("artist1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 2);
        QCOMPARE(popOneViewSpy.count(), 0);
    }

    void openAlbumView3Test()
    {
        Elisa::ElisaConfiguration::self()->setDefaults();
        ViewManager viewManager;
        ViewsListData viewsData;
        viewManager.setViewsData(&viewsData);

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy openListViewSpy(&viewManager, &ViewManager::openListView);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.setInitialIndex(0);

        QCOMPARE(openGridViewSpy.count(), 0);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openView(3);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(0).count(), 1);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::NoFilter);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Album);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openChildView({{DataTypes::TitleRole, QStringLiteral("album1")},
                                   {DataTypes::ArtistRole, QStringLiteral("artist1")},
                                   {DataTypes::DatabaseIdRole, 12},
                                   {DataTypes::ElementTypeRole, ElisaUtils::Album}});

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 1);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openListViewSpy.at(0).count(), 1);
        QCOMPARE(openListViewSpy.at(0).at(0).value<ViewConfigurationData*>()->dataFilter()[DataTypes::TitleRole].toString(), QStringLiteral("album1"));
        QCOMPARE(openListViewSpy.at(0).at(0).value<ViewConfigurationData*>()->dataFilter()[DataTypes::ArtistRole].toString(), QStringLiteral("artist1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 1);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.goBack();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 1);
        QCOMPARE(popOneViewSpy.count(), 1);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 1);
        QCOMPARE(popOneViewSpy.count(), 1);

        viewManager.openChildView({{DataTypes::TitleRole, QStringLiteral("album1")},
                                   {DataTypes::ArtistRole, QStringLiteral("artist1")},
                                   {DataTypes::DatabaseIdRole, 12},
                                   {DataTypes::ElementTypeRole, ElisaUtils::Album}});

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 2);
        QCOMPARE(popOneViewSpy.count(), 1);

        QCOMPARE(openListViewSpy.at(1).count(), 1);
        QCOMPARE(openListViewSpy.at(1).at(0).value<ViewConfigurationData*>()->dataFilter()[DataTypes::TitleRole].toString(), QStringLiteral("album1"));
        QCOMPARE(openListViewSpy.at(1).at(0).value<ViewConfigurationData*>()->dataFilter()[DataTypes::ArtistRole].toString(), QStringLiteral("artist1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 2);
        QCOMPARE(popOneViewSpy.count(), 1);
    }

    void openArtistViewTest()
    {
        Elisa::ElisaConfiguration::self()->setDefaults();
        ViewManager viewManager;
        ViewsListData viewsData;
        viewManager.setViewsData(&viewsData);

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy openListViewSpy(&viewManager, &ViewManager::openListView);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.setInitialIndex(0);

        QCOMPARE(openGridViewSpy.count(), 0);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openChildView({{DataTypes::TitleRole, QStringLiteral("artist1")},
                                   {DataTypes::DatabaseIdRole, 12},
                                   {DataTypes::ElementTypeRole, ElisaUtils::Artist}});

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(0).count(), 1);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::NoFilter);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Artist);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(1).count(), 1);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::FilterByArtist);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Album);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ViewConfigurationData*>()->dataFilter()[DataTypes::ArtistRole].toString(), QStringLiteral("artist1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);
    }

    void openArtistView2Test()
    {
        Elisa::ElisaConfiguration::self()->setDefaults();
        ViewManager viewManager;
        ViewsListData viewsData;
        viewManager.setViewsData(&viewsData);

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy openListViewSpy(&viewManager, &ViewManager::openListView);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.setInitialIndex(0);

        QCOMPARE(openGridViewSpy.count(), 0);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openView(4);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(0).count(), 1);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::NoFilter);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Artist);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openChildView({{DataTypes::TitleRole, QStringLiteral("artist1")},
                                   {DataTypes::DatabaseIdRole, 12},
                                   {DataTypes::ElementTypeRole, ElisaUtils::Artist}});

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(1).count(), 1);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::FilterByArtist);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Album);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ViewConfigurationData*>()->dataFilter()[DataTypes::ArtistRole].toString(), QStringLiteral("artist1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openView(4);

        QCOMPARE(openGridViewSpy.count(), 3);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(2).count(), 1);
        QCOMPARE(openGridViewSpy.at(2).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::NoFilter);
        QCOMPARE(openGridViewSpy.at(2).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Artist);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 3);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openChildView({{DataTypes::TitleRole, QStringLiteral("artist1")},
                                   {DataTypes::DatabaseIdRole, 12},
                                   {DataTypes::ElementTypeRole, ElisaUtils::Artist}});

        QCOMPARE(openGridViewSpy.count(), 4);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(3).count(), 1);
        QCOMPARE(openGridViewSpy.at(3).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::FilterByArtist);
        QCOMPARE(openGridViewSpy.at(3).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Album);
        QCOMPARE(openGridViewSpy.at(3).at(0).value<ViewConfigurationData*>()->dataFilter()[DataTypes::ArtistRole].toString(), QStringLiteral("artist1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 4);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);
    }

    void openArtistView3Test()
    {
        Elisa::ElisaConfiguration::self()->setDefaults();
        ViewManager viewManager;
        ViewsListData viewsData;
        viewManager.setViewsData(&viewsData);

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy openListViewSpy(&viewManager, &ViewManager::openListView);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.setInitialIndex(0);

        QCOMPARE(openGridViewSpy.count(), 0);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openView(4);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(0).count(), 1);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::NoFilter);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Artist);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openChildView({{DataTypes::TitleRole, QStringLiteral("artist1")},
                                   {DataTypes::DatabaseIdRole, 12},
                                   {DataTypes::ElementTypeRole, ElisaUtils::Artist}});

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(1).count(), 1);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::FilterByArtist);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Album);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ViewConfigurationData*>()->dataFilter()[DataTypes::ArtistRole].toString(), QStringLiteral("artist1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.goBack();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 1);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 1);

        viewManager.openChildView({{DataTypes::TitleRole, QStringLiteral("artist1")},
                                   {DataTypes::DatabaseIdRole, 12},
                                   {DataTypes::ElementTypeRole, ElisaUtils::Artist}});

        QCOMPARE(openGridViewSpy.count(), 3);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 1);

        QCOMPARE(openGridViewSpy.at(2).count(), 1);
        QCOMPARE(openGridViewSpy.at(2).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::FilterByArtist);
        QCOMPARE(openGridViewSpy.at(2).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Album);
        QCOMPARE(openGridViewSpy.at(2).at(0).value<ViewConfigurationData*>()->dataFilter()[DataTypes::ArtistRole].toString(), QStringLiteral("artist1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 3);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 1);
    }

    void openGenreViewTest()
    {
        Elisa::ElisaConfiguration::self()->setDefaults();
        ViewManager viewManager;
        ViewsListData viewsData;
        viewManager.setViewsData(&viewsData);

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy openListViewSpy(&viewManager, &ViewManager::openListView);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.setInitialIndex(0);

        QCOMPARE(openGridViewSpy.count(), 0);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openChildView({{DataTypes::TitleRole, QStringLiteral("genre1")},
                                   {DataTypes::DatabaseIdRole, 12},
                                   {DataTypes::ElementTypeRole, ElisaUtils::Genre}});

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(0).count(), 1);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::NoFilter);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Genre);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(1).count(), 1);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::FilterByGenre);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Artist);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ViewConfigurationData*>()->dataFilter()[DataTypes::GenreRole].toString(), QStringLiteral("genre1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);
    }

    void openGenreView2Test()
    {
        Elisa::ElisaConfiguration::self()->setDefaults();
        ViewManager viewManager;
        ViewsListData viewsData;
        viewManager.setViewsData(&viewsData);

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy openListViewSpy(&viewManager, &ViewManager::openListView);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.setInitialIndex(0);

        QCOMPARE(openGridViewSpy.count(), 0);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openView(6);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(0).count(), 1);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::NoFilter);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Genre);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openChildView({{DataTypes::TitleRole, QStringLiteral("genre1")},
                                   {DataTypes::DatabaseIdRole, 12},
                                   {DataTypes::ElementTypeRole, ElisaUtils::Genre}});

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(1).count(), 1);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::FilterByGenre);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Artist);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ViewConfigurationData*>()->dataFilter()[DataTypes::GenreRole].toString(), QStringLiteral("genre1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openView(6);

        QCOMPARE(openGridViewSpy.count(), 3);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(2).count(), 1);
        QCOMPARE(openGridViewSpy.at(2).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::NoFilter);
        QCOMPARE(openGridViewSpy.at(2).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Genre);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 3);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openChildView({{DataTypes::TitleRole, QStringLiteral("genre1")},
                                   {DataTypes::DatabaseIdRole, 12},
                                   {DataTypes::ElementTypeRole, ElisaUtils::Genre}});

        QCOMPARE(openGridViewSpy.count(), 4);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(3).count(), 1);
        QCOMPARE(openGridViewSpy.at(3).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::FilterByGenre);
        QCOMPARE(openGridViewSpy.at(3).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Artist);
        QCOMPARE(openGridViewSpy.at(3).at(0).value<ViewConfigurationData*>()->dataFilter()[DataTypes::GenreRole].toString(), QStringLiteral("genre1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 4);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);
    }

    void openGenreView3Test()
    {
        Elisa::ElisaConfiguration::self()->setDefaults();
        ViewManager viewManager;
        ViewsListData viewsData;
        viewManager.setViewsData(&viewsData);

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy openListViewSpy(&viewManager, &ViewManager::openListView);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.setInitialIndex(0);

        QCOMPARE(openGridViewSpy.count(), 0);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openView(6);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(0).count(), 1);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::NoFilter);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Genre);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openChildView({{DataTypes::TitleRole, QStringLiteral("genre1")},
                                   {DataTypes::DatabaseIdRole, 12},
                                   {DataTypes::ElementTypeRole, ElisaUtils::Genre}});

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(1).count(), 1);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::FilterByGenre);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Artist);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ViewConfigurationData*>()->dataFilter()[DataTypes::GenreRole].toString(), QStringLiteral("genre1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.goBack();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 1);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 1);

        viewManager.openChildView({{DataTypes::TitleRole, QStringLiteral("genre1")},
                                   {DataTypes::DatabaseIdRole, 12},
                                   {DataTypes::ElementTypeRole, ElisaUtils::Genre}});

        QCOMPARE(openGridViewSpy.count(), 3);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 1);

        QCOMPARE(openGridViewSpy.at(2).count(), 1);
        QCOMPARE(openGridViewSpy.at(2).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::FilterByGenre);
        QCOMPARE(openGridViewSpy.at(2).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Artist);
        QCOMPARE(openGridViewSpy.at(2).at(0).value<ViewConfigurationData*>()->dataFilter()[DataTypes::GenreRole].toString(), QStringLiteral("genre1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 3);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 1);
    }

    void openArtistFromGenreViewTest()
    {
        Elisa::ElisaConfiguration::self()->setDefaults();
        ViewManager viewManager;
        ViewsListData viewsData;
        viewManager.setViewsData(&viewsData);

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy openListViewSpy(&viewManager, &ViewManager::openListView);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.setInitialIndex(0);

        QCOMPARE(openGridViewSpy.count(), 0);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openView(6);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(0).count(), 1);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::NoFilter);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Genre);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openChildView({{DataTypes::TitleRole, QStringLiteral("genre1")},
                                   {DataTypes::DatabaseIdRole, 12},
                                   {DataTypes::ElementTypeRole, ElisaUtils::Genre}});

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(1).count(), 1);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::FilterByGenre);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Artist);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ViewConfigurationData*>()->dataFilter()[DataTypes::GenreRole].toString(), QStringLiteral("genre1"));

        viewManager.viewIsLoaded();

        viewManager.openChildView({{DataTypes::TitleRole, QStringLiteral("artist1")},
                                   {DataTypes::DatabaseIdRole, 12},
                                   {DataTypes::ElementTypeRole, ElisaUtils::Artist}});

        QCOMPARE(openGridViewSpy.count(), 3);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(2).count(), 1);
        QCOMPARE(openGridViewSpy.at(2).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::FilterByGenreAndArtist);
        QCOMPARE(openGridViewSpy.at(2).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Album);
        QCOMPARE(openGridViewSpy.at(2).at(0).value<ViewConfigurationData*>()->dataFilter()[DataTypes::GenreRole].toString(), QStringLiteral("genre1"));
        QCOMPARE(openGridViewSpy.at(2).at(0).value<ViewConfigurationData*>()->dataFilter()[DataTypes::ArtistRole].toString(), QStringLiteral("artist1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 3);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);
    }

    void openArtistViewAndAlbumFromAnotherArtistTest()
    {
        Elisa::ElisaConfiguration::self()->setDefaults();
        ViewManager viewManager;
        ViewsListData viewsData;
        viewManager.setViewsData(&viewsData);

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy openListViewSpy(&viewManager, &ViewManager::openListView);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.setInitialIndex(0);

        QCOMPARE(openGridViewSpy.count(), 0);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openChildView({{DataTypes::TitleRole, QStringLiteral("artist1")},
                                   {DataTypes::DatabaseIdRole, 12},
                                   {DataTypes::ElementTypeRole, ElisaUtils::Artist}});

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(0).count(), 1);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::NoFilter);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Artist);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(1).count(), 1);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::FilterByArtist);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Album);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ViewConfigurationData*>()->dataFilter()[DataTypes::ArtistRole].toString(), QStringLiteral("artist1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openChildView({{DataTypes::TitleRole, QStringLiteral("album1")},
                                   {DataTypes::ArtistRole, QStringLiteral("artist2")},
                                   {DataTypes::DatabaseIdRole, 12},
                                   {DataTypes::ElementTypeRole, ElisaUtils::Album}});

        QCOMPARE(openGridViewSpy.count(), 3);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(2).count(), 1);
        QCOMPARE(openGridViewSpy.at(2).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::FilterByArtist);
        QCOMPARE(openGridViewSpy.at(2).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Album);
        QCOMPARE(openGridViewSpy.at(2).at(0).value<ViewConfigurationData*>()->dataFilter()[DataTypes::ArtistRole].toString(), QStringLiteral("artist2"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 3);
        QCOMPARE(openListViewSpy.count(), 1);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openListViewSpy.at(0).count(), 1);
        QCOMPARE(openListViewSpy.at(0).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::FilterById);
        QCOMPARE(openListViewSpy.at(0).at(0).value<ViewConfigurationData*>()->dataFilter()[DataTypes::TitleRole].toString(), QStringLiteral("album1"));
        QCOMPARE(openListViewSpy.at(0).at(0).value<ViewConfigurationData*>()->dataFilter()[DataTypes::ArtistRole].toString(), QStringLiteral("artist2"));
        QCOMPARE(openListViewSpy.at(0).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Track);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 3);
        QCOMPARE(openListViewSpy.count(), 1);
        QCOMPARE(popOneViewSpy.count(), 0);
    }

    void openNowPlayingViewTest()
    {
        Elisa::ElisaConfiguration::self()->setDefaults();
        ViewManager viewManager;
        ViewsListData viewsData;
        viewManager.setViewsData(&viewsData);

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy openListViewSpy(&viewManager, &ViewManager::openListView);
        QSignalSpy switchContextViewSpy(&viewManager, &ViewManager::switchContextView);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.setInitialIndex(0);

        QCOMPARE(openGridViewSpy.count(), 0);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchContextViewSpy.count(), 1);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openChildView({{DataTypes::TitleRole, QStringLiteral("album1")},
                                   {DataTypes::ArtistRole, QStringLiteral("artist1")},
                                   {DataTypes::DatabaseIdRole, 12},
                                   {DataTypes::ElementTypeRole, ElisaUtils::Album}});

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(switchContextViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(0).count(), 1);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewConfigurationData*>()->filterType(), ElisaUtils::NoFilter);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewConfigurationData*>()->dataType(), ElisaUtils::Album);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 1);
        QCOMPARE(switchContextViewSpy.count(), 1);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openListViewSpy.at(0).count(), 1);
        QCOMPARE(openListViewSpy.at(0).at(0).value<ViewConfigurationData*>()->dataFilter()[DataTypes::TitleRole].toString(), QStringLiteral("album1"));
        QCOMPARE(openListViewSpy.at(0).at(0).value<ViewConfigurationData*>()->dataFilter()[DataTypes::ArtistRole].toString(), QStringLiteral("artist1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 1);
        QCOMPARE(switchContextViewSpy.count(), 1);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openNowPlaying();
        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 1);
        QCOMPARE(switchContextViewSpy.count(), 2);
        QCOMPARE(popOneViewSpy.count(), 0);
    }
};

QTEST_GUILESS_MAIN(ViewManagerTests)


#include "viewmanagertest.moc"
