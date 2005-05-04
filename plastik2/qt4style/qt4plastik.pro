TEMPLATE= lib
SOURCES = qt4plastik.cpp
HEADERS = qt4plastik.h
CONFIG += qt warn_on rtti plugin
TARGET = plastikstyle

target.path = ${QTDIR}/plugins/styles/
INSTALLS += target
