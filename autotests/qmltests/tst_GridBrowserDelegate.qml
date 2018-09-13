/*
 * Copyright 2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

import QtQuick 2.3
import QtTest 1.0
import "../../src/qml"

FocusScope {
    GridBrowserDelegate {
        id: delegateItem

        height: 160
        width: 100

        shadowForImage: true
        mainText: "hello"
        secondaryText: "hello secondary"

        function i18nc() {
        }

        Item {
            id: myPalette

            property color text
            property color shadow
        }

        Item {
            id: elisaTheme
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

            var enqueueButtonItem = findChild(delegateItem, "enqueueButton");
            var openButtonItem = findChild(delegateItem, "openButton");
            var replaceAndPlayButtonItem = findChild(delegateItem, "replaceAndPlayButton");
            verify(enqueueButtonItem !== null, "valid enqueueButton")
            verify(openButtonItem !== null, "valid openButton")
            verify(replaceAndPlayButtonItem !== null, "valid replaceAndPlayButton")

            compare(enqueueButtonItem.focus, false, "enqueueButton.focus");
            compare(openButtonItem.focus, false, "enqueueButton.focus");
            compare(replaceAndPlayButtonItem.focus, false, "enqueueButton.focus");

            keyClick(Qt.Key_Tab)
            compare(enqueueSpy.count, 0)
            compare(replaceAndPlaySpy.count, 0)
            compare(openSpy.count, 0)
            compare(selectedSpy.count, 0)
            compare(delegateItem.focus, true, "delegateItem.focus");
            compare(enqueueButtonItem.focus, true, "enqueueButton.focus");
            compare(openButtonItem.focus, false, "enqueueButton.focus");
            compare(replaceAndPlayButtonItem.focus, false, "enqueueButton.focus");

            keyClick(Qt.Key_Tab)
            compare(enqueueSpy.count, 0)
            compare(replaceAndPlaySpy.count, 0)
            compare(openSpy.count, 0)
            compare(selectedSpy.count, 0)
            compare(delegateItem.focus, true, "delegateItem.focus");
            compare(enqueueButtonItem.focus, false, "enqueueButton.focus");
            compare(openButtonItem.focus, true, "enqueueButton.focus");
            compare(replaceAndPlayButtonItem.focus, false, "enqueueButton.focus");

            keyClick(Qt.Key_Tab)
            compare(enqueueSpy.count, 0)
            compare(replaceAndPlaySpy.count, 0)
            compare(openSpy.count, 0)
            compare(selectedSpy.count, 0)
            compare(delegateItem.focus, true, "delegateItem.focus");
            compare(enqueueButtonItem.focus, false, "enqueueButton.focus");
            compare(openButtonItem.focus, false, "enqueueButton.focus");
            compare(replaceAndPlayButtonItem.focus, true, "enqueueButton.focus");

            enqueueButtonItem.focus = false;
            openButtonItem.focus = false;
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

            var enqueueButtonItem = findChild(delegateItem, "enqueueButton");
            var openButtonItem = findChild(delegateItem, "openButton");
            var replaceAndPlayButtonItem = findChild(delegateItem, "replaceAndPlayButton");
            verify(enqueueButtonItem !== null, "valid enqueueButton")
            verify(openButtonItem !== null, "valid openButton")
            verify(replaceAndPlayButtonItem !== null, "valid replaceAndPlayButton")
            compare(enqueueButtonItem.focus, false, "enqueueButton.focus");
            compare(openButtonItem.focus, false, "enqueueButton.focus");
            compare(replaceAndPlayButtonItem.focus, false, "enqueueButton.focus");

            mouseClick(delegateItem, 0, 0);
            compare(delegateItem.focus, false, "delegateItem.focus");
            compare(enqueueButtonItem.focus, false, "enqueueButton.focus");
            compare(openButtonItem.focus, false, "enqueueButton.focus");
            compare(replaceAndPlayButtonItem.focus, false, "enqueueButton.focus");
            compare(enqueueSpy.count, 0);
            compare(replaceAndPlaySpy.count, 0);
            compare(openSpy.count, 0);
            compare(selectedSpy.count, 1);

            mouseDoubleClickSequence(delegateItem, 0, 0);
            openSpy.wait(150);
            compare(delegateItem.focus, false, "delegateItem.focus");
            compare(enqueueButtonItem.focus, false, "enqueueButton.focus");
            compare(openButtonItem.focus, false, "enqueueButton.focus");
            compare(replaceAndPlayButtonItem.focus, false, "enqueueButton.focus");
            compare(enqueueSpy.count, 0);
            compare(replaceAndPlaySpy.count, 0);
            compare(openSpy.count, 1);
            compare(selectedSpy.count, 2);

            mouseMove(enqueueButtonItem);
            mouseClick(enqueueButtonItem);
            compare(delegateItem.focus, false, "delegateItem.focus");
            compare(enqueueButtonItem.focus, false, "enqueueButton.focus");
            compare(openButtonItem.focus, false, "enqueueButton.focus");
            compare(replaceAndPlayButtonItem.focus, false, "enqueueButton.focus");
            compare(enqueueSpy.count, 1);
            compare(replaceAndPlaySpy.count, 0);
            compare(openSpy.count, 1);
            compare(selectedSpy.count, 2);

            mouseMove(openButtonItem);
            mouseClick(openButtonItem);
            compare(delegateItem.focus, false, "delegateItem.focus");
            compare(enqueueButtonItem.focus, false, "enqueueButton.focus");
            compare(openButtonItem.focus, false, "enqueueButton.focus");
            compare(replaceAndPlayButtonItem.focus, false, "enqueueButton.focus");
            compare(enqueueSpy.count, 1);
            compare(replaceAndPlaySpy.count, 0);
            compare(openSpy.count, 2);
            compare(selectedSpy.count, 2);

            mouseMove(replaceAndPlayButtonItem);
            mouseClick(replaceAndPlayButtonItem);
            compare(delegateItem.focus, false, "delegateItem.focus");
            compare(enqueueButtonItem.focus, false, "enqueueButton.focus");
            compare(openButtonItem.focus, false, "enqueueButton.focus");
            compare(replaceAndPlayButtonItem.focus, false, "enqueueButton.focus");
            compare(enqueueSpy.count, 1);
            compare(replaceAndPlaySpy.count, 1);
            compare(openSpy.count, 2);
            compare(selectedSpy.count, 2);

            mouseMove(otherItem, 0, 0);
            compare(delegateItem.focus, false, "delegateItem.focus");
            compare(enqueueButtonItem.focus, false, "enqueueButton.focus");
            compare(openButtonItem.focus, false, "enqueueButton.focus");
            compare(replaceAndPlayButtonItem.focus, false, "enqueueButton.focus");
            compare(enqueueSpy.count, 1);
            compare(replaceAndPlaySpy.count, 1);
            compare(openSpy.count, 2);
            compare(selectedSpy.count, 2);

            enqueueButtonItem.focus = false;
            openButtonItem.focus = false;
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

            var enqueueButtonItem = findChild(delegateItem, "enqueueButton");
            var openButtonItem = findChild(delegateItem, "openButton");
            var replaceAndPlayButtonItem = findChild(delegateItem, "replaceAndPlayButton");
            verify(enqueueButtonItem !== null, "valid enqueueButton")
            verify(openButtonItem !== null, "valid openButton")
            verify(replaceAndPlayButtonItem !== null, "valid replaceAndPlayButton")

            compare(enqueueButtonItem.focus, false, "enqueueButton.focus");
            compare(openButtonItem.focus, false, "enqueueButton.focus");
            compare(replaceAndPlayButtonItem.focus, false, "enqueueButton.focus");

            keyClick(Qt.Key_Tab);
            compare(enqueueSpy.count, 0);
            compare(replaceAndPlaySpy.count, 0);
            compare(openSpy.count, 0);
            compare(selectedSpy.count, 0);
            compare(delegateItem.focus, true, "delegateItem.focus");
            compare(enqueueButtonItem.focus, true, "enqueueButton.focus");
            compare(openButtonItem.focus, false, "enqueueButton.focus");
            compare(replaceAndPlayButtonItem.focus, false, "enqueueButton.focus");

            keyClick(Qt.Key_Tab);
            compare(enqueueSpy.count, 0);
            compare(replaceAndPlaySpy.count, 0);
            compare(openSpy.count, 0);
            compare(selectedSpy.count, 0);
            compare(delegateItem.focus, true, "delegateItem.focus");
            compare(enqueueButtonItem.focus, false, "enqueueButton.focus");
            compare(openButtonItem.focus, true, "enqueueButton.focus");
            compare(replaceAndPlayButtonItem.focus, false, "enqueueButton.focus");

            keyClick(Qt.Key_Tab);
            compare(enqueueSpy.count, 0);
            compare(replaceAndPlaySpy.count, 0);
            compare(openSpy.count, 0);
            compare(selectedSpy.count, 0);
            compare(delegateItem.focus, true, "delegateItem.focus");
            compare(enqueueButtonItem.focus, false, "enqueueButton.focus");
            compare(openButtonItem.focus, false, "enqueueButton.focus");
            compare(replaceAndPlayButtonItem.focus, true, "enqueueButton.focus");

            keyClick(Qt.Key_Enter);
            compare(enqueueSpy.count, 0);
            compare(replaceAndPlaySpy.count, 0);
            compare(openSpy.count, 1);
            compare(selectedSpy.count, 0);
            compare(delegateItem.focus, true, "delegateItem.focus");
            compare(enqueueButtonItem.focus, false, "enqueueButton.focus");
            compare(openButtonItem.focus, false, "enqueueButton.focus");
            compare(replaceAndPlayButtonItem.focus, true, "enqueueButton.focus");

            keyClick(Qt.Key_Return);
            compare(enqueueSpy.count, 0);
            compare(replaceAndPlaySpy.count, 0);
            compare(openSpy.count, 2);
            compare(selectedSpy.count, 0);
            compare(delegateItem.focus, true, "delegateItem.focus");
            compare(enqueueButtonItem.focus, false, "enqueueButton.focus");
            compare(openButtonItem.focus, false, "enqueueButton.focus");
            compare(replaceAndPlayButtonItem.focus, true, "enqueueButton.focus");

            enqueueButtonItem.focus = false;
            openButtonItem.focus = false;
            replaceAndPlayButtonItem.focus = false;
            otherItem.focus = true;
        }
    }
}
