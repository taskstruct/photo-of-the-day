find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS KIO I18n CoreAddons)

########### National Geographic ############

set( natgeo_SRCS natgeo.cpp )

kservice_desktop_to_json(photooftheday_provider_natgeo photooftheday-provider-natgeo.desktop)

add_library(photooftheday_provider_natgeo MODULE ${natgeo_SRCS})

target_link_libraries(photooftheday_provider_natgeo photooftheday_providercore KF5::KIOCore KF5::I18n KF5::CoreAddons )

install(TARGETS photooftheday_provider_natgeo DESTINATION ${PLUGIN_INSTALL_DIR} )
install(FILES photooftheday-provider-natgeo.desktop DESTINATION ${SERVICES_INSTALL_DIR})


########### NASA Photo of the day ############

set( apod_SRCS apod.cpp )

kservice_desktop_to_json(photooftheday_provider_apod photooftheday-provider-apod.desktop)

add_library(photooftheday_provider_apod MODULE ${apod_SRCS})

target_link_libraries(photooftheday_provider_apod photooftheday_providercore KF5::KIOCore KF5::I18n KF5::CoreAddons )

install(TARGETS photooftheday_provider_apod DESTINATION ${PLUGIN_INSTALL_DIR} )
install(FILES photooftheday-provider-apod.desktop DESTINATION ${SERVICES_INSTALL_DIR})