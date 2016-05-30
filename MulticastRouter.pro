QT += core
QT -= gui

CONFIG += c++11

LIBS += -L/usr/lib64/mysql -lmysqlclient -lz -lcrypt -lnsl -lm

TARGET = MulticastRouter
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    streamprocessor.cpp \
    streammanager.cpp \
    dbhandler.cpp \
    commandserver.cpp

HEADERS += \
    streamprocessor.h \
    streammanager.h \
    dbhandler.h \
    commandserver.h

DISTFILES += \
    other/mcastrouter.service \
    other/mroute_gw.sql
