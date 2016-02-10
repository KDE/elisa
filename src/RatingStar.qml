import QtQuick 2.0
import QtQuick.Layouts 1.1

RowLayout {
    property int starSize
    property int starRating

    spacing: 0

    Item {
        Layout.preferredHeight: starSize
        Layout.preferredWidth: starSize
        Layout.maximumHeight: starSize
        Layout.maximumWidth: starSize
        Layout.minimumHeight: starSize
        Layout.minimumWidth: starSize

        Image {
            width: starSize * 1.5
            height: starSize * 1.5
            anchors.centerIn: parent
            sourceSize.width: width
            sourceSize.height: width
            fillMode: Image.PreserveAspectFit

            source: if (starRating == 1)
                        'image://icon/rating'
                    else
                        'image://icon/draw-star'
            opacity: if (starRating == 1)
                        1
                    else
                        0.1
        }

        MouseArea {
            anchors.fill: parent
            onClicked: rating = 1
        }
    }
    Item {
        Layout.preferredHeight: starSize
        Layout.preferredWidth: starSize
        Layout.maximumHeight: starSize
        Layout.maximumWidth: starSize
        Layout.minimumHeight: starSize
        Layout.minimumWidth: starSize

        Image {
            width: starSize * 1.5
            height: starSize * 1.5
            anchors.centerIn: parent
            sourceSize.width: width
            sourceSize.height: width
            fillMode: Image.PreserveAspectFit

            source: if (starRating == 2)
                        'image://icon/rating'
                    else
                        'image://icon/draw-star'
            opacity: if (starRating == 2)
                        1
                    else
                        0.1
        }

        MouseArea {
            anchors.fill: parent
            onClicked: rating = 1
        }
    }
    Item {
        Layout.preferredHeight: starSize
        Layout.preferredWidth: starSize
        Layout.maximumHeight: starSize
        Layout.maximumWidth: starSize
        Layout.minimumHeight: starSize
        Layout.minimumWidth: starSize

        Image {
            width: starSize * 1.5
            height: starSize * 1.5
            anchors.centerIn: parent
            sourceSize.width: width
            sourceSize.height: width
            fillMode: Image.PreserveAspectFit

            source: if (starRating == 3)
                        'image://icon/rating'
                    else
                        'image://icon/draw-star'
            opacity: if (starRating == 3)
                        1
                    else
                        0.1
        }

        MouseArea {
            anchors.fill: parent
            onClicked: rating = 1
        }
    }
    Item {
        Layout.preferredHeight: starSize
        Layout.preferredWidth: starSize
        Layout.maximumHeight: starSize
        Layout.maximumWidth: starSize
        Layout.minimumHeight: starSize
        Layout.minimumWidth: starSize

        Image {
            width: starSize * 1.5
            height: starSize * 1.5
            anchors.centerIn: parent
            sourceSize.width: width
            sourceSize.height: width
            fillMode: Image.PreserveAspectFit

            source: if (starRating == 4)
                        'image://icon/rating'
                    else
                        'image://icon/draw-star'
            opacity: if (starRating == 4)
                        1
                    else
                        0.1
        }

        MouseArea {
            anchors.fill: parent
            onClicked: rating = 1
        }
    }
    Item {
        Layout.preferredHeight: starSize
        Layout.preferredWidth: starSize
        Layout.maximumHeight: starSize
        Layout.maximumWidth: starSize
        Layout.minimumHeight: starSize
        Layout.minimumWidth: starSize

        Image {
            width: starSize * 1.5
            height: starSize * 1.5
            anchors.centerIn: parent
            sourceSize.width: width
            sourceSize.height: width
            fillMode: Image.PreserveAspectFit

            source: if (starRating == 5)
                        'image://icon/rating'
                    else
                        'image://icon/draw-star'
            opacity: if (starRating == 5)
                        1
                    else
                        0.1
        }

        MouseArea {
            anchors.fill: parent
            onClicked: rating = 1
        }
    }
}

