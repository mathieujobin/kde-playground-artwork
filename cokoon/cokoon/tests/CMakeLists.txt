if(KDE4_BUILD_TESTS)

find_package(Cokoon REQUIRED)

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )

# don't use the shared library so we can access non-EXPORT-ed interfaces...
set(COKOONTEST_LIB_SRCS ../bridges.cpp ../expression.cpp ../document.cpp ../object.cpp ../tile.cpp ../tilesource.cpp ../tilelayout.cpp ../specification.cpp )

########### next target ###############

set(cokoontest_SRCS cokoontest.cpp ${COKOONTEST_LIB_SRCS})

cokoon_add_spec_files(cokoontest_SRCS CokoonTestSpec.xml)

QT4_ADD_RESOURCES(cokoontest_SRCS cokoontest-resources.qrc)

kde4_add_executable(cokoontest ${cokoontest_SRCS})

target_link_libraries(cokoontest  ${QT_QTSVG_LIBRARY} ${KDE4_KDECORE_LIBS} ${QT_QTTEST_LIBRARY} )


########### next target ###############
set(themewidget_SRCS themewidget.cpp ${COKOONTEST_LIB_SRCS})

cokoon_add_spec_files(themewidget_SRCS ThemeWidgetSpec.xml)

kde4_add_executable(themewidget ${themewidget_SRCS})

target_link_libraries(themewidget  ${QT_QTSVG_LIBRARY} ${KDE4_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

