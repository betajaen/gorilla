#include <OGRE/Ogre.h>
#include "Gorilla.h"
#include <sstream>

class App : public Ogre::FrameListener
{
  
 public:
  
  App()
  {
   
   mRoot = new Ogre::Root("","");
   mRoot->addFrameListener(this);
  #ifdef _DEBUG
   mRoot->loadPlugin("RenderSystem_Direct3D9_d");
  #else
   mRoot->loadPlugin("RenderSystem_Direct3D9");
  #endif
   
   mRoot->setRenderSystem(mRoot->getAvailableRenderers()[0]);
   
   mRoot->initialise(false);
   
   mWindow = mRoot->createRenderWindow("Gorilla", 1024, 768, false);
   mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
   mCamera = mSceneMgr->createCamera("Camera");
   mViewport = mWindow->addViewport(mCamera);
   
   // Initialise Gorilla
   mScreen = new Gorilla::Screen(mViewport);
   mScreen->createRectangle(Gorilla::Pixel(0,0), Gorilla::Size(100,768), Gorilla::Colour(Gorilla::Colour::HotPink),50);
   mStatsText = mScreen->createMonoText(Gorilla::Pixel(0,0), 32, 1, "FPS:0 Batches:0");
   mNextUpdate = 0;
  }
  
 ~App()
  {
   delete mScreen;
   delete mRoot;
  }
  
  bool frameStarted(const Ogre::FrameEvent& evt)
  {
   
   if (mWindow->isClosed())
    return false;
   
   mNextUpdate+=evt.timeSinceLastFrame;
   if (mNextUpdate >= 0.16667f) // Only update every 1/60th of a second.
   {
    std::stringstream s;
    s << "Batches " << mWindow->getBatchCount() << " FPS " << mWindow->getAverageFPS();
    
    mStatsText->setText(s.str());
    mNextUpdate = 0;
   }
   return true;
  }
  
  
  Ogre::Root*          mRoot;
  Ogre::RenderWindow*  mWindow;
  Ogre::Viewport*      mViewport;
  Ogre::SceneManager*  mSceneMgr;
  Ogre::Camera*        mCamera;
  
  Gorilla::Screen*     mScreen;
  Gorilla::MonoText*   mStatsText;
  Ogre::Real           mNextUpdate;
};

void main()
{
 App* app = new App();
 app->mRoot->startRendering();
 delete app;
}
