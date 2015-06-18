import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

import org.kde.plasma.plasmoid 2.0

import org.task_struct.private.photooftheday 1.0

ColumnLayout {
    id: configSlideshowForm

    signal configurationChanged

    //BEGIN functions
    function saveConfig() {
        plasmoid.configuration.slideshowInterval = ( hoursInterval.value * 3600 +
                                                     minsInterval.value * 60 + secsInterval.value ) * 1000

        plasmoid.configuration.recursiveScan = recurCheckBox.checked
        plasmoid.configuration.randomize = randomCheckBox.checked

        plasmoid.configuration.slideshowPaths = slideshowModel.paths
    }

    //    function restoreConfig() {
    //    }
    //END functions

    SystemPalette {
        id: sysPal
    }

    Component.onCompleted: {
        var tmp = Math.floor( plasmoid.configuration.slideshowInterval / 1000 )
        var secs = tmp % 60

        tmp = Math.floor( tmp / 60 )
        var mins = tmp % 60

        // the rest should be hours
        hoursInterval.value = Math.floor( tmp / 60 )
        minsInterval.value = mins
        secsInterval.value = secs
    }

    RowLayout {
        Label {
            text: i18n("Change every:")
        }

        SpinBox {
            id: hoursInterval
            minimumValue: 0
            maximumValue: 24

            onValueChanged: configSlideshowForm.configurationChanged()
        }

        Label {
            text: i18n("hours")
        }

        SpinBox {
            id: minsInterval
            minimumValue: 0
            maximumValue: 59

            onValueChanged: configSlideshowForm.configurationChanged()
        }

        Label {
            text: i18n("minutes")
        }

        SpinBox {
            id: secsInterval
            minimumValue: ( hoursInterval.value == 0 && minsInterval.value == 0 ) ? 10 : 0
            maximumValue: 59

            onValueChanged: configSlideshowForm.configurationChanged()
        }

        Label {
            text: i18n("seconds")
        }
    }

    CheckBox {
        id: randomCheckBox

        checked: plasmoid.configuration.randomize
        text: i18n("Show images in random order")

        onCheckedChanged: configSlideshowForm.configurationChanged()
    }

    ScrollView {

        frameVisible: true

        Layout.fillHeight: true;

        anchors {
            left: parent.left
            right: parent.right
        }

        ListView {
            id: slidePathsView
            anchors.margins: 4

            spacing: units.smallSpacing

            model: SlideShowModel {
                id: slideshowModel

                recursive: plasmoid.configuration.recursiveScan

                Component.onCompleted: paths = plasmoid.configuration.slideshowPaths

                onPathsChanged: configSlideshowForm.configurationChanged()
            }

            delegate: Rectangle {

                width: slidePathsView.width
                height: Math.max(title.paintedHeight + infoText.paintedHeight + units.smallSpacing, removeButton.height) + 2 * units.smallSpacing;

                color: ( index % 2 == 0 ) ? sysPal.base :sysPal.alternateBase
                radius: units.smallSpacing

                border {
                    color: sysPal.mid
                    width: 1
                }

                Column {

                    spacing: units.smallSpacing

                    anchors.fill: parent
                    anchors.margins: units.smallSpacing

                    Label {
                        id: title
                        text: pathRole
                    }

                    Label {
                        id: infoText

                        text: i18n("Images: ") + fileCountRole

                        font {
                            italic: true
                            pointSize: theme.defaultFont.pointSize * 0.8
                        }
                    }
                }

                MouseArea {
                    id: mouseArea

                    hoverEnabled: true

                    anchors.fill: parent

                    ToolButton {
                        id: removeButton

                        visible: mouseArea.containsMouse

                        anchors {
                            verticalCenter: parent.verticalCenter
                            right: parent.right
                            rightMargin: units.smallSpacing
                        }

                        iconName: "list-remove"

                        onClicked: {
                            slideshowModel.removePath(index)
                        }
                    }
                }
            }
        }
    }

    RowLayout {

        width: parent.width

        CheckBox {
            id: recurCheckBox

            text: i18n("Search in subdirectories")

            checked: plasmoid.configuration.recursiveScan

            onCheckedChanged: {
                slideshowModel.recursive = checked
                configSlideshowForm.configurationChanged()
            }
        }

        Item {
            Layout.fillWidth: true
        }

        Button {
            iconName: "list-add"
            text: i18n("Add folder")

            FileDialog {
                id: fileDialog

                selectFolder: true

                onAccepted: {
                    slideshowModel.addPath(fileDialog.folder);
                }
            }

            onClicked: fileDialog.open()
        }
    }
}
