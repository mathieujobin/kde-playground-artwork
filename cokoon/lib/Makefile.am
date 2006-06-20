INCLUDES= $(all_includes)

lib_LTLIBRARIES = libcokoon.la
libcokoon_la_LDFLAGS = $(KDE_RPATH) $(KDE_MT_LDFLAGS) $(all_libraries) -no-undefined -version-info 6:0:2
libcokoon_la_LIBADD = $(LIB_QT) $(LIB_QTXML) -lQtSvg_debug $(LIB_QTSVG)
# libcokoon_la_NMCHECK = $(srcdir)/libkdefx.nmcheck
# libcokoon_la_NMCHECKWEAK = $(srcdir)/libkdefx_weak.nmcheck $(top_srcdir)/kdecore/libqt-mt_weak.nmcheck \
#         $(top_srcdir)/kdecore/standard_weak.nmcheck

cokoonincludedir = $(includedir)/cokoon
cokooninclude_HEADERS = document.h bridges.h

libcokoon_la_SOURCES = document.cpp expression.cpp object.cpp tile.cpp tilelayout.cpp tilesource.cpp bridges.cpp

METASOURCES = AUTO

include $(top_srcdir)/admin/Doxyfile.am

SUBDIRS=tests
