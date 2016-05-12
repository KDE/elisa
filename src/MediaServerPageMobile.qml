/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.2
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQml.Models 2.1
import org.kde.kirigami 1.0 as MobileComponents
import org.mgallien.QmlExtension 1.0

MobileComponents.Page {
    property var serverModel
    property var globalPlayListModel
    property string globalBrowseFlag: 'BrowseDirectChildren'
    property string globalFilter: '*'
    property string globalSortCriteria: ''

    id: rootItem
    title: "UPnP Servers"

    objectName: "ServerList"

    ColumnLayout {
        anchors.fill: parent

        ScrollView {
            Layout.fillHeight: true
            Layout.fillWidth: true

            ListView {
                id: serverView
                model: DelegateModel {
                    model: serverModel

                    delegate: MobileComponents.AbstractListItem {
                        enabled: true
                        supportsMouseEvents: true

                        MobileComponents.Label {
                            id: nameLabel
                            anchors.fill: parent
                            text: model.name
                        }


                        onClicked: {
                            contentDirectoryItem.contentModel = serverModel.remoteAlbumModel(DelegateModel.itemsIndex)
                            contentDirectoryItem.serverName = serverModel.remoteAlbumModel(DelegateModel.itemsIndex).serverName
                            stackView.push(contentDirectoryItem)
                        }
                    }
                }
            }
        }

        MediaServerListingMobile {
            id: contentDirectoryItem

            playListModel: globalPlayListModel

            visible: false
        }
    }
}
