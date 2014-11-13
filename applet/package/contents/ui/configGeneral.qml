import QtQuick 2.0
import QtQuick.Controls 1.1 as QtControls
// import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1 as QtLayouts
import QtQuick.Dialogs 1.0

// QtControls.GroupBox {
Item {
    width: childrenRect.width
    height: childrenRect.height
    
//     flat: true
//     title: i18n("Appearance")
    
    property alias cfg_showShadow: showShadowCheckBox.checked
    property alias cfg_shadowWidth: shadowWidthSpinBox.value
    property alias cfg_shadowSpread: shadowSpreadSpinBox.value
    property color cfg_shadowColor: "black"
    
    property alias cfg_showBorder: showBorderCheckBox.checked
    property alias cfg_borderWidth: borderWidthSpinBox.value
    property alias cfg_borderOpacity: borderOpacitySpinBox.value
    property color cfg_borderColor: "red"
    
    property alias cfg_roundedCorners: roundedCornersSpinBox.value
    
    property alias cfg_animatedTransitions: animTransCheckBox.checked
    
    QtLayouts.ColumnLayout {
        
        width: parent.width
        
        QtControls.GroupBox {
            title: i18n("Shadow")
            
            QtLayouts.Layout.fillWidth: true
            
            QtLayouts.GridLayout {
                
                columns: 2
                
                QtControls.CheckBox {
                    id: showShadowCheckBox
                    text: i18n("Show shadow around image")
                    
                    QtLayouts.Layout.columnSpan: 2
                }
                 
                QtControls.Label {
                    text: i18n("Width")
                }
                
                QtControls.SpinBox {
                    id: shadowWidthSpinBox
                    enabled: showShadowCheckBox.checked
                    
                    minimumValue: 1
                    maximumValue: 100
                }
                
                QtControls.Label {
                    text: i18n("Spread")
                }
                
                QtControls.SpinBox {
                    id: shadowSpreadSpinBox
                    enabled: showShadowCheckBox.checked
                    
                    decimals: 1
                    minimumValue: 0.0
                    maximumValue: 1.0
                    stepSize: 0.1
                }
                
                QtControls.Label {
                    text: i18n("Color")
                }
                    
                QtControls.Button {
                    iconName: "color-picker"
                    enabled: showShadowCheckBox.checked
                    
//                         ColorDialog {
//                             id: colorDialog
//                             title: "Please choose a color"
//                             visible: false
//                             
//                             onAccepted: {
//                                 cfg_shadowColor = colorDialog.color
//                                 close()
//                             }
//                             onRejected: {
//                                 close()
//                             }
//                             Component.onCompleted: visible = true
//                         }
//                         
//                         onClicked: { colorDialog.open() }
                    
//                         style: ButtonStyle {
//                             background: Rectangle {
//                                 implicitWidth: 100
//                                 implicitHeight: 25
//                                 border.width: control.activeFocus ? 2 : 1
//                                 border.color: "#888"
//                                 radius: 4
//                                 gradient: Gradient {
//                                     GradientStop { position: 0 ; color: control.pressed ? cfg_shadowColor : Qt.lighter(cfg_shadowColor) }
//                                     GradientStop { position: 1 ; color: control.pressed ? cfg_shadowColor : Qt.darker(cfg_shadowColor) }
//                                 }
// //                                 color: cfg_shadowColor
//                             }
//                         }
                }
            }
        }
        
        QtControls.GroupBox {
            title: i18n("Border")
            
            QtLayouts.Layout.fillWidth: true
            
            QtLayouts.GridLayout {
                
                columns: 2
        
                QtControls.CheckBox {
                    id: showBorderCheckBox
                    text: i18n("Show border around image")
                    
                    QtLayouts.Layout.columnSpan: 2
                }
                
                QtControls.Label {
                    text: i18n("Width")
                }
                
                QtControls.SpinBox {
                    id: borderWidthSpinBox
                    enabled: showBorderCheckBox.checked
                    
                    minimumValue: 1
                    maximumValue: 100
                }
                
                QtControls.Label {
                    text: i18n("Opacity")
                }
                
                QtControls.SpinBox {
                    id: borderOpacitySpinBox
                    enabled: showBorderCheckBox.checked
                    
                    decimals: 2
                    minimumValue: 0.05
                    maximumValue: 1.0
                    stepSize: 0.05
                }
                
                QtControls.Label {
                    text: i18n("Color")
                }
                
                QtControls.Button {
                    iconName: "color-picker"
                    enabled: showBorderCheckBox.checked
                }
            }
        }
        
        
        QtLayouts.RowLayout {
            QtControls.Label {
                text: i18n("Rounded corners")
            }
            
            QtControls.SpinBox {
                id: roundedCornersSpinBox
                
                minimumValue: 0
                maximumValue: 50 //TODO: Max should be plasmioid.width / 2 !
            }
        }
        
        QtControls.CheckBox {
            id: animTransCheckBox
            
            text: i18n("Animated transitions")
        }
    }
}