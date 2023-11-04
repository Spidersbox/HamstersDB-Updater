TEMPLATE = app
TARGET = HamstersDB-Updater

QT += network

greaterThan(QT_MAJOR_VERSION,4): QT += widgets

CONFIG += c++17

# add no pie so you can click on your app to run in linux
QMAKE_LFLAGS += -no-pie

#LIBS += -Wl,-Bstatic

OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build

SOURCES = src/main.cpp src/mainwindow.cpp
HEADERS = src/mainwindow.h
RESOURCES = src/updater.qrc
FORMS = src/forms/mainwindow.ui


windows:DEFINES += WIN32
windows:RC_FILE = src/updater.rc

windows:LIBS += -static-libgcc 


!windows:!mac{
  DEFINES += LINUX
  LIBS += -lrt
}
QMAKE_CXXFLAGS += -msse2 -w
QMAKE_CFLAGS += -msse2

QMAKE_CXXFLAGS_WARN_ON = -fdiagnostics-show-option -Wall -Wextra -Wformat -Wformat-security -Wno-unused-parameter -Wstack-protector

macx:HEADERS += src/macdockiconhandler.h
macx:OBJECTIVE_SOURCES += src/macdockiconhandler.mm
macx:LIBS += -framework Foundation -framework ApplicationServices -framework AppKit
macx:DEFINES += MAC_OSX MSG_NOSIGNAL=0
macx:ICON = src/images/spb.icns
macx:TARGET = "HamstersDB-Updater"

