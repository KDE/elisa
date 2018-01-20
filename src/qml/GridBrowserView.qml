/*
 * Copyright 2016-2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.2
import QtQml.Models 2.1
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0

import org.kde.elisa 1.0

FocusScope {
    id: gridView

    property bool isFirstPage: false
    property string mainTitle
    property string secondaryTitle
    property url image
    property alias model: proxyModel.model
    property bool showRating: true
    property bool delegateDisplaySecondaryText: true

    signal enqueue(var data)
    signal replaceAndPlay(var data)
    signal open(var innerModel, var innerMainTitle, var innerSecondaryTitle, var innerImage, var databaseId)
    signal goBack()

    SystemPalette {
        id: myPalette
        colorGroup: SystemPalette.Active
    }

    Theme {
        id: elisaTheme
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Loader {
            sourceComponent: FilterBar {
                id: filterBar

                labelText: mainTitle
                showRating: gridView.showRating

                anchors.fill: parent

                Binding {
                    target: model
                    property: 'filterText'
                    value: filterBar.filterText
                    when: isFirstPage
                }

                Binding {
                    target: model
                    property: 'filterRating'
                    value: filterBar.filterRating
                    when: isFirstPage
                }
            }

            height: elisaTheme.navigationBarHeight
            Layout.preferredHeight: height
            Layout.minimumHeight: height
            Layout.maximumHeight: height
            Layout.fillWidth: true

            active: isFirstPage
            visible: isFirstPage
        }

        Loader {
            sourceComponent: NavigationActionBar {
                id: navigationBar

                mainTitle: gridView.mainTitle
                secondaryTitle: gridView.secondaryTitle
                image: gridView.image

                anchors.fill: parent

                onEnqueue: gridView.enqueue(mainTitle)
                onReplaceAndPlay: gridView.replaceAndPlay(mainTitle)
                onGoBack: gridView.goBack()
            }

            height: elisaTheme.navigationBarHeight
            Layout.preferredHeight: height
            Layout.minimumHeight: height
            Layout.maximumHeight: height
            Layout.fillWidth: true

            active: !isFirstPage
            visible: !isFirstPage
        }

        Rectangle {
            color: myPalette.base

            Layout.fillHeight: true
            Layout.fillWidth: true

            ScrollView {
                anchors.fill: parent
                flickableItem.boundsBehavior: Flickable.StopAtBounds
                flickableItem.interactive: true

                GridView {
                    id: contentDirectoryView

                    focus: true

                    TextMetrics {
                        id: secondaryLabelSize
                        text: 'example'
                    }

                    cellWidth: elisaTheme.gridDelegateWidth
                    cellHeight: (delegateDisplaySecondaryText ? elisaTheme.gridDelegateHeight : elisaTheme.gridDelegateHeight - secondaryLabelSize.height)

                    model: DelegateModel {
                        id: proxyModel

                        delegate: GridBrowserDelegate {
                            width: contentDirectoryView.cellWidth
                            height: contentDirectoryView.cellHeight

                            focus: true

                            mainText: model.display
                            secondaryText: model.secondaryText
                            imageUrl: model.imageUrl
                            shadowForImage: model.shadowForImage
                            containerData: model.containerData
                            delegateDisplaySecondaryText: gridView.delegateDisplaySecondaryText

                            onEnqueue: gridView.enqueue(data)
                            onReplaceAndPlay: gridView.replaceAndPlay(data)
                            onOpen: gridView.open(model.childModel, model.display, model.secondaryText, model.imageUrl, model.databaseId)
                            onSelected: {
                                forceActiveFocus()
                                contentDirectoryView.currentIndex = model.index
                            }
                        }
                    }
                }
            }
        }
    }
}
