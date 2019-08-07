/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtQml.Models 2.1
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0

import org.kde.elisa 1.0

FocusScope {
    id: gridView

    property bool isSubPage: false
    property string mainTitle
    property string secondaryTitle
    property url image
    property alias contentModel: contentDirectoryView.model
    property alias showRating: navigationBar.showRating
    property bool delegateDisplaySecondaryText: true
    property alias expandedFilterView: navigationBar.expandedFilterView
    property var stackView
    property url defaultIcon

    signal enqueue(int databaseId, string name)
    signal replaceAndPlay(int databaseId, string name)
    signal open(string innerMainTitle, string innerSecondaryTitle, url innerImage, int databaseId, var dataType, var showDiscHeader)
    signal goBack()

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        NavigationActionBar {
            id: navigationBar

            mainTitle: gridView.mainTitle
            secondaryTitle: gridView.secondaryTitle
            image: gridView.image
            enableGoBack: isSubPage
            sortOrder: if (contentModel) {contentModel.sortedAscending} else true

            Layout.fillWidth: true

            Loader {
                active: contentModel !== undefined

                sourceComponent: Binding {
                    target: contentModel
                    property: 'filterText'
                    value: navigationBar.filterText
                }
            }

            Loader {
                active: contentModel

                sourceComponent: Binding {
                    target: contentModel
                    property: 'filterRating'
                    value: navigationBar.filterRating
                }
            }

            onEnqueue: contentModel.enqueueToPlayList()

            onReplaceAndPlay:contentModel.replaceAndPlayOfPlayList()

            onGoBack: gridView.goBack()

            onSort: contentModel.sortModel(order)
        }

        FocusScope {

            Layout.fillHeight: true
            Layout.fillWidth: true

            clip: true

            GridView {
                id: contentDirectoryView
                property int availableWidth: width - scrollBar.width

                activeFocusOnTab: true
                keyNavigationEnabled: true

                anchors.fill: parent
                anchors.margins: elisaTheme.layoutHorizontalMargin

                ScrollBar.vertical: ScrollBar {
                    id: scrollBar
                }
                boundsBehavior: Flickable.StopAtBounds

                currentIndex: -1

                Accessible.role: Accessible.List
                Accessible.name: mainTitle

                TextMetrics {
                    id: secondaryLabelSize
                    text: 'example'
                }

                ScrollHelper {
                    id: scrollHelper
                    flickable: contentDirectoryView
                    anchors.fill: contentDirectoryView
                }

                cellWidth: Math.floor(availableWidth / Math.max(Math.floor(availableWidth / elisaTheme.gridDelegateWidth), 2))
                cellHeight: delegateDisplaySecondaryText ?
                                elisaTheme.gridDelegateHeight :
                                elisaTheme.gridDelegateHeight - (secondaryLabelSize.boundingRect.height - secondaryLabelSize.boundingRect.y)

                delegate: GridBrowserDelegate {
                    width: elisaTheme.gridDelegateWidth
                    height: contentDirectoryView.cellHeight

                    focus: true

                    isSelected: contentDirectoryView.currentIndex === index

                    isPartial: false

                    mainText: model.display
                    secondaryText: if (gridView.delegateDisplaySecondaryText) {model.secondaryText} else {""}
                    imageUrl: (model && model.imageUrl && model.imageUrl.toString() !== "" ? model.imageUrl : defaultIcon)
                    shadowForImage: (model && model.imageUrl && model.imageUrl.toString() !== "" ? true : false)
                    databaseId: model.databaseId
                    delegateDisplaySecondaryText: gridView.delegateDisplaySecondaryText

                    onEnqueue: gridView.enqueue(databaseId, name)
                    onReplaceAndPlay: gridView.replaceAndPlay(databaseId, name)
                    onOpen: gridView.open(model.display, model.secondaryText,
                                          (model && model.imageUrl && model.imageUrl.toString() !== "" ? model.imageUrl : defaultIcon),
                                          model.databaseId, model.dataType, (model.isSingleDiscAlbum ? ViewManager.NoDiscHeaders : ViewManager.DiscHeaders))
                    onSelected: {
                        forceActiveFocus()
                        contentDirectoryView.currentIndex = model.index
                    }

                    onActiveFocusChanged: {
                        if (activeFocus && contentDirectoryView.currentIndex !== model.index) {
                            contentDirectoryView.currentIndex = model.index
                        }
                    }
                }
            }
        }
    }
}
