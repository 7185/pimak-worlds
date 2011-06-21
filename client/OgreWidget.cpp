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
    OGRE_DELETE ogreTerrain;
    OGRE_DELETE ogreTerrainGlobals;
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
    QString str = QString::number(ogreRenderWindow->getAverageFPS(),'f',2)+" FPS";
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
    ogreSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);
    ogreSceneMgr->setAmbientLight(Ogre::ColourValue(1,1,1));
    ogreSceneMgr->setSkyDome(true,"CloudySky");

    Ogre::SceneNode* node = ogreSceneMgr->createSceneNode("Node1");
    ogreSceneMgr->getRootSceneNode()->addChild(node);
/*
    Ogre::Plane plane(Ogre::Vector3::UNIT_Y, -10);
    Ogre::MeshManager::getSingleton().createPlane("plane",
     Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
     1500, 1500,200,200,true,1,5,5,Ogre::Vector3::UNIT_Z);
    Ogre::Entity* ent = ogreSceneMgr->createEntity("LightPlaneEntity","plane");
    ogreSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
    ent->setMaterialName("grass");
*/
    Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
    Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(8);

    Ogre::Light* terLight = ogreSceneMgr->createLight("terrainLight");
    terLight->setType(Ogre::Light::LT_DIRECTIONAL);
    terLight->setDirection(Ogre::Vector3(0.55f,-0.3f,0.75f));
    terLight->setDiffuseColour(Ogre::ColourValue::White);
    terLight->setSpecularColour(Ogre::ColourValue(0.4f, 0.4f, 0.4f));

    ogreTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();
    ogreTerrainGlobals->setMaxPixelError(8);
    ogreTerrainGlobals->setLightMapDirection(terLight->getDerivedDirection());
    ogreTerrainGlobals->setCompositeMapDistance(3000);
    ogreTerrainGlobals->setCompositeMapAmbient(ogreSceneMgr->getAmbientLight());
    ogreTerrainGlobals->setCompositeMapDiffuse(terLight->getDiffuseColour());

    ogreTerrain = OGRE_NEW Ogre::Terrain(ogreSceneMgr);
    Ogre::Image img;
    img.load("terrain.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    Ogre::Terrain::ImportData imp;
    imp.inputImage = &img;
    imp.terrainSize = img.getWidth();
    imp.worldSize = 8000;
    imp.inputScale = 600;
    imp.minBatchSize = 33;
    imp.maxBatchSize = 65;
    imp.layerList.resize(3);
    imp.layerList[0].worldSize = 100;
    imp.layerList[0].textureNames.push_back("grass_green-01_diffusespecular.dds");
    imp.layerList[0].textureNames.push_back("grass_green-01_normalheight.dds");
    imp.layerList[1].worldSize = 30;
    imp.layerList[1].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
    imp.layerList[1].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");
    imp.layerList[2].worldSize = 200;
    imp.layerList[2].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
    imp.layerList[2].textureNames.push_back("dirt_grayrocky_normalheight.dds");
    ogreTerrain->prepare(imp);
    ogreTerrain->load();

    Ogre::TerrainLayerBlendMap* blendMap1 = ogreTerrain->getLayerBlendMap(1);
    float* pBlend1 = blendMap1->getBlendPointer();
    for (Ogre::uint16 y=0;y<ogreTerrain->getLayerBlendMapSize();++y)
    {
        for (Ogre::uint16 x=0;x<ogreTerrain->getLayerBlendMapSize();++x)
        {
            Ogre::Real terrainX, terrainY;
            blendMap1->convertImageToTerrainSpace(x, y, &terrainX, &terrainY);
            Ogre::Real height = ogreTerrain->getHeightAtTerrainPosition(terrainX, terrainY);
            if(height < 200) *pBlend1 = 1;
            *pBlend1++;
        }
    }
    blendMap1->dirty();
    blendMap1->update();

    ogreTerrain->freeTemporaryResources();

    Ogre::Entity* House =  ogreSceneMgr->createEntity("House", "House.mesh");
    Ogre::SceneNode* HouseNode = node->createChildSceneNode("HouseNode");
    HouseNode->setScale(0.1f,0.1f,0.1f);
    HouseNode->setPosition(Ogre::Vector3(1400.0f,55.0f,1020.0f));
    HouseNode->attachObject(House);

    avatar = ogreSceneMgr->createEntity("Avatar", "Sinbad.mesh");
    
    ogreListener = new OgreFrameListener(avatar);
    ogreRoot->addFrameListener(ogreListener);
    
    Ogre::SceneNode* avatarNode = cameraPitchNode->createChildSceneNode("avatarNode");
    avatarNode->setPosition(Ogre::Vector3(0.0f,-5.0f,0.0f));
    avatarNode->yaw(Ogre::Degree(180.0f)); // 3rd view avatar 180° offset
    avatarNode->attachObject(avatar);
    cameraPitchNode->attachObject(ogreFirstCamera);
    
    Ogre::SceneNode* nodeThirdView = cameraNode->createChildSceneNode("nodeThirdView");
    nodeThirdView->attachObject(ogreThirdCamera);
    nodeThirdView->setPosition(avatarNode->getPosition()+Ogre::Vector3(0.0f,4.0f,26.0));
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

    emit dispPosition("x:"+QString::number(cameraNode->getPosition().x,'f',0)+" y:"+QString::number(cameraNode->getPosition().y,'f',0)+" z:"+QString::number(cameraNode->getPosition().z,'f',0));
}

QPaintEngine *OgreWidget:: paintEngine() const { return 0; }
