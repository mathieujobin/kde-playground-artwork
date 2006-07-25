
project(kstyle-cokoon)

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )

########### next target ###############

set(cokoon_PART_SRCS cokoonstyle.cpp )

kde4_automoc(${cokoon_PART_SRCS})

kde4_add_plugin(cokoonstyle ${cokoon_PART_SRCS})

target_link_libraries(cokoonstyle ${QT_LIBRARIES} kdefx cokoon ) # ${KDE4_KDECORE_LIBS}

kde4_install_libtool_file(${PLUGIN_INSTALL_DIR}/plugins/styles cokoonstyle)
install_targets(${PLUGIN_INSTALL_DIR}/plugins/styles/ cokoonstyle )


########### install files ###############

install_files(FILES cokoonstyle.themerc DESTINATION ${DATA_INSTALL_DIR}/kstyle/themes  )
