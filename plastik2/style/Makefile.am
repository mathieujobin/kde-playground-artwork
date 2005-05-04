AUTOMAKE_OPTIONS = foreign

SUBDIRS = config

KDE_CXXFLAGS = -DQT_PLUGIN

INCLUDES = $(all_includes)

colorshemedir = $(kde_datadir)/kdisplay/color-schemes
colorsheme_DATA = Plastik2.kcsrc

stylercdir = $(kde_datadir)/kstyle/themes
stylerc_DATA = plastik2.themerc

EXTRA_DIST = $(stylerc_DATA) $(colorsheme_DATA)

noinst_HEADERS = plastik.h

kde_style_LTLIBRARIES = plastik2.la
plastik2_la_SOURCES = plastik.cpp
plastik2_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
plastik2_la_LIBADD = $(kde_libraries)/libkdefx.la
plastik2_la_METASOURCES = AUTO

DISTCLEANFILES = $(plastik2_la_METASOURCES)
