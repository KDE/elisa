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
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import org.mgallien.QmlExtension 1.0
import org.kde.kirigami 1.0 as MobileComponents
import QtMultimedia 5.5

MobileComponents.Page {
    property var pagesModel
    property var contentDirectoryModel
    property MediaPlayList playListModel
    property var remoteMediaServer: ({})

    id: contentDirectoryRoot

    objectName: "ServerContent"
    color: MobileComponents.Theme.viewBackgroundColor

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        /*Button {
            id: backButton

            height: Screen.pixelDensity * 8.
            Layout.preferredHeight: height
            Layout.minimumHeight: height
            Layout.maximumHeight: height
            Layout.fillWidth: true

            onClicked: parentStackView.pop()
            text: 'Back'
        }*/
    }

    MediaServerListing {
        contentDirectoryService: contentDirectoryModel.contentDirectory
        rootIndex: '0'
        contentModel: contentDirectoryModel
        playListModel: contentDirectoryRoot.playListModel
        visible: false
    }
}
