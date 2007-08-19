
project(cokoon-testdecoration)

install(FILES TestTheme.desktop DESTINATION ${SERVICES_INSTALL_DIR}/cokoon/decorations)
install(FILES TestTheme.xml
              windowmask.png
              testdecoration.svg
              testdecorationbutton.svg
              icon_above_restore.svg
              icon_above.svg
              icon_below_restore.svg
              icon_below.svg
              icon_close.svg
              icon_help.svg
              icon_maximize.svg
              icon_max_restore.svg
              icon_minimize.svg
              icon_oad_restore.svg
              icon_oad.svg
              icon_shade_restore.svg
              icon_shade.svg DESTINATION ${DATA_INSTALL_DIR}/cokoon/decorations/TestTheme)

