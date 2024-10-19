/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2020 (c) Devin Lin <espidev@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.10
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.5 as Kirigami
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
            textFormat: Text.PlainText
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

        active: !delegateRow.readOnly && delegateRow.hasData &&
                (delegateRow.type === EditableTrackMetadataModel.TextEntry ||
                 delegateRow.type === EditableTrackMetadataModel.UrlEntry ||
                 delegateRow.type === EditableTrackMetadataModel.IntegerEntry)
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

        sourceComponent: ElisaApplication.useFavoriteStyleRatings ? favoriteButton : ratingStars

        Component {
            id: ratingStars

            RatingStar {
                starRating: delegateRow.display

                readOnly: delegateRow.readOnly

                anchors.verticalCenter: parent.verticalCenter

                onRatingEdited: {
                    if (display !== starRating) {
                        display = starRating
                        ElisaApplication.musicManager.updateSingleFileMetaData(url, DataTypes.RatingRole, starRating)
                        edited()
                    }
                }
            }
        }

        Component {
            id: favoriteButton

            FlatButtonWithToolTip {
                readonly property bool isFavorite: delegateRow.display === 10

                text: isFavorite ? i18nc("@action:button", "Un-mark this song as a favorite") : i18nc("@action:button", "Mark this song as a favorite")
                icon.name: isFavorite ? "rating" : "rating-unrated"

                onClicked: {
                    const newRating = isFavorite ? 0 : 10;
                    // Change icon immediately in case backend is slow
                    icon.name = isFavorite ? "rating-unrated" : "rating";
                    ElisaApplication.musicManager.updateSingleFileMetaData(url, DataTypes.RatingRole, newRating)
                    edited()
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
                text: display

                focus: index === 0

                horizontalAlignment: TextEdit.AlignLeft

                selectByMouse: true

                wrapMode: TextEdit.Wrap

                onEditingFinished: {
                    if (display !== text) {
                        display = text

                        edited()
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
        onClicked: deleteField()
    }

    Button {
        icon.name: 'list-add'
        text: i18nc("@action:button", "Add tag")
        visible: !delegateRow.readOnly && !delegateRow.hasData

        onClicked: addField()
    }
}

