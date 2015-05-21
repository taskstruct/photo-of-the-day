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

            onClicked: rootItem.action_prevprovider()
        }

        PlasmaComponents.Button {
            iconSource: "go-next"

            enabled: selectedProviderCfg.length != 0

            onClicked: rootItem.action_nextprovider()
        }

        PlasmaComponents.Button {
            iconSource: "go-home"

            onClicked: Qt.openUrlExternally( potdEngine.data[rootItem.selectedProviderCfg]["PageUrl"] )
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
