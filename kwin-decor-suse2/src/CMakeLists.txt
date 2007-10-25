add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -DHAVE_CONFIG_H=1)

add_subdirectory( config )

set(kwin3_SUSE2_PART_SRCS 
   SUSE2.cpp 
   SUSE2client.cpp 
   SUSE2button.cpp
   shadow.cpp
   misc.cpp )

kde4_add_plugin(kwin3_SUSE2 ${kwin3_SUSE2_PART_SRCS})

target_link_libraries(kwin3_SUSE2 kdecorations ${KDE4_KDEUI_LIBS} ${KDE4_KDECORE_LIBS} ${KDE4_KDEFX_LIBS} ${BLITZ_LIBRARIES})

install(TARGETS kwin3_SUSE2  DESTINATION ${PLUGIN_INSTALL_DIR} )

########### install files ###############

install( FILES SUSE2.desktop  DESTINATION  ${DATA_INSTALL_DIR}/kwin/ )
install( FILES SUSE2  DESTINATION  ${DATA_INSTALL_DIR}/color-schemes/ )
install( FILES titlebar_decor.png  DESTINATION  ${DATA_INSTALL_DIR}/kwin/pics/ )

