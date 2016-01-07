QT       += core gui network widgets sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BLBanManager
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    currentPlayers.cpp \
    log.cpp \
    database.cpp \
    serverconnection.cpp \
    about.cpp

HEADERS  += mainwindow.h \
    currentPlayers.h \
    log.h \
    database.h \
    serverconnection.h \
    about.h

FORMS    += mainwindow.ui
RESOURCES += images.qrc \
    images.qrc
