#ifndef OGREWINDOW_H
#define OGREWINDOW_H

#include <QtWidgets/QApplication>
#include <QtGui/QKeyEvent>
#include <QtGui/QWindow>

#include <Ogre.h>
#ifdef Q_WS_WIN
#include <Terrain/OgreTerrain.h>
#else
#include <OGRE/Terrain/OgreTerrain.h>
#endif

#include "OgreFrameListener.h"
#include "User.h"

class OgreWindow : public QWindow {
    Q_OBJECT

public:
    explicit OgreWindow(QWindow *parent = NULL);
    ~OgreWindow();

    virtual void render(QPainter *painter);
    virtual void render();
    virtual void initialize();
#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
    virtual void createCompositor();
#endif

    void setAnimating(bool animating);
    void setActiveCam(bool); //0: First, 1:Third

public slots:

    virtual void renderLater();
    virtual void renderNow();

    /*
    We use an event filter to be able to capture keyboard/mouse events. More on this later.
    */
    virtual bool eventFilter(QObject *target, QEvent *event);

 void createAvatar(User *u);
 void destroyAvatar(User *u);
 void moveAvatar(User *u);
 void posSend();

signals:
 void dispAverageFps(QString);
 void dispPosition(QString);
 void positionSend(float,float,float,float,float);

protected:
    /*
    Ogre3D pointers added here. Useful to have the pointers here for use by the window later.
    */
    Ogre::ColourValue ogreBackground;

    bool m_update_pending;
    bool m_animating;

    /*
    The below methods are what is actually fired when they keys on the keyboard are hit.
    Similar events are fired when the mouse is pressed or other events occur.
    */
    virtual void exposeEvent(QExposeEvent *event);
    virtual bool event(QEvent *event);

    /*
    Write log messages to Ogre log
    */
    void log(Ogre::String msg);
    void log(QString msg);

    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);

 private:
    void setupResources();
    void createCamera();
    void createViewport();
    void createTerrain();
    void createScene();
    void moveCamera();

    Ogre::Root                 *ogreRoot;
    Ogre::SceneManager         *ogreSceneMgr;
    Ogre::RenderWindow         *ogreRenderWindow;
    Ogre::Viewport             *ogreViewport;
    Ogre::Terrain              *ogreTerrain;
    Ogre::TerrainGlobalOptions *ogreTerrainGlobals;
    Ogre::Camera       *ogreFirstCamera;
    Ogre::Camera       *ogreThirdCamera; // 3rd view cam
    Ogre::Camera       *activeCamera;    // Current cam
    Ogre::SceneNode    *cameraNode;      // Camera node
    Ogre::SceneNode    *cameraPitchNode; // Separate pitch node
    Ogre::Entity       *avatar;
    OgreFrameListener  *ogreListener;

    int turbo;
    enum keys { UP=0, RIGHT, DOWN, LEFT, PGUP, PGDOWN, PLUS, MINUS, CTRL, SHIFT };
};

#endif // OGREWINDOW_H
