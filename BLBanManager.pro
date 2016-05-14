QT       += core gui network widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BLBanManager
TEMPLATE = app

LIBS += quazip.lib


SOURCES += main.cpp\
        mainwindow.cpp \
    log.cpp \
    serverconnection.cpp \
    about.cpp \
    mainwindow_addons.cpp \
    mainwindow_serverlauncher.cpp \
    mainwindow_colorset.cpp \
    mainwindow_banlist.cpp \
    mainwindow_players.cpp \
    mainwindow_flagged.cpp \
    mainwindow_music.cpp \
    mainwindow_prefs.cpp \
    mainwindow_admin.cpp \
    addban.cpp

HEADERS  += mainwindow.h \
    log.h \
    serverconnection.h \
    about.h \
    addban.h

FORMS    += mainwindow.ui
RESOURCES += images.qrc \
    images.qrc

win32:RC_ICONS += brickIcon.ico
