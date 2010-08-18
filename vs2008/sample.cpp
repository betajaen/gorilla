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
   mViewport->setBackgroundColour(Ogre::ColourValue(0.45,0.45,0.45,1.0));
   
   rgm->initialiseAllResourceGroups();
   
   // Initialise Gorilla
   mGorilla = new Gorilla::System();
   mGorilla->loadAtlas("dejavu");
   mScreen = mGorilla->createScreen(mViewport, "dejavu");
   mScreen->createRectangle(10,10, 105, 304, Ogre::ColourValue::Red);
   mScreen->createRectangle(115,10, 40, 104, Ogre::ColourValue::Blue);
   mScreen->createSprite(10,10, "ogrehead");
   mScreen->createText(50,50, "hat in a th tax whou, now exis Alp bovid th frot my con ye Gar, Che T.\n"
"Do th-se-she dre be 16 Beld Pegair end th at anize by of piry throd\n"
"rive in the thave sag pat natin reproce cou co ing; the a Divold\n"
"dabough ther stich gant infe are subin Compas oto Pologs, Effe, and\n"
"cad tord to Res swerge indoul ce cal Batic), wouse fut syna neend inal\n"
"a-Come raclue annobir 1 ager cat the le in whict, in yeeced th yout I\n"
"com hics forit rine the rompar A was the Coducin and my to the Darivey\n"
"ficave accurvin an asigin of ou mothe pre the is thampla: 32 Hown;\n"
"Bolot he ferno to hated has and blivis alichow iseedevid. To shoplear\n"
"for of he se zercumbic al Saff. Yore-Amord baly of exply the by wor re\n"
"the be fate mand im, M. 860 by preake enot Z. 30,00 alcen; al\n"
"expeaciat dit; art actiode plence dookes exionse your metem. Thint min\n"
"breher of ther ther. Bran tryoust, ed fla lay of Tesuene whort, welf,\n"
"ad he of en patumaim way, rep al if Bar The ary meteman my gloge\n"
"dismirl, whent vild samplip dowithys, ponsir to decom om is re bons\n"
"withann, is for laster, thich witer. Sheat hapeatur commad mety or a\n"
"parthe he pre then he Borne dons, the vocial, th, a sortum thalsopme\n"
"of the for of to your torbat later the all usequin cartager hatted\n"
"buisobany sationta pan en Fremplic, of inite sonever a cor vinve by\n"
"withe fid, Mor whe will dyint res Beated ithe of in's beaccee noly\n"
"falation the pris und the dampor whin am to tor powdeur inhaso\n"
"nobliare by becanner (e. The whin throsibe the dernagood of a to begis\n");

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
    
    mNextUpdate = 0;
   }
   
   return true;
  }
  
  Ogre::Root*             mRoot;
  Ogre::RenderWindow*     mWindow;
  Ogre::Viewport*         mViewport;
  Ogre::SceneManager*     mSceneMgr;
  Ogre::Camera*           mCamera;
  Ogre::Real              mNextUpdate;
  
  Gorilla::System*        mGorilla;
  Gorilla::Screen*        mScreen;
  
};

void main()
{
 App* app = new App();
 app->mRoot->startRendering();
 delete app;
}


