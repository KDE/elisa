/*
 * Copyright 2014 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import org.mgallien.QmlExtension 1.0

ApplicationWindow {
    visible: true
    minimumWidth: 150
    minimumHeight: 100
    title: 'upnp'
    id: mainWindow

    Item {
        id: upnpLight1

        UpnpSsdpEngine {
            id: mySsdpEngine
        }

        BinaryLight {
            id: light1
        }

        Component.onCompleted: {
            mySsdpEngine.subscribeDevice(light1)
        }
    }

    Button {
        id: publishLigth
        text: "Publish Light"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 40

        onClicked: {
            mySsdpEngine.publishDevice(light1);
        }
    }

    Button {
        id: switchLigth
        text: "Switch Light"
        anchors.top: publishLigth.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 40

        onClicked: {
            light1.serviceById('urn:upnp-org:serviceId:SwitchPower').switchTarget();
        }
    }
}
