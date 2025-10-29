/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.elisa

RowLayout {
    id: delegateRow
    property real maximumWidth: Number.POSITIVE_INFINITY

    property string name
    property int index
    property var type
    property var display
    property bool isRemovable
    property bool readOnly
    property string url
    required property bool hasData

    spacing: 0

    signal edited()
    signal addField()
    signal deleteField()

    Loader {
        id: textDisplayLoader
        active: {
            if (!delegateRow.readOnly || typeof delegateRow.display === "undefined") {
                return false;
            }

            switch (delegateRow.type) {
            case EditableTrackMetadataModel.TextEntry:
            case EditableTrackMetadataModel.IntegerEntry:
            case EditableTrackMetadataModel.UrlEntry:
            case EditableTrackMetadataModel.DurationEntry:
                return true;
            default:
                return false;
            }
        }
        visible: active
        Layout.maximumWidth: Math.min(Kirigami.Units.gridUnit * 20, delegateRow.maximumWidth)

        sourceComponent: LabelWithToolTip {
            text: delegateRow.display
            horizontalAlignment: Text.AlignLeft
            elide: Text.ElideRight
            wrapMode: Text.WordWrap
        }
    }

    Loader {
        id: longTextDisplayLoader
        active: delegateRow.readOnly && (delegateRow.type === EditableTrackMetadataModel.LongTextEntry) && typeof delegateRow.display !== "undefined"
        visible: active
        Layout.maximumWidth: Math.min(Kirigami.Units.gridUnit * 20, delegateRow.maximumWidth)

        sourceComponent: Label {
            text: delegateRow.display
            textFormat: Text.PlainText
            horizontalAlignment: Text.AlignLeft
            elide: Text.ElideRight
            wrapMode: Text.WordWrap
        }
    }

    Loader {
        active: delegateRow.readOnly && (delegateRow.type === EditableTrackMetadataModel.DateEntry) && typeof delegateRow.display !== "undefined"
        visible: active
        Layout.maximumWidth: Math.min(Kirigami.Units.gridUnit * 20, delegateRow.maximumWidth)

        sourceComponent: LabelWithToolTip {
            text: rawDate.toLocaleDateString(Locale.ShortFormat)

            horizontalAlignment: Text.AlignLeft
            elide: Text.ElideRight
            property date rawDate: new Date(delegateRow.display)
        }
    }

    Loader {
        id: editTextDisplayLoader

        focus: delegateRow.index === 0

        active: !delegateRow.readOnly && delegateRow.hasData &&
                (delegateRow.type === EditableTrackMetadataModel.TextEntry ||
                 delegateRow.type === EditableTrackMetadataModel.UrlEntry ||
                 delegateRow.type === EditableTrackMetadataModel.IntegerEntry)
        visible: active

        sourceComponent: TextField {
            enabled: !delegateRow.readOnly
            text: delegateRow.display

            focus: delegateRow.index === 0

            horizontalAlignment: Text.AlignLeft

            onTextEdited: {
                if (delegateRow.display !== text) {
                    delegateRow.display = text

                    delegateRow.edited()
                }
            }
        }
    }

    Loader {
        focus: delegateRow.index === 0

        active: delegateRow.type === EditableTrackMetadataModel.RatingEntry && typeof delegateRow.display !== "undefined"
        visible: active

        sourceComponent: ElisaApplication.useFavoriteStyleRatings ? favoriteButton : ratingStars

        Component {
            id: ratingStars

            RatingStar {
                starRating: delegateRow.display

                readOnly: delegateRow.readOnly

                anchors.verticalCenter: parent.verticalCenter

                onRatingEdited: {
                    if (delegateRow.display !== starRating) {
                        delegateRow.display = starRating
                        ElisaApplication.musicManager.updateSingleFileMetaData(delegateRow.url, DataTypes.RatingRole, starRating)
                        delegateRow.edited()
                    }
                }
            }
        }

        Component {
            id: favoriteButton

            FlatButtonWithToolTip {
                readonly property bool isFavorite: delegateRow.display === 10

                text: isFavorite ? i18nc("@action:button", "Un-mark this song as a favorite") : i18nc("@action:button", "Mark this song as a favorite")
                icon.name: isFavorite ? "starred" : "non-starred"

                onClicked: {
                    const newRating = isFavorite ? 0 : 10;
                    // Change icon immediately in case backend is slow
                    icon.name = isFavorite ? "non-starred" : "starred";
                    ElisaApplication.musicManager.updateSingleFileMetaData(delegateRow.url, DataTypes.RatingRole, newRating)
                    delegateRow.edited()
                }
            }
        }
    }

    Loader {
        id: editLongTextDisplayLoader

        active: !delegateRow.readOnly && delegateRow.hasData && (delegateRow.type === EditableTrackMetadataModel.LongTextEntry)
        visible: active
        Layout.maximumHeight: Kirigami.Units.gridUnit * 10
        Layout.minimumWidth: Kirigami.Units.gridUnit * 8

        sourceComponent: ScrollView {
            TextArea {
                enabled: !delegateRow.readOnly
                text: delegateRow.display

                focus: delegateRow.index === 0

                horizontalAlignment: TextEdit.AlignLeft

                selectByMouse: true

                wrapMode: TextEdit.Wrap

                onEditingFinished: {
                    if (delegateRow.display !== text) {
                        delegateRow.display = text

                        delegateRow.edited()
                    }
                }
            }
        }
    }

    FlatButtonWithToolTip {
        Layout.preferredHeight: Kirigami.Units.iconSizes.smallMedium
        Layout.preferredWidth: Kirigami.Units.iconSizes.smallMedium

        icon.name: 'delete'
        text: i18nc("@action:button remove a metadata tag", "Remove this tag")

        visible: !delegateRow.readOnly && delegateRow.hasData && delegateRow.isRemovable
        onClicked: delegateRow.deleteField()
    }

    Button {
        icon.name: 'list-add'
        text: i18nc("@action:button", "Add tag")
        visible: !delegateRow.readOnly && !delegateRow.hasData

        onClicked: delegateRow.addField()
    }
}

