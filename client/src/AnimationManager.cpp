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

#include "AnimationManager.h"
#include <Ogre.h>

AnimationManager *AnimationManager::singleton = NULL;

AnimationManager::AnimationManager() {
  movingAvatars = new QMap<quint16, MovingAvatar>;
}

AnimationManager *AnimationManager::getSingleton() {
  if (singleton == NULL) {
    singleton = new AnimationManager();
  }
  return singleton;
}

void AnimationManager::setAvatar(Ogre::Entity *a) {
  avatar = a;
  running = false;

  aniStateTop = avatar->getAnimationState("IdleTop");
  aniStateTop->setEnabled(true);
  aniStateTop->setLoop(true);

  aniStateBase = avatar->getAnimationState("IdleBase");
  aniStateBase->setEnabled(true);
  aniStateBase->setLoop(true);
}

void AnimationManager::setInputSystem(InputSystem *i) { inputSystem = i; }

void AnimationManager::moveAvatar(const User *u) {
  bool idle = (u->x == u->oldX && u->y == u->oldY && u->z == u->oldZ &&
               u->pitch == u->oldPitch && u->yaw == u->oldYaw);

  if (movingAvatars->contains(u->id)) {
    MovingAvatar ma = (*movingAvatars)[u->id];
    ma.idle = idle;
    ma.oldX = ma.oldX + (ma.x - ma.oldX) * ma.completion;
    ma.oldY = ma.oldY + (ma.y - ma.oldY) * ma.completion;
    ma.oldZ = ma.oldZ + (ma.z - ma.oldZ) * ma.completion;
    ma.x = u->x;
    ma.y = u->y - 5.0f;
    ma.z = u->z;
    ma.pitch = u->pitch;
    ma.yaw = u->yaw;
    ma.oldPitch = u->oldPitch;
    ma.oldYaw = u->oldYaw;
    ma.completion = 0.0;
    movingAvatars->insert(u->id, ma);
  } else {
    movingAvatars->insert(u->id, {
                                     u->avatar,
                                     u->node,
                                     u->x,
                                     u->y - 5.0f,
                                     u->z,
                                     u->oldX,
                                     u->oldY - 5.0f,
                                     u->oldZ,
                                     u->pitch,
                                     u->yaw,
                                     u->oldPitch,
                                     u->oldYaw,
                                     0.0,
                                     idle,
                                 });

    Ogre::AnimationState *aniStateBase =
        u->avatar->getAnimationState("IdleTop");
    Ogre::AnimationState *aniStateTop =
        u->avatar->getAnimationState("IdleBase");

    aniStateTop->setEnabled(true);
    aniStateTop->setLoop(true);

    aniStateBase->setEnabled(true);
    aniStateBase->setLoop(true);
  }
}

void AnimationManager::animate(const Ogre::FrameEvent &evt) {
  bool *ogreControls = inputSystem->getControls();

  if (ogreControls[UP] || ogreControls[DOWN] ||
      (ogreControls[SHIFT] && (ogreControls[LEFT] || ogreControls[RIGHT]))) {
    if (!running) {
      running = true;
      aniStateTop->setEnabled(false);
      aniStateBase->setEnabled(false);
      aniStateTop = avatar->getAnimationState("RunTop");
      aniStateBase = avatar->getAnimationState("RunBase");
      aniStateTop->setEnabled(true);
      aniStateBase->setEnabled(true);
    }
  } else {
    if (running) {
      running = false;
      aniStateTop->setEnabled(false);
      aniStateBase->setEnabled(false);
      aniStateTop = avatar->getAnimationState("IdleTop");
      aniStateBase = avatar->getAnimationState("IdleBase");
      aniStateTop->setEnabled(true);
      aniStateBase->setEnabled(true);
    }
  }
  aniStateTop->addTime(evt.timeSinceLastFrame);
  aniStateBase->addTime(evt.timeSinceLastFrame);

  QList<quint16> keys = (*movingAvatars).keys();
  QList<quint16> removeList;

  for (const quint16 &key : keys) {
    MovingAvatar &ma = (*movingAvatars)[key];
    Ogre::SceneNode *node = ma.node;

    if (ma.completion >= 1.0) {
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
    else if (deltaYaw < -Ogre::Math::PI)
      deltaYaw += Ogre::Math::TWO_PI;

    node->yaw(Ogre::Radian(ma.oldYaw + deltaYaw * ma.completion));

    if (deltaPitch > Ogre::Math::PI)
      deltaPitch -= Ogre::Math::TWO_PI;
    else if (deltaPitch < -Ogre::Math::PI)
      deltaPitch += Ogre::Math::TWO_PI;

    node->pitch(Ogre::Radian(ma.oldPitch + deltaPitch * ma.completion));
    // we assume one update every 200ms
    ma.completion += evt.timeSinceLastFrame * 5;
    Ogre::AnimationState *aniStateOldTop, *aniStateOldBase, *aniStateNewTop,
        *aniStateNewBase;

    try {
      if (!ma.idle) {
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
    } catch (Ogre::ItemIdentityException iie) {
    }
  }

  for (const quint16 &key : removeList) movingAvatars->remove(key);

  removeList.clear();
}
