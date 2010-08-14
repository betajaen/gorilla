#include <OGRE/Ogre.h>
#include "Gorilla.h"
#include <sstream>

#pragma warning ( disable : 4244 )

class App : public Ogre::FrameListener
{
  
 public:
  
  App() : mNextUpdate(0)
  {
   
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
   
   rgm->initialiseAllResourceGroups();
   
   // Initialise Gorilla
   mTextureAtlas = new Gorilla::TextureAtlas("dejavu.gorilla");
   mScreen = new Gorilla::Screen(mViewport, mTextureAtlas);
   
   for (int i=0;i < 80;i++)
   {
    makeRandomSprite();
    makeRandomRectangle();
   }
   
   mScreen->addText(10,10, "ABCDEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n01234567890\nThese tutorials are where you should start learning Ogre, but they\n assume you know nothing about Ogre, and step you through the\n process of using the engine one concept at a time.\n Otherwise see Setting up an Application for a general guidance on how to set up your own project.");
  }
  
 ~App()
  {
   std::cout << "** Batch Count: " << mWindow->getBatchCount() << "\n";
   std::cout << "** Triangle Count: " << mWindow->getTriangleCount() << "\n";
   std::cout << "** Average FPS: " << mWindow->getAverageFPS() << "\n";
   
   delete mScreen;
   delete mTextureAtlas;
   delete mRoot;
  }
  
  bool frameStarted(const Ogre::FrameEvent& evt)
  {
   
   if (mWindow->isClosed())
    return false;
   
   mNextUpdate+=evt.timeSinceLastFrame;
   if (mNextUpdate >= 1.0f/10.0f) // Only update every 1/10th of a second.
   {
    std::stringstream s;
    s << "FPS: " << mWindow->getLastFPS() << " Batches:" << mWindow->getBatchCount();
    
    randomiseExistingRectangle();
    randomiseExistingSprite();
    mNextUpdate = 0;
   }
   return true;
  }
  
  void makeRandomSprite()
  {
   
   int x = Ogre::Math::RangeRandom(0, mViewport->getActualWidth());
   int y = Ogre::Math::RangeRandom(0, mViewport->getActualHeight());
   int d = Ogre::Math::RangeRandom(-100, 100);
   
   mSprites.push_back(   mScreen->addSprite(x,y, 0, d)   );
   
  }
  
  void makeRandomRectangle()
  {
   int x = Ogre::Math::RangeRandom(0, mViewport->getActualWidth());
   int y = Ogre::Math::RangeRandom(0, mViewport->getActualHeight());
   int w = Ogre::Math::RangeRandom(0, mViewport->getActualWidth());
   int h = Ogre::Math::RangeRandom(0, mViewport->getActualHeight());
   int d = Ogre::Math::RangeRandom(-100, 100);
   Gorilla::Colour colour = Gorilla::Colour::Random(0.5f);
   
   mRectangles.push_back(   mScreen->addRectangle(x,y, w,h, d, colour)   );
  }

  void randomiseExistingRectangle()
  {
   
   Ogre::uint rectId = mRectangles[size_t(Ogre::Math::RangeRandom(0, mRectangles.size()))];
   
   int x = Ogre::Math::RangeRandom(0, mViewport->getActualWidth());
   int y = Ogre::Math::RangeRandom(0, mViewport->getActualHeight());
   int w = Ogre::Math::RangeRandom(0, mViewport->getActualWidth());
   int h = Ogre::Math::RangeRandom(0, mViewport->getActualHeight());
   int d = Ogre::Math::RangeRandom(-100, 100);
   
   Gorilla::Colour colour = Gorilla::Colour::Random(0.5f);
   
   mScreen->updateRectangle(rectId, x,y, w,h,d,colour);
   
  }

  void randomiseExistingSprite()
  {
   
   Ogre::uint spriteId = mSprites[size_t(Ogre::Math::RangeRandom(0, mSprites.size()))];
   
   int x = Ogre::Math::RangeRandom(0, mViewport->getActualWidth());
   int y = Ogre::Math::RangeRandom(0, mViewport->getActualHeight());
   int d = Ogre::Math::RangeRandom(-100, 100);
   
   mScreen->updateSprite(spriteId, x, y, d);
   
  }
  
  Ogre::Root*             mRoot;
  Ogre::RenderWindow*     mWindow;
  Ogre::Viewport*         mViewport;
  Ogre::SceneManager*     mSceneMgr;
  Ogre::Camera*           mCamera;
  
  Gorilla::TextureAtlas*  mTextureAtlas;
  Gorilla::Screen*        mScreen;
  
  Ogre::Real              mNextUpdate;
  std::vector<Ogre::uint> mSprites;
  std::vector<Ogre::uint> mRectangles;

  
};

void main()
{
 App* app = new App();
 app->mRoot->startRendering();
 delete app;
}
