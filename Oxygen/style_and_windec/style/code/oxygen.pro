HEADERS = oxygen.h dynamicbrush.h \
          debug.h
SOURCES = oxygen.cpp stylehint.cpp \
          sizefromcontents.cpp qsubcmetrics.cpp \
          pixelmetric.cpp stdpix.cpp \
          drawcomplexcontrol.cpp drawcontrol.cpp \
          drawprimitive.cpp dynamicbrush.cpp
TEMPLATE = lib
PLUGIN = true
RESOURCES = oxygen.qrc
CONFIG += qt x11 plugin
QT += qt3support opengl
VERSION       = 0.1
