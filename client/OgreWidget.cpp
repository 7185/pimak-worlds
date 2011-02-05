#include <QtDebug>
#include <QtGui/QX11Info>

#include "OgreWidget.h"

OgreWidget::OgreWidget(QWidget *parent) :
    QWidget(parent),ogreRoot(0), ogreSceneMgr(0), ogreRenderWindow(0), ogreViewport(0),ogreCamera(0)
{
    setAttribute(Qt::WA_OpaquePaintEvent,true);
    setAttribute(Qt::WA_PaintOnScreen,true);
    setMinimumSize(320,240);
    setFocusPolicy(Qt::ClickFocus);

    ogreRoot = NULL;
    ogreSceneMgr = NULL;
    ogreListener = NULL;
    activeCamera = NULL;

    paintTimer = new QTimer;
    paintTimer->start(20); //ton oeil en voit que 10 par seconde pd
    connect(paintTimer,SIGNAL(timeout()),this,SLOT(update()));
}

OgreWidget::~OgreWidget()
{
    if(ogreRenderWindow)
    {
        ogreRenderWindow->removeAllViewports();
    }
    
    if(ogreRoot)
    {
        ogreRoot->detachRenderTarget(ogreRenderWindow);
        if(ogreSceneMgr)
        {
            ogreRoot->destroySceneManager(ogreSceneMgr);
        }
    }
    delete ogreListener;
    delete ogreRoot;
}

void OgreWidget::moveEvent(QMoveEvent *e)
{
    QWidget::moveEvent(e);
    
    if(e->isAccepted() && ogreRenderWindow)
    {
        ogreRenderWindow->windowMovedOrResized();
        update();
    }
}

void OgreWidget::paintEvent(QPaintEvent *e)
{
    moveCamera();
    ogreRoot->renderOneFrame();
    ogreRenderWindow->update();
    e->accept();
}

void OgreWidget::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    
    if(e->isAccepted())
    {
        const QSize &newSize = e->size();
        if(ogreRenderWindow)
        {
            ogreRenderWindow->resize(newSize.width(), newSize.height());
            ogreRenderWindow->windowMovedOrResized();
        }
        if(activeCamera)
        {
            Ogre::Real aspectRatio = Ogre::Real(newSize.width()) / Ogre::Real(newSize.height());
            activeCamera->setAspectRatio(aspectRatio);
        }
    }
}

void OgreWidget::showEvent(QShowEvent *e)
{
    if(!ogreRoot) initOgreSystem();
    QWidget::showEvent(e);
}

void OgreWidget::initOgreSystem()
{
    ogreRoot = new Ogre::Root();
    
    Ogre::RenderSystem *renderSystem = ogreRoot->getRenderSystemByName("OpenGL Rendering Subsystem");
    ogreRoot->setRenderSystem(renderSystem);
    ogreRoot->initialise(false);

    ogreSceneMgr = ogreRoot->createSceneManager(Ogre::ST_GENERIC);
    

    Ogre::NameValuePairList viewConfig;
    Ogre::String widgetHandle;

#ifdef Q_WS_WIN
    widgetHandle = Ogre::StringConverter::toString((size_t)((HWND)winId()));
#else

    QWidget *q_parent = dynamic_cast <QWidget *> (parent());
    QX11Info xInfo = x11Info();

    widgetHandle = Ogre::StringConverter::toString ((unsigned long)xInfo.display()) +
        ":" + Ogre::StringConverter::toString ((unsigned int)xInfo.screen()) +
        ":" + Ogre::StringConverter::toString ((unsigned long)q_parent->winId());
#endif

    viewConfig["externalWindowHandle"] = widgetHandle;
    ogreRenderWindow = ogreRoot->createRenderWindow("Ogre rendering window",width(),height(),false,&viewConfig);

    setupNLoadResources();
    createCamera();
    createViewport();
    createScene();
}

void OgreWidget::setupNLoadResources()
{
        // Load resource paths from config file
        Ogre::ConfigFile cf;
        cf.load("resources.cfg");

        // Go through all sections & settings in the file
        Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

        Ogre::String secName, typeName, archName;
        while (seci.hasMoreElements())
        {
                secName = seci.peekNextKey();
                Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
                Ogre::ConfigFile::SettingsMultiMap::iterator i;
                for (i = settings->begin(); i != settings->end(); ++i)
                {
                        typeName = i->first;
                        archName = i->second;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
                        // OS X does not set the working directory relative to the app,
                        // In order to make things portable on OS X we need to provide
                        // the loading with it's own bundle path location
                        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(Ogre::String(macBundlePath()
                                                                              + "/" + archName), typeName, secName);
#else
                        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
#endif
                }
        }

        // Initialise, parse scripts etc
        Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}


void OgreWidget::createCamera()
{
    ogreRootCamera = ogreSceneMgr->createCamera("CameraRoot");
    ogreRootCamera->setNearClipDistance(5);

    ogreCamera = ogreSceneMgr->createCamera("MyCamera");
    ogreCamera->setNearClipDistance(5);

    cameraNode = ogreSceneMgr->getRootSceneNode()->createChildSceneNode();
    //cameraNode->setPosition(0,10,100);
    cameraYawNode = cameraNode->createChildSceneNode();
    cameraPitchNode = cameraYawNode->createChildSceneNode();

}

void OgreWidget::createViewport()
{
    activeCamera = ogreRootCamera;
    ogreViewport = ogreRenderWindow->addViewport(activeCamera);
    ogreViewport->setBackgroundColour(Ogre::ColourValue(0,0,0));
    activeCamera->setAspectRatio(Ogre::Real(ogreViewport->getActualWidth()) / Ogre::Real(ogreViewport->getActualHeight()));
    // activeCamera->setPolygonMode(Ogre::PM_WIREFRAME);
}

void OgreWidget::createScene()
{

    ogreSceneMgr->setAmbientLight(Ogre::ColourValue(1,1,1));
    ogreSceneMgr->setSkyDome(true,"CloudySky");

    Ogre::SceneNode* node = ogreSceneMgr->createSceneNode("Node1");
    ogreSceneMgr->getRootSceneNode()->addChild(node);

    Ogre::Plane plane(Ogre::Vector3::UNIT_Y, -10);
    Ogre::MeshManager::getSingleton().createPlane("plane",
     Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
     1500, 1500,200,200,true,1,5,5,Ogre::Vector3::UNIT_Z);
    Ogre::Entity* ent = ogreSceneMgr->createEntity("LightPlaneEntity","plane");
    ogreSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
    ent->setMaterialName("grass");

    Ogre::Light* light = ogreSceneMgr->createLight("Light1");
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    light->setDiffuseColour(Ogre::ColourValue(1.0f,1.0f,1.0f));
    light->setDirection(Ogre::Vector3(1,-1,0));

/*
    Ogre::Entity* Sinbad =  ogreSceneMgr->createEntity("Sinbad", "Sinbad.mesh");
    Ogre::SceneNode* SinbadNode = node->createChildSceneNode("SinbadNode");

    SinbadNode->setScale(3.0f,3.0f,3.0f);
    SinbadNode->setPosition(Ogre::Vector3(0.0f,5.0f,0.0f));
    SinbadNode->attachObject(Sinbad);
*/

    avatar = ogreSceneMgr->createEntity("Avatar", "Sinbad.mesh");
    

    //cameraNode->setScale(3.0f,3.0f,3.0f);
    cameraPitchNode->attachObject(avatar);
    cameraPitchNode->attachObject(ogreCamera);
    
    ogreListener = new OgreFrameListener(avatar);
    ogreRoot->addFrameListener(ogreListener);

    ogreSceneMgr->setShadowTechnique(Ogre:: SHADOWTYPE_STENCIL_ADDITIVE);
}

void OgreWidget::keyPressEvent(QKeyEvent *e)
{
   if (e->isAutoRepeat()) return;
   ogreListener->handleKeys(e->key(),true);
   e->accept();
}


void OgreWidget::keyReleaseEvent(QKeyEvent *e)
{
   if (e->isAutoRepeat()) return;
   ogreListener->handleKeys(e->key(),false);
   e->accept();
}

void OgreWidget::moveCamera()
{
    // lolwut
    ogreCamera->setOrientation(cameraYawNode->getOrientation());

    // Should not be frame based
    if (ogreListener->ogreControls[UP]) cameraYawNode->translate(-Ogre::Vector3(ogreCamera->getDirection().x,0,ogreCamera->getDirection().z));
    if (ogreListener->ogreControls[DOWN]) cameraYawNode->translate(Ogre::Vector3(ogreCamera->getDirection().x,0,ogreCamera->getDirection().z));
    if (ogreListener->ogreControls[PLUS]) cameraYawNode->translate(Ogre::Vector3( 0, 1, 0));
    if (ogreListener->ogreControls[MINUS]) cameraYawNode->translate(Ogre::Vector3( 0, -1, 0));

    if (ogreListener->ogreControls[LEFT]) cameraYawNode->yaw(Ogre::Radian(0.05));
    if (ogreListener->ogreControls[RIGHT]) cameraYawNode->yaw(Ogre::Radian(-0.05));
    if (ogreListener->ogreControls[PGUP]) cameraPitchNode->pitch(Ogre::Radian(0.05));
    if (ogreListener->ogreControls[PGDOWN]) cameraPitchNode->pitch(Ogre::Radian(-0.05));

    ogreRootCamera->lookAt(cameraYawNode->getPosition());
}

QPaintEngine *OgreWidget:: paintEngine() const
{
    return 0;
}
