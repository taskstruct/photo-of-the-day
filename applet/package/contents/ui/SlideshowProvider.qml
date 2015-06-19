import QtQuick 2.2

import org.kde.plasma.plasmoid 2.0

import org.task_struct.private.photooftheday 1.0

QtObject {
    id: slideshow

    //exported API
    property var pixmap
    property string toolTipTitle: ""
    property string toolTipContent: ""
    property string errorText: ""

    function next() {
        slideshow.pixmap = __model.requestNext()
        __slideshowTimer.restart()
    }

    property SlideShowModel __model: SlideShowModel {
        id: slideshowModel

        paths: plasmoid.configuration.slideshowPaths
        random: plasmoid.configuration.randomize
        recursive: plasmoid.configuration.recursiveScan

        Component.onCompleted: slideshow.pixmap = requestNext()
    }

    property Timer __slideshowTimer: Timer {
        interval: plasmoid.configuration.slideshowInterval
        running: true

        onTriggered: slideshow.pixmap = slideshowModel.requestNext()
    }
}
