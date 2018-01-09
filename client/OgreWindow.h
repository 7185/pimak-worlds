#ifndef OGREWINDOW_H
#define OGREWINDOW_H

/*
Qt headers
*/
#include <QtWidgets/QApplication>
#include <QtGui/QKeyEvent>
#include <QtGui/QWindow>

/*
Ogre3D header
*/
#include <Ogre.h>

/*
With the headers included we now need to inherit from QWindow.
*/
class OgreWindow : public QWindow, public Ogre::FrameListener
{
    /*
    A QWindow still inherits from QObject and can have signals/slots; we need to add the appropriate
    Q_OBJECT keyword so that Qt's intermediate compiler can do the necessary wireup between our class
    and the rest of Qt.
    */
    Q_OBJECT

public:
    explicit OgreWindow(QWindow *parent = NULL);
    ~OgreWindow();

    /*
    We declare these methods virtual to allow for further inheritance.
    */
    virtual void render(QPainter *painter);
    virtual void render();
    virtual void initialize();
    virtual void createScene();
#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
    virtual void createCompositor();
#endif

    void setAnimating(bool animating);

public slots:

    virtual void renderLater();
    virtual void renderNow();

    /*
    We use an event filter to be able to capture keyboard/mouse events. More on this later.
    */
    virtual bool eventFilter(QObject *target, QEvent *event);

signals:
    /*
    Event for clicking on an entity.
    */
    void entitySelected(Ogre::Entity* entity);

protected:
    /*
    Ogre3D pointers added here. Useful to have the pointers here for use by the window later.
    */
    Ogre::Root* m_ogreRoot;
    Ogre::RenderWindow* m_ogreWindow;
    Ogre::SceneManager* m_ogreSceneMgr;
    Ogre::Camera* m_ogreCamera;
    Ogre::ColourValue m_ogreBackground;

    bool m_update_pending;
    bool m_animating;

    /*
    The below methods are what is actually fired when they keys on the keyboard are hit.
    Similar events are fired when the mouse is pressed or other events occur.
    */
    virtual void exposeEvent(QExposeEvent *event);
    virtual bool event(QEvent *event);

    /*
    FrameListener method
    */
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

    /*
    Write log messages to Ogre log
    */
    void log(Ogre::String msg);
    void log(QString msg);
};

#endif // OGREWINDOW_H
