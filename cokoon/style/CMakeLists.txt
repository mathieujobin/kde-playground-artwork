
project(kstyle-cokoon)

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_SOURCE_DIR}/lib )

########### next target ###############

set(cokoon_PART_SRCS cokoonstyle.cpp )


kde4_add_plugin(cokoonstyle ${cokoon_PART_SRCS})

target_link_libraries(cokoonstyle ${KDE4_KDEFX_LIBS} ${KDE4_KDECORE_LIBS} cokoon )


install(TARGETS cokoonstyle DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/styles/ )


########### install files ###############

install(FILES CokoonStyleTheme.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})

install(FILES CokoonStyleSpec.xml DESTINATION ${DATA_INSTALL_DIR}/cokoon/specs )
install(FILES CokoonStyleSpec.xml DESTINATION ${DATA_INSTALL_DIR}/cokoon/specs )
install(FILES cokoonstyle.themerc DESTINATION ${DATA_INSTALL_DIR}/kstyle/themes  )

add_subdirectory( TestTheme )
