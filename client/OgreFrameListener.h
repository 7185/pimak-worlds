#ifndef OGREFRAMELISTENER_H
#define OGREFRAMELISTENER_H

#include <Ogre.h>
#include <Qt>

class OgreFrameListener : public Ogre::FrameListener
{
   public:
       OgreFrameListener();
       ~OgreFrameListener();
       bool frameStarted(const Ogre::FrameEvent &);
       bool frameEnded(const Ogre::FrameEvent &);
       bool frameRenderingQueued(const Ogre::FrameEvent &);

       void handleKeys(int, bool);

       enum keys
       {
           UP=0,
           RIGHT,
           DOWN,
           LEFT,
           PGUP,
           PGDOWN,
           PLUS,
           MINUS
       };
       bool               ogreControls[8];
};

#endif // OGREFRAMELISTENER_H
