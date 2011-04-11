#include <QtDebug>
#include <QtGui/QX11Info>

#include "OgreWidget.h"

OgreWidget::OgreWidget(QWidget *parent) :
    QWidget(parent),ogreRoot(0), ogreSceneMgr(0), ogreRenderWindow(0), ogreViewport(0),ogreFirstCamera(0)
{
    setAttribute(Qt::WA_OpaquePaintEvent,true);
    setAttribute(Qt::WA_PaintOnScreen,true);
    setMinimumSize(96,96);
    setFocusPolicy(Qt::ClickFocus);

    ogreRoot = NULL;
    ogreSceneMgr = NULL;
    ogreListener = NULL;
    activeCamera = NULL;
}

OgreWidget::~OgreWidget()
{
    if(ogreRenderWindow) ogreRenderWindow->removeAllViewports();
    
    if(ogreRoot)
    {
        ogreRoot->detachRenderTarget(ogreRenderWindow);
        if(ogreSceneMgr) ogreRoot->destroySceneManager(ogreSceneMgr);
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
    QString str = QString::number(ogreRenderWindow->getAverageFPS())+" FPS";
    emit dispAverageFps(str);
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
            ogreRenderWindow->resize(newSize.width(),newSize.height());
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

    widgetHandle = Ogre::StringConverter::toString((unsigned long)xInfo.display()) +
        ":" + Ogre::StringConverter::toString((unsigned int)xInfo.screen()) +
        ":" + Ogre::StringConverter::toString((unsigned long)q_parent->winId());
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
    ogreFirstCamera = ogreSceneMgr->createCamera("FirstView");
    ogreFirstCamera->setNearClipDistance(5);
    
    ogreThirdCamera = ogreSceneMgr->createCamera("ThirdView");
    ogreThirdCamera->setNearClipDistance(5);

    cameraNode = ogreSceneMgr->getRootSceneNode()->createChildSceneNode();
    cameraPitchNode = cameraNode->createChildSceneNode();
}

void OgreWidget::createViewport()
{
    //activeCamera = ogreFirstCamera;
    activeCamera = ogreFirstCamera;
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


    Ogre::Entity* House =  ogreSceneMgr->createEntity("House", "House.mesh");
    Ogre::SceneNode* HouseNode = node->createChildSceneNode("HouseNode");
    HouseNode->setScale(0.1f,0.1f,0.1f);
    HouseNode->setPosition(Ogre::Vector3(0.0f,45.0f,0.0f));
    HouseNode->attachObject(House);


    avatar = ogreSceneMgr->createEntity("Avatar", "Sinbad.mesh");
    
    ogreListener = new OgreFrameListener(avatar);
    ogreRoot->addFrameListener(ogreListener);
    
    Ogre::SceneNode* avatarNode = cameraPitchNode->createChildSceneNode("AvatarNode");
    avatarNode->setPosition(Ogre::Vector3(0.0f,-5.0f,0.0f));
    avatarNode->yaw(Ogre::Degree(180.0f)); // 3rd view avatar 180° offset
    avatarNode->attachObject(avatar);
    cameraPitchNode->attachObject(ogreFirstCamera);
    
    Ogre::SceneNode* nodeThirdView = cameraNode->createChildSceneNode("nodeThirdView");
    nodeThirdView->attachObject(ogreThirdCamera);
    nodeThirdView->setPosition(Ogre::Vector3(0.0f,-1.0f,26.0f));
    
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

void OgreWidget::setActiveCam(bool cam)
{
    if (cam) activeCamera = ogreThirdCamera;
    else activeCamera = ogreFirstCamera;
    activeCamera->setAspectRatio(Ogre::Real(ogreViewport->getActualWidth()) / Ogre::Real(ogreViewport->getActualHeight()));
    ogreViewport->setCamera(activeCamera);
}

void OgreWidget::moveCamera()
{
    Ogre::Real pitchAngle = 2*Ogre::Degree(Ogre::Math::ACos(cameraPitchNode->getOrientation().w)).valueDegrees();
    Ogre::Real pitchAngleSign = cameraPitchNode->getOrientation().x;

    // Should not be frame based
    if (ogreListener->ogreControls[CTRL]) turbo = 5;
    else turbo = 1;
    if (ogreListener->ogreControls[UP]) cameraNode->translate(turbo*-Ogre::Vector3(cameraNode->getOrientation().zAxis().x,0,cameraNode->getOrientation().zAxis().z));
    if (ogreListener->ogreControls[DOWN]) cameraNode->translate(turbo*Ogre::Vector3(cameraNode->getOrientation().zAxis().x,0,cameraNode->getOrientation().zAxis().z));
    if (ogreListener->ogreControls[PLUS]) cameraNode->translate(turbo*Ogre::Vector3( 0, 1, 0));
    if (ogreListener->ogreControls[MINUS]) cameraNode->translate(turbo*Ogre::Vector3( 0, -1, 0));

    if (ogreListener->ogreControls[LEFT]) {
        if (ogreListener->ogreControls[SHIFT]) cameraNode->translate(turbo*Ogre::Vector3(-cameraNode->getOrientation().zAxis().z,0,cameraNode->getOrientation().zAxis().x));
        else cameraNode->yaw(turbo*Ogre::Radian(0.05f));
    }
    if (ogreListener->ogreControls[RIGHT]) {
        if (ogreListener->ogreControls[SHIFT]) cameraNode->translate(turbo*Ogre::Vector3(cameraNode->getOrientation().zAxis().z,0,-cameraNode->getOrientation().zAxis().x));
        else cameraNode->yaw(turbo*Ogre::Radian(-0.05f));
    }
    if (ogreListener->ogreControls[PGUP] && (pitchAngle < 90.0f || pitchAngleSign < 0))
        cameraPitchNode->pitch(turbo*Ogre::Radian(0.05f));
    if (ogreListener->ogreControls[PGDOWN] && (pitchAngle < 90.0f || pitchAngleSign > 0))
        cameraPitchNode->pitch(turbo*Ogre::Radian(-0.05f));

    ogreThirdCamera->lookAt(cameraNode->getPosition());
}

QPaintEngine *OgreWidget:: paintEngine() const { return 0; }
