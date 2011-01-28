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

public slots:
    void setCameraPosition(const Ogre::Vector3 &pos);
    void setCameraPitch(const Ogre::Radian &ang);
    void setCameraYaw(const Ogre::Radian &ang);
signals:
    void cameraPositionChanged(const Ogre::Vector3 &pos);

protected:
    virtual void moveEvent(QMoveEvent *e);
    virtual void paintEvent(QPaintEvent *e);
    virtual void resizeEvent(QResizeEvent *e);
    virtual void showEvent(QShowEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);
    virtual QPaintEngine* paintEngine() const;
    
private:
    void initOgreSystem();
    void setupNLoadResources();
    void createCamera();
    void createViewport();
    void createScene();

private:
    Ogre::Root         *ogreRoot;
    Ogre::SceneManager *ogreSceneMgr;
    Ogre::RenderWindow *ogreRenderWindow;
    Ogre::Viewport     *ogreViewport;
    Ogre::Camera       *ogreCamera;
    OgreFrameListener  *ogreListener;
};

#endif // OGREWIDGET_H
