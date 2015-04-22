import QtQuick 2.0
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore


import org.task_struct.private.photooftheday 1.0
 
Item {
    id: rootItem
    
    width: 400
    height: 400
    
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
    property real _strenght: 0.0
    
    // constants
    readonly property int __animationsDuration: 2000
    
    property string _providerSource: ""

    signal newSizeUpdated( size newSize)

    Image {
        id: photo

        property bool updated: false

        visible: false
        layer.enabled: true;
        layer.smooth: true

        onImplicitWidthChanged: {
            console.debug( "Photo.implicitWidth = ", implicitWidth )

            if(updated) {
                rootItem.calcNewSize()
                updated = false;
            }
            else {
                updated = true;
            }
        }

        onImplicitHeightChanged: {
            console.debug( "Photo.implicitHeight = ", implicitHeight )

            if(updated) {
                rootItem.calcNewSize()
                updated = false;
            }
            else {
                updated = true;
            }
        }

        onStateChanged: console.debug("Photo.state = ", state )

        onSourceSizeChanged: console.debug("Photo.sourceSize = ", sourceSize)
    }

    Image {
        id: photoBuffer
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

//        console.debug("Plasmoid object dump:")
//        for( var key in plasmoid ) {
//            console.debug( key," = ", plasmoid[key] )
//        }

//        console.debug("Plasmoid.nativeInterface object dump:")
//        for( key in plasmoid.nativeInterface ) {
//            console.debug( key," = ", plasmoid[key] )
//        }

//        photo.source = "/home/nikolay/Свалени/walls/wallpaper-3032560.jpg"
    }

    function calcNewSize() {
        var maxSize = Math.max( plasmoid.width, plasmoid.height )

        console.debug("Plasmoid size is ", Qt.size( plasmoid.width, plasmoid.height ) )

        var newSize = Qt.size( 0, 0 )

        if( photo.implicitWidth >= photo.implicitHeight ) {
            newSize.width = maxSize
            newSize.height = maxSize * ( photo.implicitHeight / photo.implicitWidth )
        }
        else {
            newSize.height = maxSize
            newSize.width = maxSize * ( photo.implicitWidth / photo.implicitHeight )
        }

        console.debug("New plasmoid size is ", newSize)

        newSizeUpdated(newSize)
    }
    
    onSelectedProviderCfgChanged: {
        if( selectedProviderCfg.length != 0 ) {
            _providerSource = selectedProviderCfg + ":" + plasmoid.id;
            
            potdEngine.connectSource( _providerSource )
        
            Plasmoid.busy = true;
        }   
    }

    onNewSizeUpdated: {
        if( !animatedTransitionsCfg ) {
            plasmoid.width = newSize.width
            plasmoid.height = newSize.height
        } else {
            plasmoidWidthAnim.to = newSize.width
            plasmoidHeightAnim.to = newSize.height

            transitionAnimation.start()
        }
    }

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

        layer.enabled: true
        layer.effect: ShaderEffect {
            id: shaderEffect

            property var photoSource: photo.source != "" ? photo : blank
            property var prevSource: photoBuffer.source != "" ? photoBuffer : blank
            property real strenght: rootItem._strenght

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

    MouseArea {
        id: appletArea

        anchors.fill: parent

        hoverEnabled: true
        preventStealing: false
        acceptedButtons: Qt.LeftButton | Qt.MiddleButton

        ButtonBar {
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: photoItem.x + 2
        }
    }
    
    
    PlasmaCore.DataSource {
        id: potdEngine
        engine: "org.task_struct.photooftheday"
        connectedSources: "Providers"
        
        onSourceAdded: {
            console.debug( "onSourceAdded: " + source ) 
        }
                
        onNewData: {
            console.debug( "onNewData " + sourceName )
            
            if( _providerSource == sourceName ) {

                plasmoid.busy = false
                
                // set current image as back
                photoBuffer.source = photo.source

                // load new image
                photo.source = data.Photo

                plasmoid.toolTipMainText = data.Title
                
            }
        }
    }

    ParallelAnimation {
        id: transitionAnimation

        NumberAnimation {
            id: plasmoidWidthAnim
            target: plasmoid
            property: "width"
            duration: __animationsDuration
        }

        NumberAnimation {
            id: plasmoidHeightAnim
            target: plasmoid
            property: "height"
            duration: __animationsDuration
        }

        NumberAnimation {
            target: rootItem
            property: "_strenght"
            duration: __animationsDuration
            from: 1.0; to: 0.0
        }

    }
}
