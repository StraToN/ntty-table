#-------------------------------------------------
#
# Project created by QtCreator 2015-04-22T23:14:27
#
#-------------------------------------------------

QT       += core
QT       += core gui
QT       += network
QT       += widgets

#QT       -= gui

TARGET = KinectDetector2Unity
CONFIG   += console
CONFIG   += c++11
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    meanbgs.cpp \
    mykinect.cpp \
    qsocket.cpp

HEADERS += \
    meanbgs.h \
    mykinect.h \
    qsocket.h

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += opencv

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += libfreenect
