/*
   SPDX-FileCopyrightText: 2021 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigami 2.18 as Kirigami
import org.kde.elisa 1.0

Kirigami.AbstractCard {
    id: card

    property alias fallbackIcon: albumCover.fallback
    property ViewManager manager
    property var fullData

    contentItem: Item {
        // implicitWidth/Height define the natural width/height of an item if no width or height is specified.
        // The setting below defines a component's preferred size based on its content
        implicitWidth: delegateLayout.implicitWidth
        implicitHeight: delegateLayout.implicitHeight

        GridLayout {
            id: delegateLayout

            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
            }

            rowSpacing: Kirigami.Units.largeSpacing
            columnSpacing: Kirigami.Units.largeSpacing
            columns: 4

            Kirigami.Icon {
                id: albumCover

                implicitWidth: 64
                implicitHeight: 64
                source: model.imageUrl ? model.imageUrl : fallbackIcon
            }

            ColumnLayout {
                Layout.fillWidth: true

                Kirigami.Heading {
                    Layout.fillHeight: true
                    level: 1
                    text: model.display
                }

                Kirigami.Separator {
                    Layout.fillWidth: true
                }

                Kirigami.Heading {
                    level: 2
                    text: model.artist
                }
            }

            Controls.Button {
                id: detailButton

                Layout.alignment: Qt.AlignRight
                Layout.columnSpan: 2
                text: i18n("Expand")
                checkable: true
            }

            AlbumView {
                id: detailsView

                Layout.row: 1
                Layout.column: 0
                Layout.columnSpan: 4
                Layout.fillWidth: true
                Layout.minimumHeight: 28
                Layout.preferredHeight: implicitHeight
                visible: detailButton.checked

                onVisibleChanged: {
                    if (visible) {
                        card.manager.openChildView(card.fullData)
                    } else {
                        detailsView.realModel = dummyModel
                        detailsView.model = dummyModel
                    }
                }

                Connections {
                    target: card.manager

                    function onOpenListView(configurationData) {
                        detailsView.model = configurationData.associatedProxyModel
                        detailsView.realModel = configurationData.model

                        detailsView.model.sourceModel = detailsView.realModel
                        detailsView.model.sourceModel.dataType = configurationData.dataType
                        detailsView.model.sourceModel.playList = Qt.binding(function() { return ElisaApplication.mediaPlayListProxyModel })

                        detailsView.model.sortModel(configurationData.sortOrder)

                        detailsView.realModel.initializeByData(ElisaApplication.musicManager,
                                                                 ElisaApplication.musicManager.viewDatabase,
                                                                 configurationData.dataType,
                                                                 configurationData.filterType,
                                                                 configurationData.dataFilter)
                    }
                }
            }
        }
    }

    ListModel {
        id: dummyModel
    }

    ListView.onPooled: {
        detailButton.checked = false
    }

    ListView.onReused: {
        detailButton.checked = false
    }
}
