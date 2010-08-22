/*
  Description:
   
   This is a port of the OgreConsole code presented by PixL in the Ogre Forums then later added
   to the Ogre Wiki.
   
   This is a straight port replacing all the Overlay code with Gorilla code, some changes have
   been added but they are minor and do not add to the actual functionality of the class.
   
   
*/

#include "OgreConsoleForGorilla.h"

template<> OgreConsole* Ogre::Singleton<OgreConsole>::ms_Singleton=0;

#define CONSOLE_LINE_LENGTH 85
#define CONSOLE_LINE_COUNT 15
static const char legalchars[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890+!\"'#%&/()=?[]\\*-_.:,; ";

OgreConsole::OgreConsole()
: mScreen(0), mStartline(0), mUpdateConsole(false), mUpdatePrompt(false), mIsInitialised(false), mIsVisible(true)
{
}
 
OgreConsole::~OgreConsole()
{
 if (mIsInitialised)
  shutdown();
}

void OgreConsole::init(Gorilla::Screen* screen)
{
   
   if(mIsInitialised)
    shutdown();
   
   Ogre::Root::getSingletonPtr()->addFrameListener(this);
   Ogre::LogManager::getSingleton().getDefaultLog()->addListener(this);
   
   // Create gorilla things here.
   mScreen = screen;
   mConsoleText = mScreen->createText(0,0, "", 15);
   mPromptText = mScreen->createText(0, 0, "%3> %R_", 15);
   mCanvas = mScreen->createCanvas(14);
   mConsoleBackground = mCanvas->addRectangle(0,0, mScreen->getViewportWidth(), mScreen->getAtlas()->getGlyphLineHeight(), Gorilla::rgb(15,15,15,50));

   mIsInitialised = true;
   
   print("OgreConsole Activated. Press F1 to show/hide.");
}

void OgreConsole::shutdown()
{
 if(!mIsInitialised)
    return;
 
 mIsInitialised = false;
 
 Ogre::Root::getSingletonPtr()->removeFrameListener(this);
 Ogre::LogManager::getSingleton().getDefaultLog()->removeListener(this);
 
//mScreen->destroyCanvas(mCanvas);
// mScreen->destroyText(mPromptText);
// mScreen->destroyText(mConsoleText);

}


void OgreConsole::onKeyPressed(const OIS::KeyEvent &arg)
{
 
 if(!mIsVisible)
  return;

 if (arg.key == OIS::KC_RETURN || arg.key == OIS::KC_NUMPADENTER)
 {
  
  print("%3> " + prompt + "%R");
  
  //split the parameter list
  Ogre::StringVector params = Ogre::StringUtil::split(prompt, " ");

  if (params.size())
  {
   std::map<Ogre::String, OgreConsoleFunctionPtr>::iterator i;
   for(i=commands.begin();i!=commands.end();i++){
    if((*i).first==params[0]){
     if((*i).second)
      (*i).second(params);
     break;
    }
   }
   prompt.clear();
   mUpdateConsole = true;
   mUpdatePrompt = true;
  }
 }

 else if (arg.key == OIS::KC_BACK)
 {
  prompt.erase(prompt.end() - 1); //=prompt.substr(0,prompt.length()-1);
  mUpdatePrompt = true;
 }
 else if (arg.key == OIS::KC_PGUP)
 {
    if(mStartline>0)
       mStartline--;
  mUpdateConsole = true;
 }

 else if (arg.key == OIS::KC_PGDOWN)
 {
    if(mStartline<lines.size())
       mStartline++;
  mUpdateConsole = true;
 }
 
 else
 {
    for(int c=0;c<sizeof(legalchars);c++){
       if(legalchars[c]==arg.text){
          prompt+=arg.text;
          break;
       }
    }
   mUpdatePrompt = true;
 }
 
}

bool OgreConsole::frameStarted(const Ogre::FrameEvent &evt)
{
   if(mUpdateConsole)
    updateConsole();
    
   if (mUpdatePrompt)
    updatePrompt();
   
   return true;
}

void OgreConsole::updateConsole()
{
 
 mUpdateConsole = false;
 
 std::stringstream text;
 std::list<Ogre::String>::iterator i,start,end;

 //make sure is in range
 if(mStartline>lines.size())
  mStartline=lines.size();

 int lcount=0;
 start=lines.begin();
 for(int c=0;c<mStartline;c++)
    start++;
 end=start;
 for(int c=0;c<CONSOLE_LINE_COUNT;c++){
    if(end==lines.end())
       break;
    end++;
 }

 for(i=start;i!=end;i++)
 {
  lcount++;
  text << (*i) << "\n";
 }
 mConsoleText->setText(text.str());
 
 // Move prompt downwards.
 mPromptText->setTop(lcount * mScreen->getAtlas()->getGlyphLineHeight());
 
 // Change background height so it covers the text and prompt
 mCanvas->setRectangleSize(mConsoleBackground, mScreen->getViewportWidth(), (lcount+1) * mScreen->getAtlas()->getGlyphLineHeight());
}

void OgreConsole::updatePrompt()
{
 mUpdatePrompt = false;
 std::stringstream text;
 text << "%3> %R" << prompt << "_";
 mPromptText->setText(text.str());
}

void OgreConsole::print(const Ogre::String &text)
{
   //subdivide it into lines
   const char *str=text.c_str();
   int start=0,count=0;
   int len=text.length();
   Ogre::String line;
   for(int c=0;c<len;c++){
      if(str[c]=='\n'||line.length()>=CONSOLE_LINE_LENGTH){
         lines.push_back(line);
         line="";
      }
      if(str[c]!='\n')
         line+=str[c];
   }
   if(line.length())
      lines.push_back(line);
   if(lines.size()>CONSOLE_LINE_COUNT)
      mStartline=lines.size()-CONSOLE_LINE_COUNT;
   else
      mStartline=0;
   mUpdateConsole=true;
}

bool OgreConsole::frameEnded(const Ogre::FrameEvent &evt)
{
 return true;
}

void OgreConsole::setVisible(bool isVisible)
{
 mIsVisible = isVisible;
 mCanvas->setVisible(mIsVisible);
 mPromptText->setVisible(mIsVisible);
 mConsoleText->setVisible(mIsVisible);
}

void OgreConsole::addCommand(const Ogre::String &command, OgreConsoleFunctionPtr func)
{
 commands[command]=func;
}

void OgreConsole::removeCommand(const Ogre::String &command)
{
 commands.erase(commands.find(command));
}

void OgreConsole::messageLogged( const Ogre::String& message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String &logName )
{
 print(message);
}