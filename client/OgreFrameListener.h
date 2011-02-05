#ifndef OGREFRAMELISTENER_H
#define OGREFRAMELISTENER_H

#include <Ogre.h>
#include <Qt>

class OgreFrameListener : public Ogre::FrameListener
{
   public:
       OgreFrameListener(Ogre::Entity* ent);
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
       bool *ogreControls;

    private:
       bool running;
       Ogre::Entity* _ent;
       Ogre::AnimationState* _aniStateBase;
       Ogre::AnimationState* _aniStateTop;
};

#endif // OGREFRAMELISTENER_H
