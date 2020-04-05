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
import org.kde.kirigami 2.5 as Kirigami
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
    signal open(string innerMainTitle, string innerSecondaryTitle, url innerImage, int databaseId, var dataType)
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
                property int availableWidth: scrollBar.visible ? width - scrollBar.width : width

                activeFocusOnTab: true
                keyNavigationEnabled: true

                anchors.fill: parent
                anchors.leftMargin: (LayoutMirroring.enabled && scrollBar.visible) ? 0 : Kirigami.Units.largeSpacing
                anchors.rightMargin: (!LayoutMirroring.enabled && scrollBar.visible) ? 0 : Kirigami.Units.largeSpacing
                anchors.topMargin: Kirigami.Units.largeSpacing
                anchors.bottomMargin: Kirigami.Units.largeSpacing

                ScrollBar.vertical: ScrollBar {
                    id: scrollBar
                }
                boundsBehavior: Flickable.StopAtBounds

                currentIndex: -1

                Accessible.role: Accessible.List
                Accessible.name: mainTitle

                ScrollHelper {
                    id: scrollHelper
                    flickable: contentDirectoryView
                    anchors.fill: contentDirectoryView
                }

                cellWidth: Math.floor(availableWidth / Math.max(Math.floor(availableWidth / elisaTheme.gridDelegateSize), 2))
                cellHeight: elisaTheme.gridDelegateSize + Kirigami.Units.gridUnit * 2 + Kirigami.Units.largeSpacing

                delegate: GridBrowserDelegate {
                    width: elisaTheme.gridDelegateSize
                    height: contentDirectoryView.cellHeight

                    focus: true

                    isSelected: contentDirectoryView.currentIndex === index

                    isPartial: false

                    mainText: model.display
                    fileUrl: model.url
                    secondaryText: if (gridView.delegateDisplaySecondaryText) {model.secondaryText} else {""}
                    imageUrl: model.imageUrl ? model.imageUrl : ''
                    imageFallbackUrl: defaultIcon
                    databaseId: model.databaseId
                    delegateDisplaySecondaryText: gridView.delegateDisplaySecondaryText
                    entryType: model.dataType

                    onEnqueue: gridView.enqueue(databaseId, name)
                    onReplaceAndPlay: gridView.replaceAndPlay(databaseId, name)
                    onOpen: gridView.open(model.display, model.secondaryText,
                                          (model && model.imageUrl && model.imageUrl.toString() !== "" ? model.imageUrl : defaultIcon),
                                          model.databaseId, model.dataType)
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
