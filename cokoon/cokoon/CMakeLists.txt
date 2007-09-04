#project(libcokoon)
#cmake_minimum_required(VERSION 2.4.0)
#find_package(Qt4 REQUIRED)

add_subdirectory( tests )
add_subdirectory( themeeditor )

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


# set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")
# message(STATUS "src dir: ${CMAKE_SOURCE_DIR}")
find_package(Cokoon REQUIRED)



########### next target ###############

set(cokoon_LIB_SRCS 
   document.cpp 
   expression.cpp 
   object.cpp 
   tile.cpp 
   tilelayout.cpp 
   tilesource.cpp 
   bridges.cpp )

#set(LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib")
#set(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include")

#qt4_automoc(${cokoon_LIB_SRCS})
kde4_add_library(cokoon SHARED ${cokoon_LIB_SRCS})

SET(QT_USE_QTXML TRUE)
INCLUDE(${QT_USE_FILE})

# target_link_libraries(cokoon  ${QT_QTSVG_LIBRARY} ${KDE4_KDECORE_LIBS} ${QT_LIBRARIES})
target_link_libraries(cokoon  ${QT_QTSVG_LIBRARY} ${QT_LIBRARIES})

set_target_properties(cokoon PROPERTIES VERSION 4.2.0 SOVERSION 4 )
install(TARGETS cokoon DESTINATION ${LIB_INSTALL_DIR} )

########### next target ###############

set(cokoonspec_SRCS 
   specification.cpp
   cokoonspec_compiler.cpp)

kde4_add_executable(cokoonspec_compiler ${cokoonspec_SRCS})

target_link_libraries(cokoonspec_compiler ${QT_QTSVG_LIBRARY} ${QT_LIBRARIES} )

########### install files ###############

install(TARGETS cokoonspec_compiler DESTINATION ${BIN_INSTALL_DIR} )

install(FILES  document.h bridges.h DESTINATION  ${INCLUDE_INSTALL_DIR}/cokoon )

# install cokoon cmake files..
set(cmakeModule_install_dir ${DATA_INSTALL_DIR}/cmake/modules )
install(FILES  cmake_modules/FindCokoon.cmake DESTINATION ${cmakeModule_install_dir}  )
