/**
 * Copyright (c) 2012, Thibault Signor <tibsou@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "OgreWindow.h"
#include "AnimationManager.h"
#include "InputSystem.h"
#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
#include <Compositor/OgreCompositorManager2.h>
#endif

/**
 * Note that we pass any supplied QWindow parent to the base QWindow class. This
 * is necessary should we need to use our class within a container.
 */
OgreWindow::OgreWindow(QWindow *parent)
    : QWindow(parent),
      updatePending(false),
      animating(false),
      ogreRoot(nullptr),
      ogreRenderWindow(nullptr) {
  setAnimating(true);
  installEventFilter(this);
  ogreBackground = Ogre::ColourValue(0.0f, 0.5f, 1.0f);

  ogreRoot = nullptr;
  ogreSceneMgr = nullptr;
  ogreListener = nullptr;
  activeCamera = nullptr;

  inputSystem = new InputSystem();
}

OgreWindow::~OgreWindow() {
  if (ogreTerrain) {
    OGRE_DELETE ogreTerrain;
    OGRE_DELETE ogreTerrainGlobals;
  }
  if (ogreRenderWindow) ogreRenderWindow->removeAllViewports();
  if (ogreRoot) {
    ogreRoot->detachRenderTarget(ogreRenderWindow);
    if (ogreSceneMgr) ogreRoot->destroySceneManager(ogreSceneMgr);
  }
  delete ogreListener;
  delete ogreRoot;
}

/**
 * In case any drawing surface backing stores (QRasterWindow or QOpenGLWindow)
 * of Qt are supplied to this class in any way we inform Qt that they will be
 * unused.
 */
void OgreWindow::render(QPainter *painter) { Q_UNUSED(painter); }

/**
 * Our initialization function. Called by our renderNow() function once when the
 * window is first exposed.
 */
void OgreWindow::initialize() {
#ifdef _MSC_VER
  ogreRoot = new Ogre::Root(Ogre::String("plugins" OGRE_BUILD_SUFFIX ".cfg"));
#else
  ogreRoot = new Ogre::Root(Ogre::String("plugins.cfg"));
#endif

  const Ogre::RenderSystemList &rsList = ogreRoot->getAvailableRenderers();
  Ogre::RenderSystem *rs = rsList[0];

  // This list setup the search order for used render system.
  Ogre::StringVector renderOrder;
#if defined(Q_OS_WIN)
  renderOrder.push_back("Direct3D9");
  renderOrder.push_back("Direct3D11");
#endif
  renderOrder.push_back("OpenGL");
  renderOrder.push_back("OpenGL 3+");
  for (Ogre::StringVector::iterator iter = renderOrder.begin();
       iter != renderOrder.end(); iter++) {
    for (Ogre::RenderSystemList::const_iterator it = rsList.begin();
         it != rsList.end(); it++) {
      if ((*it)->getName().find(*iter) != Ogre::String::npos) {
        rs = *it;
        break;
      }
    }
    if (rs != nullptr) break;
  }
  if (rs == nullptr) {
    if (!ogreRoot->restoreConfig()) {
      if (!ogreRoot->showConfigDialog(OgreBites::getNativeConfigDialog()))
        OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS,
                    "Abort render system configuration",
                    "QTOgreWindow::initialize");
    }
  }

  // Setting size and VSync on windows will solve a lot of problems
  if (rs != nullptr) {
    QString dimensions =
        QString("%1 x %2").arg(this->width()).arg(this->height());
    rs->setConfigOption("Video Mode", dimensions.toStdString());
    rs->setConfigOption("Full Screen", "No");
    rs->setConfigOption("VSync", "Yes");
  }
  ogreRoot->setRenderSystem(rs);
  ogreRoot->initialise(false);

  Ogre::NameValuePairList parameters;

  // Flag within the parameters set so that Ogre3D initializes an OpenGL context
  // on it's own.

  if (rs->getName().find("GL") <= rs->getName().size())
    parameters["currentGLContext"] = Ogre::String("false");

    /**
     * We need to supply the low level OS window handle to this QWindow so that
     * Ogre3D knows where to draw the scene. Below is a cross-platform method on
     * how to do this. If you set both options (externalWindowHandle and
     * parentWindowHandle) this code will work with OpenGL and DirectX.
     */
#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
  parameters["externalWindowHandle"] =
      Ogre::StringConverter::toString((size_t)(this->winId()));
  parameters["parentWindowHandle"] =
      Ogre::StringConverter::toString((size_t)(this->winId()));
#else
  parameters["externalWindowHandle"] = Ogre::StringConverter::toString(
      static_cast<unsigned long>(this->winId()));
  parameters["parentWindowHandle"] = Ogre::StringConverter::toString(
      static_cast<unsigned long>(this->winId()));
#endif

#if defined(Q_OS_MAC)
  parameters["macAPI"] = "cocoa";
  parameters["macAPICocoaUseNSView"] = "true";
#endif

  ogreRenderWindow = ogreRoot->createRenderWindow(
      "QT Window", this->width(), this->height(), false, &parameters);
  ogreRenderWindow->setVisible(true);

#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
  const size_t numThreads =
      std::max<int>(1, Ogre::PlatformInformation::getNumLogicalCores());
  Ogre::InstancingThreadedCullingMethod threadedCullingMethod =
      Ogre::INSTANCING_CULLING_SINGLETHREAD;
  if (numThreads > 1) threadedCullingMethod = Ogre::INSTANCING_CULLING_THREADED;
  ogreSceneMgr = ogreRoot->createSceneManager(Ogre::ST_GENERIC, numThreads,
                                              threadedCullingMethod);
#else
  ogreSceneMgr = ogreRoot->createSceneManager(Ogre::ST_GENERIC);
#endif

#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
  createCompositor();
#else
  createCamera();
  createViewport();
#endif

  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
  setupResources();
  createTerrain();
  createScene();
}

void OgreWindow::setupResources() {
  // Load resource paths from config file
  Ogre::ConfigFile cf;
  cf.load("resources.cfg");

  // Go through all sections & settings in the file
  Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

  Ogre::String secName, typeName, archName;
  while (seci.hasMoreElements()) {
    secName = seci.peekNextKey();
    Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
    Ogre::ConfigFile::SettingsMultiMap::iterator i;
    for (i = settings->begin(); i != settings->end(); ++i) {
      typeName = i->first;
      archName = i->second;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
      // OS X does not set the working directory relative to the app,
      // In order to make things portable on OS X we need to provide
      // the loading with it's own bundle path location
      Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
          Ogre::String(macBundlePath() + "/" + archName), typeName, secName);
#else
      Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
          archName, typeName, secName);
#endif
    }
  }

  // Load font
  /*
  Ogre::FontPtr mFont = Ogre::FontManager::getSingleton().create("FSEX300",
  "General"); mFont->setType(Ogre::FT_TRUETYPE);
  mFont->setSource("FSEX300.ttf");
  mFont->setTrueTypeSize(18);
  mFont->setTrueTypeResolution(96);
  mFont->addCodePointRange(Ogre::Font::CodePointRange(33, 255));
  */
  // Initialise, parse scripts etc
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void OgreWindow::createCamera() {
  ogreFirstCamera = ogreSceneMgr->createCamera("FirstView");
  ogreFirstCamera->setNearClipDistance(5);

  ogreThirdCamera = ogreSceneMgr->createCamera("ThirdView");
  ogreThirdCamera->setNearClipDistance(5);

  cameraNode = ogreSceneMgr->getRootSceneNode()->createChildSceneNode();
  cameraPitchNode = cameraNode->createChildSceneNode();

  cameraNode->setPosition(Ogre::Vector3(1445.0f, 10.0f, 910.0f));
  cameraNode->yaw(Ogre::Degree(180.0f));
}

void OgreWindow::createViewport() {
  activeCamera = ogreFirstCamera;
  ogreViewport = ogreRenderWindow->addViewport(activeCamera);
  ogreViewport->setBackgroundColour(Ogre::ColourValue(0.9f, 0.9f, 0.9f));
  activeCamera->setAspectRatio(Ogre::Real(ogreViewport->getActualWidth()) /
                               Ogre::Real(ogreViewport->getActualHeight()));
}

void OgreWindow::createTerrain() {
  ogreSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);
  ogreSceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));
  ogreSceneMgr->setSkyDome(true, "CloudySky");

  Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(
      Ogre::TFO_ANISOTROPIC);
  Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(8);

  Ogre::Light *terLight = ogreSceneMgr->createLight("terrainLight");
  terLight->setType(Ogre::Light::LT_DIRECTIONAL);
  terLight->setDirection(Ogre::Vector3(0.60f, -0.3f, 0.55f));
  terLight->setDiffuseColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f));
  terLight->setSpecularColour(Ogre::ColourValue(0.4f, 0.4f, 0.4f));

  ogreTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();
  ogreTerrainGlobals->setMaxPixelError(8);
  ogreTerrainGlobals->setLightMapDirection(terLight->getDerivedDirection());
  ogreTerrainGlobals->setCompositeMapDistance(3000);
  ogreTerrainGlobals->setCompositeMapAmbient(ogreSceneMgr->getAmbientLight());
  ogreTerrainGlobals->setCompositeMapDiffuse(terLight->getDiffuseColour());

  ogreTerrain = OGRE_NEW Ogre::Terrain(ogreSceneMgr);
  Ogre::Image img;
  img.load("terrain.png",
           Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  Ogre::Terrain::ImportData imp;
  imp.inputImage = &img;
  imp.terrainSize = static_cast<unsigned short>(img.getWidth());
  imp.worldSize = 8000;
  imp.inputScale = 600;
  imp.minBatchSize = 33;
  imp.maxBatchSize = 65;
  imp.layerList.resize(3);
  imp.layerList[1].worldSize = 100;
  imp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
  imp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
  imp.layerList[0].worldSize = 300;
  imp.layerList[0].textureNames.push_back(
      "growth_weirdfungus-03_diffusespecular.dds");
  imp.layerList[0].textureNames.push_back(
      "growth_weirdfungus-03_normalheight.dds");
  imp.layerList[2].worldSize = 200;
  imp.layerList[2].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
  imp.layerList[2].textureNames.push_back("dirt_grayrocky_normalheight.dds");
  ogreTerrain->prepare(imp);
  ogreTerrain->load();

  Ogre::TerrainLayerBlendMap *blendMap1 = ogreTerrain->getLayerBlendMap(1);
  float *pBlend1 = blendMap1->getBlendPointer();
  for (Ogre::uint16 y = 0; y < ogreTerrain->getLayerBlendMapSize(); ++y) {
    for (Ogre::uint16 x = 0; x < ogreTerrain->getLayerBlendMapSize(); ++x) {
      Ogre::Real terrainX, terrainY;
      blendMap1->convertImageToTerrainSpace(x, y, &terrainX, &terrainY);
      Ogre::Real height =
          ogreTerrain->getHeightAtTerrainPosition(terrainX, terrainY);
      if (height < 200) *pBlend1 = 1;
      pBlend1++;
    }
  }
  blendMap1->dirty();
  blendMap1->update();

  ogreTerrain->freeTemporaryResources();
}

void OgreWindow::createScene() {
  Ogre::SceneNode *node = ogreSceneMgr->createSceneNode("Node");
  ogreSceneMgr->getRootSceneNode()->addChild(node);

  Ogre::Entity *house = ogreSceneMgr->createEntity("House", "House.mesh");
  Ogre::SceneNode *houseNode = node->createChildSceneNode("HouseNode");
  houseNode->setScale(0.1f, 0.1f, 0.1f);
  houseNode->setPosition(Ogre::Vector3(1400.0f, 55.0f, 1020.0f));
  houseNode->attachObject(house);

  Ogre::Entity *knuckles =
      ogreSceneMgr->createEntity("Knuckles", "Knuckles.mesh");
  Ogre::SceneNode *knucklesNode = node->createChildSceneNode("KnucklesNode");
  knucklesNode->setPosition(Ogre::Vector3(1429.5f, 12.0f, 990.0f));
  knucklesNode->yaw(Ogre::Degree(180.0f));
  knucklesNode->setScale(0.15f, 0.15f, 0.15f);
  knucklesNode->attachObject(knuckles);

  avatar = ogreSceneMgr->createEntity("Avatar", "Sinbad.mesh");

  ogreListener = new OgreFrameListener();
  ogreRoot->addFrameListener(ogreListener);

  AnimationManager::getSingleton()->setAvatar(avatar);
  AnimationManager::getSingleton()->setInputSystem(inputSystem);

  Ogre::SceneNode *avatarNode =
      cameraPitchNode->createChildSceneNode("avatarNode");
  avatarNode->setPosition(Ogre::Vector3(0.0f, -5.0f, 0.0f));
  avatarNode->yaw(Ogre::Degree(180.0f));  // 3rd view avatar 180° offset
  avatarNode->attachObject(avatar);
  cameraPitchNode->attachObject(ogreFirstCamera);

  Ogre::SceneNode *nodeThirdView =
      cameraNode->createChildSceneNode("nodeThirdView");
  nodeThirdView->attachObject(ogreThirdCamera);
  nodeThirdView->setPosition(avatarNode->getPosition() +
                             Ogre::Vector3(0.0f, 4.0f, 26.0));
}

#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
void OgreWindow::createCompositor() {
  /*
  Example compositor
  Derive this class for your own purpose and overwite this function to have a
  working Ogre widget with your own compositor.
  */
  Ogre::CompositorManager2 *compMan = ogreRoot->getCompositorManager2();
  const Ogre::String workspaceName = "default scene workspace";
  const Ogre::IdString workspaceNameHash = workspaceName;
  compMan->createBasicWorkspaceDef(workspaceName, ogreBackground);
  compMan->addWorkspace(ogreSceneMgr, ogreRenderWindow, ogreCamera,
                        workspaceNameHash, true);
}
#endif

void OgreWindow::createAvatar(User *u) {
  if (ogreSceneMgr->hasEntity("Avatar_" + u->getNickname().toStdString()))
    ogreSceneMgr->destroyEntity("Avatar_" + u->getNickname().toStdString());
  u->avatar = ogreSceneMgr->createEntity(
      "Avatar_" + u->getNickname().toStdString(), "Sinbad.mesh");
  u->node = ogreSceneMgr->getRootSceneNode()->createChildSceneNode();
  u->node->attachObject(u->avatar);
  u->node->setPosition(Ogre::Vector3(0.0f, 0.0f, 0.0f));
}

void OgreWindow::destroyAvatar(User *u) {
  ogreSceneMgr->getRootSceneNode()->removeChild(u->node);
  ogreSceneMgr->destroyEntity(u->avatar);
}

void OgreWindow::moveAvatar(User *u) {
  AnimationManager *animMgr = AnimationManager::getSingleton();
  animMgr->moveAvatar(u);
}

void OgreWindow::posSend() {
  emit positionSend(cameraNode->getPosition().x, cameraNode->getPosition().y,
                    cameraNode->getPosition().z,
                    cameraNode->getOrientation().getPitch().valueRadians(),
                    cameraNode->getOrientation().getYaw().valueRadians());
}

void OgreWindow::setActiveCam(bool cam) {
  if (cam)
    activeCamera = ogreThirdCamera;
  else
    activeCamera = ogreFirstCamera;
  activeCamera->setAspectRatio(Ogre::Real(ogreViewport->getActualWidth()) /
                               Ogre::Real(ogreViewport->getActualHeight()));
  ogreViewport->setCamera(activeCamera);
}

void OgreWindow::moveCamera() {
  Ogre::Real pitchAngle =
      2 * Ogre::Degree(Ogre::Math::ACos(cameraPitchNode->getOrientation().w))
              .valueDegrees();
  Ogre::Real pitchAngleSign = cameraPitchNode->getOrientation().x;

  bool *ogreControls = inputSystem->getControls();

  // Should not be frame based
  if (ogreControls[CTRL])
    turbo = 5;
  else
    turbo = 1;
  if (ogreControls[UP])
    cameraNode->translate(
        turbo * -Ogre::Vector3(cameraNode->getOrientation().zAxis().x, 0,
                               cameraNode->getOrientation().zAxis().z));
  if (ogreControls[DOWN])
    cameraNode->translate(
        turbo * Ogre::Vector3(cameraNode->getOrientation().zAxis().x, 0,
                              cameraNode->getOrientation().zAxis().z));
  if (ogreControls[PLUS]) cameraNode->translate(turbo * Ogre::Vector3(0, 1, 0));
  if (ogreControls[MINUS])
    cameraNode->translate(turbo * Ogre::Vector3(0, -1, 0));

  if (ogreControls[LEFT]) {
    if (ogreControls[SHIFT])
      cameraNode->translate(
          turbo * Ogre::Vector3(-cameraNode->getOrientation().zAxis().z, 0,
                                cameraNode->getOrientation().zAxis().x));
    else
      cameraNode->yaw(turbo * Ogre::Radian(0.05f));
  }
  if (ogreControls[RIGHT]) {
    if (ogreControls[SHIFT])
      cameraNode->translate(
          turbo * Ogre::Vector3(cameraNode->getOrientation().zAxis().z, 0,
                                -cameraNode->getOrientation().zAxis().x));
    else
      cameraNode->yaw(turbo * Ogre::Radian(-0.05f));
  }
  if (ogreControls[PGUP] && (pitchAngle < 90.0f || pitchAngleSign < 0))
    cameraPitchNode->pitch(turbo * Ogre::Radian(0.05f));
  if (ogreControls[PGDOWN] && (pitchAngle < 90.0f || pitchAngleSign > 0))
    cameraPitchNode->pitch(turbo * Ogre::Radian(-0.05f));

  ogreThirdCamera->lookAt(cameraNode->getPosition());

  emit dispPosition(
      "x:" + QString::number(cameraNode->getPosition().x, 'f', 0) +
      " y:" + QString::number(cameraNode->getPosition().y, 'f', 0) +
      " z:" + QString::number(cameraNode->getPosition().z, 'f', 0));
}

void OgreWindow::render() {
  /**
   * How we tied in the render function for OGre3D with QWindow's render
   * function. This is what gets call repeatedly. Note that we don't call this
   * function directly; rather we use the renderNow() function to call this
   * method as we don't want to render the Ogre3D scene unless everything is set
   * up first. That is what renderNow() does.
   *
   *  Theoretically you can have one function that does this check but from my
   * experience it seems better to keep things separate and keep the render
   * function as simple as possible.
   */
  Ogre::WindowEventUtilities::messagePump();
  moveCamera();
  ogreRoot->renderOneFrame();
  if (ogreRenderWindow != nullptr) {
    QString str =
        QString::number(static_cast<double>(ogreRenderWindow->getStatistics().avgFPS), 'f', 1) +
        " FPS";
    emit dispAverageFps(str);
    ogreRenderWindow->update();
  }
}

void OgreWindow::renderLater() {
  /**
   * This function forces QWindow to keep rendering. Omitting this causes the
   * renderNow() function to only get called when the window is resized, moved,
   * etc. as opposed to all of the time; which is generally what we need.
   */
  if (!updatePending) {
    updatePending = true;
    QApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
  }
}

bool OgreWindow::event(QEvent *event) {
  /**
   * QWindow's "message pump". The base method that handles all QWindow events.
   * As you will see there are other methods that actually process the
   * keyboard/other events of Qt and the underlying OS.
   *
   * Note that we call the renderNow() function which checks to see if
   * everything is initialized, etc. before calling the render() function.
   */

  switch (event->type()) {
    case QEvent::UpdateRequest:
      updatePending = false;
      renderNow();
      return true;
    default:
      return QWindow::event(event);
  }
}

void OgreWindow::exposeEvent(QExposeEvent *event) {
  // Called after the QWindow is reopened or when the QWindow is first opened.
  Q_UNUSED(event);

  if (isExposed()) renderNow();
}

/**
 * The renderNow() function calls the initialize() function when needed and if
 * the QWindow is already initialized and prepped calls the render() method.
 */
void OgreWindow::renderNow() {
  if (!isExposed()) return;

  if (ogreRoot == nullptr) {
    initialize();
  }

  render();

  if (animating) renderLater();
}

/**
 * Our event filter; handles the resizing of the QWindow. When the size of the
 * QWindow changes note the call to the Ogre3D window and camera. This keeps the
 * Ogre3D scene looking correct.
 */
bool OgreWindow::eventFilter(QObject *target, QEvent *event) {
  if (target == this) {
    if (event->type() == QEvent::Resize) {
      if (isExposed() && ogreRenderWindow != nullptr) {
        ogreRenderWindow->resize(static_cast<unsigned int>(this->width()),
                                 static_cast<unsigned int>(this->height()));
        ogreRenderWindow->windowMovedOrResized();
      }
      if (activeCamera) {
        Ogre::Real aspectRatio =
            Ogre::Real(this->width()) / Ogre::Real(this->height());
        activeCamera->setAspectRatio(aspectRatio);
      }
    } else if (event->type() == QEvent::KeyPress) {
      QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
      inputSystem->handleKeys(keyEvent->key(), true);
    } else if (event->type() == QEvent::KeyRelease) {
      QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
      inputSystem->handleKeys(keyEvent->key(), false);
    }
  }

  return false;
}

/**
 * Function to keep track of when we should and shouldn't redraw the window; we
 * wouldn't want to do rendering when the QWindow is minimized. This takes care
 * of those scenarios.
 */
void OgreWindow::setAnimating(bool anim) {
  animating = anim;

  if (animating) renderLater();
}
