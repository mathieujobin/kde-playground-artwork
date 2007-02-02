
add_subdirectory( tests )

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(cokoon_LIB_SRCS 
   document.cpp 
   expression.cpp 
   object.cpp 
   tile.cpp 
   tilelayout.cpp 
   tilesource.cpp 
   bridges.cpp )

kde4_automoc(${cokoon_LIB_SRCS})

kde4_add_library(cokoon SHARED ${cokoon_LIB_SRCS})

SET(QT_USE_QTXML TRUE)
INCLUDE(${QT_USE_FILE})

target_link_libraries(cokoon  ${KDE4_KDECORE_LIBS} ${QT_LIBRARIES})

set_target_properties(cokoon PROPERTIES VERSION 4.2.0 SOVERSION 4 )
install(TARGETS cokoon DESTINATION ${LIB_INSTALL_DIR} )


########### install files ###############

install(FILES  document.h bridges.h DESTINATION  ${INCLUDE_INSTALL_DIR}/cokoon )
