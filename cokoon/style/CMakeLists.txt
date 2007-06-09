
project(kstyle-cokoon)

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_SOURCE_DIR}/lib )

########### next target ###############

set(cokoon_PART_SRCS cokoonstyle.cpp )

kde4_automoc(${cokoon_PART_SRCS})

kde4_add_plugin(cokoonstyle ${cokoon_PART_SRCS})

target_link_libraries(cokoonstyle ${QT_LIBRARIES} ${KDE4_KDEFX_LIBS}  cokoon ) # ${KDE4_KDECORE_LIBS}


install(TARGETS cokoonstyle DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/styles/ )


########### install files ###############

install(FILES CokoonStyleSpec.xml DESTINATION ${DATA_INSTALL_DIR}/cokoon/specs )
install(FILES cokoonstyle.themerc DESTINATION ${DATA_INSTALL_DIR}/kstyle/themes  )
