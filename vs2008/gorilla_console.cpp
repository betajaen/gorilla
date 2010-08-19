
#include <OGRE/Ogre.h>
#include <OIS/OIS.h>
 
#include "Gorilla.h"
#include <sstream>

#pragma warning ( disable : 4244 )


/*
  GorillaConsole
  --------------
  
  A rewritten class from the original OgreConsole by PixL code[1] using Gorilla
  instead of the Ogre Overlay system.
  
  [1] http://www.ogre3d.org/tikiwiki/OgreConsole&structure=Cookbook
  
  This code is under the same licence as the original code in the Ogre Wiki article.
  
  Original Author: PixL 
  Gorilla Author: Betajaen
*/
class GorillaConsole
{
  
 public:
  
  enum
  { 
   MaxLines = 15,
   MaxRows = 75
  };
  
  typedef void(*function_t)(std::vector<Ogre::String>&);
  
  GorillaConsole(Gorilla::Screen* gorilla_screen) : start_line(0), screen(gorilla_screen), update_gorilla(false)
  {
   initialise();
  }
  
 ~GorillaConsole()
  {
  }
  
  void initialise()
  {
   Ogre::Real lineHeight = screen->getAtlas()->getGlyphLineHeight();
   Ogre::Real right = screen->getViewportWidth();
   Ogre::Real bottom = lineHeight * MaxLines;
   
   mConsoleLinesID = screen->createText(1,1, ".");
   mConsolePromptID = screen->createText(1, bottom + lineHeight+1, "_");
   
   mConsoleLinesBackgroundID = screen->createRectangle(1,1, right, bottom + lineHeight, Gorilla::rgb(13,13,13,80),-1);
   mConsolePromptBackgroundID = screen->createRectangle(1, bottom + lineHeight+1, right, lineHeight, Gorilla::rgb(13,13,13,160),-1);
   
  }
  
  bool frameStarted(const Ogre::FrameEvent &evt)
  {
   if (update_gorilla)
   {
    update_gorilla = false;
    std::stringstream text;
    
    std::list<Ogre::String>::iterator i,start,end;
    
    //make sure is in range
    if(start_line>lines.size())
       start_line=lines.size();
    
    int lcount=0;
    start=lines.begin();
    for(int c=0;c<start_line;c++)
       start++;
    end=start;
    for(int c=0;c<MaxLines;c++){
       if(end==lines.end())
          break;
       end++;
    }
    
    for(i=start;i!=end;i++)
       text << (*i) << "\n";
    
    screen->setTextCaption(mConsoleLinesID, text.str());
    screen->setTextCaption(mConsolePromptID, prompt + "_");
    
   }
   return true;
  }
  
  void onKeyPressed(const OIS::KeyEvent& arg)
  {
    if (arg.key == OIS::KC_RETURN)
    {
       
       print("> %3" + prompt + "%R");
       
       //split the parameter list
       const char *str=prompt.c_str();
       std::vector<Ogre::String> params;
       Ogre::String param="";
       for(int c=0;c<prompt.length();c++){
          if(str[c]==' '){
             if(param.length())
                params.push_back(param);
             param="";
          }
          else
             param+=str[c];
       }
       if(param.length())
          params.push_back(param);
   
         //try to execute the command
         std::map<Ogre::String, function_t >::iterator i;
         for(i=commands.begin();i!=commands.end();i++){
            if((*i).first==params[0]){
               if((*i).second)
                  (*i).second(params);
               break;
            }
         }
   
         prompt="";
      }
      if (arg.key == OIS::KC_BACK)
         prompt=prompt.substr(0,prompt.length()-1);
      if (arg.key == OIS::KC_PGUP)
      {
         if(start_line>0)
            start_line--;
      }
      if (arg.key == OIS::KC_PGDOWN)
      {
         if(start_line<lines.size())
            start_line++;
      }
      else
      {
         char legalchars[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890+!\"#%&/()=?[]\\*-_.:,; ";
         for(int c=0;c<sizeof(legalchars);c++){
            if(legalchars[c]==arg.text){
               prompt+=arg.text;
               break;
            }
         }
      }
      update_gorilla=true;
   }

 void print(const Ogre::String &text){
    //subdivide it into lines
    const char *str=text.c_str();
    int start=0,count=0;
    int len=text.length();
    Ogre::String line;
    for(int c=0;c<len;c++){
       if(str[c]=='\n'||line.length()>=MaxRows){
          lines.push_back(line);
          line="";
       }
       if(str[c]!='\n')
          line+=str[c];
    }
    if(line.length())
       lines.push_back(line);
    if(lines.size()>MaxLines)
       start_line=lines.size()-MaxLines;
    else
       start_line=0;
    update_gorilla=true;
 }

 void addCommand(const Ogre::String &command, function_t func){
    commands[command]=func;
 }
 
 void removeCommand(const Ogre::String &command){
    commands.erase(commands.find(command));
 }

  Gorilla::Screen*                    screen;
  bool                                visible;
  std::list<Ogre::String>             lines;
  std::map<Ogre::String, function_t>  commands;
  int                                 start_line;
  Ogre::String                        prompt;
  size_t                              mConsoleLinesID, mConsolePromptID,
                                      mConsoleLinesBackgroundID, mConsolePromptBackgroundID;
  bool                                update_gorilla;
  
};

void sample_version(std::vector<Ogre::String>&);

class App : public Ogre::FrameListener, public OIS::KeyListener
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
   mViewport->setBackgroundColour(Gorilla::rgb(227, 66, 52));
   
   rgm->initialiseAllResourceGroups();
   
   // Initialise OIS
   OIS::ParamList pl;
   size_t windowHnd = 0;
   std::ostringstream windowHndStr;
   mWindow->getCustomAttribute("WINDOW", &windowHnd);
   windowHndStr << windowHnd;
   pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
   mInputManager = OIS::InputManager::createInputSystem( pl );
   mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, false));
   mKeyboard->setBuffered(true);
   mKeyboard->setEventCallback(this);
   
   // Initialise Gorilla
   mGorilla = new Gorilla::Silverback();
   mGorilla->loadAtlas("dejavu");
   mScreen = mGorilla->createScreen(mViewport, "dejavu");
   mFPSMonitor = mScreen->createText(1,-25, "FPS: 0");
   mConsole = new GorillaConsole(mScreen);
   mConsole->addCommand("version", &sample_version);
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
   
   mConsole->frameStarted(evt);
   
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
  
  bool keyPressed( const OIS::KeyEvent &e )
  {
   mConsole->onKeyPressed(e);
   return true;
  }
  
  bool keyReleased( const OIS::KeyEvent &e )
  {
   return true;
  }
  
  Ogre::Root*             mRoot;
  Ogre::RenderWindow*     mWindow;
  Ogre::Viewport*         mViewport;
  Ogre::SceneManager*     mSceneMgr;
  Ogre::Camera*           mCamera;
  Ogre::Real              mNextUpdate;
  Gorilla::Silverback*    mGorilla;
  Gorilla::Screen*        mScreen;
  GorillaConsole*         mConsole;
  size_t                  mFPSMonitor;
  OIS::InputManager*      mInputManager;
  OIS::Keyboard*          mKeyboard;
  
};

App* app = 0;

void sample_version(std::vector<Ogre::String>&)
{
 std::stringstream s;
 s << "I am OGRE " << OGRE_VERSION_MAJOR << "." << OGRE_VERSION_MINOR << "." << OGRE_VERSION_PATCH << " '" << OGRE_VERSION_NAME << "'\n";
 app->mConsole->print(s.str());
}

void main()
{
 app = new App();
 app->mRoot->startRendering();
 delete app;
}


