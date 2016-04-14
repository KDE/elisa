import QtQuick 2.4
import org.kde.kirigami 1.0 as MobileComponents

MobileComponents.AbstractListItem {
    id: viewAlbumDelegate
    enabled: true
    supportsMouseEvents: true

    property alias title: audioTrackDelegate.title
    property alias artist: audioTrackDelegate.artist
    property alias album: audioTrackDelegate.album
    property alias itemDecoration: audioTrackDelegate.itemDecoration
    property alias duration: audioTrackDelegate.duration

    AbstractAudioTrackDelegateMobile {
        id: audioTrackDelegate
    }
}
