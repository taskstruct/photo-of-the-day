import QtQuick 2.4
import QtQuick.Layouts 1.1
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.plasmoid 2.0

PlasmaCore.FrameSvgItem {
    id: configView

    imagePath: "widgets/background"

    anchors.fill: parent

    PlasmaComponents.Button {
        text: i18n("Configure")
        iconName: "configure"

        anchors.centerIn: parent

        onClicked: plasmoid.action("configure").trigger()
    }
}
