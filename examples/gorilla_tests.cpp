#include <OGRE/Ogre.h>
#include <OIS/OIS.h>
 
#include "Gorilla.h"

#include <sstream>

#pragma warning ( disable : 4244 )

/// UNIT TEST SETUP CODE
class UnitTest{public:UnitTest(Gorilla::Silverback* sb, Ogre::Viewport* vp, Ogre::SceneManager* sm):mSilverback(sb),mViewport(vp),
mSceneMgr(sm){}virtual ~UnitTest(){}virtual bool periodic(Ogre::Real){return true;}Gorilla::Silverback* mSilverback;Ogre::Viewport* mViewport;
Ogre::SceneManager* mSceneMgr;};
#define BEGIN_TEST(NAME) class NAME :public UnitTest{public:NAME(Gorilla::Silverback* sb, Ogre::Viewport* vp, Ogre::SceneManager* sm):\
UnitTest(sb,vp,sm){start();}~NAME(){stop();}
#define END_TEST(NAME) };
#define TEST_START bool start()
#define TEST_STOP bool stop()
#define TEST_PERIODIC bool periodic(Ogre::Real timeElapsed)
#define TEST_CONTINUE return true;
#define TEST_FAIL(DESCRIPTION) std::cout << "\n\n\n---\n"DESCRIPTION << "\n---\n\n\n";return false;
/////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
// Rectangle Tests
/////////////////////////////////////////////////////////////////////////////////////////

BEGIN_TEST(RectangleTest)
  
  Gorilla::Screen*    mScreen;
  Gorilla::Layer*     mLayer;
  Gorilla::Rectangle* mRectangle;
  
  TEST_START
  {
   mScreen = mSilverback->createScreen(mViewport, "dejavu");
   TEST_CONTINUE
  }

  TEST_STOP
  {
   TEST_CONTINUE
  }

  TEST_PERIODIC
  {
   TEST_CONTINUE
  }

END_TEST(RectangleTest)

/////////////////////////////////////////////////////////////////////////////////////////






















void registerTests(std::map<Ogre::uint, UnitTest*>& tests, Gorilla::Silverback* sb, Ogre::Viewport* vp, Ogre::SceneManager* sm)
{
 tests[0] = new RectangleTest(sb, vp, sm);
}





class App : public Ogre::FrameListener, public OIS::KeyListener, public OIS::MouseListener
{
  
 public:
  
  std::map<Ogre::uint, UnitTest*> mTests;
  UnitTest*                       mCurrentTest;
  Gorilla::Silverback*            mSilverback;
  
  App() : mCurrentTest(0)
  {
   
   _makeOgre();
   _makeOIS();
   
   // Create Silverback and load in dejavu
   mSilverback = new Gorilla::Silverback();
   mSilverback->loadAtlas("dejavu");
   
   registerTests(mTests, mSilverback, mViewport, mSceneMgr);
   
  }
  
 ~App()
  {
   delete mSilverback;
   delete mRoot;
  }
  
  void startTest(Ogre::uint index)
  {
  }
  
  void stopTest()
  {
  }
   
  void updateTest(Ogre::Real)
  {
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
  
  void  _makeOgre()
  {
   srand(time(0));
   
   mRoot = new Ogre::Root("","");
   mRoot->addFrameListener(this);
   
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
   mRoot->loadPlugin(OGRE_RENDERER);
#else
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
#endif
   
   mRoot->setRenderSystem(mRoot->getAvailableRenderers()[0]);
   
   Ogre::ResourceGroupManager* rgm = Ogre::ResourceGroupManager::getSingletonPtr();
  	rgm->addResourceLocation(".", "FileSystem");
   
   mRoot->initialise(false);
   
   mWindow = mRoot->createRenderWindow("Gorilla - Press 0 to 9 for unit tests. C to Clear.", 1024, 768, false);
   mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
   mCamera = mSceneMgr->createCamera("Camera");
   mViewport = mWindow->addViewport(mCamera);
   mViewport->setBackgroundColour(Ogre::ColourValue::Black);
   
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
  OIS::InputManager*      mInputManager;
  OIS::Keyboard*          mKeyboard;
  OIS::Mouse*             mMouse;
  
};

int main()
{
 App* app  = new App();
 app->mRoot->startRendering();
 delete app;

 return 0;
}


