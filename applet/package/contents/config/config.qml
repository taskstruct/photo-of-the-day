import QtQuick 2.0

import org.kde.plasma.configuration 2.0

ConfigModel {
    ConfigCategory {
        name: i18n("Appearance")
        icon: "preferences-desktop-color"
        source: "configGeneral.qml"
    }
    
    ConfigCategory {
        name: i18n("Providers")
        icon: "preferences-system-network.svgz"
        source: "configProviders.qml"
    }
}