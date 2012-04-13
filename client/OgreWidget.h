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

#ifndef OGREWIDGET_H
#define OGREWIDGET_H

#include <QtGui>
#include <Ogre.h>
#include <OGRE/Terrain/OgreTerrain.h>
#include <OgreFont.h>
#include <OgreFontManager.h>
#include "MovableText.h"
#include "OgreFrameListener.h"
#include "User.h"

class OgreWidget : public QWidget
{
    Q_OBJECT
    
public:
    OgreWidget(QWidget *parent=0);
    ~OgreWidget();
    void setActiveCam(bool); //0: First, 1:Third

public slots:
     void createAvatar(User *u);
     void destroyAvatar(User *u);
     void moveAvatar(User *u);
     void posSend();

signals:
    void dispAverageFps(QString);
    void dispPosition(QString);
    void positionSend(float,float,float,float,float);

protected:
    virtual void moveEvent(QMoveEvent *e);
    virtual void paintEvent(QPaintEvent *e);
    virtual void resizeEvent(QResizeEvent *e);
    virtual void showEvent(QShowEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);
    virtual QPaintEngine* paintEngine() const;
    
private:
    void initOgreSystem();
    void setupNLoadResources();
    void createCamera();
    void createViewport();
    void createTerrain();
    void createScene();
    void moveCamera();

private:
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

#endif // OGREWIDGET_H
