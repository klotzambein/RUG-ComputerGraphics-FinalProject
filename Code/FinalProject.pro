#-------------------------------------------------
#
# Project created by QtCreator 2017-01-24T12:26:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OpenGL_animation
TEMPLATE = app
CONFIG += c++14

SOURCES += main.cpp\
    mainwindow.cpp \
    mainview.cpp \
    user_input.cpp \
    utility.cpp \

HEADERS  += mainwindow.h \
    mainview.h \

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc
