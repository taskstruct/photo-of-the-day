import QtQuick 2.4
import QtQuick.Layouts 1.1
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.kquickcontrolsaddons 2.0

import org.task_struct.private.photooftheday 1.0
 
Item {
    id: rootItem
    
    // 16:10
    width: 400
    height: 250

    
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
    
    // logic
    property string selectedProviderCfg: Plasmoid.configuration.selectedProvider
    property int updateIntervalCgf: Plasmoid.configuration.updateInterval
    
    // constants
    readonly property int __animationsDuration: 2000
    
    property string _providerSource: ""

    signal newSizeUpdated( size newSize)

    QPixmapItem {
        id: photo

        width: nativeWidth
        height: nativeHeight

        smooth: true
        visible: false
        layer.enabled: true;
        layer.smooth: true

        onPixmapChanged: {
            console.debug( "Photo.nativeWidth = ", nativeWidth )
            console.debug( "Photo.nativeHeight = ", nativeHeight )

            if( !photo.null ) {
                var newSize = rootItem.rescale()
                console.debug("onPixmapChanged ", newSize.width, ",", newSize.height, " ? ", animatedTransitionsCfg)
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
    
    Plasmoid.backgroundHints: Plasmoid.NoBackground
//     Plasmoid.aspectRatioMode: Plasmoid.Square NOTE: Not implemented in KF5...
    
    Component.onCompleted: {
        console.debug( "Component.onCompleted" )
        plasmoid.setConfigurationRequired( selectedProviderCfg === "", i18n("No provider is specified") )
//         plasmoid.setConfigurationRequired( selectedProviderCfg.length == 0, i18n("No provider is specified") )
//         Plasmoid.aspectRatioMode = Plasmoid.KeepAspectRatio
    }

    function rescale() {
        var rw = rootItem.height * photo.nativeWidth / photo.nativeHeight

        console.debug("RESCALE: rootItem = ", rootItem.width, ",", rootItem.height )
        console.debug("RESCALE: plasmoid = ", plasmoid.width, ",", plasmoid.height )
        console.debug("RESCALE: rw = ", rw)

        var size;

        if(rw <= plasmoid.width) {
            size = Qt.size( rw, plasmoid.height )
        } else {
            size = Qt.size( plasmoid.width, plasmoid.width * photo.nativeHeight / photo.nativeWidth )
        }

        console.debug("RESCALE: size = ", size.width, ",", size.height)

        return size
    }

    function updateDrawingArea() {
        var newSize = rescale()
        drawingArea.width = newSize.width
        drawingArea.height = newSize.height
    }
    
    onSelectedProviderCfgChanged: {
//        console.debug("New selected provider: ", selectedProviderCfg)
        if( selectedProviderCfg.length != 0 ) {
            _providerSource = selectedProviderCfg;
            
            potdEngine.connectSource( _providerSource )
        
            Plasmoid.busy = true;
        }   
    }

    onNewSizeUpdated: {
        console.debug("onNewSizeUpdated ", newSize.width, ",", newSize.height, " ? ", animatedTransitionsCfg)
        if( !animatedTransitionsCfg ) {
            drawingArea.width = newSize.width
            drawingArea.height = newSize.height
        } else {
            plasmoidWidthAnim.to = newSize.width
            plasmoidHeightAnim.to = newSize.height

            transitionAnimation.start()
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

            onRadiusChanged: console.debug("Border radius is", radius)

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
            layer.effect: ShaderEffect {
                id: shaderEffect

                blending: false

                property var photoSource: photo.null ? blank : photo
                property var prevSource: photoBuffer.null ? blank : photoBuffer
                property real strenght: photoItem.strenght

                fragmentShader: "
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

                            gl_FragColor = mixed * m.a * qt_Opacity;
                        }"
            }
        }
    }

    PlasmaCore.ToolTipArea {
        id: toolTipArea
        icon: "image"

        anchors.fill: drawingArea

        ButtonBar {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.margins: photoItem.x + 2
        }
    }
    
    PlasmaCore.DataSource {
        id: potdEngine
        engine: "org.task_struct.photooftheday"
        connectedSources: "Providers"
        interval: updateIntervalCgf * 60000 // convert minutes to milliseconds
        
        onSourceAdded: {
            console.debug( "onSourceAdded: " + source ) 
        }
                
        onNewData: {
            console.debug( "onNewData " + sourceName )
            
            if( _providerSource == sourceName ) {

                if( undefined === data.Photo ) {
                    return
                }

                plasmoid.busy = false
                
                // set current image as back
                photoBuffer.pixmap = photo.pixmap

                // load new image
                photo.pixmap = data.Photo

                toolTipArea.mainText = data.Title
            }
        }
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
