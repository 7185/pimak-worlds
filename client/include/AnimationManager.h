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

#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include <Qt>
#include <QMap>
#include <Ogre.h>

#include "InputSystem.h"
#include "User.h"

typedef struct {
 Ogre::Entity *avatar;
 Ogre::SceneNode *node;
 float x,y,z;
 float oldX,oldY,oldZ;
 float pitch;
 float yaw;
 float oldPitch;
 float oldYaw;
 float completion;
} MovingAvatar;

class AnimationManager {
 public:
  static AnimationManager* getSingleton();
  void animate(const Ogre::FrameEvent &evt);
  void setAvatar(Ogre::Entity* avatar);
  void setInputSystem(InputSystem* inputSystem);
  void moveAvatar(const User* u);

 private:
  AnimationManager();

  bool running;
  Ogre::AnimationState* aniStateBase;
  Ogre::AnimationState* aniStateTop;
  static AnimationManager* singleton;
  Ogre::Entity* avatar;
  InputSystem* inputSystem;
  QMap<quint16, MovingAvatar> *movingAvatars;
};

#endif // ANIMATIONMANAGER_H
