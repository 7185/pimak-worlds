/**
 * Copyright (c) 2012, Thibault Signor <tibsou@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef OGREWINDOW_H
#define OGREWINDOW_H

#include <QtWidgets/QApplication>
#include <QtGui/QKeyEvent>
#include <QtGui/QWindow>

#include <Ogre.h>
#ifdef Q_WS_WIN
#include <Terrain/OgreTerrain.h>
#else
#include <OGRE/Terrain/OgreTerrain.h>
#endif

#include "OgreFrameListener.h"
#include "User.h"

class OgreWindow : public QWindow {
    Q_OBJECT

public:
    explicit OgreWindow(QWindow *parent = NULL);
    ~OgreWindow();

    virtual void render(QPainter *painter);
    virtual void render();
    virtual void initialize();
#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
    virtual void createCompositor();
#endif

    void setAnimating(bool animating);
    void setActiveCam(bool); //0: First, 1:Third

public slots:

    virtual void renderLater();
    virtual void renderNow();

    /*
    We use an event filter to be able to capture keyboard/mouse events. More on this later.
    */
    virtual bool eventFilter(QObject *target, QEvent *event);

 void createAvatar(User *u);
 void destroyAvatar(User *u);
 void moveAvatar(User *u);
 void posSend();

signals:
 void dispAverageFps(QString);
 void dispPosition(QString);
 void positionSend(float,float,float,float,float);

protected:
    /*
    Ogre3D pointers added here. Useful to have the pointers here for use by the window later.
    */
    Ogre::ColourValue ogreBackground;

    bool m_update_pending;
    bool m_animating;

    /*
    The below methods are what is actually fired when they keys on the keyboard are hit.
    Similar events are fired when the mouse is pressed or other events occur.
    */
    virtual void exposeEvent(QExposeEvent *event);
    virtual bool event(QEvent *event);

    /*
    Write log messages to Ogre log
    */
    void log(Ogre::String msg);
    void log(QString msg);

 private:
    void setupResources();
    void createCamera();
    void createViewport();
    void createTerrain();
    void createScene();
    void moveCamera();

    Ogre::Root                 *ogreRoot;
    Ogre::SceneManager         *ogreSceneMgr;
    Ogre::RenderWindow         *ogreRenderWindow;
    Ogre::Viewport             *ogreViewport;
    Ogre::Terrain              *ogreTerrain;
    Ogre::TerrainGlobalOptions *ogreTerrainGlobals;
    Ogre::Camera       *ogreFirstCamera;
    Ogre::Camera       *ogreThirdCamera; // 3rd view cam
    Ogre::Camera       *activeCamera;    // Current cam
    Ogre::SceneNode    *cameraNode;      // Camera node
    Ogre::SceneNode    *cameraPitchNode; // Separate pitch node
    Ogre::Entity       *avatar;
    OgreFrameListener  *ogreListener;

    int turbo;
    enum keys { UP=0, RIGHT, DOWN, LEFT, PGUP, PGDOWN, PLUS, MINUS, CTRL, SHIFT };
};

#endif // OGREWINDOW_H
