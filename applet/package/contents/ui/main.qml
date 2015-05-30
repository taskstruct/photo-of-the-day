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
    property string selectedProviderCfg: plasmoid.configuration.selectedProvider

    property string __connectedProvider: ""

    property int updateIntervalCgf: Plasmoid.configuration.updateInterval
    
    // constants
    readonly property int __animationsDuration: 2000

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
    
    Plasmoid.backgroundHints: Plasmoid.NoBackground

    // HACK: Delay data source connection. Let all properties to be initialized and emit on<Property>Changed signals
    // If we connect to data source in onSelectedProviderCfgChanged and we have valid cache in data engine,
    // plasmoid and rootItem width and height are still 0s
    Timer {
        id: initTimer
        interval: 100
        running: true

        onTriggered: {
            drawingArea.width = rootItem.width
            drawingArea.height = rootItem.height

            if( rootItem.selectedProviderCfg != "" ) {
                potdEngine.connectSource( rootItem.selectedProviderCfg )
                __connectedProvider = rootItem.selectedProviderCfg
            }

            rootItem.selectedProviderCfgChanged.connect( function(fasd) {

                if( rootItem.__connectedProvider != "" ) {
                    potdEngine.disconnectSource( rootItem.__connectedProvider )
                }

                plasmoid.busy = true;
                potdEngine.connectSource( rootItem.selectedProviderCfg )
                rootItem.__connectedProvider = rootItem.selectedProviderCfg
            } )
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

        var keys = Object.keys(potdEngine.data["Providers"])

        // find provider position
        var i;
        for( i = 0; i < keys.length; ++i ) {
            if( keys[i] === selectedProviderCfg ) {
                break;
            }
        }

        ++i; // go to next

        if( i === keys.length ) {
            // current provider is the last one. Go to first
            i = 0;
        }

        plasmoid.configuration.selectedProvider = keys[i]
    }

    function action_prevprovider() {
        plasmoid.busy = true

        var keys = Object.keys(potdEngine.data["Providers"])

        // find provider position
        var i;
        for( i = 0; i < keys.length; ++i ) {
            if( keys[i] === selectedProviderCfg ) {
                break;
            }
        }

        --i; // go to previous

        if( i < 0 ) {
            // current provider is the first one. Go to last
            i = keys.length - 1;
        }

        plasmoid.configuration.selectedProvider = keys[i]
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
        icon: "image"
        textFormat: Text.PlainText

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

        onNewData: {
            console.debug( "onNewData " + sourceName )
            
            if( selectedProviderCfg == sourceName ) {

                plasmoid.busy = false

                if( data.Error !== undefined &&  data.Error != "" ) {
                    errorLabel.errorText = data.Error
                }

                if( undefined === data.Photo ) {
                    return
                }
                
                // set current image as back
                photoBuffer.pixmap = photo.pixmap

                // load new image
                photo.pixmap = data.Photo

                toolTipArea.mainText = data.Title
                toolTipArea.subText = potdEngine.data["Providers"][sourceName]
            }
        }
    }

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
