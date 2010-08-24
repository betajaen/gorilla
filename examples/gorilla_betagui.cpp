#include <OGRE/Ogre.h>
#include <OIS/OIS.h>
 
#include "Gorilla.h"
#include "BetaGUI4.h"

#include <sstream>

#pragma warning ( disable : 4244 )

class App : public Ogre::FrameListener, public OIS::KeyListener, public OIS::MouseListener, public BetaGUI::Callback
{
  
 public:
  
  Gorilla::Silverback*    mGorilla;
  Gorilla::Screen*        mScreen;
  BetaGUI::GUI*           mGUI;
  
  Gorilla::Text*          mFPS;
  Ogre::Real              mTimer;
  
  App() : mNextUpdate(0), mTimer(0)
  {
   
   _makeOgre();
   _makeOIS();
   _makeScene();
   
   // Initialise Gorilla
   mGorilla = new Gorilla::Silverback();
   mGorilla->loadAtlas("dejavu");
   
   // FPS Counter (on a seperate screen)
   Gorilla::Screen* fpsScreen = mGorilla->createScreen(mViewport, "dejavu");
   mFPS = fpsScreen->createText(1, mViewport->getActualHeight() - 24, "0 (0)", 15);

   // Do BetaGUI!

   mScreen = mGorilla->createScreen(mViewport, "dejavu");
   mGUI = new BetaGUI::GUI(mScreen, this);
   BetaGUI::Window* window = mGUI->createWindow("Hello! This is a lot of text here.", Ogre::Vector2(100,100));
   window->createButton(Ogre::Vector2(5,15), "This is a button");
   window->createTextBox(Ogre::Vector2(5,40), "1234");
   window->createProgress(Ogre::Vector2(5,70), 0.35);
   window->createLabel(Ogre::Vector2(5,100), "This is a label");
  }
  
 ~App()
  {
   delete mGUI;
   delete mGorilla;
   delete mRoot;
  }
  
  bool frameStarted(const Ogre::FrameEvent& evt)
  {
   
   if (mWindow->isClosed())
    return false;
   
   mKeyboard->capture();
   if (mKeyboard->isKeyDown(OIS::KC_ESCAPE))
     return false;
   mMouse->capture();
   
   mTimer += evt.timeSinceLastFrame;
   if (mTimer > 1.0f / 60.0f)
   {
    mTimer = 0;
    std::stringstream s;
    s << "F " << int(mWindow->getLastFPS()) << " B " << mRoot->getRenderSystem()->_getBatchCount() << " T " << mWindow->getTriangleCount();
    mFPS->setTop(mViewport->getActualHeight() - 24);
    mFPS->setText(s.str());
    
   }

   return true;
  }
  
  bool keyPressed( const OIS::KeyEvent &e )
  {
   if (e.key == OIS::KC_RETURN || e.key == OIS::KC_NUMPADENTER)
    mGUI->checkKey('\n');
   else
    mGUI->checkKey(e.text);
   return true;
  }
  
  bool keyReleased( const OIS::KeyEvent &e )
  {
   return true;
  }
  
  bool mouseMoved( const OIS::MouseEvent &arg )
  {
   mGUI->checkMouse(arg.state.X.abs, arg.state.Y.abs, arg.state.buttonDown(OIS::MB_Left));
   return true;
  }
  
  bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
  {
   mGUI->checkMouse(arg.state.X.abs, arg.state.Y.abs, arg.state.buttonDown(OIS::MB_Left));
   return true;
  }
  
  bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
  {
   mGUI->checkMouse(arg.state.X.abs, arg.state.Y.abs, arg.state.buttonDown(OIS::MB_Left));
   return true;
  }
  
  void  _makeOgre()
  {
   srand(time(0));
   
   mRoot = new Ogre::Root("","");
   mRoot->addFrameListener(this);
  #ifdef _DEBUG
   mRoot->loadPlugin("RenderSystem_Direct3D9_d");
  #else
   mRoot->loadPlugin("RenderSystem_Direct3D9");
  #endif
   
   mRoot->setRenderSystem(mRoot->getAvailableRenderers()[0]);
   
   Ogre::ResourceGroupManager* rgm = Ogre::ResourceGroupManager::getSingletonPtr();
  	rgm->addResourceLocation(".", "FileSystem");
   
   mRoot->initialise(false);
   
   mWindow = mRoot->createRenderWindow("Gorilla", 1024, 768, false);
   mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
   mCamera = mSceneMgr->createCamera("Camera");
   mViewport = mWindow->addViewport(mCamera);
   mViewport->setBackgroundColour(Gorilla::rgb(13, 13, 13));
   
   rgm->initialiseAllResourceGroups();
  }
  
  void  _makeOIS()
  {
   // Initialise OIS
   OIS::ParamList pl;
   size_t windowHnd = 0;
   std::ostringstream windowHndStr;
   mWindow->getCustomAttribute("WINDOW", &windowHnd);
   windowHndStr << windowHnd;
   pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
   mInputManager = OIS::InputManager::createInputSystem( pl );
   mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
   mKeyboard->setEventCallback(this);
   mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));
   mMouse->setEventCallback(this);
   mMouse->getMouseState().width = mViewport->getActualWidth();
   mMouse->getMouseState().height = mViewport->getActualHeight();
  }
  
  void  _makeScene()
  {
   mCamera->setNearClipDistance(1);
   mCamera->setFarClipDistance(1000);
   // Create the texture
   Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().createManual(
       "DynamicTexture", // name
       Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
       Ogre::TEX_TYPE_2D,      // type
       128, 128,         // width & height
       0,                // number of mipmaps
       Ogre::PF_BYTE_BGRA,     // pixel format
       Ogre::TU_DEFAULT);      // usage; should be TU_DYNAMIC_WRITE_ONLY_DISCARDABLE for
                         // textures updated very often (e.g. each frame)
    
   // Get the pixel buffer
   Ogre::HardwarePixelBufferSharedPtr pixelBuffer = texture->getBuffer();
    
   // Lock the pixel buffer and get a pixel box
   pixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL); // for best performance use HBL_DISCARD!
   const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();
    
   Ogre::uint8* pDest = static_cast<Ogre::uint8*>(pixelBox.data);
    
   // Fill in some pixel data. This will give a semi-transparent blue,
   // but this is of course dependent on the chosen pixel format.
   for (size_t j = 0; j < 128; j++)
       for(size_t i = 0; i < 128; i++)
       {
        *pDest++ = 82  + (Ogre::Math::Sin(i) + Ogre::Math::Cos(j) * 10.5);
        *pDest++ = 45  + (Ogre::Math::Cos(i) + Ogre::Math::Sin(j) * 10.5);
        *pDest++ = 128 + (Ogre::Math::Tan(i) + Ogre::Math::Cos(j) * 10.5);
        *pDest++ = 255; // A
       }
    
   // Unlock the pixel buffer
   pixelBuffer->unlock();
    
   // Create a material using the texture
   Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(
       "DynamicTextureMaterial", // name
       Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    
   Ogre::TextureUnitState* tus = material->getTechnique(0)->getPass(0)->createTextureUnitState("DynamicTexture");
   tus->setScrollAnimation(0.005f, 0.0025f);
   tus->setRotateAnimation(0.009f);
   material->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
   
   Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
   
    Ogre::MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        plane, 1500, 1500, 1, 1, true, 1, 1, 1, Ogre::Vector3::UNIT_Z);
    
    Ogre::Entity* entGround = mSceneMgr->createEntity("GroundEntity", "ground");
    mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(entGround);
    
    entGround->setMaterialName("DynamicTextureMaterial");
    entGround->setCastShadows(false);

   mCamera->setPosition(25,100,30);
   mCamera->lookAt(0,0,0);
  }
  
  Ogre::Root*             mRoot;
  Ogre::RenderWindow*     mWindow;
  Ogre::Viewport*         mViewport;
  Ogre::SceneManager*     mSceneMgr;
  Ogre::Camera*           mCamera;
  Ogre::Real              mNextUpdate;
  OIS::InputManager*      mInputManager;
  OIS::Keyboard*          mKeyboard;
  OIS::Mouse*             mMouse;
  
};

void main()
{
 App* app  = new App();
 app->mRoot->startRendering();
 delete app;
}


