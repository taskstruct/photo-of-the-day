import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import org.kde.plasma.plasmoid 2.0

import org.task_struct.private.photooftheday 1.0

Item {
    width: childrenRect.width
    height: childrenRect.height
    
    property string cfg_selectedProvider: ""

    GridLayout {

        width: parent.width

        columns: 2

        Label {
            text: i18n("Select provider:")
        }
    
        ComboBox {
            id: providersBomboBox
            
            textRole: "RealNameRole"

            model: ProvidersModel {
                id: providersModel
            }

            Layout.fillWidth: true

            onActivated: {
                cfg_selectedProvider = providersModel.getPluginName(index)
            }
            
            Component.onCompleted: {
                currentIndex = providersModel.getIndex(plasmoid.configuration.selectedProvider)
            }
        }

        Label {
            text: i18n("Update interval:")
        }

        SpinBox {
            minimumValue: 10
            maximumValue: 180
            suffix: i18n("min")
        }
    }
}
