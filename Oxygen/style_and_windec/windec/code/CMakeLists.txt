include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/workspace/kwin/lib  )


########### next target ###############

set(
     kwin_oxy_SRCS 
     oxygenclient.cpp 
   )

kde4_automoc(${kwin_oxy_SRCS})

kde4_add_plugin(kwin_oxy ${kwin_oxy_SRCS})


target_link_libraries(kwin_oxy ${KDE4_KDEFX_LIBS} kdecorations)

install(TARGETS kwin_oxy  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES oxygenclient.desktop  DESTINATION  ${DATA_INSTALL_DIR}/kwin/ )


