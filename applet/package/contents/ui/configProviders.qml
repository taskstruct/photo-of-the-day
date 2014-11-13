import QtQuick 2.0
import QtQuick.Controls 1.2 as QtControls
import QtQuick.Layouts 1.1 as QtLayouts



import org.task_struct.private.photooftheday 1.0

Item {
    width: childrenRect.width
    height: childrenRect.height
    
    property string cfg_selectedProvider: ""
    
    QtLayouts.RowLayout {
        
        QtControls.Label {
            text: i18n("Select provider:")
        }
    
        QtControls.ComboBox {
            id: providersBomboBox
            
            textRole: "RealNameRole"

            model: ProvidersModel {
                id: providersModel
            }
            
            onActivated: {
                cfg_selectedProvider = providersModel.getPluginName(index)
            }
            
            Component.onCompleted: {
                currentIndex = providersModel.getIndex(cfg_selectedProvider)
                console.debug( "cfg_selectedProvider = " + cfg_selectedProvider )
                console.debug( "currentIndex = " + currentIndex )
            }
        }
    }
}