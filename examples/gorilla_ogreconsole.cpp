#include <OGRE/Ogre.h>
#include <OIS/OIS.h>
 
#include "Gorilla.h"
#include "OgreConsoleForGorilla.h"

#include <sstream>

#pragma warning ( disable : 4244 )

bool quitApp = false;

void whoami(Ogre::StringVector&)
{
 OgreConsole::getSingleton().print("Your you!");
}

void version(Ogre::StringVector&)
{
 std::stringstream s;
 s << "Ogre " << OGRE_VERSION_MAJOR << "." << OGRE_VERSION_MINOR << "." << OGRE_VERSION_PATCH << " '" << OGRE_VERSION_NAME << "'";
 OgreConsole::getSingleton().print(s.str());
}

void make(Ogre::StringVector& vec)
{
 if (vec.size() == 1)
  OgreConsole::getSingleton().print("Make you what?");
 else
 {
  Ogre::String item_to_make = vec[1];
  Ogre::StringUtil::toLowerCase(item_to_make);
  if (item_to_make == "sandwich")
   OgreConsole::getSingleton().print("Make it yourself!");
  else if (item_to_make == "universe")
   OgreConsole::getSingleton().print("Boom!");
  else if (item_to_make == "ogre")
   OgreConsole::getSingleton().print("I need a Daddy and Mommy Ogre");
  else if (item_to_make == "gorilla")
   OgreConsole::getSingleton().print("He wouldn't like that");
  else
   OgreConsole::getSingleton().print("Go check your fridge.");
 }
}

void quit(Ogre::StringVector&)
{
 quitApp = true;
}

class App : public Ogre::FrameListener, public OIS::KeyListener, public OIS::MouseListener
{
  
 public:
  
  Gorilla::Silverback*    mGorilla;
  Gorilla::Screen*        mScreen;
  OgreConsole*            mConsole;
  
  Gorilla::Layer*         mFPSLayer;
  Gorilla::Caption*       mFPS;
  Ogre::Real              mTimer;
  
  App() : mNextUpdate(0), mTimer(0)
  {
   
   _makeOgre();
   _makeOIS();
   _makeScene();
   
   // Initialise Gorilla
   mGorilla = new Gorilla::Silverback();
   mGorilla->loadAtlas("dejavu");
   mScreen = mGorilla->createScreen(mViewport, "dejavu");
   
   mFPSLayer = mScreen->createLayer(14);
   mFPS = mFPSLayer->createCaption(14, 10,10, Ogre::StringUtil::BLANK);

   mConsole = new OgreConsole();
   mConsole->init(mScreen);
   mConsole->addCommand("whoami", whoami);
   mConsole->addCommand("version", version);
   mConsole->addCommand("make", make);
   mConsole->addCommand("quit", quit);
  }
  
 ~App()
  {
   delete mGorilla;
   delete mRoot;
  }
  
  bool frameStarted(const Ogre::FrameEvent& evt)
  {
   
   if (mWindow->isClosed() || quitApp)
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
    s << "FPS: " << mWindow->getLastFPS() << ", Batches: " << mRoot->getRenderSystem()->_getBatchCount() << "\n";
    mFPS->top(mViewport->getActualHeight() - 25);
    mFPS->text(s.str());
    
   }
   return true;
  }
  
  bool keyPressed( const OIS::KeyEvent &e )
  {
   mConsole->onKeyPressed(e);
   return true;
  }
  
  bool keyReleased( const OIS::KeyEvent &e )
  {
   if (e.key == OIS::KC_F1)
   {
    mConsole->setVisible(!mConsole->isVisible());
    return true;
   }
   return true;
  }
  
  bool mouseMoved( const OIS::MouseEvent &arg )
  {
   return true;
  }
  
  bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
  {
   return true;
  }
  
  bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
  {
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


