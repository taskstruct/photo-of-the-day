import QtQuick 2.0
import QtQuick.Layouts 1.1
import org.kde.plasma.components 2.0 as PlasmaComponents

Item {
    id: buttonBar

    implicitWidth: buttonsRow.width
    implicitHeight: buttonsRow.height

    opacity: 0

    RowLayout
    {
        id: buttonsRow

        PlasmaComponents.ToolButton
        {
            iconSource: "go-previous"

            enabled: selectedProviderCfg.length != 0

            onClicked: {
                plasmoid.busy = true

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
//                photoBuffer.source = photo.source
//                photo.source = "/home/nikolay/Свалени/walls/1509102.jpg"

////                console.debug("photoBuffer = ", photoBuffer.source)
////                console.debug("photo = ", photo.source)
//                border.visible = !border.visible
//                photoItem.visible = !photoItem.visible
                console.debug("Plasmoid object dump:")
                for( var key in plasmoid ) {
                    console.debug( key," = ", plasmoid[key] )
                }
            }
        }
    }

    states: State {
        name: "show"; when: buttonBar.visible
        PropertyChanges { target: buttonBar; opacity: 1; }
    }

    transitions: Transition {
        from: ""; to: "show"; reversible: true
        NumberAnimation { target: buttonBar; properties: "opacity"; duration: 2000 /*units.longDuration*/; easing.type: Easing.InOutQuad }
    }
}
