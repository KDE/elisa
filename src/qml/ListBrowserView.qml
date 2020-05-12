/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtQml.Models 2.2
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0

import org.kde.elisa 1.0

FocusScope {
    id: listView

    property bool isSubPage: false
    property alias mainTitle: navigationBar.mainTitle
    property alias secondaryTitle: navigationBar.secondaryTitle
    property alias image: navigationBar.image
    property int databaseId
    property alias delegate: delegateModel.delegate
    property bool showSection: false
    property AbstractProxyModel contentModel
    property alias expandedFilterView: navigationBar.expandedFilterView
    property bool haveTreeModel: false
    property alias showRating: navigationBar.showRating
    property alias allowArtistNavigation: navigationBar.allowArtistNavigation
    property var delegateWidth: scrollBar.visible ? contentDirectoryView.width - scrollBar.width : contentDirectoryView.width
    property alias currentIndex: contentDirectoryView.currentIndex
    property alias enableSorting: navigationBar.enableSorting
    property var stackView
    property alias showEnqueueButton: navigationBar.showEnqueueButton
    property alias showCreateRadioButton: navigationBar.showCreateRadioButton
    property alias navigationBar: navigationBar
    property int depth: 1

    signal goBackRequested()
    signal showArtist(var name)

    function goToBack() {
        if (haveTreeModel) {
            delegateModel.rootIndex = delegateModel.parentModelIndex()
            --depth
        } else {
            listView.goBackRequested()
        }
    }

    SystemPalette {
        id: myPalette
        colorGroup: SystemPalette.Active
    }

    Theme {
        id: elisaTheme
    }

    DelegateModel {
        id: delegateModel

        model: listView.contentModel
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        NavigationActionBar {
            id: navigationBar

            enableGoBack: listView.isSubPage || depth > 1
            sortOrder: contentModel.sortedAscending

            Layout.fillWidth: true

            Loader {
                active: listView.contentModel

                sourceComponent: Binding {
                    target: listView.contentModel
                    property: 'filterText'
                    when: listView.contentModel
                    value: navigationBar.filterText
                }
            }

            Loader {
                active: listView.contentModel

                sourceComponent: Binding {
                    target: listView.contentModel
                    property: 'filterRating'
                    when: listView.contentModel
                    value: navigationBar.filterRating
                }
            }

            onEnqueue: contentModel.enqueueToPlayList()

            onReplaceAndPlay: contentModel.replaceAndPlayOfPlayList()

            onGoBack: {
                listView.goToBack()
            }

            onShowArtist: listView.showArtist(listView.contentModel.sourceModel.author)

            onSort: contentModel.sortModel(order)
        }

        Rectangle {
            color: myPalette.base

            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.margins: 2

            ListView {
                id: contentDirectoryView
                anchors.fill: parent

                Accessible.role: Accessible.List
                Accessible.name: mainTitle
                Accessible.description: mainTitle

                activeFocusOnTab: true
                keyNavigationEnabled: true

                model: delegateModel

                currentIndex: -1

                section.property: (showSection ? 'discNumber' : '')
                section.criteria: ViewSection.FullString
                section.labelPositioning: ViewSection.InlineLabels
                section.delegate: TracksDiscHeader {
                    discNumber: section
                    width: scrollBar.visible ? (!LayoutMirroring.enabled ? contentDirectoryView.width - scrollBar.width : contentDirectoryView.width) : contentDirectoryView.width
                }

                ScrollBar.vertical: ScrollBar {
                    id: scrollBar
                }
                boundsBehavior: Flickable.StopAtBounds
                clip: true

                ScrollHelper {
                    id: scrollHelper
                    flickable: contentDirectoryView
                    anchors.fill: contentDirectoryView
                }

                onCountChanged: if (count === 0) {
                                    currentIndex = -1;
                                }
            }
        }
    }
}

