QT += core
QT -= gui

CONFIG += c++11

TARGET = QtJsonModifyValue
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    json.cpp

HEADERS += \
    json.h
