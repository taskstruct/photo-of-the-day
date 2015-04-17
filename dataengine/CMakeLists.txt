project(plasma_engine_photooftheday)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS Plasma Service )

include(GenerateExportHeader)


########### data engine ############

set( plasma_engine_photooftheday_SRCS photooftheday.cpp potddatacontainer.cpp )

kcoreaddons_desktop_to_json(plasma_engine_photooftheday plasma-dataengine-photooftheday.desktop)

add_library(plasma_engine_photooftheday MODULE ${plasma_engine_photooftheday_SRCS})

generate_export_header(plasma_engine_photooftheday EXPORT_FILE_NAME "photooftheday_export.h" BASE_NAME "PHOTOOFTHEDAY")

target_link_libraries(plasma_engine_photooftheday photooftheday_providercore KF5::Plasma )

install(TARGETS plasma_engine_photooftheday DESTINATION ${PLUGIN_INSTALL_DIR}/plasma/dataengine )
install(FILES plasma-dataengine-photooftheday.desktop DESTINATION ${SERVICES_INSTALL_DIR})


########### provider core library ############
set(photooftheday_provider_core_SRCS
    providercore.cpp
    cachemanager.cpp
)

add_library( photooftheday_providercore SHARED ${photooftheday_provider_core_SRCS} )
target_link_libraries( photooftheday_providercore LINK_PUBLIC Qt5::Gui KF5::Plasma )
set_target_properties( photooftheday_providercore PROPERTIES VERSION 1.0.0 SOVERSION 1 )

install( TARGETS photooftheday_providercore ${INSTALL_TARGETS_DEFAULT_ARGS} )
install(FILES photooftheday-provider.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR} ) 

########### providers ############
add_subdirectory(providers)
