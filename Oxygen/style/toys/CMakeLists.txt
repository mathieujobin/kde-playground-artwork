project(oxytoys)

include_directories (${KDE4_INCLUDES} ${QT_INCLUDES})

# bgtest
kde4_add_executable(oxybgtest bgtest.cpp)
target_link_libraries(oxybgtest ${KDE4_KDEUI_LIBS})

# buttontest
kde4_add_executable(oxybuttontest buttontest.cpp tileset.cpp)
target_link_libraries(oxybuttontest ${KDE4_KDEUI_LIBS})
