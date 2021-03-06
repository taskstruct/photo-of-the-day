import QtQuick 2.0
import QtQuick.Controls 1.1 as QtControls
import QtQuick.Layouts 1.1
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
    property color cfg_shadowColor
    
    property alias cfg_showBorder: showBorderCheckBox.checked
    property alias cfg_borderWidth: borderWidthSpinBox.value
    property alias cfg_borderOpacity: borderOpacitySpinBox.value
    property color cfg_borderColor
    
    property alias cfg_roundedCorners: roundedCornersSpinBox.value
    
    property alias cfg_animatedTransitions: animTransCheckBox.checked
    
    ColumnLayout {

        width: parent.width

        QtControls.GroupBox {
            title: i18n("Shadow")

            Layout.fillWidth: true

            GridLayout {

                columns: 2

                QtControls.CheckBox {
                    id: showShadowCheckBox
                    text: i18n("Show shadow around image")

                    Layout.columnSpan: 2
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

                    ColorDialog {
                        id: shadowColorDialog
                        title: i18n("Please choose a color")
                        visible: false

                        onAccepted: {
                            cfg_shadowColor = color
                            close()
                        }
                        onRejected: {
                            close()
                        }
                    }

                    onClicked: { shadowColorDialog.open() }
                }
            }
        }

        QtControls.GroupBox {
            title: i18n("Border")

            Layout.fillWidth: true

            GridLayout {

                columns: 2

                QtControls.CheckBox {
                    id: showBorderCheckBox
                    text: i18n("Show border around image")

                    Layout.columnSpan: 2
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

                    ColorDialog {
                        id: borderColorDialog
                        title: i18n("Please choose a color")
                        visible: false

                        onAccepted: {
                            cfg_borderColor = color
                            close()
                        }
                        onRejected: {
                            close()
                        }
                    }

                    onClicked: { borderColorDialog.open() }
                }
            }
        }


        RowLayout {
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
