AUTOMAKE_OPTIONS = foreign

SUBDIRS = 

KDE_CXXFLAGS = -DQT_PLUGIN

INCLUDES = $(all_includes)

noinst_HEADERS = plastik.h

kde_style_LTLIBRARIES = plastik.la
plastik_la_SOURCES = plastik.cpp
plastik_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
#plastik_la_LIBADD = ../../kdefx/libkdefx.la
plastik_la_LIBADD = -lkdefx
plastik_la_METASOURCES = AUTO

DISTCLEANFILES = $(plastik_la_METASOURCES)
