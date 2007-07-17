project(oxybgtest)

include_directories (${KDE4_INCLUDES} ${QT_INCLUDES})

kde4_add_executable(oxybgtest main.cpp)
target_link_libraries(oxybgtest ${KDE4_KDEUI_LIBS})
