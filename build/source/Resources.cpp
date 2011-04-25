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

#include "Resources.h"

namespace Gorilla
{
 
 SharedImage::SharedImage(const Ogre::String& file_name)
 : mFileName(file_name)
 {
  mTexture = Ogre::TextureManager::getSingletonPtr()->getByName(mFileName);
 }
 
 SharedImage::~SharedImage()
 {
 }
 
 bool SharedImage::isValidImage() const
 {
  return mTexture.isNull() == false;
 }
 
 Ogre::TexturePtr SharedImage::getTexturePtr() const
 {
  return mTexture;
 }
 
 Ogre::uint SharedImage::getWidth() const
 {
  return mTexture->getWidth();
 }
 
 Ogre::uint SharedImage::getHeight() const
 {
  return mTexture->getHeight();
 }
 
 Ogre::MaterialPtr  SharedImage::getOverlayMaterial() const
 {
  return mOverlayMaterial;
 }
 
 Ogre::MaterialPtr  SharedImage::getMovableObjectMaterial() const
 {
  return mMovableObjectMaterial;
 }
 
 Ogre::uint  SharedImage::getNbReferences() const
 {
  return mReferences;
 }
 
 void SharedImage::_addReference()
 {
  mReferences++;
 }
 
 void SharedImage::_removeReference()
 {
  mReferences--;
 }
 
 void SharedImage::_makeMaterials()
 {
  // TODO
 }
 

 SubImage::SubImage(SharedImage* image, const Region& region)
 : mImage(image)
 {
  mRegion = mImage->convertUnits(region, Region::Relative);
  mImage->_addReference();
  clearAnimation();
 }
 
 SubImage::~SubImage()
 {
  mImage->_removeReference();
 }
 
 void SubImage::clearAnimation()
 {
  mNbFrames = 1;
  mDeltaX = 0;
  mDeltaY = 0;
  mDeltaWidth = 0;
  mDeltaHeight = 0;
  mFramesPerSecond = 0;
 }
 
 void SubImage::setAnimationFrameCount(unsigned int nbFrames)
 {
  mNbFrames = nbFrames;
 }

 void SubImage::setAnimationDeltaX(Ogre::Real deltaX)
 {
  mDeltaX = deltaX;
 }

 void SubImage::setAnimationDeltaY(Ogre::Real deltaY)
 {
  mDeltaY = deltaY;
 }

 void SubImage::setAnimationDeltaWidth(Ogre::Real deltaWidth)
 {
  mDeltaWidth = deltaWidth;
 }

 void SubImage::setAnimationDeltaHeight(Ogre::Real deltaHeight)
 {
  mDeltaHeight = deltaHeight;
 }

 void SubImage::setAnimationLoops(bool doesLoop)
 {
  mDoesLoop = doesLoop;
 }

 void SubImage::setAnimationFramesPerSecond(Ogre::Real frames_per_second)
 {
  mFramesPerSecond = frames_per_second;
 }
 
 bool SubImage::isAnimated() const
 {
  return mFramesPerSecond > 1;
 }
 
 Ogre::uint SubImage::getAnimationNbFrames() const
 {
  return mNbFrames;
 }
   
 Ogre::Real SubImage::getAnimationDeltaX() const
 {
  return mDeltaX;
 }
 
 Ogre::Real SubImage::getAnimationDeltaY() const
 {
  return mDeltaY;
 }
 
 Ogre::Real SubImage::getAnimationDeltaWidth() const
 {
  return mDeltaWidth;
 }
 
 Ogre::Real SubImage::getAnimationDeltaHeight() const
 {
  return mDeltaHeight;
 }
 
 bool SubImage::getAnimationLoops() const
 {
  return mDoesLoop;
 }
 
 Ogre::Real SubImage::getAnimationFramesPerSecond() const
 {
  return mFramesPerSecond;
 }
 
 void SubImage::_increaseReference()
 {
  mReferences++;
 }
 
 void SubImage::_decreaseReference()
 {
  mReferences--;
 }
 
}
