#-------------------------------------------------
#
# Project created by QtCreator 2015-08-07T09:26:35
#
#-------------------------------------------------

QT       += core gui xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


INCLUDEPATH += /home/hamidreza/UntitledFolder/paria-1.0/src/libmaia-master/
LIBS += /home/hamidreza/UntitledFolder/paria-1.0/src/libmaia-master/libmaia.a

TARGET = paria
TEMPLATE = app


DESTDIR = ../bin
MOC_DIR = ../build/moc
RCC_DIR = ../build/rcc
UI_DIR = ../build/ui
unix:OBJECTS_DIR = ../build/o/unix
win32:OBJECTS_DIR = ../build/o/win32
macx:OBJECTS_DIR = ../build/o/mac


SOURCES += main.cpp\
        mainwindow.cpp \
    get_url.cpp \
    download_info.cpp \
    category.cpp \
    download_dialog.cpp \
    download_complete.cpp
SOURCES +=

HEADERS  += mainwindow.h \
    get_url.h \
    download_info.h \
    category.h \
    download_dialog.h \
    download_complete.h
HEADERS  +=

FORMS    += mainwindow.ui \
    get_url.ui \
    download_info.ui \
    category.ui \
    download_dialog.ui \
    download_complete.ui

OTHER_FILES += \
    icon/delete.jpg \
    icon/options.jpg \
    icon/add.png \
    icon/logo.png \
    icon/resume.png \
    icon/stop.png \
    icon/zip.png

RESOURCES += \
    resources.qrc
