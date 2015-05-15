import QtQuick 2.4
import QtQuick.Layouts 1.1
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.plasmoid 2.0

Item {
    id: buttonBar

    implicitWidth: buttonsRow.width
    implicitHeight: buttonsRow.height

    opacity: 0.0
    visible: false

    RowLayout
    {
        id: buttonsRow

        PlasmaComponents.Button
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

                console.debug( "New provider is: " + keys[i] + " at " + i )
                plasmoid.configuration.selectedProvider = keys[i]
            }
        }

        PlasmaComponents.Button {
            iconSource: "go-next"

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

                ++i; // go to next

                if( i === keys.length ) {
                    // current provider is the last one. Go to first
                    i = 0;
                }

                console.debug( "New provider is: " + keys[i] + " at " + i )
                plasmoid.configuration.selectedProvider = keys[i]
            }
        }

        PlasmaComponents.Button {
            iconSource: "go-home"

            onClicked: {
            }
        }
    }

    states: State {
        name: "show"; when: toolTipArea.containsMouse
    }

    transitions: [ Transition {
            from: ""; to: "show";

            PlasmaExtras.AppearAnimation {
                targetItem: buttonBar
            }
        },

        Transition {
            from: "show"; to: "";

            PlasmaExtras.DisappearAnimation {
                targetItem: buttonBar
            }
        }
    ]
}
