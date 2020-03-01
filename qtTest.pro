QT       += core gui widgets

TARGET = qtTest
TEMPLATE = app

CONFIG += c++14
#CONFIG += debug
CONFIG += console
CONFIG += silent
$#QMAKE_CXXFLAGS += -Wno-unused-parameter

SOURCES += main.cpp \
        MainWindow.cpp

HEADERS += MainWindow.hpp
