import QtQuick 2.0

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.plasmoid 2.0

QtObject {
    id: potdProvider

    //exported API
    property var pixmap
    property string toolTipTitle
    property string toolTipContent
    property string errorText

    function next() {
        var keys = Object.keys(__potdEngine.data["Providers"])

        // find provider position
        var i;
        for( i = 0; i < keys.length; ++i ) {
            if( keys[i] === plasmoid.configuration.selectedProvider ) {
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

    function previous() {
        var keys = Object.keys(__potdEngine.data["Providers"])

        // find provider position
        var i;
        for( i = 0; i < keys.length; ++i ) {
            if( keys[i] === plasmoid.configuration.selectedProvider ) {
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

    // local
    property string selectedProviderCfg: plasmoid.configuration.selectedProvider

    property string __connectedProvider: ""

    property int updateIntervalCgf: plasmoid.configuration.updateInterval


    property var __potdEngine: PlasmaCore.DataSource {
        engine: "org.task_struct.photooftheday"
        connectedSources: "Providers"
        interval: updateIntervalCgf * 60000 // convert minutes to milliseconds

        onNewData: {
            console.debug( "onNewData " + sourceName )

            if( plasmoid.configuration.selectedProvider == sourceName ) {

                plasmoid.busy = false

                if( data.Error !== undefined &&  data.Error != "" ) {
                    potdProvider.errorText = data.Error
                }

                if( undefined === data.Photo ) {
                    return
                }

                // load new image
                potdProvider.pixmap = data.Photo
                potdProvider.toolTipTitle = data.Title
                potdProvider.toolTipContent = __potdEngine.data["Providers"][sourceName]
            }
        }

        Component.onCompleted: {
            if( selectedProviderCfg != "" ) {
                __potdEngine.connectSource(selectedProviderCfg)
                __connectedProvider = selectedProviderCfg
            }
        }
    }

    onSelectedProviderCfgChanged: {
        if( __connectedProvider != "" ) {
            __potdEngine.disconnectSource(__connectedProvider)
        }

        if( selectedProviderCfg != "" ) {
            __potdEngine.connectSource( selectedProviderCfg )
            __connectedProvider = selectedProviderCfg

            parent.setUpActions()

        } else {
            parent.clearActions()
        }
    }
}
