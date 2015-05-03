project(photooftheday)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Quick QuickWidgets)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS Plasma PlasmaQuick NotifyConfig)

plasma_install_package(package org.task_struct.photooftheday)

add_subdirectory(plugin)
