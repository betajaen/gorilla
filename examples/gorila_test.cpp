#include <OGRE/Ogre.h>
#include <OIS/OIS.h>
 
#include "Gorilla.h"

#include <sstream>

#pragma warning ( disable : 4244 )

class App : public Ogre::FrameListener, public OIS::KeyListener, public OIS::MouseListener
{
  
 public:
  
  App() : mNextUpdate(0)
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
   
   // Initialise Gorilla
   mGorilla = new Gorilla::Silverback();
   mGorilla->loadAtlas("dejavu");
   mScreen = mGorilla->createScreen(mViewport, "dejavu");
   mCanvas = mScreen->createCanvas();
   mCanvas->addRectangle(10,10, 128,128, Gorilla::rgb(255,0,0,255));
   size_t id = mCanvas->addRectangle(10+128,10, 128,128, Gorilla::rgb(0,255,0,255));
   mCanvas->setRectangleColour(id, Gorilla::rgb(255,0,0,255), Gorilla::rgb(0,255,0,255), Gorilla::rgb(0,0,255,255), Gorilla::rgb(0,0,0,255));
   mCanvas->setRectangleSize(id, 256,256);
   mCanvas->setRectangleBackground(id, "ogrehead");
   
   Gorilla::Canvas* second = mScreen->createCanvas(15);
   second->addRectangle(5,5, 400,400, Gorilla::rgb(0,255,0,150));


   Gorilla::Canvas* third = mScreen->createCanvas(4);
   third->addRectangle(5,5, 600,200, Gorilla::rgb(0,0,255,150));

  }
  
 ~App()
  {
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
   
   return true;
  }
  
  bool keyPressed( const OIS::KeyEvent &e )
  {
   return true;
  }
  
  bool keyReleased( const OIS::KeyEvent &e )
  {
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
  
  Ogre::Root*             mRoot;
  Ogre::RenderWindow*     mWindow;
  Ogre::Viewport*         mViewport;
  Ogre::SceneManager*     mSceneMgr;
  Ogre::Camera*           mCamera;
  Ogre::Real              mNextUpdate;
  OIS::InputManager*      mInputManager;
  OIS::Keyboard*          mKeyboard;
  OIS::Mouse*             mMouse;
  
  Gorilla::Silverback*    mGorilla;
  Gorilla::Screen*        mScreen;
  Gorilla::Canvas*        mCanvas;
  
};

void main()
{
 App* app  = new App();
 app->mRoot->startRendering();
 delete app;
}


