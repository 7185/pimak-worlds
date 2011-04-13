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
}

OgreFrameListener::~OgreFrameListener()
{
    delete [] ogreControls;
}

bool OgreFrameListener::frameStarted(const Ogre::FrameEvent &evt)
{
    if (ogreControls[UP] || ogreControls[DOWN] ||
        ogreControls[SHIFT] && (ogreControls[LEFT] || ogreControls[RIGHT])) {
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
