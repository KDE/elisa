/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtQml.Models 2.2
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.12 as Kirigami
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
    property var delegateWidth: contentDirectoryView.width
    property alias currentIndex: contentDirectoryView.currentIndex
    property alias enableSorting: navigationBar.enableSorting
    property alias sortRole: navigationBar.sortRole
    property alias sortRoles: navigationBar.sortRoles
    property alias sortRoleNames: navigationBar.sortRoleNames
    property alias sortOrderNames: navigationBar.sortOrderNames
    property alias sortOrder: navigationBar.sortOrder
    property var stackView
    property alias showEnqueueButton: navigationBar.showEnqueueButton
    property alias showCreateRadioButton: navigationBar.showCreateRadioButton
    property alias navigationBar: navigationBar
    property int depth: 1
    property alias viewManager: navigationBar.viewManager
    property bool suppressNoDataPlaceholderMessage: false

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

            z: 1 // on top of track list

            enableGoBack: listView.isSubPage || depth > 1

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

            Loader {
                active: listView.contentModel && navigationBar.enableSorting

                sourceComponent: Binding {
                    target: listView.contentModel
                    property: 'sortRole'
                    when: listView.contentModel && navigationBar.enableSorting
                    value: navigationBar.sortRole
                }
            }

            onEnqueue: contentModel.enqueueToPlayList(delegateModel.rootIndex)

            onReplaceAndPlay: contentModel.replaceAndPlayOfPlayList(delegateModel.rootIndex)

            onGoBack: {
                listView.goToBack()
            }

            onShowArtist: listView.showArtist(listView.contentModel.sourceModel.author)

            onSortOrderChanged: {
                if (!contentModel || !navigationBar.enableSorting) {
                    return
                }

                if ((contentModel.sortedAscending && sortOrder !== Qt.AscendingOrder) ||
                        (!contentModel.sortedAscending && sortOrder !== Qt.DescendingOrder)) {
                    contentModel.sortModel(sortOrder)
                }
            }
        }

        Rectangle {
            color: myPalette.base

            Layout.fillHeight: true
            Layout.fillWidth: true

            ScrollView {
                anchors.fill: parent

                ListView {
                    id: contentDirectoryView

                    Accessible.role: Accessible.List
                    Accessible.name: mainTitle
                    Accessible.description: mainTitle

                    activeFocusOnTab: true
                    keyNavigationEnabled: true

                    reuseItems: true

                    model: delegateModel

                    // HACK: setting currentIndex to -1 in mobile for some reason causes segfaults, no idea why
                    currentIndex: Kirigami.Settings.isMobile ? 0 : -1

                    section.property: (showSection ? 'discNumber' : '')
                    section.criteria: ViewSection.FullString
                    section.labelPositioning: ViewSection.InlineLabels
                    section.delegate: TracksDiscHeader {
                        discNumber: section
                        width: contentDirectoryView.width
                    }

                    Kirigami.PlaceholderMessage {
                        anchors.centerIn: parent
                        width: parent.width - (Kirigami.Units.largeSpacing * 4)
                        visible: contentDirectoryView.count === 0 && !suppressNoDataPlaceholderMessage
                        text: i18n("Nothing to display")
                    }

                    onCountChanged: if (count === 0) {
                        currentIndex = -1;
                    }
                }
            }
        }
    }
}
