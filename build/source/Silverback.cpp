/*
    Gorilla
    -------
    
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

#include "Silverback.h"
#include "ook.cpp"
#include "Resources.h"

namespace Gorilla
{
  
  Silverback::Silverback()
  {
   // TODO
  }
  
  Silverback::~Silverback()
  {
   // TODO
  }
  
  void Silverback::_createShaders()
  {
   // TODO
  }

  void Silverback::_createOverlayCanvasBaseMaterial()
  {
   // TODO
  }
  
  void Silverback::_createMovableCanvasBaseMaterial()
  {
   // TODO
  }
  
  void Silverback::_createSolidWhiteSprite()
  {
   // TODO
  }

  Font* Silverback::loadFont(const Ogre::String& filename, const Ogre::String& resourceGroup)
  {
   // TODO
   return 0;
  }
  
  Font* Silverback::createFont(const Ogre::String& name)
  {
   // TODO
   return 0;
  }
  
  bool  Silverback::destroyFont(const Ogre::String& name)
  {
   // TODO
   return false;
  }
  
  Font* Silverback::getFont(const Ogre::String& name) const
  {
   // TODO
   return 0;
  }
  
  bool  Silverback::hasFont(const Ogre::String& name) const
  {
   // TODO
   return 0;
  }
  
  void Silverback::loadSprites(const Ogre::String& filename, const Ogre::String& resourceGroup)
  {
   // TODO
  }
  
  Sprite* Silverback::createSprite(const Ogre::String& name)
  {
   // TODO
  }
  
  void  Silverback::destroySprite(const Ogre::String& name)
  {
   // TODO
  }
  
  Sprite* Silverback::getSprite(const Ogre::String& name) const
  {
   // TODO
   return 0;
  }
  
  bool  Silverback::hasSprite(const Ogre::String& name) const
  {
   // TODO
   return false;
  }
  
  OverlayCanvas*  Silverback::createOverlayCanvas(Ogre::RenderQueueGroupID id)
  {
   // TODO
   return 0;
  }
  
  MovableCanvas*  Silverback::createMovableCanvas(const Ogre::String& movable_object_name)
  {
   // TODO
   return 0;
  }
  
  void Silverback::destroyCanvas(Canvas*)
  {
   // TODO
  }
  
  void Silverback::destroyAllOverlayCanvases()
  {
   // TODO
  }
  
  void Silverback::destroyAllMovableCanvases()
  {
   // TODO
  }
  
  Ogre::ConstVectorIterator<OverlayCanvas*> Silverback::getOverlayCanvases() const
  {
   return Ogre::ConstVectorIterator<OverlayCanvas*>(mOverlayCanvases.begin(), mOverlayCanvases.end());
  }
  
  Ogre::ConstVectorIterator<MovableCanvas*> Silverback::getMovableCanvases() const
  {
   return Ogre::ConstVectorIterator<MovableCanvas*>(mMovableCanvases.begin(), mMovableCanvases.end());
  }
  
} // namespace Gorilla