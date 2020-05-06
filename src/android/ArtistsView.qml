/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
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
    property var contentModel: elisa.allArtistsProxyModel
    property bool showRating: false
    property bool delegateDisplaySecondaryText: true
    property bool expandedFilterView: false
    property var stackView

    title: i18nc("Title of the view of all artists", "Artists")

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
            delegateDisplaySecondaryText: pageElement.delegateDisplaySecondaryText

            onEnqueue: elisa.mediaPlayList.enqueue(databaseId, name, ElisaUtils.Artist,
                                                   ElisaUtils.AppendPlayList,
                                                   ElisaUtils.DoNotTriggerPlay)

            onReplaceAndPlay: elisa.mediaPlayList.enqueue(databaseId, name, ElisaUtils.Artist,
                                                          ElisaUtils.ReplacePlayList,
                                                          ElisaUtils.TriggerPlay)

            onOpen: pageElement.open(model.display, model.secondaryText, model.imageUrl, model.databaseId)
            onSelected: {
                forceActiveFocus()
                contentDirectoryView.currentIndex = model.index
            }
        }
    }
}
