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

isEmpty(INSTALL_PREFIX) {
    INSTALL_PREFIX = /usr/local
}
message("INSTALL_PREFIX for make install is: "$$INSTALL_PREFIX)

SOURCES += main.cpp\
        MainWindow.cpp \
    OgreWidget.cpp \
    OgreFrameListener.cpp \
    SettingsWindow.cpp \
    AboutWindow.cpp \
    Connection.cpp \
    User.cpp \
    MovableText.cpp

HEADERS  += Protocol.h \
    MainWindow.h \
    OgreWidget.h \
    OgreFrameListener.h \
    SettingsWindow.h \
    AboutWindow.h \
    Connection.h \
    User.h \
    MovableText.h

FORMS    += MainWindow.ui \
    SettingsWindow.ui \
    AboutWindow.ui

RESOURCES += \
    pwclient.qrc

TRANSLATIONS += pwclient_fr.ts

target.path = $$INSTALL_PREFIX/bin/
desktop.files = pwclient.desktop
desktop.path = $$INSTALL_PREFIX/share/applications/
icon.files = img/pimakworlds.png
icon.path = $$INSTALL_PREFIX/share/pixmaps/

INSTALLS += target desktop icon
