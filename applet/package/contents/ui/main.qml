import QtQuick 2.0
import QtGraphicalEffects 1.0
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
 
Item {
    id: rootItem
    
    width: 400
    height: 400
//     implicitWidth: 400
//     implicitHeight: 400
    
    property bool showShadowCfg: Plasmoid.configuration.showShadow
    property int shadowWidthCfg: Plasmoid.configuration.shadowWidth
    property real shadowSpreadCfg: Plasmoid.configuration.shadowSpread
    
    property bool showBorderCfg: Plasmoid.configuration.showBorder
    property int borderWidthCfg: Plasmoid.configuration.borderWidth
    property real borderOpacityCfg: Plasmoid.configuration.borderOpacity
    
    property int roundedCornersCfg: Plasmoid.configuration.roundedCorners
    property bool animatedTransitionsCfg: Plasmoid.configuration.animatedTransitions
    
    property string selectedProviderCfg: Plasmoid.configuration.selectedProvider
    
    property int __animationsDuration: 1000
    
    property string _providerSource: ""
    
    Plasmoid.backgroundHints: Plasmoid.NoBackground
//     Plasmoid.aspectRatioMode: Plasmoid.Square NOTE: Not implemented in KF5...
    
    Component.onCompleted: {
        console.debug( "Component.onCompleted" )
        Plasmoid.setConfigurationRequired( selectedProviderCfg.length == 0, i18n("No provider is specified") )
//         plasmoid.setConfigurationRequired( selectedProviderCfg.length == 0, i18n("No provider is specified") )
        
//         Plasmoid.aspectRatioMode = Plasmoid.KeepAspectRatio
    }
    
    onSelectedProviderCfgChanged: {
        if( selectedProviderCfg.length != 0 ) {
            _providerSource = selectedProviderCfg + ":" + plasmoid.id;
            
            potdEngine.connectSource( _providerSource )
        }   
    }
    
    Item {
        id: visualContent
        
        width: if( imageEffect.aspectRatio > 1.0 ) {
            return plasmoid.width
        } else {
            return plasmoid.width * imageEffect.aspectRatio
        }
        
        height: if( imageEffect.aspectRatio > 1.0 ) {
            return plasmoid.height / imageEffect.aspectRatio 
        } else {
            return plasmoid.height
        }
        
        anchors.top: parent.top
        anchors.left: parent.left
        
        Behavior on width {
            enabled: animatedTransitionsCfg /*imageEffect.running*/
            NumberAnimation { duration: __animationsDuration }
        }
        
        Behavior on height {
            enabled: animatedTransitionsCfg /*imageEffect.running*/
            NumberAnimation { duration: __animationsDuration }
        }
    }
    
    RectangularGlow {
        id: glowEffect
        
        anchors.fill: visualContent
        anchors.margins: glowRadius
        
        glowRadius: shadowWidthCfg
        spread: shadowSpreadCfg
        color: "black"
        cached: true //TODO: disable during transitions
        
        cornerRadius: border.radius + glowRadius
        
        visible: showShadowCfg
    }
    
    Rectangle {
        id: border
        
        color: "transparent"
        
        opacity: borderOpacityCfg
        radius: roundedCornersCfg
        
        visible: showBorderCfg
        
        border.width: borderWidthCfg
        border.color: "red"
        
        anchors.fill: visualContent
        anchors.margins: showShadowCfg ? glowEffect.glowRadius : 0
    }
    
    ImageEffect {
        id: imageEffect
        mask: maskRect
        
        frontSource: "/home/nikolay/Свалени/walls/wallpaper-3032560.jpg"
        
        anchors.fill: maskRect        
    }
    
    Rectangle {
        id: maskRect
        
        color: "#FF0000"
        
        radius: border.radius - border.border.width
        
        visible: false
        
        anchors.fill: border
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
                var keys = Object.keys(potdEngine.data["Providers"])
                
                // find provider position
                var i;
                for( i = 0; i < keys.length; ++i ) {
                    if( keys[i] == selectedProviderCfg ) {
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
                var keys = Object.keys(potdEngine.data["Providers"])
                
                // find provider position
                var i;
                for( i = 0; i < keys.length; ++i ) {
                    if( keys[i] == selectedProviderCfg ) {
                        console.debug( "Provider found: " + keys[i] + " at " + i );
                        break;
                    }
                }
                
                ++i; // go to next
                
                if( i == keys.length ) {
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
                
                imageEffect.frontSource = data.Photo
                plasmoid.toolTipMainText = data.Title
                
                dbg.text = data.Title
            }
        }
    }
}