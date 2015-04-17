import QtQuick 2.0
import QtGraphicalEffects 1.0
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents

import org.task_struct.private.photooftheday 1.0
 
Item {
    id: rootItem
    
    width: 400
    height: 400
    
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
    
    property string selectedProviderCfg: Plasmoid.configuration.selectedProvider
    
    property int __animationsDuration: 1000
    
    property string _providerSource: ""

    signal newSizeUpdated( size newSize)

    property Image photo: Image {

        property bool updated: false

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

    property Image photoBuffer: Image {


    }
    
    Plasmoid.backgroundHints: Plasmoid.NoBackground
//     Plasmoid.aspectRatioMode: Plasmoid.Square NOTE: Not implemented in KF5...
    
    Component.onCompleted: {
        console.debug( "Component.onCompleted" )
        plasmoid.setConfigurationRequired( selectedProviderCfg.length == 0, i18n("No provider is specified") )
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
//        if( !animatedTransitionsCfg ) {
            plasmoid.width = newSize.width
            plasmoid.height = newSize.height
//        }
    }

    Behavior on Plasmoid.width {
        NumberAnimation { duration: __animationsDuration }
    }

    Behavior on Plasmoid.height {
        NumberAnimation { duration: __animationsDuration }
    }
    
    BorderImage {
        id: shadow

        // color/size/strength
        source: "image://shadow/" + shadowColorCfg+ "/" + shadowWidthCfg + "/" + shadowSpreadCfg

        visible: showShadowCfg

        border {
            left: shadowWidthCfg
            right: shadowWidthCfg
            top: shadowWidthCfg
            bottom: shadowWidthCfg
        }

        anchors.fill: parent
    }
    
    Rectangle {
        id: border
        
        color: "transparent"
        
        opacity: borderOpacityCfg
        radius: roundedCornersCfg
        
        visible: showBorderCfg
        
        border.width: showBorderCfg ? borderWidthCfg : 1
        border.color: showBorderCfg ? borderColorCfg: "black"
        
        anchors.fill: showShadowCfg ? shadow : parent
        anchors.margins: showShadowCfg ? shadowWidthCfg : 0
    }
    
    ImageEffect {
        id: imageEffect
        mask: maskRect
        
        frontSource: "/home/nikolay/Свалени/walls/wallpaper-3032560.jpg"
        
        anchors.fill: border   
        anchors.margins: border.border.width
    }
    
    Rectangle {
        id: maskRect
        
        color: "#FF0000"
        
        radius: roundedCornersCfg - borderWidthCfg
        
        anchors.fill: parent
        anchors.margins: border.border.width
    }
    
    PlasmaComponents.ButtonRow {
        
        id: buttons
                
        anchors.right: imageEffect.right
        anchors.bottom: imageEffect.bottom
        anchors.margins: 5
        
        PlasmaComponents.ToolButton {
            iconSource: "go-previous"
            
            enabled: selectedProviderCfg.length != 0
            
            onClicked: {      
                Plasmoid.busy = true
                
                var keys = Object.keys(potdEngine.data["Providers"])
                
                // find provider position
                var i;
                for( i = 0; i < keys.length; ++i ) {
                    if( keys[i] === selectedProviderCfg ) {
                        console.debug( "Provider found: " + keys[i] + " at " + i );
                        break;
                    }
                }
                
                --i; // go to next
                
                if( i < 0 ) {
                    // current provider is the first one. Go to last
                    i = keys.length - 1;
                }
                
                // remove old one
                potdEngine.disconnectSource( selectedProviderCfg + ":" + plasmoid.id )
                
                console.debug( "New provider is: " + keys[i] + " at " + i )
                selectedProviderCfg = keys[i]
            }
        }
        
        PlasmaComponents.ToolButton {
            iconSource: "go-next"
            
            enabled: selectedProviderCfg.length != 0
            
            onClicked: {       
                Plasmoid.busy = true
                
                var keys = Object.keys(potdEngine.data["Providers"])
                
                // find provider position
                var i;
                for( i = 0; i < keys.length; ++i ) {
                    if( keys[i] === selectedProviderCfg ) {
                        console.debug( "Provider found: " + keys[i] + " at " + i );
                        break;
                    }
                }
                
                ++i; // go to next
                
                if( i === keys.length ) {
                    // current provider is the last one. Go to first
                    i = 0;
                }
                
                // remove old one
                potdEngine.disconnectSource( selectedProviderCfg + ":" + plasmoid.id )
                
                console.debug( "New provider is: " + keys[i] + " at " + i )
                selectedProviderCfg = keys[i]
            }
        }
        
        PlasmaComponents.ToolButton {
            iconSource: "go-home"

            onClicked: {
                photo.source = "/home/nikolay/Свалени/walls/1509102.jpg"
            }
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
                
                photo.source = data.Photo

                plasmoid.toolTipMainText = data.Title
                
            }
        }
    }
}
