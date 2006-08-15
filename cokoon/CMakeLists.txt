
find_package(KDE4 REQUIRED)
include (KDE4Defaults)

set(CMAKE_VERBOSE_MAKEFILE ON)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

link_directories(${KDE4_LIB_DIR})

#include(ConvenienceLibs.cmake)
#include(ManualStuff.cmake)
#include(ConfigureChecks.cmake)

add_subdirectory( lib )
add_subdirectory( decoration )
add_subdirectory( style )




