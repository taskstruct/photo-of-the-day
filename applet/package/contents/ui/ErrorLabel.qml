import QtQuick 2.0
import QtQuick.Layouts 1.1
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents

PlasmaCore.FrameSvgItem {
    id: errorItem

    property alias errorText: label.text

    width: anchors.centerIn.width * 0.5
    height: width * 0.5
    imagePath: "widgets/background"
    visible: false

    RowLayout {
        id: layout

        anchors.fill: parent
        anchors.margins: units.gridUnit

        PlasmaCore.IconItem {
            source: "emblem-important"

            implicitWidth: units.iconSizes.large
            implicitHeight: units.iconSizes.large

            Layout.minimumWidth: implicitWidth
        }

        PlasmaComponents.Label {
            id: label

            wrapMode: Text.WordWrap

            Layout.fillWidth: true
        }
    }

    Timer {
        id: showTimer
        interval: 10000

        onTriggered: errorItem.visible = false
    }

    onErrorTextChanged: {
        errorItem.visible = true;
        showTimer.restart()
    }
}

