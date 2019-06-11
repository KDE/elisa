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

#include <QDebug>

#include <QtTest>

class ViewManagerTests: public QObject
{
    Q_OBJECT

private:

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<ViewManager::ViewsType>("ViewManager::ViewsType");
        qRegisterMetaType<ElisaUtils::PlayListEntryType>("ElisaUtils::PlayListEntryType");
        qRegisterMetaType<ElisaUtils::FilterType>("ElisaUtils::FilterType");
    }

    void closeAllViewsTest()
    {
        ViewManager viewManager;

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy openListViewSpy(&viewManager, &ViewManager::openListView);
        QSignalSpy switchFilesBrowserViewSpy(&viewManager, &ViewManager::switchFilesBrowserView);
        QSignalSpy switchOffAllViewsSpy(&viewManager, &ViewManager::switchOffAllViews);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.closeAllViews();

        QCOMPARE(openGridViewSpy.count(), 0);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(switchOffAllViewsSpy.count(), 1);
        QCOMPARE(popOneViewSpy.count(), 0);
    }

    void openAlbumViewTest()
    {
        ViewManager viewManager;

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy openListViewSpy(&viewManager, &ViewManager::openListView);
        QSignalSpy switchFilesBrowserViewSpy(&viewManager, &ViewManager::switchFilesBrowserView);
        QSignalSpy switchOffAllViewsSpy(&viewManager, &ViewManager::switchOffAllViews);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.openChildView(QStringLiteral("album1"), QStringLiteral("artist1"), {}, 12, ElisaUtils::Album);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(switchOffAllViewsSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(0).count(), 12);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewManager::ViewsType>(), ViewManager::AllAlbums);

        viewManager.viewIsLoaded(ViewManager::AllAlbums);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 1);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(switchOffAllViewsSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openListViewSpy.at(0).count(), 8);
        QCOMPARE(openListViewSpy.at(0).at(3), QStringLiteral("album1"));
        QCOMPARE(openListViewSpy.at(0).at(4), QStringLiteral("artist1"));
    }

    void openArtistViewTest()
    {
        ViewManager viewManager;

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy openListViewSpy(&viewManager, &ViewManager::openListView);
        QSignalSpy switchFilesBrowserViewSpy(&viewManager, &ViewManager::switchFilesBrowserView);
        QSignalSpy switchOffAllViewsSpy(&viewManager, &ViewManager::switchOffAllViews);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.openChildView(QStringLiteral("artist1"), {}, {}, 0, ElisaUtils::Artist);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(switchOffAllViewsSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(0).count(), 12);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewManager::ViewsType>(), ViewManager::AllArtists);

        viewManager.viewIsLoaded(ViewManager::AllArtists);

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(switchOffAllViewsSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(1).count(), 12);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ViewManager::ViewsType>(), ViewManager::OneArtist);
    }

    void openGenreViewTest()
    {
        ViewManager viewManager;

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy openListViewSpy(&viewManager, &ViewManager::openListView);
        QSignalSpy switchFilesBrowserViewSpy(&viewManager, &ViewManager::switchFilesBrowserView);
        QSignalSpy switchOffAllViewsSpy(&viewManager, &ViewManager::switchOffAllViews);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.openChildView(QStringLiteral("genre1"), {}, {}, 0, ElisaUtils::Genre);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(switchOffAllViewsSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(0).count(), 12);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewManager::ViewsType>(), ViewManager::AllGenres);

        viewManager.viewIsLoaded(ViewManager::AllArtists);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(switchOffAllViewsSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);
    }

    void openArtistFromGenreViewTest()
    {
        ViewManager viewManager;

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy openListViewSpy(&viewManager, &ViewManager::openListView);
        QSignalSpy switchFilesBrowserViewSpy(&viewManager, &ViewManager::switchFilesBrowserView);
        QSignalSpy switchOffAllViewsSpy(&viewManager, &ViewManager::switchOffAllViews);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.openChildView(QStringLiteral("genre1"), {}, {}, 0, ElisaUtils::Genre);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(switchOffAllViewsSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(0).count(), 12);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewManager::ViewsType>(), ViewManager::AllGenres);

        viewManager.viewIsLoaded(ViewManager::AllGenres);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(switchOffAllViewsSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openChildView(QStringLiteral("genre1"), {}, {}, 0, ElisaUtils::Genre);

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(switchOffAllViewsSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(1).count(), 12);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ViewManager::ViewsType>(), ViewManager::AllArtistsFromGenre);

        viewManager.viewIsLoaded(ViewManager::AllArtistsFromGenre);

        viewManager.openChildView(QStringLiteral("artist1"), {}, {}, 0, ElisaUtils::Artist);

        QCOMPARE(openGridViewSpy.count(), 3);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(switchOffAllViewsSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(2).count(), 12);
        QCOMPARE(openGridViewSpy.at(2).at(0).value<ViewManager::ViewsType>(), ViewManager::OneArtistFromGenre);

        viewManager.viewIsLoaded(ViewManager::OneArtistFromGenre);
    }

    void openArtistViewAndAlbumFromAnotherArtistTest()
    {
        ViewManager viewManager;

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy openListViewSpy(&viewManager, &ViewManager::openListView);
        QSignalSpy switchFilesBrowserViewSpy(&viewManager, &ViewManager::switchFilesBrowserView);
        QSignalSpy switchOffAllViewsSpy(&viewManager, &ViewManager::switchOffAllViews);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.openChildView(QStringLiteral("artist1"), {}, {}, 0, ElisaUtils::Artist);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(switchOffAllViewsSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(0).count(), 12);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewManager::ViewsType>(), ViewManager::AllArtists);

        viewManager.viewIsLoaded(ViewManager::AllArtists);

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(switchOffAllViewsSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(1).count(), 12);
        QCOMPARE(openGridViewSpy.at(1).at(0).value<ViewManager::ViewsType>(), ViewManager::OneArtist);

        viewManager.viewIsLoaded(ViewManager::OneArtist);

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(switchOffAllViewsSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        viewManager.openChildView(QStringLiteral("album1"), QStringLiteral("artist2"), {}, 0, ElisaUtils::Album);

        QCOMPARE(openGridViewSpy.count(), 2);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(switchOffAllViewsSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 1);

        viewManager.viewIsLoaded(ViewManager::AllArtists);

        QCOMPARE(openGridViewSpy.count(), 3);
        QCOMPARE(openListViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(switchOffAllViewsSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 1);

        QCOMPARE(openGridViewSpy.at(2).count(), 12);
        QCOMPARE(openGridViewSpy.at(2).at(0).value<ViewManager::ViewsType>(), ViewManager::OneArtist);

        viewManager.viewIsLoaded(ViewManager::OneArtist);

        QCOMPARE(openGridViewSpy.count(), 3);
        QCOMPARE(openListViewSpy.count(), 1);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(switchOffAllViewsSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 1);

        QCOMPARE(openListViewSpy.at(0).count(), 8);
        QCOMPARE(openListViewSpy.at(0).at(0).value<ViewManager::ViewsType>(), ViewManager::OneAlbumFromArtist);
    }
};

QTEST_GUILESS_MAIN(ViewManagerTests)


#include "viewmanagertest.moc"
