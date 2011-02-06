#ifndef OGREWIDGET_H
#define OGREWIDGET_H

#include <QtGui>
#include <Ogre.h>

#include "OgreFrameListener.h"

class OgreWidget : public QWidget
{
    Q_OBJECT
    
public:
    OgreWidget(QWidget *parent = 0);
    ~OgreWidget();

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
    void createScene();
    void moveCamera();


private:
    QTimer *paintTimer;

    Ogre::Root         *ogreRoot;
    Ogre::SceneManager *ogreSceneMgr;
    Ogre::RenderWindow *ogreRenderWindow;
    Ogre::Viewport     *ogreViewport;
    Ogre::Camera       *ogreCamera;
    Ogre::Camera       *cameraThirdView; // 3rd view cam
    Ogre::Camera       *ogreRootCamera;
    Ogre::Camera       *activeCamera; // Current cam
    Ogre::SceneNode    *cameraNode;      // Camera node
    Ogre::SceneNode    *cameraPitchNode; // Separate pitch node
    Ogre::Entity       *avatar;
    OgreFrameListener  *ogreListener;

    int turbo;
    enum keys
    {
        UP=0,
        RIGHT,
        DOWN,
        LEFT,
        PGUP,
        PGDOWN,
        PLUS,
        MINUS,
        CTRL
    };
};

#endif // OGREWIDGET_H
