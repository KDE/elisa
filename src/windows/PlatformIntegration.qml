import QtQuick 2.0
import org.mgallien.QmlExtension 1.0
import QtWinExtras 1.0

Item {
    id: rootItem

    property var playListModel
    property var playListControler
    property var audioPlayerManager
    property var headerBarManager
    property var manageMediaPlayerControl

    signal raisePlayer()

    Theme {
        id: elisaTheme
    }

    ThumbnailToolBar {
        iconicThumbnailSource: (headerBarManager.image ? headerBarManager.image : Qt.resolvedUrl(elisaTheme.albumCover))

        ThumbnailToolButton {
            iconSource: Qt.resolvedUrl(elisaTheme.skipBackwardIcon)
            onClicked: playListControler.skipPreviousTrack()
            enabled: manageMediaPlayerControl.skipBackwardControlEnabled
        }

        ThumbnailToolButton {
            iconSource: (myPlayControlManager.musicPlaying ? Qt.resolvedUrl(elisaTheme.pauseIcon) : Qt.resolvedUrl(elisaTheme.playIcon))
            onClicked: manageAudioPlayer.playPause()
            enabled: manageMediaPlayerControl.playControlEnabled
        }

        ThumbnailToolButton {
            iconSource: Qt.resolvedUrl(elisaTheme.skipForwardIcon)
            onClicked: playListControler.skipNextTrack()
            enabled: manageMediaPlayerControl.skipForwardControlEnabled
        }
    }
}
