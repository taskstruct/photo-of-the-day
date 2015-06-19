/*
 * Copyright 2015  Nikolay S Nikolov <nikolay.stef.nikolov@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.4
import QtQuick.Layouts 1.1
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.kquickcontrolsaddons 2.0

import org.task_struct.private.photooftheday 1.0

Item {
    id: rootItem
    
    // 16:10. This is used by Plasma for initial size, when plasmoid is shown for the first time
    width: 400
    height: 250

    Plasmoid.backgroundHints: Plasmoid.NoBackground
    
    // Configuration
    property bool showShadowCfg: Plasmoid.configuration.showShadow
    property int shadowWidthCfg: Plasmoid.configuration.shadowWidth
    property real shadowSpreadCfg: Plasmoid.configuration.shadowSpread
    property color shadowColorCfg: Plasmoid.configuration.shadowColor
    
    property bool showBorderCfg: Plasmoid.configuration.showBorder
    property int borderWidthCfg: Plasmoid.configuration.borderWidth
    property real borderOpacityCfg: Plasmoid.configuration.borderOpacity
    property color borderColorCfg: Plasmoid.configuration.borderColor
    
    property int roundedCornersCfg: Plasmoid.configuration.roundedCorners
    property bool animatedTransitionsCfg: Plasmoid.configuration.animatedTransitions

    property string cfg_contentType: plasmoid.configuration.contentType
    
    // logic
    property string selectedProviderCfg: plasmoid.configuration.selectedProvider

    property string __connectedProvider: ""

    property int updateIntervalCgf: Plasmoid.configuration.updateInterval
    
    // constants
    readonly property int __animationsDuration: 2000

    readonly property var __contentProvidersMap: { "image" : "ImageProvider.qml", "slideshow" : "SlideshowProvider.qml", "potd" : "PotdProvider.qml"  }

    signal newSizeUpdated( size newSize)

    //HACK: Let plasmoid to restore it's width and height. They are updated too late.
    Timer {
        id: initTimer
        interval: 100
        running: true

        onTriggered: contentProviderLoader.active = true
    }

    Loader {
        id: contentProviderLoader

        active: false
        source: __contentProvidersMap[cfg_contentType]

        onLoaded: {
            if( animatedTransitionsCfg ) {
                photoBuffer.pixmap = photo.pixmap
            }
            photo.pixmap = item.pixmap
            toolTipArea.mainText = item.toolTipTitle
            toolTipArea.subText = item.toolTipContent
            errorLabel.errorText = item.errorText
        }
    }

    Connections {
        target: contentProviderLoader.item

        onPixmapChanged: {

            console.debug("IMAGE RECEIVED")

            if( animatedTransitionsCfg ) {
                photoBuffer.pixmap = photo.pixmap
            }

            photo.pixmap = contentProviderLoader.item.pixmap
        }

        onToolTipTitleChanged: toolTipArea.mainText = contentProviderLoader.item.toolTipTitle
        onToolTipContentChanged: toolTipArea.subText = contentProviderLoader.item.toolTipContent
        onErrorTextChanged: errorLabel.errorText = contentProviderLoader.item.errorText
    }

    QPixmapItem {
        id: photo

        width: nativeWidth
        height: nativeHeight

        smooth: true
        visible: false
        layer.enabled: true;
        layer.smooth: true

        onPixmapChanged: {
            console.debug( "Photo.nativeSize = ", nativeWidth, "x", nativeHeight )

            if( !photo.null ) {
                var newSize = rootItem.rescale()
                console.debug("NEW SIZE: ", newSize )
                rootItem.newSizeUpdated( newSize )
            }
        }
    }

    QPixmapItem {
        id: photoBuffer
        width: nativeWidth
        height: nativeHeight
        visible: false
        layer.enabled: true;
        layer.smooth: true
    }

    Rectangle {
        id: blank

        color: palette.window
        width: 1; height: 1
        visible: false
        layer.enabled: true;
        layer.smooth: true

        SystemPalette {
            id: palette;
            colorGroup: SystemPalette.Active
        }
    }

    function rescale() {

        if( photo.null ) {
            return Qt.size( plasmoid.width, plasmoid.height )
        }

        var rw = plasmoid.height * photo.nativeWidth / photo.nativeHeight

        var size;

        if(rw <= plasmoid.width) {
            size = Qt.size( rw, plasmoid.height )
        } else {
            size = Qt.size( plasmoid.width, plasmoid.width * photo.nativeHeight / photo.nativeWidth )
        }

        return size
    }

    function updateDrawingArea() {
        var newSize = rescale()
        drawingArea.width = newSize.width
        drawingArea.height = newSize.height
    }

    function setUpActions() {
        plasmoid.setAction("nextprovider", i18n("Next provider"), "go-next")
        plasmoid.setAction("prevprovider", i18n("Previous provider"), "go-previous")
    }

    function clearActions() {
        plasmoid.removeAction("nextprovider")
        plasmoid.removeAction("prevprovider")
    }

    function action_nextprovider() {
        plasmoid.busy = true

        if( contentProviderLoader.item.next ) {
            contentProviderLoader.item.next()
        }
    }

    function action_prevprovider() {
        plasmoid.busy = true

        if( contentProviderLoader.item.previous ) {
            contentProviderLoader.item.previous()
        }
    }
    
    onSelectedProviderCfgChanged: {
        if( selectedProviderCfg.length != 0 ) {
            setUpActions()

        } else {
            clearActions()
        }
    }

    onNewSizeUpdated: {
        console.debug("onNewSizeUpdated")
        if( animatedTransitionsCfg ) {
            plasmoidWidthAnim.to = newSize.width
            plasmoidHeightAnim.to = newSize.height

            transitionAnimation.start()

        } else {
            drawingArea.width = newSize.width
            drawingArea.height = newSize.height
        }
    }

    Plasmoid.onWidthChanged: updateDrawingArea()
    Plasmoid.onHeightChanged: updateDrawingArea()

    // This item always have the same aspect ratio as current photo image and tries to be as big as possible inside of applet
    Item {
        id: drawingArea

        BorderImage {
            id: shadow

            // color/size/strength
            source: "image://shadow/" + shadowColorCfg+ "/" + shadowWidthCfg + "/" + roundedCornersCfg + "/" + shadowSpreadCfg

            visible: showShadowCfg

            property int borderWidth: Math.max( shadowWidthCfg + roundedCornersCfg )

            border {
                left: borderWidth
                right: borderWidth
                top: borderWidth
                bottom: borderWidth
            }

            anchors.fill: parent
        }

        Rectangle {
            id: border

            color: "transparent"

            opacity: showBorderCfg ? borderOpacityCfg : 1.0

            radius: roundedCornersCfg

            border.width: showBorderCfg ? borderWidthCfg : 1
            border.color: showBorderCfg ? borderColorCfg : "black"

            anchors.fill: showShadowCfg ? shadow : parent
            anchors.margins: showShadowCfg ? shadowWidthCfg : 0
        }

        Rectangle {
            id: photoItem

            color: "white"
            radius: border.radius - border.border.width //TODO if border is disabled than get from shadow and than global

            anchors.fill: border
            anchors.margins: border.border.width

            property real strenght: 1.0

            layer.enabled: true
            layer.smooth: true
            layer.effect: ShaderEffect {
                id: shaderEffect

                cullMode: ShaderEffect.BackFaceCulling

                property var photoSource: photo.null ? blank : photo
                property var prevSource: photoBuffer.null ? blank : photoBuffer
                property real strenght: photoItem.strenght

                property string blendingShader: "
                        uniform lowp sampler2D source; // this item used as mask
                        uniform lowp sampler2D photoSource; // photo item
                        uniform lowp sampler2D prevSource; // previous photo
                        uniform lowp float strenght;
                        uniform lowp float qt_Opacity; // inherited opacity of this item
                        varying highp vec2 qt_TexCoord0;
                        void main() {
                            lowp vec4 m = texture2D(source, qt_TexCoord0);
                            lowp vec4 s = texture2D(photoSource, qt_TexCoord0);
                            lowp vec4 d = texture2D(prevSource, qt_TexCoord0);

                            lowp vec4 mixed = s * ( 1.0 - strenght ) + d * strenght;

                            gl_FragColor = mixed * texture2D(source, qt_TexCoord0).a * qt_Opacity;
                        }"

                property string simpleShader: "
                        uniform lowp sampler2D source; // this item used as mask
                        uniform lowp sampler2D photoSource; // photo item
                        uniform lowp float qt_Opacity; // inherited opacity of this item
                        varying highp vec2 qt_TexCoord0;
                        void main() {
                            gl_FragColor = texture2D(photoSource, qt_TexCoord0) * texture2D(source, qt_TexCoord0).a * qt_Opacity;
                        }"

                fragmentShader: strenght == 0.0 ? simpleShader : blendingShader
            }
        }
    }

    PlasmaCore.ToolTipArea {
        id: toolTipArea
        icon: "dialog-information"
        textFormat: Text.PlainText

        anchors.fill: drawingArea

        ButtonBar {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.margins: photoItem.x + 2

            showNextButton: contentProviderLoader.item != undefined && contentProviderLoader.item.next != undefined
            showPreviousButton: contentProviderLoader.item != undefined && contentProviderLoader.item.previous != undefined
        }
    }
    
//    PlasmaCore.DataSource {
//        id: potdEngine
//        engine: "org.task_struct.photooftheday"
//        connectedSources: "Providers"
//        interval: updateIntervalCgf * 60000 // convert minutes to milliseconds

//        onNewData: {
//            console.debug( "onNewData " + sourceName )
            
//            if( selectedProviderCfg == sourceName ) {

//                plasmoid.busy = false

//                if( data.Error !== undefined &&  data.Error != "" ) {
//                    errorLabel.errorText = data.Error
//                }

//                if( undefined === data.Photo ) {
//                    return
//                }
                
//                // set current image as back
//                photoBuffer.pixmap = photo.pixmap

//                // load new image
//                photo.pixmap = data.Photo

//                toolTipArea.mainText = data.Title
//                toolTipArea.subText = potdEngine.data["Providers"][sourceName]
//            }
//        }
//    }

    ErrorLabel {
        id: errorLabel
        anchors.centerIn: drawingArea
    }


    ConfigureView {
        visible: selectedProviderCfg == ""
    }

    ParallelAnimation {
        id: transitionAnimation

        NumberAnimation {
            id: plasmoidWidthAnim
            target: drawingArea
            property: "width"
            duration: __animationsDuration
        }

        NumberAnimation {
            id: plasmoidHeightAnim
            target: drawingArea
            property: "height"
            duration: __animationsDuration
        }

        NumberAnimation {
            target: photoItem
            property: "strenght"
            duration: __animationsDuration
            from: 1.0; to: 0.0
        }
    }
}
