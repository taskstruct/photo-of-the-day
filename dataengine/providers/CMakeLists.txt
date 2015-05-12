find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS CoreAddons)

########### National Geographic ############

set( natgeo_SRCS natgeo.cpp )

add_library(photooftheday_provider_natgeo MODULE ${natgeo_SRCS})

kcoreaddons_desktop_to_json(photooftheday_provider_natgeo photooftheday-provider-natgeo.desktop)

target_link_libraries(photooftheday_provider_natgeo photooftheday_providercore KF5::KIOCore KF5::I18n KF5::CoreAddons )

install(TARGETS photooftheday_provider_natgeo DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/dataengine/photooftheday )
install(FILES photooftheday-provider-natgeo.desktop DESTINATION ${SERVICES_INSTALL_DIR})


########### NASA Photo of the day ############

set( apod_SRCS apod.cpp )

add_library(photooftheday_provider_apod MODULE ${apod_SRCS})

kcoreaddons_desktop_to_json(photooftheday_provider_apod photooftheday-provider-apod.desktop)

target_link_libraries(photooftheday_provider_apod photooftheday_providercore KF5::KIOCore KF5::I18n KF5::CoreAddons )

install(TARGETS photooftheday_provider_apod DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/dataengine/photooftheday )
install(FILES photooftheday-provider-apod.desktop DESTINATION ${SERVICES_INSTALL_DIR})

########### Earth Shots ############

set( earthshots_SRCS earthshots.cpp )

add_library(photooftheday_provider_earthshots MODULE ${earthshots_SRCS})

kcoreaddons_desktop_to_json(photooftheday_provider_earthshots photooftheday-provider-earthshots.desktop)

target_link_libraries(photooftheday_provider_earthshots photooftheday_providercore KF5::KIOCore KF5::I18n KF5::CoreAddons )

install(TARGETS photooftheday_provider_earthshots DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/dataengine/photooftheday )
install(FILES photooftheday-provider-earthshots.desktop DESTINATION ${SERVICES_INSTALL_DIR})

########### Interfacelift ############

set( interfacelift_SRCS interfacelift.cpp )

add_library(photooftheday_provider_interfacelift MODULE ${interfacelift_SRCS})

kcoreaddons_desktop_to_json(photooftheday_provider_interfacelift photooftheday-provider-interfacelift.desktop)

target_link_libraries(photooftheday_provider_interfacelift photooftheday_providercore KF5::KIOCore KF5::I18n KF5::CoreAddons )

install(TARGETS photooftheday_provider_interfacelift DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/dataengine/photooftheday )
install(FILES photooftheday-provider-interfacelift.desktop DESTINATION ${SERVICES_INSTALL_DIR})

########### Steve's Digicams ############
set( stevesdigicams_SRCS stevesdigicams.cpp )

add_library(photooftheday_provider_stevesdigicams MODULE ${stevesdigicams_SRCS})

kcoreaddons_desktop_to_json(photooftheday_provider_stevesdigicams photooftheday-provider-stevesdigicams.desktop)

target_link_libraries(photooftheday_provider_stevesdigicams photooftheday_providercore KF5::KIOCore KF5::I18n KF5::CoreAddons )

install(TARGETS photooftheday_provider_stevesdigicams DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/dataengine/photooftheday )
install(FILES photooftheday-provider-stevesdigicams.desktop DESTINATION ${SERVICES_INSTALL_DIR})

########### Bonjour Madame ############
#if(ENABLE_BONJOUR_MADAME)
    set( bonjourmadame_SRCS bonjourmadame.cpp )

    add_library(photooftheday_provider_bonjourmadame MODULE ${bonjourmadame_SRCS})

    kcoreaddons_desktop_to_json(photooftheday_provider_bonjourmadame photooftheday-provider-bonjourmadame.desktop)

    target_link_libraries(photooftheday_provider_bonjourmadame photooftheday_providercore KF5::KIOCore KF5::I18n KF5::CoreAddons )

    install(TARGETS photooftheday_provider_bonjourmadame DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/dataengine/photooftheday )
    install(FILES photooftheday-provider-bonjourmadame.desktop DESTINATION ${SERVICES_INSTALL_DIR})
#endif()
########### Wikimedia ############
