#include <OGRE/Ogre.h>
#include "Gorilla.h"
#include <sstream>

#pragma warning ( disable : 4244 )

class App : public Ogre::FrameListener
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
   mViewport->setBackgroundColour(Ogre::ColourValue(0.0f,0.0f,0.0f,1.0f));
   
   rgm->initialiseAllResourceGroups();
   
   // Initialise Gorilla
   mGorilla = new Gorilla::Silverback();
   mGorilla->loadAtlas("dejavu");
   mScreen = mGorilla->createScreen(mViewport, "dejavu");
   for (int i=0;i < 80;i++)
   {
    makeRandomRectangle();
   }
   
   mScreen->createSprite(-49,-49, "ogrehead");
   mScreen->createText(10,10, "Gorilla Text Example\n"
"This text feature of Gorilla can do amazing things, such as:\n"
"- It has wonderful text rendering with kerning from a simple TextureAtlas.\n"
"- Change the %7colo%3ur %4of %6te%5xt%0 at a whim.\n"
"- And the amazing feature to insert sprites %:ogrehead% directly into the text, just like HTML!\n"
"These are just some of the many things that Gorilla can do.");
  
   mFPSMonitor = mScreen->createText(1,-25, "FPS: 0");
   mFPSMonitorBackground = mScreen->createRectangle(1,-25, 100, 25, Ogre::ColourValue::Red);
  }
  
 ~App()
  {
   std::cout << "** Batch Count: " << mWindow->getBatchCount() << "\n";
   std::cout << "** Triangle Count: " << mWindow->getTriangleCount() << "\n";
   std::cout << "** Average FPS: " << mWindow->getAverageFPS() << "\n";
   delete mGorilla;
   delete mRoot;
  }
  
  bool frameStarted(const Ogre::FrameEvent& evt)
  {
   
   if (mWindow->isClosed())
    return false;
   
   mNextUpdate+=evt.timeSinceLastFrame;
   if (mNextUpdate >= 1.0f/60.0f) // Only update every 1/60th of a second.
   {
    std::stringstream s;
    s << "FPS: " << mWindow->getLastFPS() << ", " << "Triangles:" << mWindow->getTriangleCount() << "\n";
    mScreen->setTextCaption(mFPSMonitor, s.str());
    int w = 0, h = 0;
    mScreen->getTextSize(mFPSMonitor, w,h);
    
    mNextUpdate = 0;
   }
   
   return true;
  }
  
   void makeRandomRectangle()
  {
      
   int x = Ogre::Math::RangeRandom(0, mViewport->getActualWidth());
   int y = Ogre::Math::RangeRandom(0, mViewport->getActualHeight());
   int w = Ogre::Math::RangeRandom(0, mViewport->getActualWidth());
   int h = Ogre::Math::RangeRandom(0, mViewport->getActualHeight());
   int d = Ogre::Math::RangeRandom(-100,0);
   
   mScreen->createRectangle(x,y,w,h, Gorilla::random_colour(0.25f), d);
  }

  Ogre::Root*             mRoot;
  Ogre::RenderWindow*     mWindow;
  Ogre::Viewport*         mViewport;
  Ogre::SceneManager*     mSceneMgr;
  Ogre::Camera*           mCamera;
  Ogre::Real              mNextUpdate;
  Gorilla::Silverback*    mGorilla;
  Gorilla::Screen*        mScreen;
  size_t                  mFPSMonitor, mFPSMonitorBackground;
  
};

void main()
{
 App* app = new App();
 app->mRoot->startRendering();
 delete app;
}


