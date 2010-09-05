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

  // Create our drawing layer
  mLayer = mScreen->createLayer(0);

  // Create a rectangle with a pink border and no background.
  rect = mLayer->createRectangle(10,10,500,100);
  rect->border(5, Gorilla::Colours::Pink);
  rect->no_background();

  // Create a closed line loop.
  list = mLayer->createLineList();
  list->begin();
  list->position(100,100);
  list->position(100,200);
  list->position(50,200);
  list->end(true);

  // Create a polygon with coco as the background and a OrangeRed border
  poly = mLayer->createPolygon(250,250, 250, 8);
  poly->background_image("coco");
  poly->border_width(12);
  poly->border_colour(Gorilla::Colours::OrangeRed);

  // Create a QuadList with various bits in it.
  quads = mLayer->createQuadList();
  quads->begin();
  quads->gradient(100,100,  300,300, Gorilla::Gradient_NorthSouth, Gorilla::Colours::Aqua, Gorilla::Colours::Red);
  quads->border(100,100, 300,300, 10,Gorilla::Colours::Pink, Gorilla::Colours::Red, Gorilla::Colours::Blue, Gorilla::Colours::Green);
  quads->glyph(100,100, 'A', Gorilla::Colours::BlueViolet);
  quads->end();
   
   
   caption = mLayer->createCaption(10,10, "This is some left-aligned text");
   caption->size(500, 100);
   caption->colour(Gorilla::Colours::White);
   caption->background(Gorilla::Colours::Burlywood);
   
   caption = mLayer->createCaption(10,10, "This is some right-aligned text");
   caption->size(500, 100);
   caption->align(Gorilla::TextAlign_Right);
   caption->vertical_align(Gorilla::VerticalAlign_Middle);
   caption->colour(Gorilla::Colours::White);
   
   caption = mLayer->createCaption(10,10, "This is some centered-aligned text");
   caption->size(500, 100);
   caption->align(Gorilla::TextAlign_Centre);
   caption->vertical_align(Gorilla::VerticalAlign_Bottom);
   caption->colour(Gorilla::Colours::White);
   
   markup = mLayer->createMarkupText(50,250, "Wow this i%%s a t%2o%3n o%4f t%%ext h%Rere isn't it?\nI mean really!And this is some %MMonospaced Text!!11%M. Yeah! %:coco% yeah!\nThat was coco.");

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
   
     

   if (mTimer >= 1.0f)
   {
    //rect->background_colour(  Ogre::ColourValue(Ogre::Math::RangeRandom(0,1), 0,0, 1)  );
    //rect->colour(
    poly->sides(poly->sides() + 1);
    if (poly->sides() == 16)
     poly->sides(3);
    mTimer = 0;
   }
   
   if (mTimer2 >= 1.0f/60.0f)
   {
    poly->angle(poly->angle() + Ogre::Radian(0.015f));
    mTimer2 = 0;
   }
   
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


