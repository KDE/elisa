/*
   SPDX-FileCopyrightText: 2018 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.3
import QtTest 1.0
import "../../src/qml"

FocusScope {

    Item {
        id: persistentSettings

        property bool expandedFilterView: false
    }

    function i18nc(string1,string2) {
        return string2
    }

    function i18n(string) {
        return string;
    }


    Item {
        id: elisaTheme
        property int layoutHorizontalMargin: 8
        property int ratingStarSize: 16
    }

    SystemPalette {
        id: myPalette
        colorGroup: SystemPalette.Active
    }

    NavigationActionBar {
        id: navigationActionBar1

        mainTitle: 'testTitle1'
        secondaryTitle: 'secondaryTitle1'

        enableGoBack: true
        allowArtistNavigation: true
        showRating: true
        expandedFilterView: persistentSettings.expandedFilterView
        height: 100
    }

    NavigationActionBar {
        id: navigationActionBar2
        mainTitle: 'testTitle2'
        secondaryTitle: 'secondaryTitle2'

        enableGoBack: false
        allowArtistNavigation: false
        showRating: false
        expandedFilterView: persistentSettings.expandedFilterView

        height: 100
        y: 200
    }

    TestCase {
        name: "TestNavigationActionBar"

        SignalSpy {
            id: enqueueSpy1
            target: navigationActionBar1
            signalName: "enqueue"
        }

        SignalSpy {
            id: enqueueSpy2
            target: navigationActionBar2
            signalName: "enqueue"
        }

        SignalSpy {
            id: replaceAndPlaySpy1
            target: navigationActionBar1
            signalName: "replaceAndPlay"
        }

        SignalSpy {
            id: replaceAndPlaySpy2
            target: navigationActionBar2
            signalName: "replaceAndPlay"
        }

        SignalSpy {
            id: goBackSpy1
            target: navigationActionBar1
            signalName: "goBack"
        }

        SignalSpy {
            id: goBackSpy2
            target: navigationActionBar2
            signalName: "goBack"
        }

        SignalSpy {
            id: showArtistSpy1
            target: navigationActionBar1
            signalName: "showArtist"
        }

        SignalSpy {
            id: showArtistSpy2
            target: navigationActionBar2
            signalName: "showArtist"
        }

        when: windowShown

        function init() {
            enqueueSpy1.clear();
            enqueueSpy2.clear();
            replaceAndPlaySpy1.clear();
            replaceAndPlaySpy2.clear();
            goBackSpy1.clear();
            goBackSpy2.clear();
            showArtistSpy1.clear();
            showArtistSpy2.clear();
            persistentSettings.expandedFilterView = false;
        }

        function test_goBack() {
            compare(enqueueSpy1.count, 0);
            compare(enqueueSpy2.count, 0);
            compare(replaceAndPlaySpy1.count, 0);
            compare(replaceAndPlaySpy2.count, 0);
            compare(goBackSpy1.count, 0);
            compare(goBackSpy2.count, 0);
            compare(showArtistSpy1.count, 0);
            compare(showArtistSpy2.count, 0);

            var goPreviousButtonItem1 = findChild(navigationActionBar1, "goPreviousButton");
            verify(goPreviousButtonItem1 !== null, "valid goPreviousButton")
            mouseClick(goPreviousButtonItem1);
            wait(200)
            compare(goBackSpy1.count, 1);
            var goPreviousButtonItem2 = findChild(navigationActionBar2, "goPreviousButton");
            verify(goPreviousButtonItem2 !== null, "valid goPreviousButton")
            mouseClick(goPreviousButtonItem2);
            wait(200)
            compare(goBackSpy2.count, 0);
        }

        function test_enqueue() {
            compare(enqueueSpy1.count, 0);
            compare(enqueueSpy2.count, 0);
            compare(replaceAndPlaySpy1.count, 0);
            compare(replaceAndPlaySpy2.count, 0);
            compare(goBackSpy1.count, 0);
            compare(goBackSpy2.count, 0);
            compare(showArtistSpy1.count, 0);
            compare(showArtistSpy2.count, 0);

            var enqueueButtonItem = findChild(navigationActionBar1, "enqueueButton");
            verify(enqueueButtonItem !== null, "valid enqueueButton")
            mouseClick(enqueueButtonItem);
            compare(enqueueSpy1.count, 1);
        }

        function test_filterState() {
            compare(enqueueSpy1.count, 0);
            compare(enqueueSpy2.count, 0);
            compare(replaceAndPlaySpy1.count, 0);
            compare(replaceAndPlaySpy2.count, 0);
            compare(goBackSpy1.count, 0);
            compare(goBackSpy2.count, 0);
            compare(showArtistSpy1.count, 0);
            compare(showArtistSpy2.count, 0);

            var showFilterButtonItem1 = findChild(navigationActionBar1, "showFilterButton");
            verify(showFilterButtonItem1 !== null, "valid showFilterButton")
            mouseClick(showFilterButtonItem1);
            compare(navigationActionBar1.state,'expanded')
            var showFilterButtonItem2 = findChild(navigationActionBar2, "showFilterButton");
            verify(showFilterButtonItem2 !== null, "valid showFilterButton")
            mouseClick(showFilterButtonItem2);
            compare(navigationActionBar2.expandedFilterView, false)
            compare(navigationActionBar2.state, 'collapsed')
        }

        function test_replaceAndPlay() {
            compare(enqueueSpy1.count, 0);
            compare(enqueueSpy2.count, 0);
            compare(replaceAndPlaySpy1.count, 0);
            compare(replaceAndPlaySpy2.count, 0);
            compare(goBackSpy1.count, 0);
            compare(goBackSpy2.count, 0);
            compare(showArtistSpy1.count, 0);
            compare(showArtistSpy2.count, 0);

            var replaceAndPlayButtonItem = findChild(navigationActionBar1, "replaceAndPlayButton");
            verify(replaceAndPlayButtonItem !== null, "valid replaceAndPlayButton")
            mouseClick(replaceAndPlayButtonItem);
            compare(replaceAndPlaySpy1.count, 1);
        }

        function test_showArtist() {
            compare(enqueueSpy1.count, 0);
            compare(enqueueSpy2.count, 0);
            compare(replaceAndPlaySpy1.count, 0);
            compare(replaceAndPlaySpy2.count, 0);
            compare(goBackSpy1.count, 0);
            compare(goBackSpy2.count, 0);
            compare(showArtistSpy1.count, 0);
            compare(showArtistSpy2.count, 0);

            var showArtistButtonItem1 = findChild(navigationActionBar1, "showArtistButton");
            verify(showArtistButtonItem1 !== null, "valid showArtistButton")
            mouseClick(showArtistButtonItem1);
            compare(showArtistSpy1.count, 1);
            // artist button is unloaded in navigationActionBar2
        }

        function test_filterRating() {
            persistentSettings.expandedFilterView = true;
            wait(200);
            var ratingFilterItem1 = findChild(navigationActionBar1, "ratingFilter");
            verify(ratingFilterItem1 !== null, "valid ratingFilter")
            mouseClick(ratingFilterItem1,1);
            compare(navigationActionBar1.filterRating, 2);
            mouseClick(ratingFilterItem1,1);
            compare(navigationActionBar1.filterRating, 0);
            mouseClick(ratingFilterItem1,1 + elisaTheme.ratingStarSize);
            compare(navigationActionBar1.filterRating, 4);
            mouseClick(ratingFilterItem1,1 + elisaTheme.ratingStarSize);
            compare(navigationActionBar1.filterRating, 0);
            mouseClick(ratingFilterItem1,1 + 2 * elisaTheme.ratingStarSize);
            compare(navigationActionBar1.filterRating, 6);
            mouseClick(ratingFilterItem1,1 + 2 * elisaTheme.ratingStarSize);
            compare(navigationActionBar1.filterRating, 0);
            mouseClick(ratingFilterItem1,1 + 3 * elisaTheme.ratingStarSize);
            compare(navigationActionBar1.filterRating, 8);
            mouseClick(ratingFilterItem1,1 + 3 * elisaTheme.ratingStarSize);
            compare(navigationActionBar1.filterRating, 0);
            mouseClick(ratingFilterItem1,1 + 4 * elisaTheme.ratingStarSize);
            compare(navigationActionBar1.filterRating, 10);
            mouseClick(ratingFilterItem1,1 + 4 * elisaTheme.ratingStarSize);
            compare(navigationActionBar1.filterRating, 0);

            var ratingFilterItem2 = findChild(navigationActionBar2, "ratingFilter");
            verify(ratingFilterItem2 !== null, "valid ratingFilter")
            mouseClick(ratingFilterItem2,1);
            compare(navigationActionBar2.filterRating, 0);
            mouseClick(ratingFilterItem2,1 + elisaTheme.ratingStarSize);
            compare(navigationActionBar2.filterRating, 0);
            mouseClick(ratingFilterItem2,1 + 2 * elisaTheme.ratingStarSize);
            compare(navigationActionBar2.filterRating, 0);
            mouseClick(ratingFilterItem2,1 + 3 * elisaTheme.ratingStarSize);
            compare(navigationActionBar2.filterRating, 0);
            mouseClick(ratingFilterItem2,1 + 4 * elisaTheme.ratingStarSize);
            compare(navigationActionBar2.filterRating, 0);
        }

        function test_filterText() {
            navigationActionBar1.expandedFilterView = true
            navigationActionBar2.expandedFilterView = false
            wait(300)
            var textsFilterItem1 = findChild(navigationActionBar1, "filterTextInput");
            verify(textsFilterItem1 !== null, "valid filterTextInput")
            textsFilterItem1.focus = false
            compare(textsFilterItem1.focus, false);
            mouseClick(textsFilterItem1);
            compare(textsFilterItem1.focus, true);
            keyClick(Qt.Key_T);
            keyClick(Qt.Key_E);
            keyClick(Qt.Key_S);
            keyClick(Qt.Key_T);
            compare(navigationActionBar1.filterText, 'test');
            wait(300)
            mouseClick(textsFilterItem1,textsFilterItem1.width - 20);
            compare(navigationActionBar1.filterText, "");
        }
    }
}
