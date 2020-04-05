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

#include "viewmanager.h"


#include <QtTest>

class ViewManagerTests: public QObject
{
    Q_OBJECT

public:

    explicit ViewManagerTests(QObject *aParent = nullptr) : QObject(aParent)
    {
    }

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<ViewManager::ViewsType>("ViewManager::ViewsType");
        qRegisterMetaType<ViewManager::SortOrder>("ViewManager::SortOrder");
        qRegisterMetaType<ViewManager::RadioSpecificStyle>("ViewManager::RadioSpecificStyle");
        qRegisterMetaType<ViewManager::AlbumViewStyle>("ViewManager::AlbumViewStyle");
        qRegisterMetaType<ElisaUtils::PlayListEntryType>("ElisaUtils::PlayListEntryType");
        qRegisterMetaType<ElisaUtils::FilterType>("ElisaUtils::FilterType");
        qRegisterMetaType<ViewManager::ViewCanBeRated>("ViewManager::ViewCanBeRated");
        qRegisterMetaType<ViewManager::DelegateUseSecondaryText>("ViewManager::DelegateUseSecondaryText");
        qRegisterMetaType<ViewManager::AlbumCardinality>("ViewManager::AlbumCardinality");
    }

    void openAlbumViewTest()
    {
        ViewManager viewManager;

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy openListViewSpy(&viewManager, &ViewManager::openListView);
        QSignalSpy switchFilesBrowserViewSpy(&viewManager, &ViewManager::switchFilesBrowserView);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.openChildView(QStringLiteral("album1"), QStringLiteral("artist1"), {}, 12, ElisaUtils::Album);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(0).count(), 11);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ElisaUtils::FilterType>(), ElisaUtils::NoFilter);
        QCOMPARE(openGridViewSpy.at(0).at(5).value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Album);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 1);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openListViewSpy.at(0).count(), 12);
        QCOMPARE(openListViewSpy.at(0).at(2), QStringLiteral("album1"));
        QCOMPARE(openListViewSpy.at(0).at(3), QStringLiteral("artist1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 1);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);
    }

    void openAlbumView2Test()
    {
        ViewManager viewManager;

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy openListViewSpy(&viewManager, &ViewManager::openListView);
        QSignalSpy switchFilesBrowserViewSpy(&viewManager, &ViewManager::switchFilesBrowserView);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.openView(3);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(0).count(), 11);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ElisaUtils::FilterType>(), ElisaUtils::NoFilter);
        QCOMPARE(openGridViewSpy.at(0).at(5).value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Album);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openChildView(QStringLiteral("album1"), QStringLiteral("artist1"), {}, 12, ElisaUtils::Album);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 1);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openListViewSpy.at(0).count(), 12);
        QCOMPARE(openListViewSpy.at(0).at(2), QStringLiteral("album1"));
        QCOMPARE(openListViewSpy.at(0).at(3), QStringLiteral("artist1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 1);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openView(3);

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 1);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(1).count(), 11);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ElisaUtils::FilterType>(), ElisaUtils::NoFilter);
        QCOMPARE(openGridViewSpy.at(1).at(5).value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Album);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 1);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openChildView(QStringLiteral("album1"), QStringLiteral("artist1"), {}, 12, ElisaUtils::Album);

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 2);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openListViewSpy.at(1).count(), 12);
        QCOMPARE(openListViewSpy.at(1).at(2), QStringLiteral("album1"));
        QCOMPARE(openListViewSpy.at(1).at(3), QStringLiteral("artist1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 2);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);
    }

    void openAlbumView3Test()
    {
        ViewManager viewManager;

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy openListViewSpy(&viewManager, &ViewManager::openListView);
        QSignalSpy switchFilesBrowserViewSpy(&viewManager, &ViewManager::switchFilesBrowserView);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.openView(3);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(0).count(), 11);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ElisaUtils::FilterType>(), ElisaUtils::NoFilter);
        QCOMPARE(openGridViewSpy.at(0).at(5).value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Album);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openChildView(QStringLiteral("album1"), QStringLiteral("artist1"), {}, 12, ElisaUtils::Album);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 1);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openListViewSpy.at(0).count(), 12);
        QCOMPARE(openListViewSpy.at(0).at(2), QStringLiteral("album1"));
        QCOMPARE(openListViewSpy.at(0).at(3), QStringLiteral("artist1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 1);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.goBack();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 1);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 1);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 1);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 1);

        viewManager.openChildView(QStringLiteral("album1"), QStringLiteral("artist1"), {}, 12, ElisaUtils::Album);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 2);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 1);

        QCOMPARE(openListViewSpy.at(1).count(), 12);
        QCOMPARE(openListViewSpy.at(1).at(2), QStringLiteral("album1"));
        QCOMPARE(openListViewSpy.at(1).at(3), QStringLiteral("artist1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 2);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 1);
    }

    void openArtistViewTest()
    {
        ViewManager viewManager;

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy openListViewSpy(&viewManager, &ViewManager::openListView);
        QSignalSpy switchFilesBrowserViewSpy(&viewManager, &ViewManager::switchFilesBrowserView);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.openChildView(QStringLiteral("artist1"), {}, {}, 0, ElisaUtils::Artist);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(0).count(), 11);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ElisaUtils::FilterType>(), ElisaUtils::NoFilter);
        QCOMPARE(openGridViewSpy.at(0).at(5).value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Artist);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(1).count(), 11);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ElisaUtils::FilterType>(), ElisaUtils::FilterByArtist);
        QCOMPARE(openGridViewSpy.at(1).at(5).value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Album);
        QCOMPARE(openGridViewSpy.at(1).at(8).toString(), QStringLiteral("artist1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);
    }

    void openArtistView2Test()
    {
        ViewManager viewManager;

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy openListViewSpy(&viewManager, &ViewManager::openListView);
        QSignalSpy switchFilesBrowserViewSpy(&viewManager, &ViewManager::switchFilesBrowserView);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.openView(4);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(0).count(), 11);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ElisaUtils::FilterType>(), ElisaUtils::NoFilter);
        QCOMPARE(openGridViewSpy.at(0).at(5).value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Artist);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openChildView(QStringLiteral("artist1"), {}, {}, 0, ElisaUtils::Artist);

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(1).count(), 11);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ElisaUtils::FilterType>(), ElisaUtils::FilterByArtist);
        QCOMPARE(openGridViewSpy.at(1).at(5).value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Album);
        QCOMPARE(openGridViewSpy.at(1).at(8).toString(), QStringLiteral("artist1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openView(4);

        QCOMPARE(openGridViewSpy.count(), 3);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(2).count(), 11);
        QCOMPARE(openGridViewSpy.at(2).at(0).value<ElisaUtils::FilterType>(), ElisaUtils::NoFilter);
        QCOMPARE(openGridViewSpy.at(2).at(5).value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Artist);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 3);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openChildView(QStringLiteral("artist1"), {}, {}, 0, ElisaUtils::Artist);

        QCOMPARE(openGridViewSpy.count(), 4);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(3).count(), 11);
        QCOMPARE(openGridViewSpy.at(3).at(0).value<ElisaUtils::FilterType>(), ElisaUtils::FilterByArtist);
        QCOMPARE(openGridViewSpy.at(3).at(5).value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Album);
        QCOMPARE(openGridViewSpy.at(3).at(8).toString(), QStringLiteral("artist1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 4);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);
    }

    void openArtistView3Test()
    {
        ViewManager viewManager;

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy openListViewSpy(&viewManager, &ViewManager::openListView);
        QSignalSpy switchFilesBrowserViewSpy(&viewManager, &ViewManager::switchFilesBrowserView);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.openView(4);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(0).count(), 11);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ElisaUtils::FilterType>(), ElisaUtils::NoFilter);
        QCOMPARE(openGridViewSpy.at(0).at(5).value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Artist);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openChildView(QStringLiteral("artist1"), {}, {}, 0, ElisaUtils::Artist);

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(1).count(), 11);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ElisaUtils::FilterType>(), ElisaUtils::FilterByArtist);
        QCOMPARE(openGridViewSpy.at(1).at(5).value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Album);
        QCOMPARE(openGridViewSpy.at(1).at(8).toString(), QStringLiteral("artist1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.goBack();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 1);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 1);

        viewManager.openChildView(QStringLiteral("artist1"), {}, {}, 0, ElisaUtils::Artist);

        QCOMPARE(openGridViewSpy.count(), 3);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 1);

        QCOMPARE(openGridViewSpy.at(2).count(), 11);
        QCOMPARE(openGridViewSpy.at(2).at(0).value<ElisaUtils::FilterType>(), ElisaUtils::FilterByArtist);
        QCOMPARE(openGridViewSpy.at(2).at(5).value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Album);
        QCOMPARE(openGridViewSpy.at(2).at(8).toString(), QStringLiteral("artist1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 3);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 1);
    }

    void openGenreViewTest()
    {
        ViewManager viewManager;

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy openListViewSpy(&viewManager, &ViewManager::openListView);
        QSignalSpy switchFilesBrowserViewSpy(&viewManager, &ViewManager::switchFilesBrowserView);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.openChildView(QStringLiteral("genre1"), {}, {}, 0, ElisaUtils::Genre);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(0).count(), 11);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ElisaUtils::FilterType>(), ElisaUtils::NoFilter);
        QCOMPARE(openGridViewSpy.at(0).at(5).value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Genre);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(1).count(), 11);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ElisaUtils::FilterType>(), ElisaUtils::FilterByGenre);
        QCOMPARE(openGridViewSpy.at(1).at(5).value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Artist);
        QCOMPARE(openGridViewSpy.at(1).at(7).toString(), QStringLiteral("genre1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);
    }

    void openGenreView2Test()
    {
        ViewManager viewManager;

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy openListViewSpy(&viewManager, &ViewManager::openListView);
        QSignalSpy switchFilesBrowserViewSpy(&viewManager, &ViewManager::switchFilesBrowserView);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.openView(6);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(0).count(), 11);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ElisaUtils::FilterType>(), ElisaUtils::NoFilter);
        QCOMPARE(openGridViewSpy.at(0).at(5).value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Genre);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openChildView(QStringLiteral("genre1"), {}, {}, 0, ElisaUtils::Genre);

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(1).count(), 11);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ElisaUtils::FilterType>(), ElisaUtils::FilterByGenre);
        QCOMPARE(openGridViewSpy.at(1).at(5).value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Artist);
        QCOMPARE(openGridViewSpy.at(1).at(7).toString(), QStringLiteral("genre1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openView(6);

        QCOMPARE(openGridViewSpy.count(), 3);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(2).count(), 11);
        QCOMPARE(openGridViewSpy.at(2).at(0).value<ElisaUtils::FilterType>(), ElisaUtils::NoFilter);
        QCOMPARE(openGridViewSpy.at(2).at(5).value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Genre);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 3);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openChildView(QStringLiteral("genre1"), {}, {}, 0, ElisaUtils::Genre);

        QCOMPARE(openGridViewSpy.count(), 4);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(3).count(), 11);
        QCOMPARE(openGridViewSpy.at(3).at(0).value<ElisaUtils::FilterType>(), ElisaUtils::FilterByGenre);
        QCOMPARE(openGridViewSpy.at(3).at(5).value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Artist);
        QCOMPARE(openGridViewSpy.at(3).at(7).toString(), QStringLiteral("genre1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 4);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);
    }

    void openGenreView3Test()
    {
        ViewManager viewManager;

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy openListViewSpy(&viewManager, &ViewManager::openListView);
        QSignalSpy switchFilesBrowserViewSpy(&viewManager, &ViewManager::switchFilesBrowserView);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.openView(6);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(0).count(), 11);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ElisaUtils::FilterType>(), ElisaUtils::NoFilter);
        QCOMPARE(openGridViewSpy.at(0).at(5).value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Genre);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openChildView(QStringLiteral("genre1"), {}, {}, 0, ElisaUtils::Genre);

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(1).count(), 11);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ElisaUtils::FilterType>(), ElisaUtils::FilterByGenre);
        QCOMPARE(openGridViewSpy.at(1).at(5).value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Artist);
        QCOMPARE(openGridViewSpy.at(1).at(7).toString(), QStringLiteral("genre1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.goBack();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 1);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 1);

        viewManager.openChildView(QStringLiteral("genre1"), {}, {}, 0, ElisaUtils::Genre);

        QCOMPARE(openGridViewSpy.count(), 3);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 1);

        QCOMPARE(openGridViewSpy.at(2).count(), 11);
        QCOMPARE(openGridViewSpy.at(2).at(0).value<ElisaUtils::FilterType>(), ElisaUtils::FilterByGenre);
        QCOMPARE(openGridViewSpy.at(2).at(5).value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Artist);
        QCOMPARE(openGridViewSpy.at(2).at(7).toString(), QStringLiteral("genre1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 3);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 1);
    }

    void openArtistFromGenreViewTest()
    {
        ViewManager viewManager;

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy openListViewSpy(&viewManager, &ViewManager::openListView);
        QSignalSpy switchFilesBrowserViewSpy(&viewManager, &ViewManager::switchFilesBrowserView);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.openView(6);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(0).count(), 11);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ElisaUtils::FilterType>(), ElisaUtils::NoFilter);
        QCOMPARE(openGridViewSpy.at(0).at(5).value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Genre);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openChildView(QStringLiteral("genre1"), {}, {}, 0, ElisaUtils::Genre);

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(1).count(), 11);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ElisaUtils::FilterType>(), ElisaUtils::FilterByGenre);
        QCOMPARE(openGridViewSpy.at(1).at(5).value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Artist);
        QCOMPARE(openGridViewSpy.at(1).at(7).toString(), QStringLiteral("genre1"));

        viewManager.viewIsLoaded();

        viewManager.openChildView(QStringLiteral("artist1"), {}, {}, 0, ElisaUtils::Artist);

        QCOMPARE(openGridViewSpy.count(), 3);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(2).count(), 11);
        QCOMPARE(openGridViewSpy.at(2).at(0).value<ElisaUtils::FilterType>(), ElisaUtils::FilterByGenreAndArtist);
        QCOMPARE(openGridViewSpy.at(2).at(5).value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Album);
        QCOMPARE(openGridViewSpy.at(2).at(7).toString(), QStringLiteral("genre1"));
        QCOMPARE(openGridViewSpy.at(2).at(8).toString(), QStringLiteral("artist1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 3);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);
    }

    void openArtistViewAndAlbumFromAnotherArtistTest()
    {
        ViewManager viewManager;

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy openListViewSpy(&viewManager, &ViewManager::openListView);
        QSignalSpy switchFilesBrowserViewSpy(&viewManager, &ViewManager::switchFilesBrowserView);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.openChildView(QStringLiteral("artist1"), {}, {}, 0, ElisaUtils::Artist);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(0).count(), 11);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ElisaUtils::FilterType>(), ElisaUtils::NoFilter);
        QCOMPARE(openGridViewSpy.at(0).at(5).value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Artist);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(1).count(), 11);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ElisaUtils::FilterType>(), ElisaUtils::FilterByArtist);
        QCOMPARE(openGridViewSpy.at(1).at(5).value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Album);
        QCOMPARE(openGridViewSpy.at(1).at(8).toString(), QStringLiteral("artist1"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openChildView(QStringLiteral("album1"), QStringLiteral("artist2"), {}, 0, ElisaUtils::Album);

        QCOMPARE(openGridViewSpy.count(), 3);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(2).count(), 11);
        QCOMPARE(openGridViewSpy.at(2).at(0).value<ElisaUtils::FilterType>(), ElisaUtils::FilterByArtist);
        QCOMPARE(openGridViewSpy.at(2).at(5).value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Album);
        QCOMPARE(openGridViewSpy.at(2).at(8).toString(), QStringLiteral("artist2"));

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 3);
        QCOMPARE(openListViewSpy.count(), 1);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openListViewSpy.at(0).count(), 12);
        QCOMPARE(openListViewSpy.at(0).at(0).value<ElisaUtils::FilterType>(), ElisaUtils::FilterById);
        QCOMPARE(openListViewSpy.at(0).at(2).toString(), QStringLiteral("album1"));
        QCOMPARE(openListViewSpy.at(0).at(3).toString(), QStringLiteral("artist2"));
        QCOMPARE(openListViewSpy.at(0).at(6).value<ElisaUtils::PlayListEntryType>(), ElisaUtils::Track);

        viewManager.viewIsLoaded();

        QCOMPARE(openGridViewSpy.count(), 3);
        QCOMPARE(openListViewSpy.count(), 1);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);
    }
};

QTEST_GUILESS_MAIN(ViewManagerTests)


#include "viewmanagertest.moc"
