#include "OgreFrameListener.h"

OgreFrameListener::OgreFrameListener(Ogre::Entity* ent)
{
    _ent = ent;
    running = false;
    
    _aniStateTop = _ent->getAnimationState("IdleTop");
    _aniStateTop->setEnabled(true);
    _aniStateTop->setLoop(true);
    
    _aniStateBase = _ent->getAnimationState("IdleBase");
    _aniStateBase->setEnabled(true);
    _aniStateBase->setLoop(true);
    
    ogreControls = new bool[9];
    for (int i=0;i<=9;i++) { ogreControls[i] = false; }
}

OgreFrameListener::~OgreFrameListener()
{
    delete [] ogreControls;
}

bool OgreFrameListener::frameStarted(const Ogre::FrameEvent &evt)
{
    // std::cout << "Frame started" << std::endl;
    if (ogreControls[UP] || ogreControls[DOWN] ||
        ogreControls[SHIFT] && (ogreControls[LEFT] || ogreControls[RIGHT])) {
        if (!running) {
            running = true;
            _aniStateTop->setEnabled(false);
            _aniStateBase->setEnabled(false);
            _aniStateTop = _ent->getAnimationState("RunTop");
            _aniStateBase = _ent->getAnimationState("RunBase");
            _aniStateTop->setEnabled(true);
            _aniStateBase->setEnabled(true);
        }
    } else {
        if (running) {
            running = false;
            _aniStateTop->setEnabled(false);
            _aniStateBase->setEnabled(false);
            _aniStateTop = _ent->getAnimationState("IdleTop");
            _aniStateBase = _ent->getAnimationState("IdleBase");
            _aniStateTop->setEnabled(true);
            _aniStateBase->setEnabled(true);
        }
    }
    _aniStateTop->addTime(evt.timeSinceLastFrame);
    _aniStateBase->addTime(evt.timeSinceLastFrame);
    return true;
}
bool OgreFrameListener::frameEnded(const Ogre::FrameEvent &evt)
{
    // std::cout << "Frame ended" << std::endl;
    return true;
}
bool OgreFrameListener::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
    // std::cout << "Frame queued" << std::endl;
    return true;
}

void OgreFrameListener::handleKeys(int key, bool state)
{
    if (key==Qt::Key_Up) ogreControls[UP] = state;
    if (key==Qt::Key_Right) ogreControls[RIGHT] = state;
    if (key==Qt::Key_Down) ogreControls[DOWN] = state;
    if (key==Qt::Key_Left) ogreControls[LEFT] = state;
    if (key==Qt::Key_PageUp) ogreControls[PGUP] = state;
    if (key==Qt::Key_PageDown) ogreControls[PGDOWN] = state;
    if (key==Qt::Key_Plus) ogreControls[PLUS] = state;
    if (key==Qt::Key_Minus) ogreControls[MINUS] = state;
    if (key==Qt::Key_Control) ogreControls[CTRL] = state;
    if (key==Qt::Key_Shift) ogreControls[SHIFT] = state;
}
