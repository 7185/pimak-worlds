#ifndef OGREFRAMELISTENER_H
#define OGREFRAMELISTENER_H

#include <Ogre.h>
#include <Qt>

class OgreFrameListener : public Ogre::FrameListener
{
   public:
       OgreFrameListener(Ogre::Entity*);
       ~OgreFrameListener();
       bool frameStarted(const Ogre::FrameEvent &);
       bool frameEnded(const Ogre::FrameEvent &);
       bool frameRenderingQueued(const Ogre::FrameEvent &);

       void handleKeys(int, bool);

       enum keys { UP=0, RIGHT, DOWN, LEFT, PGUP, PGDOWN, PLUS, MINUS, CTRL, SHIFT };
       bool *ogreControls;

    private:
       bool running;
       Ogre::Entity* ent;
       Ogre::AnimationState* aniStateBase;
       Ogre::AnimationState* aniStateTop;
};

#endif // OGREFRAMELISTENER_H
