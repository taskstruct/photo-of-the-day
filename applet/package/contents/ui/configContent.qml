import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

import org.kde.plasma.plasmoid 2.0

ColumnLayout {
    id: configContentForm
    width: parent.width

    signal configurationChanged

    //BEGIN functions
    function saveConfig() {
        plasmoid.configuration.contentType = providersBomboBox.model[providersBomboBox.currentIndex]['type']
        stackView.currentItem.saveConfig()
    }

//    function restoreConfig() {
//    }
    //END functions

    Component.onCompleted: {
        var initIndex = -1;

        for( var i = 0; i < providersBomboBox.model.length; ++i ) {

            if( providersBomboBox.model[i]['type'] === plasmoid.configuration.contentType ) {
                initIndex = i;
                break;
            }
        }

        console.debug("initIndex=", initIndex)

        providersBomboBox.currentIndex = initIndex
    }

    RowLayout {

        Layout.fillWidth: true

        Label {
            text: i18n("Select content type:")
        }

        ComboBox {
            id: providersBomboBox

            currentIndex: -1

            textRole: "label"

            model: [
                { 'label' : i18n("Image"),            'view' : "configImage.qml",     'type' : "image"     },
                { 'label' : i18n("Slideshow"),        'view' : "configSlideshow.qml", 'type' : "slideshow" },
                { 'label' : i18n("Photo of the day"), 'view' : "configProviders.qml", 'type' : "potd"      }
            ]

            Layout.fillWidth: true

            onCurrentIndexChanged: {
                console.debug("onCurrentIndexChanged ", currentIndex)
                if( -1 == currentIndex ) {
                    return
                }

                stackView.sourceFile = model[currentIndex]['view']
                configContentForm.configurationChanged()
            }
        }
    }

    Item {
        id: emptyConfig
    }

    StackView {
        id: stackView

        Layout.fillHeight: true
        anchors {
            left: parent.left;
            right: parent.right;
        }

        property string sourceFile

        onSourceFileChanged: {
            if (sourceFile != "") {
                replace(Qt.resolvedUrl(sourceFile))
            } else {
                replace(emptyConfig);
            }

            if (currentItem["configurationChanged"]) {
                currentItem["configurationChanged"].connect(configContentForm.configurationChanged)
            }
        }
    }
}
