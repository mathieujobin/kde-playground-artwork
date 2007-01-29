project(kstyle-oxygen)

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_SOURCE_DIR}/kdefx ${KDE4_KDECORE_INCLUDES} )

########### next target ###############

set( oxygen_SRCS
     oxygen.cpp
     tileset.cpp
     stylehint.cpp
     sizefromcontents.cpp
     qsubcmetrics.cpp
     pixelmetric.cpp
     stdpix.cpp
     drawcomplexcontrol.cpp
     drawcontrol.cpp
     drawprimitive.cpp
     dynamicbrush.cpp
     oxrender.cpp
   )

qt4_add_resources(oxygen.qrc oxygen.qrc)

kde4_automoc( ${oxygen_SRCS} )
kde4_add_plugin(kstyle-oxygen ${oxygen_SRCS} )

# TODO: check if too much definitions.
target_link_libraries( kstyle-oxygen ${KDE4_KDECORE_LIBS} Xtst kdefx Xrender Xfixes Xcursor Xext X11 Xrandr Xfixes Xcursor Xinerama freetype fontconfig Qt3Support QtOpenGL png GLU GL)

install( TARGETS kstyle-oxygen DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/styles/ )

########### install files ###############

install( FILES oxygen.themerc DESTINATION  ${DATA_INSTALL_DIR}/kstyle/themes )

