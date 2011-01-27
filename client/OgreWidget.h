#ifndef OGREWIDGET_H
#define OGREWIDGET_H

#include <QtGui>
#include <Ogre.h>


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
};

#endif // OGREWIDGET_H
