QT += core network
QT -= gui

CONFIG += c++11

TARGET = vkautoreply
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    lo/arma_logger.cpp \
    lo/waitforsignalhelper.cpp \
    lo/vkapi.cpp \
    lo/languageprocessing.cpp \
    lo/vkautoreplyer.cpp

HEADERS += \
    lo/arma_logger.h \
    lo/waitforsignalhelper.h \
    lo/vkapi.h \
    lo/languageprocessing.h \
    lo/vkautoreplyer.h
