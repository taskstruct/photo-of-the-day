import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

import org.kde.plasma.core 2.0
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.plasmoid 2.0

import org.task_struct.private.photooftheday 1.0

ColumnLayout {
    id: configImageForm

    signal configurationChanged

    //BEGIN functions
    function saveConfig() {
        plasmoid.configuration.recentImages = imageModel.recentImages
        plasmoid.configuration.imageFileName = imageModel.recentImages[ previewsGrid.currentIndex ]
    }

    //    function restoreConfig() {
    //    }
    //END functions

    ImageModel {
        id: imageModel

        Component.onCompleted: {
            recentImages = plasmoid.configuration.recentImages;

            var i = 0;
            var images = plasmoid.configuration.recentImages
            var selected = plasmoid.configuration.imageFileName

            while( i < images.length ) {
                if( images[i] == selected ) {
                    break
                }
                ++i
            }

            previewsGrid.currentIndex = i
        }
    }

    ScrollView {
        frameVisible: true

        Layout.fillHeight: true;

        anchors {
            left: parent.left
            right: parent.right
        }

        GridView {
            id: previewsGrid

            boundsBehavior: Flickable.StopAtBounds
            cellWidth: Math.floor(previewsGrid.width / Math.max(Math.floor(previewsGrid.width / (units.gridUnit*12)), 3))
            cellHeight: cellWidth / (plasmoid.width / plasmoid.height)
            currentIndex: -1
            focus: true

            anchors.margins: 4

            model: imageModel.recentImages

            SystemPalette {
                id: sysPal
            }

            delegate: MouseArea {
                id: imageDelegate

                width: previewsGrid.cellWidth
                height: previewsGrid.cellHeight

                hoverEnabled: true

                Rectangle {
                    id: background

                    color: previewsGrid.currentIndex == index ? sysPal.highlight : sysPal.button

                    opacity: 0.8

                    anchors {
                        fill: parent
                        margins: units.smallSpacing
                    }

                }

                Rectangle {
                    id: border

                    width: preview.paintedWidth + 2 * units.smallSpacing
                    height: preview.paintedHeight + 2 * units.smallSpacing

                    border {
                        color: plasmoid.configuration.borderColor
                        width: units.smallSpacing
                    }

                    anchors {
                        centerIn: preview
                    }
                }

                Image {
                    id: preview

                    asynchronous: true
                    fillMode: Image.PreserveAspectFit
                    source: modelData
                    smooth: true

                    anchors.fill: background
                    anchors.margins: units.smallSpacing
                }

                PlasmaComponents.ToolButton {

                    anchors {
                        top: parent.top
                        right: parent.right
                        margins: units.smallSpacing
                    }

                    iconSource: "list-remove"
                    tooltip: i18n("Remove image")
                    flat: false
                    onClicked: imageModel.removeImage( index )
                    opacity: imageDelegate.containsMouse ? 1 : 0

                    Behavior on opacity {
                        PropertyAnimation {
                            duration: units.longDuration
                            easing.type: Easing.OutQuad
                        }
                    }
                }

                Text {
                    color: "red"
                    text: index
                }

                onClicked: previewsGrid.currentIndex = index
            }

            Keys.onPressed: {
                if (count < 1) {
                    return;
                }

                if (event.key == Qt.Key_Home) {
                    currentIndex = 0;
                } else if (event.key == Qt.Key_End) {
                    currentIndex = count - 1;
                }
            }

            Keys.onLeftPressed: moveCurrentIndexLeft()
            Keys.onRightPressed: moveCurrentIndexRight()
            Keys.onUpPressed: moveCurrentIndexUp()
            Keys.onDownPressed: moveCurrentIndexDown()

            onCurrentIndexChanged: configImageForm.configurationChanged()

        }
    }

    RowLayout {

        width: parent.width

        Item {
            Layout.fillWidth: true
        }

        Button {
            iconName: "document-open-folder"
            text: i18n("Open...")

            FileDialog {
                id: fileDialog

                nameFilters: [ "Image files (*.jpg *.png)", "All files (*)" ]

                onAccepted: {
                    // remove "file://"
                    var path = fileUrl.toString().substring( 7 )

                    if( imageModel.addImage(path) ) {
                        configImageForm.configurationChanged()
                    }
                }
            }

            onClicked: fileDialog.open()
        }
    }
}
