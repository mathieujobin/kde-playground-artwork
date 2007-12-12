project(oxygen-toys)

# search packages used by KDE
find_package(KDE4 REQUIRED)
include(KDE4Defaults)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories (${KDE4_INCLUDES} ${QT_INCLUDES})

# bgtest
kde4_add_executable(oxybgtest bgtest.cpp)
target_link_libraries(oxybgtest ${KDE4_KDEUI_LIBS})

# slabtest
kde4_add_executable(oxyslabtest slabtest.cpp tileset.cpp)
target_link_libraries(oxyslabtest ${KDE4_KDEUI_LIBS})

# scrollbartest
kde4_add_executable(oxyscrollbartest scrollbartest.cpp tileset.cpp)
target_link_libraries(oxyscrollbartest ${KDE4_KDEUI_LIBS})

# scrollbardemo
set(scrollbardemo_SRCS scrollbardemo.cpp)
kde4_add_ui_files(scrollbardemo_SRCS scrollbar.ui)
kde4_add_executable(scrollbardemo ${scrollbardemo_SRCS})
target_link_libraries(scrollbardemo ${KDE4_KDEUI_LIBS})

# uidemo
set(uidemo_SRCS uidemo.cpp)
kde4_add_ui_files(uidemo_SRCS uidemo.ui)
kde4_add_executable(uidemo ${uidemo_SRCS})
target_link_libraries(uidemo ${KDE4_KDEUI_LIBS})
