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
import QtQml.Models 2.1
import org.mgallien.QmlExtension 1.0

ApplicationWindow {
    visible: true
    minimumWidth: 800
    minimumHeight: 400
    title: 'upnp'
    id: mainWindow

    UpnpSsdpEngine {
        id: mySsdpEngine
    }

    UpnpDeviceModel {
        id: deviceModel
        listenner: mySsdpEngine

        Component.onCompleted: {
            mySsdpEngine.initialize();
            mySsdpEngine.searchAllUpnpDevice();
        }
    }

    Component {
        id: rowDelegate
        Item {
            height: 45
            Rectangle {
                color: styleData.selected ? "#448" : (styleData.alternate ? "#eee" : "#fff")
                border.color:"#ccc"
                border.width: 1
                anchors.fill: parent
            }
        }
    }

    DelegateModel {
        id: delegateDeviceModel
        model: deviceModel

        delegate: Item {
            id: serviceItem
            width: peersView.width
            height: 50

            property UpnpControlSwitchPower upnpService

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    serviceItem.DelegateModel.inSelected = !serviceItem.DelegateModel.inSelected

                    var deviceType = deviceModel.get(serviceItem.DelegateModel.itemsIndex, 'upnpType')
                    var viewName = 'genericDevice.qml'

                    if (deviceType === 'urn:schemas-upnp-org:device:MediaServer:1') {
                        viewName = 'MediaServer.qml';
                    }

                    globalStackView.push({
                                    item: Qt.resolvedUrl(viewName),
                                    properties: {
                                        'parentStackView' : globalStackView,
                                        'aDevice' : deviceModel.getDeviceDescription(deviceModel.get(serviceItem.DelegateModel.itemsIndex, 'uuid'))
                                    }
                                })
                }
            }

            Label {
                id: nameLabel
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.rightMargin: 2
                anchors.leftMargin: 2
                text: {
                    if (model != undefined)
                        model.name
                    else
                        ""
                }
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                height: 30
                font.italic: true
                font.pointSize: 12
            }
            Label {
                id: deviceTypeLabel
                anchors.top: nameLabel.bottom
                anchors.bottom: parent.bottom
                anchors.right: nameLabel.right
                anchors.rightMargin: 8
                anchors.leftMargin: 2
                text: {if (model != undefined) model.upnpType ; if (model == undefined) ""}
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                height: 15
                visible: false
            }
        }
    }

    StackView {
        id: globalStackView
        anchors.fill: parent

        initialItem: TableView {
            id: peersView

            model: delegateDeviceModel

            rowDelegate: rowDelegate
            width: parent.width
            height: parent.height
            headerVisible: false
            TableViewColumn
            {
                width: peersView.width - 4
                resizable: false
                movable: false
            }
        }
    }
}
