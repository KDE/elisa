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

import QtQuick 2.11
import QtQuick.Controls 2.4
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0

Kirigami.Page {
    id: pageElement

    property bool isSubPage: false
    property string mainTitle
    property string secondaryTitle
    property url image
    property var contentModel: elisa.allAlbumsProxyModel
    property bool showRating: false
    property bool delegateDisplaySecondaryText: true
    property bool expandedFilterView: false
    property var stackView

    title: i18nc("Title of the view of all albums", "Albums")

    GridView {
        id: gridView

        focus: true
        anchors.fill: parent

        cellWidth: gridView.width / 2
        cellHeight: gridView.width / 2

        model: contentModel

        delegate: GridBrowserDelegate {
            width: gridView.cellWidth
            height: gridView.cellHeight

            focus: true

            isPartial: false

            mainText: model.display
            secondaryText: if (pageElement.delegateDisplaySecondaryText) {model.secondaryText} else {""}
            imageUrl: model.imageUrl
            shadowForImage: if (model.shadowForImage) {model.shadowForImage} else {false}
            containerData: model.containerData
            delegateDisplaySecondaryText: pageElement.delegateDisplaySecondaryText

            onEnqueue: elisa.mediaPlayList.enqueue(data)
            onReplaceAndPlay: elisa.mediaPlayList.replaceAndPlay(data)
            onOpen: pageElement.open(model.display, model.secondaryText, model.imageUrl, model.databaseId)
            onSelected: {
                forceActiveFocus()
                contentDirectoryView.currentIndex = model.index
            }
        }
    }
}
