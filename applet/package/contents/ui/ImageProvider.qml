import QtQuick 2.0

import org.kde.plasma.plasmoid 2.0

import org.task_struct.private.photooftheday 1.0

QtObject {

    //exported API
    property var pixmap: PixmapLoader.load(plasmoid.configuration.imageFileName)
    property string toolTipTitle: i18n("Image")
    property string toolTipContent: plasmoid.configuration.imageFileName
    property string errorText: ""

    onPixmapChanged: console.debug("IMAGE PROVIDER PIXMAP")
}
