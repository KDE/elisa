/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.3
import QtTest 1.0
import "../../src/qml"

FocusScope {
    GridBrowserDelegate {
        id: delegateItem

        height: 160
        width: 100

        mainText: "hello"
        secondaryText: "hello secondary"
        databaseId: 1

        function i18nc(string1,string2) {
            return string2
        }

        Item {
            id: myPalette

            property color text
            property color shadow
            property color highlight
        }
    }

    Item {
        id: otherItem

        focus: true

        width: 50
        height: 50
        anchors.top: delegateItem.bottom
    }

    TestCase {
        name: "TestGridBrowserDelegate"

        SignalSpy {
            id: enqueueSpy
            target: delegateItem
            signalName: "enqueue"
        }

        SignalSpy {
            id: replaceAndPlaySpy
            target: delegateItem
            signalName: "replaceAndPlay"
        }

        SignalSpy {
            id: openSpy
            target: delegateItem
            signalName: "open"
        }

        SignalSpy {
            id: selectedSpy
            target: delegateItem
            signalName: "selected"
        }

        when: windowShown

        function init() {
            otherItem.focus = true
            enqueueSpy.clear();
            replaceAndPlaySpy.clear();
            openSpy.clear();
            selectedSpy.clear();
        }

        function test_focus() {
            compare(enqueueSpy.count, 0);
            compare(replaceAndPlaySpy.count, 0);
            compare(openSpy.count, 0);
            compare(selectedSpy.count, 0);
            compare(delegateItem.focus, false, "delegateItem.focus");

            delegateItem.forceActiveFocus();
            compare(enqueueSpy.count, 0);
            compare(replaceAndPlaySpy.count, 0);
            compare(openSpy.count, 0);
            compare(selectedSpy.count, 0);
            compare(delegateItem.focus, true, "delegateItem.focus");

            var replaceAndPlayButtonItem = findChild(delegateItem, "replaceAndPlayButton");
            var enqueueButtonItem = findChild(delegateItem, "enqueueButton");
            verify(replaceAndPlayButtonItem !== null, "valid replaceAndPlayButton")
            verify(enqueueButtonItem !== null, "valid enqueueButton")

            compare(replaceAndPlayButtonItem.focus, false, "enqueueButton.focus");
            compare(enqueueButtonItem.focus, false, "enqueueButton.focus");

            keyClick(Qt.Key_Tab)
            compare(enqueueSpy.count, 0)
            compare(replaceAndPlaySpy.count, 0)
            compare(openSpy.count, 0)
            compare(selectedSpy.count, 0)
            compare(delegateItem.focus, true, "delegateItem.focus");
            compare(replaceAndPlayButtonItem.focus, true, "enqueueButton.focus");
            compare(enqueueButtonItem.focus, false, "enqueueButton.focus");

            keyClick(Qt.Key_Tab)
            compare(enqueueSpy.count, 0)
            compare(replaceAndPlaySpy.count, 0)
            compare(openSpy.count, 0)
            compare(selectedSpy.count, 0)
            compare(delegateItem.focus, true, "delegateItem.focus");
            compare(replaceAndPlayButtonItem.focus, false, "enqueueButton.focus");
            compare(enqueueButtonItem.focus, true, "enqueueButton.focus");

            enqueueButtonItem.focus = false;
            replaceAndPlayButtonItem.focus = false;
            otherItem.focus = true;
            compare(enqueueSpy.count, 0)
            compare(replaceAndPlaySpy.count, 0)
            compare(openSpy.count, 0)
            compare(selectedSpy.count, 0)
            compare(delegateItem.focus, false, "delegateItem.focus");
        }

        function test_mouse_clicks() {
            compare(delegateItem.focus, false, "delegateItem.focus");
            compare(enqueueSpy.count, 0);
            compare(replaceAndPlaySpy.count, 0);
            compare(openSpy.count, 0);
            compare(selectedSpy.count, 0);

            mouseMove(delegateItem, 0, 0);
            compare(delegateItem.focus, false, "delegateItem.focus");
            compare(enqueueSpy.count, 0);
            compare(replaceAndPlaySpy.count, 0);
            compare(openSpy.count, 0);
            compare(selectedSpy.count, 0);

            var replaceAndPlayButtonItem = findChild(delegateItem, "replaceAndPlayButton");
            var enqueueButtonItem = findChild(delegateItem, "enqueueButton");
            verify(replaceAndPlayButtonItem !== null, "valid replaceAndPlayButton")
            verify(enqueueButtonItem !== null, "valid enqueueButton")
            compare(replaceAndPlayButtonItem.focus, false, "enqueueButton.focus");
            compare(enqueueButtonItem.focus, false, "enqueueButton.focus");

            mouseClick(delegateItem, 0, 0);
            compare(delegateItem.focus, false, "delegateItem.focus");
            compare(replaceAndPlayButtonItem.focus, false, "enqueueButton.focus");
            compare(enqueueButtonItem.focus, false, "enqueueButton.focus");
            compare(enqueueSpy.count, 0);
            compare(replaceAndPlaySpy.count, 0);
            compare(openSpy.count, 1);
            compare(selectedSpy.count, 0);

            mouseMove(enqueueButtonItem);
            mouseClick(enqueueButtonItem);
            compare(delegateItem.focus, true, "delegateItem.focus");
            compare(replaceAndPlayButtonItem.focus, false, "enqueueButton.focus");
            compare(enqueueButtonItem.focus, true, "enqueueButton.focus");
            compare(enqueueSpy.count, 1);
            compare(replaceAndPlaySpy.count, 0);
            compare(openSpy.count, 1);
            compare(selectedSpy.count, 0);

            mouseMove(replaceAndPlayButtonItem);
            mouseClick(replaceAndPlayButtonItem);
            compare(delegateItem.focus, true, "delegateItem.focus");
            compare(replaceAndPlayButtonItem.focus, true, "enqueueButton.focus");
            compare(enqueueButtonItem.focus, false, "enqueueButton.focus");
            compare(enqueueSpy.count, 1);
            compare(replaceAndPlaySpy.count, 1);
            compare(openSpy.count, 1);
            compare(selectedSpy.count, 0);

            mouseMove(otherItem, 0, 0);
            compare(delegateItem.focus, true, "delegateItem.focus");
            compare(replaceAndPlayButtonItem.focus, true, "enqueueButton.focus");
            compare(enqueueButtonItem.focus, false, "enqueueButton.focus");
            compare(enqueueSpy.count, 1);
            compare(replaceAndPlaySpy.count, 1);
            compare(openSpy.count, 1);
            compare(selectedSpy.count, 0);

            mouseClick(delegateItem, 0, 0);
            openSpy.wait(150);
            compare(delegateItem.focus, true, "delegateItem.focus");
            compare(replaceAndPlayButtonItem.focus, true, "enqueueButton.focus");
            compare(enqueueButtonItem.focus, false, "enqueueButton.focus");
            compare(enqueueSpy.count, 1);
            compare(replaceAndPlaySpy.count, 1);
            compare(openSpy.count, 2);
            compare(selectedSpy.count, 0);

            enqueueButtonItem.focus = false;
            replaceAndPlayButtonItem.focus = false;
            otherItem.focus = true;
        }

        function test_keyboard() {
            compare(enqueueSpy.count, 0);
            compare(replaceAndPlaySpy.count, 0);
            compare(openSpy.count, 0);
            compare(selectedSpy.count, 0);
            compare(delegateItem.focus, false, "delegateItem.focus");

            delegateItem.forceActiveFocus();
            compare(enqueueSpy.count, 0);
            compare(replaceAndPlaySpy.count, 0);
            compare(openSpy.count, 0);
            compare(selectedSpy.count, 0);
            compare(delegateItem.focus, true, "delegateItem.focus");

            var replaceAndPlayButtonItem = findChild(delegateItem, "replaceAndPlayButton");
            var enqueueButtonItem = findChild(delegateItem, "enqueueButton");
            verify(replaceAndPlayButtonItem !== null, "valid replaceAndPlayButton")
            verify(enqueueButtonItem !== null, "valid enqueueButton")

            compare(replaceAndPlayButtonItem.focus, false, "enqueueButton.focus");
            compare(enqueueButtonItem.focus, false, "enqueueButton.focus");

            keyClick(Qt.Key_Tab);
            compare(replaceAndPlaySpy.count, 0);
            compare(enqueueSpy.count, 0);
            compare(openSpy.count, 0);
            compare(selectedSpy.count, 0);
            compare(delegateItem.focus, true, "delegateItem.focus");
            compare(replaceAndPlayButtonItem.focus, true, "enqueueButton.focus");
            compare(enqueueButtonItem.focus, false, "enqueueButton.focus");

            keyClick(Qt.Key_Enter);
            compare(replaceAndPlaySpy.count, 1);
            compare(enqueueSpy.count, 0);
            compare(openSpy.count, 0);
            compare(selectedSpy.count, 0);
            compare(delegateItem.focus, true, "delegateItem.focus");
            compare(replaceAndPlayButtonItem.focus, true, "enqueueButton.focus");
            compare(enqueueButtonItem.focus, false, "enqueueButton.focus");

            keyClick(Qt.Key_Return);
            compare(replaceAndPlaySpy.count, 2);
            compare(enqueueSpy.count, 0);
            compare(openSpy.count, 0);
            compare(selectedSpy.count, 0);
            compare(delegateItem.focus, true, "delegateItem.focus");
            compare(replaceAndPlayButtonItem.focus, true, "enqueueButton.focus");
            compare(enqueueButtonItem.focus, false, "enqueueButton.focus");

            keyClick(Qt.Key_Tab);
            compare(replaceAndPlaySpy.count, 2);
            compare(enqueueSpy.count, 0);
            compare(openSpy.count, 0);
            compare(selectedSpy.count, 0);
            compare(delegateItem.focus, true, "delegateItem.focus");
            compare(replaceAndPlayButtonItem.focus, false, "enqueueButton.focus");
            compare(enqueueButtonItem.focus, true, "enqueueButton.focus");

            keyClick(Qt.Key_Enter);
            compare(replaceAndPlaySpy.count, 2);
            compare(enqueueSpy.count, 1);
            compare(openSpy.count, 0);
            compare(selectedSpy.count, 0);
            compare(delegateItem.focus, true, "delegateItem.focus");
            compare(replaceAndPlayButtonItem.focus, false, "enqueueButton.focus");
            compare(enqueueButtonItem.focus, true, "enqueueButton.focus");

            keyClick(Qt.Key_Return);
            compare(replaceAndPlaySpy.count, 2);
            compare(enqueueSpy.count, 2);
            compare(openSpy.count, 0);
            compare(selectedSpy.count, 0);
            compare(delegateItem.focus, true, "delegateItem.focus");
            compare(replaceAndPlayButtonItem.focus, false, "enqueueButton.focus");
            compare(enqueueButtonItem.focus, true, "enqueueButton.focus");

            keyClick(Qt.Key_Tab);
            compare(replaceAndPlaySpy.count, 2);
            compare(enqueueSpy.count, 2);
            compare(openSpy.count, 0);
            compare(selectedSpy.count, 0);
            compare(delegateItem.focus, true, "delegateItem.focus");
            compare(replaceAndPlayButtonItem.focus, true, "enqueueButton.focus");
            compare(enqueueButtonItem.focus, false, "enqueueButton.focus");

            enqueueButtonItem.focus = false;
            replaceAndPlayButtonItem.focus = false;
            otherItem.focus = true;
        }
    }
}
