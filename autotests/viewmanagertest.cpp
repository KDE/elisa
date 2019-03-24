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
    }

    void closeAllViewsTest()
    {
        ViewManager viewManager;

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy switchRecentlyPlayedTracksViewSpy(&viewManager, &ViewManager::switchRecentlyPlayedTracksView);
        QSignalSpy switchFrequentlyPlayedTracksViewSpy(&viewManager, &ViewManager::switchFrequentlyPlayedTracksView);
        QSignalSpy switchOneAlbumViewSpy(&viewManager, &ViewManager::switchOneAlbumView);
        QSignalSpy switchAllTracksViewSpy(&viewManager, &ViewManager::switchAllTracksView);
        QSignalSpy switchFilesBrowserViewSpy(&viewManager, &ViewManager::switchFilesBrowserView);
        QSignalSpy switchOffAllViewsSpy(&viewManager, &ViewManager::switchOffAllViews);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.closeAllViews();

        QCOMPARE(openGridViewSpy.count(), 0);
        QCOMPARE(switchRecentlyPlayedTracksViewSpy.count(), 0);
        QCOMPARE(switchFrequentlyPlayedTracksViewSpy.count(), 0);
        QCOMPARE(switchOneAlbumViewSpy.count(), 0);
        QCOMPARE(switchAllTracksViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(switchOffAllViewsSpy.count(), 1);
        QCOMPARE(popOneViewSpy.count(), 0);
    }

    void openAlbumViewTest()
    {
        ViewManager viewManager;

        QSignalSpy openGridViewSpy(&viewManager, &ViewManager::openGridView);
        QSignalSpy switchRecentlyPlayedTracksViewSpy(&viewManager, &ViewManager::switchRecentlyPlayedTracksView);
        QSignalSpy switchFrequentlyPlayedTracksViewSpy(&viewManager, &ViewManager::switchFrequentlyPlayedTracksView);
        QSignalSpy switchOneAlbumViewSpy(&viewManager, &ViewManager::switchOneAlbumView);
        QSignalSpy switchAllTracksViewSpy(&viewManager, &ViewManager::switchAllTracksView);
        QSignalSpy switchFilesBrowserViewSpy(&viewManager, &ViewManager::switchFilesBrowserView);
        QSignalSpy switchOffAllViewsSpy(&viewManager, &ViewManager::switchOffAllViews);
        QSignalSpy popOneViewSpy(&viewManager, &ViewManager::popOneView);

        viewManager.openChildView(QStringLiteral("album1"), QStringLiteral("artist1"), {}, 12, ElisaUtils::Album);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(switchRecentlyPlayedTracksViewSpy.count(), 0);
        QCOMPARE(switchFrequentlyPlayedTracksViewSpy.count(), 0);
        QCOMPARE(switchOneAlbumViewSpy.count(), 0);
        QCOMPARE(switchAllTracksViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(switchOffAllViewsSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);

        QCOMPARE(openGridViewSpy.at(0).count(), 11);
        QCOMPARE(openGridViewSpy.at(0).at(0).value<ViewManager::ViewsType>(), ViewManager::AllAlbums);

        viewManager.viewIsLoaded(ViewManager::AllAlbums);

        QCOMPARE(openGridViewSpy.count(), 1);
        QCOMPARE(switchRecentlyPlayedTracksViewSpy.count(), 0);
        QCOMPARE(switchFrequentlyPlayedTracksViewSpy.count(), 0);
        QCOMPARE(switchOneAlbumViewSpy.count(), 1);
        QCOMPARE(switchAllTracksViewSpy.count(), 0);
        QCOMPARE(switchFilesBrowserViewSpy.count(), 0);
        QCOMPARE(switchOffAllViewsSpy.count(), 0);
        QCOMPARE(popOneViewSpy.count(), 0);
    }
};

QTEST_GUILESS_MAIN(ViewManagerTests)


#include "viewmanagertest.moc"
