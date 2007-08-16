
#include_directories( ${CMAKE_SOURCE_DIR}/lib ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )
include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_SOURCE_DIR}/lib )


########### next target ###############

set(kwin3_cokoon_PART_SRCS cokoondecoration.cpp )


kde4_add_plugin(kwin3_cokoon ${kwin3_cokoon_PART_SRCS})



target_link_libraries(kwin3_cokoon  ${KDE4_KDEUI_LIBS} cokoon kdecorations )

install(TARGETS kwin3_cokoon DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install(FILES CokoonDecorationTheme.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})

install(FILES CokoonDecorationSpec.xml DESTINATION ${DATA_INSTALL_DIR}/cokoon/specs )
install(FILES cokoon.desktop DESTINATION ${DATA_INSTALL_DIR}/kwin/  )

