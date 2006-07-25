
#include_directories( ${CMAKE_SOURCE_DIR}/lib ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )
include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kwin3_cokoon_PART_SRCS cokoondecoration.cpp )

kde4_automoc(${kwin3_cokoon_PART_SRCS})

kde4_add_plugin(kwin3_cokoon ${kwin3_cokoon_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kwin3_cokoon )

target_link_libraries(kwin3_cokoon  ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} cokoon kdecorations )

install(TARGETS kwin3_cokoon DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install(FILES cokoon.desktop DESTINATION ${DATA_INSTALL_DIR}/kwin/  )



#original Makefile.am contents follow:

#AUTOMAKE_OPTIONS = foreign
#
#SUBDIRS = 
#
#KDE_CXXFLAGS = -DQT_PLUGIN
#
#INCLUDES = -I$(srcdir)/../lib $(all_includes)
#
#kwindir = $(kde_datadir)/kwin/
#kwin_DATA = cokoon.desktop
#
#kde_module_LTLIBRARIES = kwin3_cokoon.la
#kwin3_cokoon_la_SOURCES = cokoondecoration.cpp
#kwin3_cokoon_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
#kwin3_cokoon_la_LIBADD = $(LIB_KDEUI) -lkdecorations  ../lib/libcokoon.la
#kwin3_cokoon_la_METASOURCES = AUTO
#
#DISTCLEANFILES = $(kwin3_cokoon_la_METASOURCES)
#
