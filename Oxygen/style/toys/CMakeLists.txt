project(oxytoys)

include_directories (${KDE4_INCLUDES} ${QT_INCLUDES})

# bgtest
kde4_add_executable(oxybgtest bgtest.cpp)
target_link_libraries(oxybgtest ${KDE4_KDEUI_LIBS})

# buttontest
kde4_add_executable(oxybuttontest buttontest.cpp tileset.cpp)
target_link_libraries(oxybuttontest ${KDE4_KDEUI_LIBS})

# scrollbartest
kde4_add_executable(oxyscrollbartest scrollbartest.cpp tileset.cpp)
target_link_libraries(oxyscrollbartest ${KDE4_KDEUI_LIBS})

# scrollbardemo
set(scrollbardemo_SRCS scrollbardemo.cpp)
kde4_add_ui_files(scrollbardemo_SRCS scrollbar.ui)
kde4_add_executable(scrollbardemo ${scrollbardemo_SRCS})
target_link_libraries(scrollbardemo ${KDE4_KDEUI_LIBS})
