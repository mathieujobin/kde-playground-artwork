
find_package(KDE4 REQUIRED)
include (KDE4Defaults)

set(CMAKE_VERBOSE_MAKEFILE OFF)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

link_directories(${KDE4_LIB_DIR})

#include(ConvenienceLibs.cmake)
#include(ManualStuff.cmake)
#include(ConfigureChecks.cmake)

set(BOOTSTRAP_COKOON True)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cokoon/cmake_modules/")

add_subdirectory( cokoon )
add_subdirectory( decoration )
add_subdirectory( style )




