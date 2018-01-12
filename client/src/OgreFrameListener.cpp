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

#include <QVector>
#include <OgreMath.h>

#include "OgreFrameListener.h"

OgreFrameListener::OgreFrameListener(Ogre::Entity* e)
{
    ent = e;
    running = false;
    
    aniStateTop = ent->getAnimationState("IdleTop");
    aniStateTop->setEnabled(true);
    aniStateTop->setLoop(true);
    
    aniStateBase = ent->getAnimationState("IdleBase");
    aniStateBase->setEnabled(true);
    aniStateBase->setLoop(true);
    
    ogreControls = new bool[9];
    for (int i=0;i<=9;i++) { ogreControls[i] = false; }

    movingAvatars = new QMap<quint16, MovingAvatar>;
    
}

OgreFrameListener::~OgreFrameListener()
{
    delete [] ogreControls;
}

bool OgreFrameListener::frameStarted(const Ogre::FrameEvent &evt)
{
    if (ogreControls[UP] || ogreControls[DOWN] ||
        (ogreControls[SHIFT] && (ogreControls[LEFT] || ogreControls[RIGHT]))) {
        if (!running) {
            running = true;
            aniStateTop->setEnabled(false);
            aniStateBase->setEnabled(false);
            aniStateTop = ent->getAnimationState("RunTop");
            aniStateBase = ent->getAnimationState("RunBase");
            aniStateTop->setEnabled(true);
            aniStateBase->setEnabled(true);
        }
    } else {
        if (running) {
            running = false;
            aniStateTop->setEnabled(false);
            aniStateBase->setEnabled(false);
            aniStateTop = ent->getAnimationState("IdleTop");
            aniStateBase = ent->getAnimationState("IdleBase");
            aniStateTop->setEnabled(true);
            aniStateBase->setEnabled(true);
        }
    }
    aniStateTop->addTime(evt.timeSinceLastFrame);
    aniStateBase->addTime(evt.timeSinceLastFrame);

    QList<quint16> keys = (*movingAvatars).keys();
    QList<quint16> removeList;
    
    for (const quint16 &key: keys) {
    
        MovingAvatar &ma = (*movingAvatars)[key];
        Ogre::SceneNode *node = ma.node;

        if (ma.completion >= 1.0){
            ma.completion = 1.0;
            removeList.append(key);
        }

        float deltaX = ma.x - ma.oldX;
        float deltaY = ma.y - ma.oldY;
        float deltaZ = ma.z - ma.oldZ;
        float deltaYaw = ma.yaw - ma.oldYaw;
        float deltaPitch = ma.pitch - ma.oldPitch;
        
        node->setPosition(ma.oldX + deltaX * ma.completion,
                          ma.oldY + deltaY * ma.completion,
                          ma.oldZ + deltaZ * ma.completion);
        node->setOrientation(Ogre::Quaternion());
        node->yaw(Ogre::Degree(180.0f));
        
        
        if (deltaYaw > Ogre::Math::PI)
          deltaYaw -= Ogre::Math::TWO_PI;
        else if (deltaYaw <  -Ogre::Math::PI )
          deltaYaw += Ogre::Math::TWO_PI;
          
        node->yaw(Ogre::Radian(ma.oldYaw + deltaYaw * ma.completion));

        if (deltaPitch > Ogre::Math::PI)
          deltaPitch -= Ogre::Math::TWO_PI;
        else if (deltaPitch <  -Ogre::Math::PI )
          deltaPitch += Ogre::Math::TWO_PI;
        
        node->pitch(Ogre::Radian(ma.oldPitch + deltaPitch * ma.completion));
        
        ma.completion += evt.timeSinceLastFrame*5; // we assume one update every 200ms

        Ogre::AnimationState *aniStateOldTop, *aniStateOldBase, *aniStateNewTop, *aniStateNewBase;

        try {
        
          if(deltaX*deltaX+deltaY*deltaY+deltaZ*deltaZ > 0.0001) {

            aniStateOldTop = ma.avatar->getAnimationState("IdleTop");
            aniStateOldBase = ma.avatar->getAnimationState("IdleBase");
            aniStateNewTop = ma.avatar->getAnimationState("RunTop");
            aniStateNewBase = ma.avatar->getAnimationState("RunBase");
        
          } else {

            aniStateOldTop = ma.avatar->getAnimationState("RunTop");
            aniStateOldBase = ma.avatar->getAnimationState("RunBase");
            aniStateNewTop = ma.avatar->getAnimationState("IdleTop");
            aniStateNewBase = ma.avatar->getAnimationState("IdleBase");

          }

          aniStateOldTop->setEnabled(false);
          aniStateOldBase->setEnabled(false);
          aniStateNewTop->setEnabled(true);
          aniStateNewTop->setLoop(true);
          aniStateNewBase->setEnabled(true);
          aniStateNewBase->setLoop(true);
        
          aniStateNewTop->addTime(evt.timeSinceLastFrame);
          aniStateNewBase->addTime(evt.timeSinceLastFrame);

        } catch (Ogre::ItemIdentityException iie) {  }
        
    }

    for (const quint16 &key: removeList)
        movingAvatars->remove(key);
    
    removeList.clear();
    
    return true;
}
bool OgreFrameListener::frameEnded(const Ogre::FrameEvent &evt)
{
    return true;
}
bool OgreFrameListener::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
    return true;
}

bool OgreFrameListener::addMovingAvatar(quint16 id,
                                        Ogre::Entity *avatar,
                                        Ogre::SceneNode *node,
                                        float x, float y, float z,
                                        float oldX, float oldY, float oldZ,
                                        float pitch, float yaw,
                                        float oldPitch, float oldYaw) {

    if (movingAvatars->contains(id)) {

      MovingAvatar ma = (*movingAvatars)[id];
      ma.oldX = ma.oldX + (ma.x - ma.oldX) * ma.completion;
      ma.oldY = ma.oldY + (ma.y - ma.oldY) * ma.completion;
      ma.oldZ = ma.oldZ + (ma.z - ma.oldZ) * ma.completion;
      ma.x = x;
      ma.y = y;
      ma.z = z;
      ma.pitch = pitch;
      ma.yaw = yaw;
      ma.oldPitch = oldPitch;
      ma.oldYaw = oldYaw;
      ma.completion = 0.0;
      movingAvatars->insert(id, ma);
      
    }else{

        movingAvatars->insert(id,
                              (MovingAvatar){
                                  avatar,
                                  node,
                                  x, y, z,
                                  oldX, oldY, oldZ,
                                  pitch, yaw,
                                  oldPitch, oldYaw,
                                  0.0
                              });

        Ogre::AnimationState* aniStateBase = avatar->getAnimationState("IdleTop");
        Ogre::AnimationState* aniStateTop = avatar->getAnimationState("IdleBase");

        aniStateTop->setEnabled(true);
        aniStateTop->setLoop(true);
    
        aniStateBase->setEnabled(true);
        aniStateBase->setLoop(true);
      
    }

    return true;
  
}

void OgreFrameListener::handleKeys(int key, bool state)
{
    switch(key) {
    case Qt::Key_Up:
        ogreControls[UP] = state;
        break;
    case Qt::Key_Right:
        ogreControls[RIGHT] = state;
        break;
    case Qt::Key_Down:
        ogreControls[DOWN] = state;
        break;
    case Qt::Key_Left:
        ogreControls[LEFT] = state;
        break;
    case Qt::Key_PageUp:
        ogreControls[PGUP] = state;
        break;
    case Qt::Key_PageDown:
        ogreControls[PGDOWN] = state;
        break;
    case Qt::Key_Plus:
        ogreControls[PLUS] = state;
        break;
    case Qt::Key_Minus:
        ogreControls[MINUS] = state;
        break;
    case Qt::Key_Control:
        ogreControls[CTRL] = state;
        break;
    case Qt::Key_Shift:
        ogreControls[SHIFT] = state;
        break;
    default:
        break;
    }
}
