import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import org.mgallien.QmlExtension 1.0


Item {
    property double volume
    property int position
    property int duration
    property bool muted
    property bool isPlaying
    property bool seekable
    property bool playEnabled
    property bool skipForwardEnabled
    property bool skipBackwardEnabled

    signal play()
    signal pause()
    signal playPrevious()
    signal playNext()
    signal seek(int position)

    id: musicWidget

    Rectangle {
        color: '#C5CBD1'
        width: parent.width
        height: parent.height

        RowLayout {
            anchors.fill: parent
            spacing: 0

            Image {
                id: previousButton
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        musicWidget.playPrevious()
                    }
                }
                source: 'image://icon/media-skip-backward'
                Layout.preferredWidth: width
                Layout.preferredHeight: height
                Layout.alignment: Qt.AlignVCenter
                Layout.maximumWidth: width
                Layout.maximumHeight: height
                Layout.minimumWidth: width
                Layout.minimumHeight: height
                sourceSize.width: width
                sourceSize.height: width
                height: width
                width: Screen.pixelDensity * 8.
                fillMode: Image.PreserveAspectFit
                enabled: skipBackwardEnabled
                opacity: enabled ? 1.0 : 0.6
            }

            Image {
                id: playPauseButton
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (musicWidget.isPlaying) {
                            musicWidget.pause()
                        } else {
                            musicWidget.play()
                        }
                    }
                }
                source: {
                    if (musicWidget.isPlaying)
                        'image://icon/media-playback-pause'
                    else
                        'image://icon/media-playback-start'
                }
                Layout.preferredWidth: width
                Layout.preferredHeight: height
                Layout.alignment: Qt.AlignVCenter
                Layout.maximumWidth: width
                Layout.maximumHeight: height
                Layout.minimumWidth: width
                Layout.minimumHeight: height
                sourceSize.width: width
                sourceSize.height: width
                height: width
                width: Screen.pixelDensity * 11.
                fillMode: Image.PreserveAspectFit
                enabled: playEnabled
                opacity: enabled ? 1.0 : 0.6
            }

            Image {
                id: nextButton
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        musicWidget.playNext()
                    }
                }
                source: 'image://icon/media-skip-forward'
                Layout.preferredWidth: width
                Layout.preferredHeight: height
                Layout.alignment: Qt.AlignVCenter
                Layout.maximumWidth: width
                Layout.maximumHeight: height
                Layout.minimumWidth: width
                Layout.minimumHeight: height
                sourceSize.width: width
                sourceSize.height: width
                height: width
                width: Screen.pixelDensity * 8.
                fillMode: Image.PreserveAspectFit
                enabled: skipForwardEnabled
                opacity: enabled ? 1.0 : 0.6
            }

            Item {
                Layout.preferredWidth: width
                Layout.maximumWidth: width
                Layout.minimumWidth: width
                width: Screen.pixelDensity * 8.
            }

            Label {
                id: positionLabel
                text: if (musicWidget.position / 1000 % 60 < 10)
                          '0:0' + (musicWidget.position / 1000 % 60).toFixed(0)
                      else
                          (musicWidget.position / 1000 / 60).toFixed(0) + ':' + (musicWidget.position / 1000 % 60).toFixed(0)
                Layout.alignment: Qt.AlignVCenter
            }

            Item {
                Layout.preferredWidth: width
                Layout.maximumWidth: width
                Layout.minimumWidth: width
                width: Screen.pixelDensity * 3.
            }

            Slider {
                property bool seekStarted: false
                property int seekValue

                id: musicProgress
                minimumValue: 0
                maximumValue: musicWidget.duration
                Layout.alignment: Qt.AlignVCenter
                Layout.fillWidth: true
                enabled: musicWidget.seekable && musicWidget.playEnabled
                updateValueWhileDragging: true

                onValueChanged: {
                    if (seekStarted) {
                        seekValue = value
                    }
                }

                onPressedChanged: {
                    if (pressed) {
                        seekStarted = true;
                        seekValue = value
                    } else {
                        musicWidget.seek(seekValue)
                        seekStarted = false;
                    }
                }
            }

            Item {
                Layout.preferredWidth: width
                Layout.maximumWidth: width
                Layout.minimumWidth: width
                width: Screen.pixelDensity * 8.
            }

            Image {
                id: volumeIcon
                MouseArea {
                    anchors.fill: parent
                    onClicked: musicWidget.muted = !musicWidget.muted
                }
                source: if (musicWidget.muted)
                            'image://icon/player-volume-muted'
                        else
                            'image://icon/player-volume'
                Layout.preferredWidth: width
                Layout.preferredHeight: height
                Layout.alignment: Qt.AlignVCenter
                Layout.maximumWidth: width
                Layout.maximumHeight: height
                Layout.minimumWidth: width
                Layout.minimumHeight: height
                sourceSize.width: width
                sourceSize.height: width
                height: width
                width: Screen.pixelDensity * 8.
                fillMode: Image.PreserveAspectFit
            }

            Item {
                Layout.preferredWidth: width
                Layout.maximumWidth: width
                Layout.minimumWidth: width
                width: Screen.pixelDensity * 3.
            }

            Slider {
                id: volumeSlider
                minimumValue: 0.0
                maximumValue: 1.0
                value: if (musicWidget.muted)
                           0
                       else
                           musicWidget.volume
                onValueChanged: {
                    if (!musicWidget.muted) {
                        musicWidget.volume = value
                    }
                }

                Layout.alignment: Qt.AlignVCenter
                Layout.preferredWidth: width
                Layout.maximumWidth: width
                Layout.minimumWidth: width
                width: Screen.pixelDensity * 35.
            }

            Item {
                Layout.preferredWidth: width
                Layout.maximumWidth: width
                Layout.minimumWidth: width
                width: Screen.pixelDensity * 8.
            }
        }
    }

    onPositionChanged: if (!musicProgress.seekStarted) {
                           musicProgress.value = position
                       }
}

