INCLUDES = $(all_includes)

AM_LDFLAGS = $(QT_LDFLAGS) $(X_LDFLAGS) $(KDE_RPATH)

check_PROGRAMS = cokoontest themewidget

TESTS = cokoontest

METASOURCES = AUTO

LDADD = ../libcokoon.la $(LIB_KDEUI) $(LIB_KDECORE) -lQtTest_debug

cokoontest_SOURCES = cokoontest.cpp

themewidget_SOURCES = themewidget.cpp

