/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.10
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0

RowLayout {
    id: delegateRow

    property string name
    property int index
    property var type
    property string display
    property bool isRemovable

    spacing: 0

    signal edited()
    signal deleteField()

    Label {
        id: metaDataLabels

        text: {
            if (name !== undefined) {
                return i18nc("Label for a piece of metadata, e.g. 'Album Artist:'", "%1:", name)
            }
            return ""
        }

        font.weight: Font.Bold

        horizontalAlignment: Text.AlignRight

        Layout.alignment: Qt.AlignVCenter
        Layout.preferredWidth: 0.8 * elisaTheme.coverImageSize
        Layout.rightMargin: !LayoutMirroring.enabled ? Kirigami.Units.smallSpacing : 0
        Layout.leftMargin: LayoutMirroring.enabled ? Kirigami.Units.smallSpacing : 0
    }

    Loader {
        id: textDisplayLoader

        focus: index === 0

        active: type === EditableTrackMetadataModel.TextEntry || type === EditableTrackMetadataModel.UrlEntry || type === EditableTrackMetadataModel.IntegerEntry
        visible: type === EditableTrackMetadataModel.TextEntry || type === EditableTrackMetadataModel.UrlEntry || type === EditableTrackMetadataModel.IntegerEntry

        Layout.fillWidth: true
        Layout.alignment: Qt.AlignTop

        sourceComponent: TextField {
            text: display

            focus: index === 0

            horizontalAlignment: Text.AlignLeft

            selectByMouse: true

            anchors.fill: parent

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

        active: type === EditableTrackMetadataModel.RatingEntry
        visible: type === EditableTrackMetadataModel.RatingEntry

        Layout.fillWidth: true
        Layout.alignment: Qt.AlignVCenter

        sourceComponent: RatingStar {
            starRating: display

            readOnly: false

            hoverWidgetOpacity: 1

            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }

            onRatingEdited: {
                if (display !== starRating) {
                    display = starRating

                    edited()
                }
            }
        }
    }

    Loader {
        id: longTextDisplayLoader

        active: type === EditableTrackMetadataModel.LongTextEntry
        visible: type === EditableTrackMetadataModel.LongTextEntry

        Layout.fillWidth: true
        Layout.maximumWidth: delegateRow.width - (0.8 * elisaTheme.coverImageSize + Kirigami.Units.largeSpacing * 2)
        Layout.alignment: Qt.AlignTop

        sourceComponent: TextArea {
            text: display

            focus: index === 0

            horizontalAlignment: Text.AlignLeft

            selectByMouse: true

            anchors.fill: parent

            wrapMode: Text.WordWrap

            onEditingFinished: {
                if (display !== text) {
                    display = text

                    edited()
                }
            }
        }
    }

    Button {
        Layout.preferredHeight: Kirigami.Units.iconSizes.smallMedium
        Layout.preferredWidth: Kirigami.Units.iconSizes.smallMedium

        Layout.alignment: Layout.horizontalCenter

        flat: true
        display: AbstractButton.IconOnly
        icon.name: 'list-remove'

        visible: isRemovable

        onClicked: deleteField()
    }
}
