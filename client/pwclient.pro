#-------------------------------------------------
#
# Project created by QtCreator 2011-04-13T16:24:09
#
#-------------------------------------------------

QT       += core gui network

TARGET = pwclient
TEMPLATE = app

INCLUDEPATH += . /usr/include/OGRE
LIBS += -lOgreMain -lOgreTerrain

SOURCES += main.cpp\
        MainWindow.cpp \
    OgreWidget.cpp \
    OgreFrameListener.cpp \
    SettingsWindow.cpp \
    AboutWindow.cpp \
    Connection.cpp \
    User.cpp

HEADERS  += MainWindow.h \
    OgreWidget.h \
    OgreFrameListener.h \
    SettingsWindow.h \
    AboutWindow.h \
    Connection.h \
    User.h

FORMS    += MainWindow.ui \
    SettingsWindow.ui \
    AboutWindow.ui

RESOURCES += \
    pwclient.qrc

TRANSLATIONS += pwclient_fr.ts
