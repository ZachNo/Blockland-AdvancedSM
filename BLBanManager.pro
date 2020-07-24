QT += core gui network widgets sql widgets

TARGET = BLBanManager
TEMPLATE = app


SOURCES += \
    src/about.cpp \
    src/addban.cpp \
    src/currentPlayers.cpp \
    src/database.cpp \
    src/log.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/mainwindow_addons.cpp \
    src/mainwindow_admin.cpp \
    src/mainwindow_banlist.cpp \
    src/mainwindow_colorset.cpp \
    src/mainwindow_flagged.cpp \
    src/mainwindow_music.cpp \
    src/mainwindow_players.cpp \
    src/mainwindow_prefs.cpp \
    src/mainwindow_serverlauncher.cpp \
    src/serverconnection.cpp

HEADERS  += \
    src/about.h \
    src/addban.h \
    src/currentPlayers.h \
    src/database.h \
    src/log.h \
    src/mainwindow.h \
    src/serverconnection.h \
    src/sleep.h

FORMS    += \
    resources/mainwindow.ui
RESOURCES += \
    images/images.qrc

win32:RC_ICONS += images/brickIcon.ico

win32: LIBS += -lQt5Quazip1

win32: LIBS += -lzlib
