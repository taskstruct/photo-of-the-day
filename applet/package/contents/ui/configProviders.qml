import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import org.kde.plasma.plasmoid 2.0

import org.task_struct.private.photooftheday 1.0

Item {
    id: configProvidersForm

    signal configurationChanged

    //BEGIN functions
    function saveConfig() {
        console.debug("saveing provider")
        plasmoid.configuration.selectedProvider = providersModel.getPluginName(providersBomboBox.currentIndex)
        plasmoid.configuration.updateInterval = hoursSb.value * 60 + minsSb.value
    }

    //    function restoreConfig() {
    //    }
    //END functions

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

                onCurrentIndexChanged: configProvidersForm.configurationChanged()

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

                onValueChanged: configProvidersForm.configurationChanged()
            }

            Label {
                text: i18n("hours")
            }

            SpinBox {
                id: minsSb
                minimumValue: hoursSb.value != 0 ? 0 : 10 // min 0h 10min
                maximumValue: 59

                onValueChanged: configProvidersForm.configurationChanged()
            }

            Label {
                text: i18n("minutes")
            }
        }
    }
}
