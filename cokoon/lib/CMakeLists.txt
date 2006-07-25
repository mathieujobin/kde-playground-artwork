
add_subdirectory( tests )

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(cokoon_LIB_SRCS 
   document.cpp 
   expression.cpp 
   object.cpp 
   tile.cpp 
   tilelayout.cpp 
   tilesource.cpp 
   bridges.cpp )

kde4_automoc(${cokoon_LIB_SRCS})

kde4_add_library(cokoon SHARED ${cokoon_LIB_SRCS})

SET(QT_USE_QTXML TRUE)
INCLUDE(${QT_USE_FILE})

target_link_libraries(cokoon  ${KDE4_KDECORE_LIBS} ${QT_LIBRARIES})

#set_target_properties(cokoon PROPERTIES VERSION 4.2.0 SOVERSION 4 )
install(TARGETS cokoon DESTINATION ${LIB_INSTALL_DIR} )


########### install files ###############

install(FILES  document.h bridges.h DESTINATION  ${INCLUDE_INSTALL_DIR}/cokoon )



#original Makefile.am contents follow:

#INCLUDES= $(all_includes)
#
#lib_LTLIBRARIES = libcokoon.la
#libcokoon_la_LDFLAGS = $(KDE_RPATH) $(KDE_MT_LDFLAGS) $(all_libraries) -no-undefined -version-info 6:0:2
#libcokoon_la_LIBADD = $(LIB_QT) $(LIB_QTXML) -lQtSvg_debug $(LIB_QTSVG)
## libcokoon_la_NMCHECK = $(srcdir)/libkdefx.nmcheck
## libcokoon_la_NMCHECKWEAK = $(srcdir)/libkdefx_weak.nmcheck $(top_srcdir)/kdecore/libqt-mt_weak.nmcheck \
##         $(top_srcdir)/kdecore/standard_weak.nmcheck
#
#cokoonincludedir = $(includedir)/cokoon
#cokooninclude_HEADERS = document.h bridges.h
#
#libcokoon_la_SOURCES = document.cpp expression.cpp object.cpp tile.cpp tilelayout.cpp tilesource.cpp bridges.cpp
#
#METASOURCES = AUTO
#
#include $(top_srcdir)/admin/Doxyfile.am
#
#SUBDIRS=tests
