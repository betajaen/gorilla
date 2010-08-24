/*
    BetaGUI 4 Gorilla
    -----------------
    
    Copyright (c) 2010 Robin Southern
                                                                                  
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
                                                                                  
    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.
                                                                                  
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE. 
    
*/

#ifndef BETAGUI4_H
#define BETAGUI4_H

#include "Gorilla.h"

namespace BetaGUI
{
 
 class Callback;
 class GUI;
 class Window;
 class Widget;
 class Button;
 class TextBox;
 class Progress;
 class Choice;
 class Label;
 class Popup;
 class Menu;
 
 enum WidgetType
 {
  WT_Button,
  WT_TextBox,
  WT_Progress,
  WT_Choice,
  WT_Label,
  WT_Popup
 };
 
 class Callback
 {
   
  public:
   
   ~Callback() {}
   
   virtual void onButtonPressed(Widget*, size_t) {}
   
   virtual void onTextEntered(TextBox*, size_t) {}
    
   virtual void onChoice(Choice*, size_t) {}
   
   virtual void onMenuSelected(Menu*, size_t) {}
   
 };
 
 struct Design
 {
   
  struct window_dt
  {
   Ogre::ColourValue  dragT, dragB, titleColour, titleColourActive, dragActive, border, borderActive, background;
   Ogre::String       resizerSprite;
   Ogre::Real         titleHeight, titleOffsetX, resizerSize;
  } window;
  
  struct WidgetDesign
  {
    WidgetDesign()
    {
     hasBorder = hasCaption = false; 
     hasBackground = true;
     paddingSides = paddingTopBottom = 0;
    }
    // Border
    bool               hasBorder;
    Ogre::ColourValue  borderColour, borderColourActive, borderColourHover, borderColourTab, borderColourDefault;
    
    // Caption
    bool               hasCaption;
    Ogre::ColourValue  colour, colour_active;
    
    bool               hasBackground;
    // Widget
    Ogre::ColourValue  backgroundT, backgroundB, backgroundActive;
    
    // Padding
    Ogre::Real         paddingSides, paddingTopBottom;
  };
  
  std::map<size_t, WidgetDesign*>  widgets;
  
  WidgetDesign* getWidget(WidgetType t) { return widgets[t]; }
 };
 
 class GUI
 {
   public:
   
   typedef Ogre::vector<Window*>::type            WindowList;
   typedef Ogre::vector<Window*>::type::iterator  WindowIterator;
   
   GUI(Gorilla::Screen*, Callback*, bool useMousePointer = true);
   
  ~GUI();
   
   void setExclusive(Window* window) { mExclusiveWindow = window; }
   
   void clearExclusive() { mExclusiveWindow = 0; }
   
   void      userDesign();
   
   Design*   getDesign() const { return mDesign; }
   
   Gorilla::Screen* getScreen() const
   {
    return mScreen;
   }
   
   Window* createWindow(const Ogre::String& title, const Ogre::Vector2& position, const Ogre::Vector2& size = Ogre::Vector2(320, 200));
   
   bool    checkMouse(Ogre::Real x, Ogre::Real y, bool LMBIsDown);
   
   void    checkKey(Ogre::uchar);
   
  protected:
   
   Gorilla::Screen*             mScreen;
   Gorilla::SpriteLayer*        mMousePointerLayer;
   size_t                       mMousePointerID;
   WindowList                   mWindows;
   Design*                      mDesign;
   Callback*                    mCallback;
   Window*                      mExclusiveWindow;
   
 };
 
 class Window
 {
   
  friend class GUI;
   
  public:
   typedef Ogre::vector<Widget*>::type            Widgets;
   typedef Ogre::vector<Widget*>::type::iterator  WidgetIterator;
   
   Window(const Ogre::Vector2& position, const Ogre::Vector2& size, const Ogre::String caption, bool canMove, bool canResize, GUI* parent);
  ~Window();
   
   inline          Design*  getDesign() const { return mDesign; }
   Button*         createButton(const Ogre::Vector2& position, const Ogre::String& caption, size_t ref = 0);
   TextBox*        createTextBox(const Ogre::Vector2& position, const Ogre::String& caption, Ogre::uint maxDisplayedCharacters = 8, size_t ref = 0);
   Progress*       createProgress(const Ogre::Vector2& position, Ogre::Real amountDone);
   Label*          createLabel(const Ogre::Vector2& position, const Ogre::String& caption);
   
   inline          Gorilla::Canvas* getCanvas() const { return mCanvas; }
   void            setActiveWidget(Widget*);
   void            setHoverWidget(Widget*);
   void            clearActiveWidget();
   void            clearHoverWidget();
   Widget*         getActiveWidget();
   Widget*         getHoverWidget();
   void            activate();
   void            deactivate();
   bool            checkMouse(Ogre::Real x, Ogre::Real y, bool lmb);
   void            checkKey(Ogre::uchar);
   inline          Ogre::Real getX() const { return mPosition.x; }
   inline          Ogre::Real getY() const { return mPosition.y; }
   void            defocus();

   void  _processMoving(Ogre::Real x, Ogre::Real y, bool LMBIsDown);
   
   void  _processResizing(Ogre::Real x, Ogre::Real y, bool LMBIsDown);
   
   inline bool            isNotIn(Ogre::Real x, Ogre::Real y)
   {
    return (!(x >= mPosition.x && y >= mPosition.y) || !(x <= mPosition.x + mSize.x && y <= mPosition.y + mSize.y));
   }

  protected:
   void            _needsUpdate();
   void            initialise();
   
  protected:
   
   Design*         mDesign;
   GUI*            mGUI;
   Gorilla::Canvas*     mCanvas;               // Window Canvas
   bool            mIsActive;             // Is Activate?
   bool            mCanMove,  mIsMoving;
   bool            mCanResize, mIsResizing;
   Ogre::Vector2   mPosition, mSize, mMinSize, mLastDragCoords;
   Ogre::String    mTitle;
   Widget*         mHoverWidget, *mActiveWidget;
   WidgetIterator  mTabIterator;
   size_t          mWindowRect, mWindowBox, mMoveRect, mResizeRect, mShadowNERect, mShadowERect, mShadowSERect, mShadowSRect, mShadowSWRect;
   size_t          mTitleCaption;
   Widgets         mWidgets;
 };
 
 class Widget
 {
   
  public:
   
   Widget(const Ogre::Vector2& position, WidgetType type, Window*);
   
  ~Widget();
   
   void initialise();
   
   void parentNotify();
   
   void needsUpdate();
   
   bool checkMouse(Ogre::Real x, Ogre::Real y);
   
   virtual void  checkKey(Ogre::uchar) {}
   
   virtual void drawOnce() {}
   
   void hover() 
   {
    mIsHovered = true;
    needsUpdate();
   }
   
   void unhover()
   {
    mIsHovered = false;
    needsUpdate();
   }
   
   virtual bool activate() {return false;}
   
   virtual void deactivate() {}
   
   virtual void onKeyPress(Ogre::uchar) {}
   
   Button*  createButton(const Ogre::Vector2& position, const Ogre::String& caption, size_t ref = 0);
   
  protected:
   
   Gorilla::Canvas*       mCanvas;          // Window Canvas to draw too.
   size_t                 mWidgetRect,      // Widget Rectangle
                          mCaption,         // Caption that goes on widget
                          mBorderBox;       // 
   Ogre::Real             mCaptionLimit;    // 0 = No Limit of size, > 0 Limit Widget size and clip caption.
   WidgetType             mType;
   Ogre::String           mCaptionText;
   Window*                mWindow;
   Ogre::Vector2          mPosition, mSize;
   Design::WidgetDesign*  mDesign;
   bool                   mIsHovered, mIsActivate;
 };
 
 class Button : public Widget
 {
   
  public:
   
   Button(const Ogre::Vector2& position, const Ogre::String& caption, size_t ref, Window* parent);
   
  ~Button();
   
   bool activate();
   
  protected:
   
 };
 
 class TextBox : public Widget
 {
  public:
   
   TextBox(const Ogre::Vector2& position, const Ogre::String& value, Ogre::uint maxNumberCharactersShown, size_t ref, Window* parent);
   
  ~TextBox();
   
   bool activate();
   
   void deactivate();
   
   void  checkKey(Ogre::uchar);
   
  protected:
   
   Ogre::uint  mNbCharactersShown;
   bool        mIsActive;
   
 };
 
 class Progress : public Widget
 {
   
  public:
   
   Progress(const Ogre::Vector2& position, Ogre::Real amountDone, Window* parent);
   
  ~Progress();
   
   void  setProgress(Ogre::Real amountDone);
  
  protected:
   
   void drawOnce();
   
   Ogre::Real  mAmountDone;
   size_t      mProgressRect;
 };
 
 
 class Label : public Widget
 {
   
  public:
   
   Label(const Ogre::Vector2& position, const Ogre::String& text, Window* parent);
   
  ~Label();
  
   void setText(const Ogre::String&);
   
 };
  
}

#endif

