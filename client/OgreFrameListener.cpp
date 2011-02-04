#include "OgreFrameListener.h"

OgreFrameListener::OgreFrameListener() {}

OgreFrameListener::~OgreFrameListener() {}

bool OgreFrameListener::frameStarted(const Ogre::FrameEvent &evt)
{
    // std::cout << "Frame started" << std::endl;
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
}


//235
