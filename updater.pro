TARGET = HamstersDB-Updater
RC_ICONS = images/favicon.ico

QT += core gui
QT += network
#CONFIG += console # for console output (debugging)

greaterThan(QT_MAJOR_VERSION,4): QT += widgets

CONFIG += c++17

# add no pie so you can click on your app to run in linux
!windows:QMAKE_LFLAGS += -no-pie

OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build

SOURCES = src/main.cpp       \
          src/mainwindow.cpp \
          src/version.cpp

HEADERS = src/mainwindow.h   \
          src/version.h      \
          src/clientversion.h

RESOURCES = src/updater.qrc
FORMS = src/forms/mainwindow.ui


windows:RC_FILE = src/updater.rc

windows:LIBS += -Wl,-Bstatic
windows:LIBS += -static-libgcc 

