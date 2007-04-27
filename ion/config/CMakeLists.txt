set(kstyle_ion_config_PART_SRCS ionstyleconfig.cpp )
kde4_automoc(${kstyle_ion_config_PART_SRCS})
kde4_add_ui_files(kstyle_ion_config_PART_SRCS styledialog.ui )
kde4_add_plugin(kstyle_ion_config ${kstyle_ion_config_PART_SRCS})

target_link_libraries(kstyle_ion_config  ${KDE4_KDEUI_LIBS} )

install(TARGETS kstyle_ion_config  DESTINATION ${PLUGIN_INSTALL_DIR} )
