
project(kstyle-cokoon)

find_package(Cokoon REQUIRED)

include_directories( ${COKOON_INCLUDE_DIR} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

########### next target ###############

set(cokoon_PART_SRCS cokoonstyle.cpp )

cokoon_add_spec_files(cokoon_PART_SRCS CokoonStyleSpec.xml)

kde4_add_plugin(cokoonstyle ${cokoon_PART_SRCS})

target_link_libraries(cokoonstyle ${KDE4_KDEFX_LIBS} ${KDE4_KDECORE_LIBS} cokoon )


install(TARGETS cokoonstyle DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/styles/ )


########### install files ###############

install(FILES CokoonStyleTheme.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})

install(FILES CokoonStyleSpec.xml DESTINATION ${DATA_INSTALL_DIR}/cokoon/specs )
install(FILES CokoonStyleSpec.xml DESTINATION ${DATA_INSTALL_DIR}/cokoon/specs )
install(FILES cokoonstyle.themerc DESTINATION ${DATA_INSTALL_DIR}/kstyle/themes  )

add_subdirectory( TestTheme )
