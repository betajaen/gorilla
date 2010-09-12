#include <OGRE/Ogre.h>
#include <OIS/OIS.h>
 
#include "Gorilla.h"

#include <sstream>

#pragma warning ( disable : 4244 )

struct Button
{
  
  Button(Ogre::Real x, Ogre::Real y, const Ogre::String& text, Gorilla::Layer* layer)
  : hovered(false)
  {
   caption = layer->createCaption(14, x,y, text);
   caption->size(64,25);
   caption->align(Gorilla::TextAlign_Centre);
   caption->vertical_align(Gorilla::VerticalAlign_Middle);
   caption->background(Gorilla::rgb(255,255,255,32));
  }
  
  bool isOver(const Ogre::Vector2& pos)
  {
   bool result = caption->intersects(pos);
   if (result && !hovered)
    caption->background(Gorilla::rgb(255,255,255,128));
   else if (!result && hovered)
    caption->background(Gorilla::rgb(255,255,255,32));
   hovered = result;
   return result;
  }
  
  bool                 hovered;
  Gorilla::Caption*    caption;
  
};

struct D3Panel
{
 
  D3Panel(Gorilla::Silverback* silverback, Ogre::SceneManager* sceneMgr, const Ogre::Vector2& size)
  : mSize(size)
  {
   
   mScreen = silverback->createScreenRenderable(Ogre::Vector2(mSize.x,mSize.y), "dejavu");
   mNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
   mNode->attachObject(mScreen);
   
   mGUILayer = mScreen->createLayer(0);
   mBackground = mGUILayer->createRectangle(0,0, mSize.x * 100, mSize.y * 100);
   mBackground->background_gradient(Gorilla::Gradient_NorthSouth, Gorilla::rgb(94,97,255,5), Gorilla::rgb(94,97,255,50));
   mBackground->border(2, Gorilla::rgb(255,255,255,150));
   
   mMousePointerLayer = mScreen->createLayer(15);
   mMousePointer = mMousePointerLayer->createRectangle(0,0,10,18);
   mMousePointer->background_image("mousepointer");
   
  }
  
  Button* check(const Ogre::Ray& ray, bool& isOver)
  {
   
   isOver = false;
   
   Ogre::Matrix4 transform;
   transform.makeTransform(mNode->getPosition(), mNode->getScale(), mNode->getOrientation());
   
   Ogre::AxisAlignedBox aabb = mScreen->getBoundingBox();
   aabb.transform(transform);
   std::pair<bool, Ogre::Real> result = Ogre::Math::intersects(ray, aabb);

   if (result.first == false)
     return 0;
   
   Ogre::Vector3 a,b,c,d;
   Ogre::Vector2 halfSize = mSize * 0.5f;
   a = transform * Ogre::Vector3(-halfSize.x,-halfSize.y,0);
   b = transform * Ogre::Vector3( halfSize.x,-halfSize.y,0);
   c = transform * Ogre::Vector3(-halfSize.x, halfSize.y,0);
   d = transform * Ogre::Vector3( halfSize.x, halfSize.y,0);
    
   result = Ogre::Math::intersects(ray, c, b, a);
   if (result.first == false)
    result = Ogre::Math::intersects(ray, c, d, b);
   if (result.first == false)
    return 0;
   
   if (result.second > 6.0f)
    return 0;
   
   isOver = true;
   
   Ogre::Vector3 hitPos = ( ray.getOrigin() + (ray.getDirection() * result.second) );
   Ogre::Vector3 localPos = transform.inverse() * hitPos;
   localPos.x += halfSize.x;
   localPos.y -= halfSize.y;
   localPos.x *= 100;
   localPos.y *= 100;
   
   // Cursor clip
   localPos.x = Ogre::Math::Clamp<Ogre::Real>(localPos.x, 0, (mSize.x * 100) - 10);
   localPos.y = Ogre::Math::Clamp<Ogre::Real>(-localPos.y, 0, (mSize.y * 100) - 18);
   
   mMousePointer->position(localPos.x, localPos.y);

   for (size_t i=0;i < mButtons.size();i++)
   {
    if (mButtons[i]->isOver(mMousePointer->position()))
     return mButtons[i];
   }
   
   return 0;
  }
  
  Gorilla::Caption* makeCaption(Ogre::Real x, Ogre::Real y, const Ogre::String& text)
  {
   return mGUILayer->createCaption(14, x, y, text);
  }
  
  Button* makeButton(Ogre::Real x, Ogre::Real y, const Ogre::String& text)
  {
   Button* button = new Button(x,y, text, mGUILayer);
   mButtons.push_back(button);
   return button;
  }
  
  Gorilla::ScreenRenderable*     mScreen;
  Ogre::SceneNode*     mNode;
  Gorilla::Layer*      mGUILayer, *mMousePointerLayer;
  Gorilla::Rectangle*  mBackground, *mMousePointer;
  Ogre::Vector2        mSize;
  
  std::vector<Button*> mButtons;
  
};

class App : public Ogre::FrameListener, public OIS::KeyListener, public OIS::MouseListener
{
  
 public:
  
  Ogre::Real              mTimer, mTimer2;
  Gorilla::Silverback*    mSilverback;
  Gorilla::Screen*        mHUD, *mControlPanel;
  Gorilla::Layer*         mHUDLayer;
  Gorilla::Layer*         mCrosshairLayer;
  Gorilla::Rectangle*     mCrosshair;
  D3Panel*                mPowerPanel;
  Gorilla::Rectangle*     mPowerValue, *mPowerValueBackground;
  Button*                 mPowerUpButton;
  Button*                 mPowerDownButton;
  Ogre::Real              mBasePower;
  Ogre::SceneNode*        mNode;
  
  App() : mNextUpdate(0), mTimer(0), mTimer2(0), mBasePower(150)
  {
   
   _makeOgre();
   _makeOIS();
   
   // Create Silverback and load in dejavu
   mSilverback = new Gorilla::Silverback();
   mSilverback->loadAtlas("dejavu");
   
   createHUD();
   createControlPanel();
   
  }
  
  void createHUD()
  {
   mHUD = mSilverback->createScreen(mViewport, "dejavu");
   mHUDLayer = mHUD->createLayer();
   
   Gorilla::Caption* fakeHealth = mHUDLayer->createCaption(24, 0, 0, "+ 100");
   fakeHealth->width(mViewport->getActualWidth()-16);
   fakeHealth->height(mViewport->getActualHeight()-4);
   fakeHealth->align(Gorilla::TextAlign_Right);
   fakeHealth->vertical_align(Gorilla::VerticalAlign_Bottom);
   
   Gorilla::Caption* fakeAmmo = mHUDLayer->createCaption(24, 16, 0, ": 60");
   fakeAmmo->width(mViewport->getActualWidth());
   fakeAmmo->height(mViewport->getActualHeight()-4);
   fakeAmmo->vertical_align(Gorilla::VerticalAlign_Bottom);
   
   mCrosshairLayer = mHUD->createLayer();
   mCrosshair = mCrosshairLayer->createRectangle((mViewport->getActualWidth() * 0.5f) - 11, (mViewport->getActualHeight() * 0.5f) - 11, 22, 22);
   mCrosshair->background_image("crosshair");
   
  }
  
  void createControlPanel()
  {
   mPowerPanel = new D3Panel(mSilverback, mSceneMgr, Ogre::Vector2(4,1));
   mPowerPanel->mNode->setPosition(Ogre::Vector3(0,1.5f,-10));
   Gorilla::Caption* caption = mPowerPanel->makeCaption(0,4, "Power Level");
   caption->width(400);
   caption->align(Gorilla::TextAlign_Centre);

   mPowerValueBackground = mPowerPanel->mGUILayer->createRectangle(10,35,380,10);
   mPowerValueBackground->background_colour(Gorilla::rgb(255,255,255,100));

   mPowerValue = mPowerPanel->mGUILayer->createRectangle(10,35,200,10);
   mPowerValue->background_gradient(Gorilla::Gradient_NorthSouth, Gorilla::rgb(255,255,255,200), Gorilla::rgb(64,64,64,200));
   mPowerDownButton = mPowerPanel->makeButton(10, 65, "-");
   mPowerUpButton = mPowerPanel->makeButton(84, 65, "+");
   
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
   
   if (mTimer > 1.0f / 60.0f)
   {
    mPowerPanel->mNode->yaw(Ogre::Radian(0.0005));
    mTimer2 += Ogre::Math::RangeRandom(0, mTimer * 10);
    mTimer = 0;
    Ogre::Math::Clamp<Ogre::Real>(mTimer2, 0, 25);
    Ogre::Real power = mBasePower + (Ogre::Math::Cos(mTimer2) * 5);
    power = Ogre::Math::Clamp<Ogre::Real>(power, 0, 380);
    mPowerValue->width(power);
   }
   bool isOver = false;
   
   Button* button = mPowerPanel->check( mCamera->getCameraToViewportRay(0.5f,0.5f), isOver );
   if (isOver)
    mCrosshairLayer->hide();
   else
    mCrosshairLayer->show();
     
   if (button != 0 && mMouse->getMouseState().buttonDown(OIS::MB_Left))
   {
    if (button == mPowerDownButton)
    {
     mBasePower -= 1.0f;
    }
    if (button == mPowerUpButton)
    {
     mBasePower += 1.0f;
    }
   }

#if 0
   Ogre::Plane plane(Ogre::Vector3::UNIT_Z, 0);
   Ogre::Ray ray = mCamera->getCameraToViewportRay(0.5f,0.5f);
   std::pair<bool, Ogre::Real> result = Ogre::Math::intersects(ray, plane);
   
   // This is really hacky, but shows you what could be done.
   if (result.first && result.second < 10.0f) 
   {
    Ogre::Vector3 hit = ray.getOrigin() + (ray.getDirection() * result.second);
    if (hit.x > 0 && hit.x < 3.90f && hit.y > 1.5f && hit.y < 3.32f)
    {
     m3DMouse->left(hit.x * 100);
     m3DMouse->top((-hit.y * 100) + 332);
     bool lmbDown = mMouse->getMouseState().buttonDown(OIS::MB_Left);
     if (mPowerUpButton->isOver(m3DMouse->position()) && lmbDown)
     { 
      if (mPowerValue->width() <= 300.0f)
      {
       mPowerValue->width( mPowerValue->width() + 0.5f);
       Ogre::Real c = (255.0f/300.0f) * mPowerValue->width();
       mPowerValue->background_gradient(Gorilla::Gradient_WestEast, Gorilla::rgb(255,255,255,1), Gorilla::rgb(255,255,255,c));
      }
     }
     else if (mPowerDownButton->isOver(m3DMouse->position()) && lmbDown)
     {
      if (mPowerValue->width() >= 0.0f)
      {
       mPowerValue->width( mPowerValue->width() - 0.5f);
       Ogre::Real c = (255.0f/300.0f) * mPowerValue->width();
       mPowerValue->background_gradient(Gorilla::Gradient_WestEast, Gorilla::rgb(255,255,255,1), Gorilla::rgb(255,255,255,c));
      }
     }
    
     mCrosshairLayer->hide();
    }
    else
    {
     mCrosshairLayer->show();
    }
   }
   else
    mCrosshairLayer->show();
#endif

   Ogre::Vector3 trans(0,0,0);
   
   if (mKeyboard->isKeyDown(OIS::KC_W))
    trans.z = -1;
   else if (mKeyboard->isKeyDown(OIS::KC_S))
    trans.z =  1;
   if (mKeyboard->isKeyDown(OIS::KC_A))
    trans.x = -1;
   else if (mKeyboard->isKeyDown(OIS::KC_D))
    trans.x =  1;
   
   if (trans.isZeroLength() == false)
   {
    Ogre::Vector3 pos = mCamera->getPosition();
    pos += mCamera->getOrientation() * (trans * 5.0f) * evt.timeSinceLastFrame;
    pos.y = 2.0f;
    mCamera->setPosition(pos);
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
   Ogre::Real pitch = Ogre::Real(arg.state.Y.rel) * -0.005f;
   Ogre::Real yaw = Ogre::Real(arg.state.X.rel) * -0.005f;
   mCamera->pitch(Ogre::Radian(pitch));
   mCamera->yaw(Ogre::Radian(yaw));
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
   Ogre::NameValuePairList options;
   options["FSAA"] = "4";
   
   mWindow = mRoot->createRenderWindow("Gorilla", 1024, 768, false, &options);
   mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
   mCamera = mSceneMgr->createCamera("Camera");
   mViewport = mWindow->addViewport(mCamera);

   Ogre::ColourValue BackgroundColour = Ogre::ColourValue(0.1337f, 0.1337f, 0.1337f, 1.0f);
   Ogre::ColourValue GridColour = Ogre::ColourValue(0.2000f, 0.2000f, 0.2000f, 1.0f);
   Ogre::ColourValue GroundColour = Ogre::ColourValue(0.2337f, 0.2337f, 0.2337f, 1.0f);

   mViewport->setBackgroundColour(BackgroundColour);
   
   rgm->initialiseAllResourceGroups();
   
   mCamera->setPosition(10,2,10);
   mCamera->lookAt(0,2,0);
   mCamera->setNearClipDistance(0.05f);
   mCamera->setFarClipDistance(1000);
   
   mReferenceObject = new Ogre::ManualObject("ReferenceGrid");

   mReferenceObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);
   
   Ogre::Real step = 1.0f;
   unsigned int count = 200;
   unsigned int halfCount = count / 2;
   Ogre::Real full = (step * count);
   Ogre::Real half = full / 2;
   Ogre::Real y = 0;
   Ogre::ColourValue c;
   for (unsigned i=0;i < count+1;i++)
   {
    
    if (i == halfCount)
     c = Ogre::ColourValue(0.5f,0.3f,0.3f,1.0f);
    else
     c = GridColour;
    
    mReferenceObject->position(-half,y,-half+(step*i));
    mReferenceObject->colour(BackgroundColour);
    mReferenceObject->position(0,y,-half+(step*i));
    mReferenceObject->colour(c);
    mReferenceObject->position(0,y,-half+(step*i));
    mReferenceObject->colour(c);
    mReferenceObject->position(half,y,-half+(step*i));
    mReferenceObject->colour(BackgroundColour);

    if (i == halfCount)
     c = Ogre::ColourValue(0.3f,0.3f,0.5f,1.0f);
    else
     c = GridColour;
    
    mReferenceObject->position(-half+(step*i),y,-half);
    mReferenceObject->colour(BackgroundColour);
    mReferenceObject->position(-half+(step*i),y,0);
    mReferenceObject->colour(c);
    mReferenceObject->position(-half+(step*i),y,0);
    mReferenceObject->colour(c);
    mReferenceObject->position(-half+(step*i),y, half);
    mReferenceObject->colour(BackgroundColour);

   }
   
   mReferenceObject->end();
   mSceneMgr->getRootSceneNode()->attachObject(mReferenceObject);
    
    
    
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
  Ogre::ManualObject*     mReferenceObject;
  
};

void main()
{
 App* app  = new App();
 app->mRoot->startRendering();
 delete app;
}


