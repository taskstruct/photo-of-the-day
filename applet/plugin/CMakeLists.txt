project(photoofthedayplugin)

add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_org.task_struct.photooftheday\")


find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED Qml)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS CoreAddons Service )

set( photoofthedayplugin_SRCS photoofthedayplugin.cpp
                              providersmodel.cpp
                              slideshowmodel.cpp
                              imagemodel.cpp
                              shadowimageprovider.cpp )

add_library(photoofthedayplugin SHARED ${photoofthedayplugin_SRCS})

target_link_libraries(photoofthedayplugin Qt5::Core Qt5::Gui
                                          Qt5::Qml Qt5::Quick
                                          KF5::CoreAddons
                                          KF5::Service )

message( STATUS "Plugin install dir is ${KDE_INSTALL_QMLDIR}" )

install(TARGETS photoofthedayplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/task_struct/private/photooftheday)

install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/task_struct/private/photooftheday)
