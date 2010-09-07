#include <OGRE/Ogre.h>
#include <OIS/OIS.h>
 
#include "Gorilla.h"

#include <sstream>

#pragma warning ( disable : 4244 )

class App : public Ogre::FrameListener, public OIS::KeyListener, public OIS::MouseListener
{
  
 public:
  
  Ogre::Real              mTimer, mTimer2;
  Gorilla::Silverback*    mSilverback;
  Gorilla::Screen*        mScreen;
  Gorilla::Layer*        mLayer;
  
  Gorilla::Polygon* poly;
  Gorilla::LineList*       list;
  Gorilla::Caption*        caption;
  Gorilla::Rectangle*      rect;
  Gorilla::QuadList*       quads;
  Gorilla::MarkupText*     markup;
  
  App() : mNextUpdate(0), mTimer(0), mTimer2(0)
  {
   
   _makeOgre();
   _makeOIS();

  // Create Silverback and load in dejavu
  mSilverback = new Gorilla::Silverback();
  mSilverback->loadAtlas("dejavu");
  mScreen = mSilverback->createScreen(mViewport, "dejavu");
  Ogre::Real vpW = mViewport->getActualWidth(), vpH = mViewport->getActualHeight();

  // Create our drawing layer
  mLayer = mScreen->createLayer(0);
  rect = mLayer->createRectangle(0,0, vpW, vpH);
  rect->background_gradient(Gorilla::Gradient_Diagonal, Gorilla::rgb(98,0,63), Gorilla::rgb(255,180,174));
  
  markup = mLayer->createMarkupText(9,5,5, "%@24%A Haiku\n%@14%Written by Betajaen%@9%\nSo many to choose from\nPretty typefaces on Ogre screen\nTime to update Git");
  
  caption = mLayer->createCaption(9, vpW - 55, 5, "9");
  caption->width(50);
  caption->align(Gorilla::TextAlign_Right);

  caption = mLayer->createCaption(14, vpW - 55, 18, "14");
  caption->width(50);
  caption->align(Gorilla::TextAlign_Right);

  caption = mLayer->createCaption(24, vpW - 55, 33, "24");
  caption->width(50);
  caption->align(Gorilla::TextAlign_Right);

  }
  
 ~App()
  {
   std::cout << "\n** Average FPS is " << mWindow->getAverageFPS() << "\n\n";
   delete mSilverback;
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
   mTimer2 += evt.timeSinceLastFrame;
   
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
  
  void  _makeOgre()
  {
   srand(time(0));
   
   mRoot = new Ogre::Root("","");
   mRoot->addFrameListener(this);
   
#if 1
  #ifdef _DEBUG
   mRoot->loadPlugin("RenderSystem_Direct3D9_d");
  #else
   mRoot->loadPlugin("RenderSystem_Direct3D9");
  #endif
#else
  #ifdef _DEBUG
   mRoot->loadPlugin("RenderSystem_GL_d.dll");
  #else
   mRoot->loadPlugin("RenderSystem_GL.dll");
  #endif
#endif
   
   mRoot->setRenderSystem(mRoot->getAvailableRenderers()[0]);
   
   Ogre::ResourceGroupManager* rgm = Ogre::ResourceGroupManager::getSingletonPtr();
  	rgm->addResourceLocation(".", "FileSystem");
   
   mRoot->initialise(false);
   
   mWindow = mRoot->createRenderWindow("Gorilla", 1024, 768, false);
   mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
   mCamera = mSceneMgr->createCamera("Camera");
   mViewport = mWindow->addViewport(mCamera);
   mViewport->setBackgroundColour(Gorilla::webcolour(Gorilla::Colours::FireBrick));
   
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


