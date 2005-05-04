INCLUDES = $(all_includes)

noinst_HEADERS = plastikconf.h
kde_module_LTLIBRARIES = kstyle_plastik2_config.la
kstyle_plastik2_config_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
kstyle_plastik2_config_la_LIBADD  = $(LIB_KDEUI)
kstyle_plastik2_config_la_SOURCES = plastikconf.cpp
kstyle_plastik2_config_la_METASOURCES = AUTO


messages:
	$(XGETTEXT) *.cpp *.h -o $(podir)/kstyle_plastik2_config.pot
