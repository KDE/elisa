/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.10
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0

RowLayout {
    id: delegateRow
    property real maximumWidth: Number.POSITIVE_INFINITY

    property string name
    property int index
    property var type
    property var display
    property bool isRemovable
    property bool readOnly

    spacing: 0

    signal edited()
    signal deleteField()

    Loader {
        id: textDisplayLoader
        active: readOnly && (type === EditableTrackMetadataModel.TextEntry || type === EditableTrackMetadataModel.IntegerEntry || type === EditableTrackMetadataModel.UrlEntry || type === EditableTrackMetadataModel.DurationEntry) && typeof display !== "undefined"
        visible: active
        Layout.maximumWidth: Math.min(Kirigami.Units.gridUnit * 20, delegateRow.maximumWidth)

        sourceComponent: LabelWithToolTip {
            text: display
            horizontalAlignment: Text.AlignLeft
            elide: Text.ElideRight
            wrapMode: Text.WordWrap
        }
    }

    Loader {
        id: longTextDisplayLoader
        active: readOnly && (type === EditableTrackMetadataModel.LongTextEntry) && typeof display !== "undefined"
        visible: active
        Layout.maximumWidth: Math.min(Kirigami.Units.gridUnit * 20, delegateRow.maximumWidth)

        sourceComponent: Label {
            text: display
            horizontalAlignment: Text.AlignLeft
            elide: Text.ElideRight
            wrapMode: Text.WordWrap
        }
    }

    Loader {
        active: readOnly && (type === EditableTrackMetadataModel.DateEntry) && typeof display !== "undefined"
        visible: active
        Layout.maximumWidth: Math.min(Kirigami.Units.gridUnit * 20, delegateRow.maximumWidth)

        sourceComponent: LabelWithToolTip {
            text: rawDate.toLocaleDateString(Locale.ShortFormat)

            horizontalAlignment: Text.AlignLeft
            elide: Text.ElideRight
            property date rawDate: new Date(display)
        }
    }

    Loader {
        id: editTextDisplayLoader

        focus: index === 0

        active: !readOnly && (type === EditableTrackMetadataModel.TextEntry || type === EditableTrackMetadataModel.UrlEntry || type === EditableTrackMetadataModel.IntegerEntry)
        visible: active

        sourceComponent: TextField {
            enabled: !delegateRow.readOnly
            text: display

            focus: index === 0

            horizontalAlignment: Text.AlignLeft

            onTextEdited: {
                if (display !== text) {
                    display = text

                    edited()
                }
            }
        }
    }

    Loader {
        focus: index === 0

        active: type === EditableTrackMetadataModel.RatingEntry && typeof display !== "undefined"
        visible: active

        sourceComponent: RatingStar {
            starRating: display

            readOnly: delegateRow.readOnly

            hoverWidgetOpacity: 1

            anchors.verticalCenter: parent.verticalCenter
            height: Kirigami.Units.gridUnit

            onRatingEdited: {
                if (display !== starRating) {
                    display = starRating

                    edited()
                }
            }
        }
    }

    Loader {
        id: editLongTextDisplayLoader

        active: !readOnly && (type === EditableTrackMetadataModel.LongTextEntry)
        visible: active
        Layout.maximumHeight: Kirigami.Units.gridUnit * 10
        Layout.minimumWidth: Kirigami.Units.gridUnit * 8

        sourceComponent: ScrollView {
            TextArea {
                enabled: !delegateRow.readOnly
                text: display

                focus: index === 0

                horizontalAlignment: Text.AlignLeft

                selectByMouse: true

                wrapMode: Text.WordWrap

                onEditingFinished: {
                    if (display !== text) {
                        display = text

                        edited()
                    }
                }
            }
        }
    }

    Button {
        Layout.preferredHeight: Kirigami.Units.iconSizes.smallMedium
        Layout.preferredWidth: Kirigami.Units.iconSizes.smallMedium

        flat: true
        display: AbstractButton.IconOnly
        icon.name: 'delete'

        visible: !readOnly && isRemovable
        onClicked: deleteField()
    }
}

