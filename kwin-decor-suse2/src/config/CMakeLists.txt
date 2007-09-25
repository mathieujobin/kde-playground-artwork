set(kwin_SUSE2_config_PART_SRCS config.cpp )

kde4_add_ui_files(kwin_SUSE2_config_PART_SRCS configdialog.ui )

kde4_add_plugin(kwin_SUSE2_config ${kwin_SUSE2_config_PART_SRCS})

target_link_libraries(kwin_SUSE2_config  ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} ${QT_QTGUI_LIBRARY})

install(TARGETS kwin_SUSE2_config  DESTINATION ${PLUGIN_INSTALL_DIR} )

