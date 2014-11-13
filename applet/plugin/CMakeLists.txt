project(photoofthedayplugin)

add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_org.task_struct.photooftheday\")


find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED Qml)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS CoreAddons Service )

set( photoofthedayplugin_SRCS photoofthedayplugin.cpp providersmodel.cpp )

add_library(photoofthedayplugin SHARED ${photoofthedayplugin_SRCS})

target_link_libraries(photoofthedayplugin Qt5::Core Qt5::Gui
                                          Qt5::Qml
                                          KF5::CoreAddons
                                          KF5::Service )

install(TARGETS photoofthedayplugin DESTINATION ${QML_INSTALL_DIR}/org/task_struct/private/photooftheday)

install(FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/task_struct/private/photooftheday)
