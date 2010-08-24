/*3
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

#include "BetaGUI4.h"

#define BETAGUI_WINDOW_SHADOW_SIZE 5

namespace BetaGUI
{

void GUI::userDesign()
{
 mDesign->window.background = Gorilla::rgb(221,221,221,255);
 mDesign->window.titleHeight = 22;
 mDesign->window.border = Gorilla::rgb(100,100,100,255);
 mDesign->window.borderActive = Gorilla::rgb(115,205,16,255);
 mDesign->window.dragT = Gorilla::rgb(255,255,255,255);
 mDesign->window.dragB = Gorilla::rgb(221,221,221,255);
 mDesign->window.titleColour = Gorilla::rgb(0,115,234,200);
 mDesign->window.titleColourActive = mDesign->window.dragT;
 mDesign->window.dragActive = mDesign->window.borderActive;
 mDesign->window.resizerSprite = "resizer";
 mDesign->window.resizerSize = 10;
 mDesign->window.titleOffsetX = 5;

 Design::WidgetDesign* button = new Design::WidgetDesign();
 mDesign->widgets[WT_Button] = button;
 button->hasBorder = true;
 button->hasCaption = true;
 button->backgroundT = Gorilla::rgb(230,230,230,255);
 button->backgroundB = Gorilla::rgb(200,200,200,255);
 button->colour = Gorilla::rgb(32,32,32,200);
 button->borderColour = Gorilla::rgb(128,128,128);
 button->paddingSides = 4;
 button->paddingTopBottom = 1;
 button->borderColourActive = Gorilla::rgb(255,0,132,255);
 
 Design::WidgetDesign* textbox = new Design::WidgetDesign();
 mDesign->widgets[WT_TextBox] = textbox;
 textbox->hasBorder = true;
 textbox->hasCaption = true;
 textbox->backgroundT = Gorilla::rgb(200,200,200,255);
 textbox->backgroundB = Gorilla::rgb(221,221,221,255);
 textbox->colour = Gorilla::rgb(48,48,48,200);
 textbox->borderColour = Gorilla::rgb(128,128,128);
 textbox->paddingSides = 4;
 textbox->paddingTopBottom = 1;
 textbox->borderColourActive = Gorilla::rgb(255,0,132,255);
 
 Design::WidgetDesign* progress = new Design::WidgetDesign();
 mDesign->widgets[WT_Progress] = progress;
 progress->hasCaption = false;
 progress->backgroundT = Gorilla::rgb(120,142,95,255);
 progress->backgroundB = Gorilla::rgb(120,142,95,255);
 progress->colour = mDesign->window.borderActive;
 
 Design::WidgetDesign* label = new Design::WidgetDesign();
 mDesign->widgets[WT_Label] = label;
 label->hasBackground = false;
 label->hasCaption = true;
 label->colour = Gorilla::rgb(75,75,75,200);
 
}


GUI::GUI(Gorilla::Screen* screen, Callback* callback, bool useMousePointer)
: mScreen(screen), mMousePointerLayer(0), mCallback(callback), mExclusiveWindow(0)
{
 mDesign = new Design();
 userDesign();
 if (useMousePointer)
 {
  mMousePointerLayer = mScreen->createSpriteLayer(15);
  mMousePointerID = mMousePointerLayer->addSprite(0,0, "mousepointer");
 }
}

GUI::~GUI()
{
 if (mMousePointerLayer)
 {
  mScreen->destroy(mMousePointerLayer);
 }
 // Delete windows here.
 delete mDesign;
 
}

bool GUI::checkMouse(Ogre::Real x, Ogre::Real y, bool LMBIsDown)
{
 
 // Move mouse pointer 
 if (mMousePointerLayer)
  mMousePointerLayer->setSpritePosition(mMousePointerID, x, y);
 
 
 for (WindowIterator it = mWindows.begin(); it != mWindows.end(); it++)
  if ((*it)->checkMouse(x,y, LMBIsDown))
   return true;
 
 return false;
}

void GUI::checkKey(Ogre::uchar c)
{
 if (mExclusiveWindow == 0)
  return;
 
 mExclusiveWindow->checkKey(c);
}

Window* GUI::createWindow(const Ogre::String& title, const Ogre::Vector2& position, const Ogre::Vector2& size)
{
 Window* window = new Window(position, size, title, true, true, this);
 mWindows.push_back(window);
 return window;
}


Window::Window(const Ogre::Vector2& position, const Ogre::Vector2& size, const Ogre::String title, bool canMove, bool canResize, GUI* parent)
: mPosition(position), mSize(size), mTitle(title), mCanMove(canMove), mCanResize(canResize), mGUI(parent), mIsMoving(false), mIsResizing(false), mMinSize(100,100), mHoverWidget(0), mActiveWidget(0)
{
 mDesign = mGUI->getDesign();
 mCanvas = mGUI->getScreen()->createCanvas();
 initialise();
}

Window::~Window()
{
}

void  Window::initialise()
{
 
 // Window Box
 mWindowBox = mCanvas->addBox(mPosition.x, mPosition.y, mSize.x, mSize.y, mDesign->window.border);
 
 // Drag
 mMoveRect = mCanvas->addRectangle(mPosition.x, mPosition.y, mSize.x, mDesign->window.titleHeight, mDesign->window.dragT);
 mCanvas->setRectangleColour(mMoveRect, mDesign->window.dragT, mDesign->window.dragT, mDesign->window.dragB, mDesign->window.dragB);
 
 // Window
 mWindowRect = mCanvas->addRectangle(mPosition.x, mPosition.y + mDesign->window.titleHeight, mSize.x, mSize.y - mDesign->window.titleHeight, mDesign->window.background);
 
 // Caption
 mTitleCaption = mCanvas->addCaption(mPosition.x + mDesign->window.titleOffsetX, mPosition.y, mTitle);
 mCanvas->setCaptionColour(mTitleCaption, mDesign->window.titleColour);

 // Resizer
 if (mCanResize)
 {
  mResizeRect = mCanvas->addRectangle(mPosition.x + mSize.x - mDesign->window.resizerSize, mPosition.y + mSize.y - mDesign->window.resizerSize, mDesign->window.resizerSize, mDesign->window.resizerSize);
  mCanvas->setRectangleBackground(mResizeRect, mDesign->window.resizerSprite);
 }
 
 // Shadows
 mShadowNERect = mCanvas->addRectangle(mPosition.x + mSize.x, mPosition.y, BETAGUI_WINDOW_SHADOW_SIZE, BETAGUI_WINDOW_SHADOW_SIZE);
 mCanvas->setRectangleBackground(mShadowNERect, "shadow_ne");
 
 mShadowERect = mCanvas->addRectangle(mPosition.x + mSize.x, mPosition.y + BETAGUI_WINDOW_SHADOW_SIZE, BETAGUI_WINDOW_SHADOW_SIZE, mSize.y - BETAGUI_WINDOW_SHADOW_SIZE);
 mCanvas->setRectangleBackground(mShadowERect, "shadow_e");
 
 mShadowSERect = mCanvas->addRectangle(mPosition.x + mSize.x, mPosition.y + mSize.y, BETAGUI_WINDOW_SHADOW_SIZE, BETAGUI_WINDOW_SHADOW_SIZE);
 mCanvas->setRectangleBackground(mShadowSERect, "shadow_se");

 mShadowSRect = mCanvas->addRectangle(mPosition.x + BETAGUI_WINDOW_SHADOW_SIZE, mPosition.y + mSize.y, mSize.x - BETAGUI_WINDOW_SHADOW_SIZE, BETAGUI_WINDOW_SHADOW_SIZE);
 mCanvas->setRectangleBackground(mShadowSRect, "shadow_s");
 
 mShadowSWRect = mCanvas->addRectangle(mPosition.x, mPosition.y + mSize.y, BETAGUI_WINDOW_SHADOW_SIZE, BETAGUI_WINDOW_SHADOW_SIZE);
 mCanvas->setRectangleBackground(mShadowSWRect, "shadow_sw");
 
}

void  Window::_needsUpdate()
{
 
 // Positions
 mCanvas->setBoxPosition(mWindowBox, mPosition.x, mPosition.y);
 mCanvas->setRectanglePosition(mMoveRect, mPosition.x, mPosition.y);
 mCanvas->setRectanglePosition(mWindowRect, mPosition.x, mPosition.y + mDesign->window.titleHeight);
 mCanvas->setCaptionPosition(mTitleCaption, mPosition.x + mDesign->window.titleOffsetX, mPosition.y);
 mCanvas->setRectanglePosition(mResizeRect, mPosition.x + mSize.x - mDesign->window.resizerSize, mPosition.y + mSize.y - mDesign->window.resizerSize);
 mCanvas->setRectanglePosition(mShadowNERect, mPosition.x + mSize.x, mPosition.y);
 mCanvas->setRectanglePosition(mShadowERect, mPosition.x + mSize.x, mPosition.y + BETAGUI_WINDOW_SHADOW_SIZE);
 mCanvas->setRectanglePosition(mShadowSERect, mPosition.x + mSize.x, mPosition.y + mSize.y);
 mCanvas->setRectanglePosition(mShadowSRect, mPosition.x + BETAGUI_WINDOW_SHADOW_SIZE, mPosition.y + mSize.y);
 mCanvas->setRectanglePosition(mShadowSWRect, mPosition.x, mPosition.y + mSize.y);
 
 // Size
 mCanvas->setBoxSize(mWindowBox, mSize.x, mSize.y);
 mCanvas->setRectangleSize(mMoveRect, mSize.x, mDesign->window.titleHeight);
 mCanvas->setRectangleSize(mWindowRect, mSize.x, mSize.y - mDesign->window.titleHeight);
 mCanvas->setRectangleSize(mShadowERect,  BETAGUI_WINDOW_SHADOW_SIZE, mSize.y - BETAGUI_WINDOW_SHADOW_SIZE);
 mCanvas->setRectangleSize(mShadowSRect, mSize.x - BETAGUI_WINDOW_SHADOW_SIZE, BETAGUI_WINDOW_SHADOW_SIZE);
 
 // Caption limitations
 mCanvas->setCaptionHorizontalClip(mTitleCaption, mPosition.x + mSize.x - mDesign->window.titleOffsetX);
 
 // Widgets
 for (WidgetIterator it = mWidgets.begin(); it != mWidgets.end(); it++)
  (*it)->parentNotify();
 
}

void  Window::setActiveWidget(Widget*)
{
}

void  Window::setHoverWidget(Widget*)
{
}

void  Window::clearActiveWidget()
{
}

void  Window::clearHoverWidget()
{
}

Widget*  Window::getActiveWidget()
{
}

Widget*  Window::getHoverWidget()
{
}

bool  Window::checkMouse(Ogre::Real x, Ogre::Real y, bool LMBIsDown)
{
  Ogre::Real relX = x - mPosition.x;
  Ogre::Real relY = y - mPosition.y;
  
  if (mIsMoving)
  {
   _processMoving(x,y,LMBIsDown);
   return true;
  }
  
  if (mIsResizing)
  {
   _processResizing(x,y,LMBIsDown);
   return true;
  }
  
  if (mActiveWidget)
  {
   
   if ( (LMBIsDown && isNotIn(x, y)) ||
        (LMBIsDown && mActiveWidget->checkMouse(relX,relY) == false)
      )
   {
    defocus();
   }
   
   return true;
  }
  
  if (isNotIn(x, y))
   return false;
  
  
  for (WidgetIterator it = mWidgets.begin(); it != mWidgets.end(); it++)
  {
   if (  (*it)->checkMouse(relX, relY)  )
   {
    if (mHoverWidget)
    {
     mHoverWidget->unhover();
     mHoverWidget = 0;
    }

    if (LMBIsDown)
    {
     
     if ( (*it)->activate() )
     {
      mActiveWidget = (*it);
      mGUI->setExclusive(this);
     }
     
    }
    else
    {
     mHoverWidget = (*it);
     mHoverWidget->hover();
    }
    
    return true;
   }
  }


  if (relY <= mDesign->window.titleHeight)
  {
   if (LMBIsDown)
   {
    mIsMoving = true;
    mLastDragCoords.x = relX;
    mLastDragCoords.y = relY;
    mCanvas->setRectangleColour(mMoveRect, mDesign->window.dragActive);
    mCanvas->setCaptionColour(mTitleCaption, mDesign->window.titleColourActive);
    mCanvas->setBoxColour(mWindowBox, mDesign->window.borderActive);
   }
   return true;
  }
  
  if ((relX >= mSize.x - mDesign->window.titleHeight) && (relY >= mSize.y - mDesign->window.titleHeight))
  {
   if (LMBIsDown)
   {
    mIsResizing = true;
    mLastDragCoords.x = relX;
    mLastDragCoords.y = relY;
    mCanvas->setBoxColour(mWindowBox, mDesign->window.borderActive);
   }
   return true;
  }
  
  if (mHoverWidget)
  {
   mHoverWidget->unhover();
   mHoverWidget = 0;
  }
  
  return true;
}

void  Window::checkKey(Ogre::uchar c)
{
 if (mActiveWidget)
 {
  mActiveWidget->checkKey(c);
 }
}


void  Window::defocus()
{
 if (mActiveWidget)
 {
  mActiveWidget->deactivate();
  mActiveWidget = 0;
  mGUI->clearExclusive();
 }
}

void  Window::_processMoving(Ogre::Real x, Ogre::Real y, bool LMBIsDown)
{
  
  Ogre::Real relX = x - mPosition.x;
  Ogre::Real relY = y - mPosition.y;
  
  if (LMBIsDown == false)
  {
   mIsMoving = false;
   mCanvas->setRectangleColour(mMoveRect, mDesign->window.dragT, mDesign->window.dragT, mDesign->window.dragB, mDesign->window.dragB);
   mCanvas->setCaptionColour(mTitleCaption, mDesign->window.titleColour);
   mCanvas->setBoxColour(mWindowBox, mDesign->window.border);
  }
  else
  {
   mPosition.x += relX - mLastDragCoords.x;
   mPosition.y += relY - mLastDragCoords.y;
   
   // Clamp
   if (mPosition.x < 0)
    mPosition.x = 0;
   if (mPosition.y < 0)
    mPosition.y = 0;
   if (mPosition.x + mSize.x > mGUI->getScreen()->getViewportWidth())
    mPosition.x = mGUI->getScreen()->getViewportWidth() - mSize.x;
   if (mPosition.y + mSize.y > mGUI->getScreen()->getViewportHeight())
    mPosition.y = mGUI->getScreen()->getViewportHeight() - mSize.y;
   
   _needsUpdate();
  } 
  
}

void  Window::_processResizing(Ogre::Real x, Ogre::Real y, bool LMBIsDown)
{
 
 Ogre::Real relX = x - mPosition.x;
 Ogre::Real relY = y - mPosition.y;
 
 if (LMBIsDown == false)
 {
  mIsResizing = false;
  
  mCanvas->setBoxColour(mWindowBox, mDesign->window.border);
 }
 else
 {
  mSize.x = relX + (mDesign->window.titleHeight * 0.5f);
  mSize.y = relY + (mDesign->window.titleHeight * 0.5f);
  
  if (mSize.x < mMinSize.x)
   mSize.x = mMinSize.x;
  
  if (mSize.y < mMinSize.y)
   mSize.y = mMinSize.y;
  
  _needsUpdate();
 }
}

Widget::Widget(const Ogre::Vector2& position, WidgetType type, Window* window)
: mPosition(position), mType(type), mWindow(window), mWidgetRect(-1), mCaption(-1), mBorderBox(-1), mCaptionLimit(0), mIsHovered(false), mIsActivate(false)
{
 mCanvas = mWindow->getCanvas();
}

Widget::~Widget()
{
}

void Widget::initialise()
{
 mDesign = mWindow->getDesign()->getWidget(mType);
 
 Ogre::Real x = mWindow->getX() + mPosition.x;
 Ogre::Real y = mWindow->getY() + mPosition.y + mWindow->getDesign()->window.titleHeight;
 
 if (mDesign->hasCaption)
 {
  mCaption = mCanvas->addCaption(x+mDesign->paddingSides, y+mDesign->paddingTopBottom, mCaptionText);
  mCanvas->setCaptionColour(mCaption, mDesign->colour);
  mSize = mCanvas->getCaptionSize(mCaption);
  if (mCaptionLimit)
  {
   mCanvas->setCaptionHorizontalClip(mCaption, x + mCaptionLimit);
   mSize.x = mCaptionLimit;
  }
 }
 
 if (mDesign->hasBorder)
 {
  mBorderBox = mCanvas->addBox(x-1,y-1,mSize.x+1+(mDesign->paddingSides*2), mSize.y+1+(mDesign->paddingTopBottom*2), mDesign->borderColour);
 }
 
 if (mDesign->hasBackground)
 {
  mWidgetRect = mCanvas->addRectangle(x,y, mSize.x+(mDesign->paddingSides*2), mSize.y+(mDesign->paddingTopBottom*2), mDesign->backgroundT);
  mCanvas->setRectangleColour(mWidgetRect, mDesign->backgroundT, mDesign->backgroundT, mDesign->backgroundB, mDesign->backgroundB);
 }
 
}

void Widget::parentNotify()
{
 needsUpdate();
}

void Widget::needsUpdate()
{
 
 Ogre::Real x = mWindow->getX() + mPosition.x;
 Ogre::Real y = mWindow->getY() + mPosition.y + mWindow->getDesign()->window.titleHeight;
 
 // Position.
 if (mDesign->hasCaption)
  mCanvas->setCaptionPosition(mCaption, x+mDesign->paddingSides, y+mDesign->paddingTopBottom);
 
 if (mDesign->hasBorder)
  mCanvas->setBoxPosition(mBorderBox,x-1,y-1);
 
 if (mDesign->hasBackground)
  mCanvas->setRectanglePosition(mWidgetRect, x,y);

 // Size
 if (mDesign->hasCaption && mCaptionLimit)
  mCanvas->setCaptionHorizontalClip(mCaption, x + mCaptionLimit);
 
 if (mDesign->hasBorder)
  mCanvas->setBoxSize(mBorderBox, mSize.x+1+(mDesign->paddingSides*2), mSize.y+1+(mDesign->paddingTopBottom*2));
 
 if (mDesign->hasBackground)
  mCanvas->setRectangleSize(mWidgetRect, mSize.x+(mDesign->paddingSides*2), mSize.y+(mDesign->paddingTopBottom*2));
 
 if (!mIsHovered && !mIsActivate && mDesign->hasBorder)
  mCanvas->setBoxColour(mBorderBox, mDesign->borderColour);
 
 if (mIsHovered && !mIsActivate && mDesign->hasBorder)
  mCanvas->setBoxColour(mBorderBox, mDesign->borderColourHover);
 
 // Custom
 drawOnce();
}

bool Widget::checkMouse(Ogre::Real x, Ogre::Real y)
{
 return ( (x >= mPosition.x && y >= mPosition.y + mWindow->getDesign()->window.titleHeight) && (x <= mPosition.x + mSize.x && y <= mPosition.y + + mWindow->getDesign()->window.titleHeight + mSize.y) );
}

Button*  Window::createButton(const Ogre::Vector2& position, const Ogre::String& caption, size_t ref)
{
 Button* button = new Button(position, caption, ref, this);
 mWidgets.push_back(button);
 return button;
}

Button::Button(const Ogre::Vector2& position, const Ogre::String& caption, size_t ref, Window* parent)
: Widget(position, WT_Button, parent)
{
 mCaptionText = caption;
 initialise();
}

Button::~Button()
{
}

bool Button::activate()
{
 std::cout << "Clicked!\n";
 return false; // Don't want focus.
}

TextBox*  Window::createTextBox(const Ogre::Vector2& position, const Ogre::String& caption, Ogre::uint maxDisplayedCharacters, size_t ref)
{
 TextBox* textbox = new TextBox(position, caption, maxDisplayedCharacters, ref, this);
 mWidgets.push_back(textbox);
 return textbox;
}

TextBox::TextBox(const Ogre::Vector2& position, const Ogre::String& value, Ogre::uint maxNumberCharactersShown, size_t ref, Window* parent)
: Widget(position, WT_TextBox, parent), mNbCharactersShown(maxNumberCharactersShown), mIsActive(false)
{
 mCaptionLimit = mNbCharactersShown * mWindow->getCanvas()->getScreen()->getAtlas()->getGlyphMonoWidth(); 
 mCaptionText = value;
 initialise();
}

TextBox::~TextBox()
{
}

bool TextBox::activate()
{
 mIsActive = true;
 mCanvas->setCaptionText(mCaption, mCaptionText + "|");
 return true; // Want to be exclusive.
}

void TextBox::deactivate()
{
 mIsActive = false;
 std::cout << "Call textbox listener with new value" << mCaptionText << "\n";
 mCanvas->setCaptionText(mCaption, mCaptionText);
}

void TextBox::checkKey(Ogre::uchar c)
{
 
 if (c == '\n' || c == '\t')
 {
  mWindow->defocus();
  return;
 }
 
 if (c == 8) // backspace
 {
  mCaptionText.erase(mCaptionText.end() - 1);
 }
 else
 {
  if (mNbCharactersShown != mCaptionText.size() + 1)
   mCaptionText += c;
 }
 
 mCanvas->setCaptionText(mCaption, mCaptionText + "|");
 
}

Progress*  Window::createProgress(const Ogre::Vector2& position, Ogre::Real amountDone)
{
 Progress* progress = new Progress(position, amountDone, this);
 mWidgets.push_back(progress);
 return progress;
}

Progress::Progress(const Ogre::Vector2& position, Ogre::Real amountDone, Window* parent)
: Widget(position, WT_Progress, parent), mAmountDone(amountDone)
{
 mSize.x = 101;
 mSize.y = 15;
 initialise();

 Ogre::Real x = mWindow->getX() + mPosition.x;
 Ogre::Real y = mWindow->getY() + mPosition.y + mWindow->getDesign()->window.titleHeight;
 
 mProgressRect = mCanvas->addRectangle(x + 1, y + 1, mAmountDone * 100, 13, mDesign->colour);
}

Progress::~Progress()
{
}

void  Progress::setProgress(Ogre::Real amountDone)
{
 mAmountDone = amountDone;
 mCanvas->setRectangleSize(mProgressRect, mAmountDone * 100, 14);
}

void Progress::drawOnce()
{
 
 Ogre::Real x = mWindow->getX() + mPosition.x;
 Ogre::Real y = mWindow->getY() + mPosition.y + mWindow->getDesign()->window.titleHeight;
 
 mCanvas->setRectanglePosition(mProgressRect, x+1,y+1);
 mCanvas->setRectangleSize(mProgressRect, mAmountDone*100, 14);
 
}

Label*  Window::createLabel(const Ogre::Vector2& position, const Ogre::String& text)
{
 Label* label = new Label(position, text, this);
 mWidgets.push_back(label);
 return label;
}

Label::Label(const Ogre::Vector2& position, const Ogre::String& text, Window* parent)
: Widget(position, WT_Label, parent)
{
 mCaptionText = text;
 initialise();
}

Label::~Label()
{
}

void Label::setText(const Ogre::String& text)
{
 mCanvas->setCaptionText(mCaption, text);
}


} // BetaGUI