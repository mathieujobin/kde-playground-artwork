include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )

# set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")
# message(STATUS "src dir: ${CMAKE_SOURCE_DIR}")
find_package(Cokoon REQUIRED)


########### next target ###############

set(themeeditor_SRCS 
   ../specification.cpp
   documentspecificationdocument.cpp
   specificationstateselector.cpp
   previewvariableeditor.cpp
   previewwidget.cpp
   thememodel.cpp
   themeitemview.cpp
   themeattributeeditor.cpp
   mainwindow.cpp
   main.cpp)

set(themeeditor_UIS
        specificationstateselector.ui
        specificationstateselectorstate.ui
        previewvariableeditor.ui
        themeitemview.ui
        mainwindowcentralwidget.ui
        edit_comment.ui edit_layout.ui edit_source.ui edit_tile.ui
        edit_expression.ui edit_object.ui edit_theme.ui)

# create UI header files...
QT4_WRAP_UI(themeeditor_SRCS ${themeeditor_UIS})

# this is where the wrapper ui_*.h's will generated
include_directories(${CMAKE_CURRENT_BINARY_DIR})

kde4_add_executable(cokoonthemeeditor ${themeeditor_SRCS})

target_link_libraries(cokoonthemeeditor cokoon ${KDE4_KDEUI_LIBS} ${QT_QTSVG_LIBRARY} ${QT_LIBRARIES} )

########### install files ###############

install( FILES CokoonThemeEditorui.rc DESTINATION  ${DATA_INSTALL_DIR}/cokoonthemeeditor )
