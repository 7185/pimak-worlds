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

//235
