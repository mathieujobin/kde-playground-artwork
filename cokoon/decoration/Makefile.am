AUTOMAKE_OPTIONS = foreign

SUBDIRS = 

KDE_CXXFLAGS = -DQT_PLUGIN

INCLUDES = -I$(srcdir)/../lib $(all_includes)

kwindir = $(kde_datadir)/kwin/
kwin_DATA = cokoon.desktop

kde_module_LTLIBRARIES = kwin3_cokoon.la
kwin3_cokoon_la_SOURCES = cokoondecoration.cpp
kwin3_cokoon_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
kwin3_cokoon_la_LIBADD = $(LIB_KDEUI) -lkdecorations  ../lib/libcokoon.la
kwin3_cokoon_la_METASOURCES = AUTO

DISTCLEANFILES = $(kwin3_cokoon_la_METASOURCES)

