project(kss-slideshow)

find_package( KDE4 REQUIRED )
include( KDE4Defaults )
include( MacroLibrary )

add_definitions( ${QT_DEFINITIONS} ${KDE4_DEFINITIONS} )
include_directories( ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES} )

FIND_PATH( KSCREENSAVER_INCLUDE_DIR kscreensaver.h
  ${KDE4_INCLUDE_DIR}
)

set( kslideshow.kss_SRCS slideshow.cpp kslideshowsetup.cpp kslideshoweffect.cpp )

kde4_add_ui_files( kslideshow.kss_SRCS slideshowcfg.ui )

kde4_add_executable( kslideshow.kss ${kslideshow.kss_SRCS} )

target_link_libraries( kslideshow.kss ${KDE4_KIO_LIBS} m kscreensaver )

install( TARGETS kslideshow.kss  ${INSTALL_TARGETS_DEFAULT_ARGS} )

