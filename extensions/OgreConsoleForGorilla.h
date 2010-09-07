/*
  Description:
   
   This is a port of the OgreConsole code presented by PixL in the Ogre Forums then later added
   to the Ogre Wiki[1].
   
   This is a straight port replacing all the Overlay code with Gorilla code, some changes have
   been added but they are minor and do not add to the actual functionality of the class.
   
  Port Author:
   
   Betajaen.
   
  Usage:
   
   
   
  References:
   
   [1] http://www.ogre3d.org/tikiwiki/OgreConsole&structure=Cookbook
   
*/

#include "OGRE/Ogre.h"
#include "OIS/OIS.h"
#include "Gorilla.h"

typedef void (*OgreConsoleFunctionPtr)(Ogre::StringVector&);

class OgreConsole : public Ogre::Singleton<OgreConsole>, Ogre::FrameListener, Ogre::LogListener
{
    
 public:
    
    OgreConsole();
    
   ~OgreConsole();
    
    void   init(Gorilla::Screen* screen_to_use);
    void   shutdown();
    
    void   setVisible(bool mIsVisible);
    bool   isVisible(){return mIsVisible;}
    
    void   print(const Ogre::String &text);
    
    virtual bool frameStarted(const Ogre::FrameEvent &evt);
    virtual bool frameEnded(const Ogre::FrameEvent &evt);
    
    void onKeyPressed(const OIS::KeyEvent &arg);
    
    void addCommand(const Ogre::String &command, OgreConsoleFunctionPtr);
    void removeCommand(const Ogre::String &command);
 
    //log
    void messageLogged( const Ogre::String& message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String &logName );
 private:
    
    void  updateConsole();
    void  updatePrompt();
    
    bool                 mIsVisible;
    bool                 mIsInitialised;
    Gorilla::Screen*     mScreen;
    Gorilla::Layer*      mLayer;
    Gorilla::Caption*    mPromptText;
    Gorilla::MarkupText* mConsoleText;
    Gorilla::Rectangle*  mDecoration;
    Gorilla::GlyphData*  mGlyphData;
    
    bool                 mUpdateConsole;
    bool                 mUpdatePrompt;

    int                     mStartline;
    std::list<Ogre::String>      lines;
    Ogre::String            prompt;
    std::map<Ogre::String, OgreConsoleFunctionPtr>  commands;
 
 
};



