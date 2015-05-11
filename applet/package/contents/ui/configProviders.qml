import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import org.kde.plasma.plasmoid 2.0

import org.task_struct.private.photooftheday 1.0

Item {
    id: configForm
    width: childrenRect.width
    height: childrenRect.height
    
    property string cfg_selectedProvider: ""
    property int cfg_updateInterval: plasmoid.configuration.updateInterval

    Component.onCompleted: {
        hoursSb.value = Math.floor( plasmoid.configuration.updateInterval / 60 )
        minsSb.value = plasmoid.configuration.updateInterval % 60
    }

    ColumnLayout {

        width: parent.width

        RowLayout {

            Layout.fillWidth: true

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
        }

        RowLayout {
            Label {
                text: i18n("Update interval:")
            }

            SpinBox {
                id: hoursSb
                minimumValue: 0
                maximumValue: 23

                onValueChanged: cfg_updateInterval = value * 60 + minsSb.value
            }

            Label {
                text: i18n("hours")
            }

            SpinBox {
                id: minsSb
                minimumValue: hoursSb.value != 0 ? 0 : 10 // min 0h 10min
                maximumValue: 59

                onValueChanged: cfg_updateInterval = hoursSb.value * 60 + value
            }

            Label {
                text: i18n("minutes")
            }
        }
    }
}
