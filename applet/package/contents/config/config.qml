import QtQuick 2.0

import org.kde.plasma.configuration 2.0

ConfigModel {
    ConfigCategory {
        name: i18n("Appearance")
        icon: "preferences-desktop-color"
        source: "configGeneral.qml"
    }
    
    ConfigCategory {
        name: i18n("Content")
        icon: "preferences-desktop-desktop-wallpaper.svg"
        source: "configContent.qml"
    }
}
