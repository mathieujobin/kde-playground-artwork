
include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )

# don't use the shared library so we can access non-EXPORT-ed interfaces...
set(COKOONTEST_LIB_SRCS ../bridges.cpp ../expression.cpp ../document.cpp ../object.cpp ../tile.cpp ../tilesource.cpp ../tilelayout.cpp)

########### next target ###############

set(cokoontest_SRCS cokoontest.cpp ${COKOONTEST_LIB_SRCS})

kde4_automoc(${cokoontest_SRCS})

if(KDE4_BUILD_TESTS)

kde4_add_executable(cokoontest ${cokoontest_SRCS})

target_link_libraries(cokoontest  ${KDE4_KDECORE_LIBS} QtTest )

endif(KDE4_BUILD_TESTS)

########### next target ###############

set(themewidget_SRCS themewidget.cpp ${COKOONTEST_LIB_SRCS})

kde4_automoc(${themewidget_SRCS})

if(KDE4_BUILD_TESTS)

kde4_add_executable(themewidget ${themewidget_SRCS})

target_link_libraries(themewidget  ${KDE4_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)



#original Makefile.am contents follow:

#INCLUDES = $(all_includes)
#
#AM_LDFLAGS = $(QT_LDFLAGS) $(X_LDFLAGS) $(KDE_RPATH)
#
#check_PROGRAMS = cokoontest themewidget
#
#TESTS = cokoontest
#
#METASOURCES = AUTO
#
#LDADD = ../libcokoon.la $(LIB_KDEUI) $(LIB_KDECORE) -lQtTest_debug
#
#cokoontest_SOURCES = cokoontest.cpp
#
#themewidget_SOURCES = themewidget.cpp
#
