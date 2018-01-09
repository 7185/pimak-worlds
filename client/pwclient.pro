#-------------------------------------------------
#
# Project created by QtCreator 2011-04-13T16:24:09
#
#-------------------------------------------------

QT       += network widgets x11extras opengl
CONFIG   += c++11

TARGET = pwclient
TEMPLATE = app

win32-* {

    isEmpty(OGRE_ROOT) {
        OGRE_ROOT = C:\OgreSDK
    }
    message("OGRE_ROOT for make is: "$$OGRE_ROOT)

    DEPENDPATH += .
    INCLUDEPATH += . include
    INCLUDEPATH += $$OGRE_ROOT\include\OGRE
    INCLUDEPATH += $$OGRE_ROOT\boost_1_44
    LIBS += -L$$OGRE_ROOT\lib\release
    LIBS += -L$$OGRE_ROOT\lib\bin\Release
    LIBS += -L$$OGRE_ROOT\lib\bin\Debug
    LIBS += -lOgreMain -lOgreTerrain -lOgreOverlay
}

linux-* {
    INCLUDEPATH += . include /usr/include/OGRE /usr/include/OGRE/Overlay
    LIBS += -lOgreMain -lOgreTerrain -lOgreOverlay -lboost_system

    isEmpty(INSTALL_PREFIX) {
        INSTALL_PREFIX = /usr/local
    }
    message("INSTALL_PREFIX for make install is: "$$INSTALL_PREFIX)
}

HEADERS += include/*.h
SOURCES += src/*.cpp
FORMS   += ui/*.ui
UI_DIR = include

RESOURCES += pwclient.qrc

TRANSLATIONS += lang/pwclient_fr.ts

linux-* {
    target.path = $$INSTALL_PREFIX/bin/
    desktop.files = pwclient.desktop
    desktop.path = $$INSTALL_PREFIX/share/applications/
    icon.files = img/pimakworlds.png
    icon.path = $$INSTALL_PREFIX/share/pixmaps/

    INSTALLS += target desktop icon
}
