#ifndef OGREWIDGET_H
#define OGREWIDGET_H

#include <QtGui>
#include <Ogre.h>
#include <OGRE/Terrain/OgreTerrain.h>

#include "OgreFrameListener.h"
#include "User.h"

class OgreWidget : public QWidget
{
    Q_OBJECT
    
public:
    OgreWidget(QWidget *parent=0);
    ~OgreWidget();
    void setActiveCam(bool); //0: First, 1:Third

public slots:
     void createAvatar(User *u);
     void destroyAvatar(User *u);
     void moveAvatar(User *u);
     void posSend();

signals:
    void dispAverageFps(QString);
    void dispPosition(QString);
    void positionSend(float,float,float,float,float);

protected:
    virtual void moveEvent(QMoveEvent *e);
    virtual void paintEvent(QPaintEvent *e);
    virtual void resizeEvent(QResizeEvent *e);
    virtual void showEvent(QShowEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);
    virtual QPaintEngine* paintEngine() const;
    
private:
    void initOgreSystem();
    void setupNLoadResources();
    void createCamera();
    void createViewport();
    void createTerrain();
    void createScene();
    void moveCamera();

private:
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

#endif // OGREWIDGET_H
