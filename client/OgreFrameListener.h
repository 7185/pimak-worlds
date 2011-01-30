#ifndef OGREFRAMELISTENER_H
#define OGREFRAMELISTENER_H

#include <Ogre.h>

class OgreFrameListener : public Ogre::FrameListener
{
   public:
       OgreFrameListener();
       ~OgreFrameListener();
       bool frameStarted(const Ogre::FrameEvent &);
       bool frameEnded(const Ogre::FrameEvent &);
       bool frameRenderingQueued(const Ogre::FrameEvent &);
};

#endif // OGREFRAMELISTENER_H
