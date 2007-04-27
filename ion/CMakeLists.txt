add_subdirectory( config )

set(ionstyle_PART_SRCS ionstyle.cpp pixmap.cpp)
kde4_automoc(${ionstyle_PART_SRCS})
kde4_add_plugin(ionstyle ${ionstyle_PART_SRCS})
target_link_libraries(ionstyle  ${KDE4_KDEFX_LIBS} )

install(TARGETS ionstyle  DESTINATION ${PLUGIN_INSTALL_DIR} )
install( FILES ion.themerc  DESTINATION  ${DATA_INSTALL_DIR}/kstyle/themes )
