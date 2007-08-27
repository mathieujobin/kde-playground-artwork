
find_package(Cokoon REQUIRED)
include_directories( ${COKOON_INCLUDE_DIR} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

########### next target ###############

set(kwin3_cokoon_PART_SRCS cokoondecoration.cpp )

cokoon_add_spec_files(kwin3_cokoon_PART_SRCS CokoonDecorationSpec.xml) 
#message(STATUS "decoration sources: ${kwin3_cokoon_PART_SRCS}")

kde4_add_plugin(kwin3_cokoon ${kwin3_cokoon_PART_SRCS})



target_link_libraries(kwin3_cokoon  ${KDE4_KDEUI_LIBS} cokoon kdecorations )

install(TARGETS kwin3_cokoon DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install(FILES CokoonDecorationTheme.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})

install(FILES CokoonDecorationSpec.xml DESTINATION ${DATA_INSTALL_DIR}/cokoon/specs )
install(FILES cokoon.desktop DESTINATION ${DATA_INSTALL_DIR}/kwin/  )

add_subdirectory( TestTheme )
