if(KDE4_BUILD_TESTS)

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )

# don't use the shared library so we can access non-EXPORT-ed interfaces...
set(COKOONTEST_LIB_SRCS ../bridges.cpp ../expression.cpp ../document.cpp ../object.cpp ../tile.cpp ../tilesource.cpp ../tilelayout.cpp)

########### next target ###############

set(cokoontest_SRCS cokoontest.cpp ${COKOONTEST_LIB_SRCS})

kde4_automoc(${cokoontest_SRCS})


kde4_add_executable(cokoontest ${cokoontest_SRCS})

target_link_libraries(cokoontest  ${KDE4_KDECORE_LIBS} ${QT_QTTEST_LIBRARY} )


########### next target ###############
set(themewidget_SRCS themewidget.cpp ${COKOONTEST_LIB_SRCS})

kde4_automoc(${themewidget_SRCS})


kde4_add_executable(themewidget ${themewidget_SRCS})

target_link_libraries(themewidget  ${KDE4_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

