project(photooftheday)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Quick QuickWidgets)

set(KF5_MIN_VERSION "5.3.0")

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS Plasma PlasmaQuick I18n NotifyConfig)

plasma_install_package(package org.task_struct.photooftheday)

add_subdirectory(plugin)