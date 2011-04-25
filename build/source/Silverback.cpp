/*
    Gorilla
    -------
    
    Copyright (c) 2011 Robin Southern
                                                                                  
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
#include "OGRE/OgreHardwarePixelBuffer.h"

namespace Gorilla
{
  
  Silverback::Silverback()
  {
   _createShaders();
   _createOverlayCanvasBaseMaterial();
   _createMovableCanvasBaseMaterial();
   _createSolidWhiteImage();
  }
  
  Silverback::~Silverback()
  {
   destroyAllOverlayCanvases();
   destroyAllMovableCanvases();
   
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
  
  void Silverback::_createSolidWhiteImage()
  {
   
   Ogre::TexturePtr texture = Ogre::TextureManager::getSingletonPtr()->createManual(
    "GorillaWhitePixel",
    Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
    Ogre::TEX_TYPE_2D,
    4,4,
    0,
    Ogre::PF_BYTE_BGRA,
    Ogre::TU_DEFAULT
   );
   
   Ogre::HardwarePixelBufferSharedPtr pixelBuffer = texture->getBuffer();
   
   pixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL);
     const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();
     Ogre::uint8* pDest = static_cast<Ogre::uint8*>(pixelBox.data);
     for (size_t i=0;i < 4*4*4;i++)
     {
      *pDest++ = 255;
     }
   pixelBuffer->unlock();
   
   _createSharedImage("GorillaWhitePixel");
  }

  SharedImage* Silverback::_createSharedImage(const Ogre::String& filename)
  {
   SharedImage* image = new SharedImage(filename);
   mSharedImages.insert(  std::pair<Ogre::String, SharedImage*>(filename, image)  );
   return image;
  }

  Font* Silverback::loadFont(const Ogre::String& filename, const Ogre::String& resourceGroup)
  {
   // TODO
   return 0;
  }
  
  Font* Silverback::_createFont(const Ogre::String& name)
  {
   // TODO
   return 0;
  }
  
  bool  Silverback::_destroyFont(const Ogre::String& name)
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
  
  void Silverback::loadSubImages(const Ogre::String& filename, const Ogre::String& resourceGroup)
  {
   // TODO
  }
  
  SubImage* Silverback::_createSubImage(const Ogre::String& name, SharedImage*)
  {
   // TODO
   return 0;
  }
  
  bool  Silverback::_destroySubImage(const Ogre::String& name)
  {
   // TODO
   return false;
  }
  
  SubImage* Silverback::getSubImage(const Ogre::String& name) const
  {
   // TODO
   return 0;
  }
  
  bool  Silverback::hasSubImage(const Ogre::String& name) const
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
  
  Ogre::ConstVectorIterator<Silverback::OverlayCanvases> Silverback::getOverlayCanvases() const
  {
   return Ogre::ConstVectorIterator<OverlayCanvases>(mOverlayCanvases.begin(), mOverlayCanvases.end());
  }
  
  Ogre::ConstVectorIterator<Silverback::MovableCanvases> Silverback::getMovableCanvases() const
  {
   return Ogre::ConstVectorIterator<MovableCanvases>(mMovableCanvases.begin(), mMovableCanvases.end());
  }
  
} // namespace Gorilla