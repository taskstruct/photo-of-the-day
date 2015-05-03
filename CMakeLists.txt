cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

set(REQUIRED_QT_VERSION 5.8.0)
set(KF5_MIN_VERSION 5.8.0)

find_package(ECM 5.8.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(FeatureSummary)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED Core Gui)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS CoreAddons Service I18n )

add_subdirectory(dataengine)
add_subdirectory(applet)
