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

#ifndef GORILLA_FILE_H
#define GORILLA_FILE_H

#include "Shared.h"

namespace Gorilla
{
 
 struct Region
 {
  enum CoordinateUnits
  {
   Pixels,
   Relative
  };
  
  Region()
    : left(0.0f), top(0.0f), width(1.0f), height(1.0f), units(Pixels)
   {
   }
  
  Region(Ogre::Real l, Ogre::Real t, Ogre::Real w, Ogre::Real h, CoordinateUnits u = Pixels) 
    : left(l), top(t), width(w), height(h), units(u)
  {
  }
  
  Ogre::Real left,
             top,
             width,
             height;
  
  CoordinateUnits units;
  
 };

 class SharedImage
 {
   
  public:
   
   friend class Font;
   friend class SubImage;
   
   SharedImage(const Ogre::String& file_name);
   
  ~SharedImage();
   
   bool  isValidImage() const;
   
   Ogre::TexturePtr  getTexturePtr() const;
   
   Ogre::uint getWidth() const; 
   
   Ogre::uint getHeight() const;
   
   Ogre::MaterialPtr  getOverlayMaterial() const;
   
   Ogre::MaterialPtr  getMovableObjectMaterial() const;
   
   Ogre::uint         getNbReferences() const;
   
   Region             convertUnits(const Region&, Region::CoordinateUnits new_units);
   
  protected:
   
   void _addReference();
   void _removeReference();
   void _makeMaterials();
   
  private:
   
   Ogre::String       mFileName;
   Ogre::TexturePtr   mTexture;
   Ogre::MaterialPtr  mOverlayMaterial;
   Ogre::MaterialPtr  mMovableObjectMaterial;
   Ogre::uint         mReferences;
   
 };
 
 class Font
 {
   
  public:
   
   typedef unsigned int GlyphID;
   
   Font(SharedImage*, unsigned int native_font_size);
   
  ~Font();
   
   void setNativeSize(unsigned int size);
   
   void setLineHeight(Ogre::Real line_height_in_pixels);
   
   void addGlyph(GlyphID, const Region& region, Ogre::Real advance = 0);
   
   void addKerning(GlyphID left, GlyphID right, Ogre::Real kerning);
   
   void getGlyphCoordinates(const Ogre::Real& font_size_in_pt, const Ogre::Real& canvas_dpi, Ogre::Real& x, Ogre::Real& y, Ogre::Real& xx, Ogre::Real& yy) const;
   
   void getGlyphKerning(const GlyphID& left, const GlyphID& right, Ogre::Real& kerning) const;
   
   void getGlyphAdvance(const GlyphID& glyph, Ogre::Real& advance);
   
  protected:
   
   struct Glyph
   {
     Glyph();
    ~Glyph();
     Region                    mRegion;                      // Region of image (in relative units).
     Ogre::Real                     mAdvance;                     // Glyph advance - in pixels.
     std::map<GlyphID, Ogre::Real>  mKerning;                     // Kerning value when to the left of another character.
   };
   
  private:
   
   SharedImage*                     mImage;                       // Image.
   std::map<GlyphID, Glyph*>        mGlyphs;                      // All of the glyphs.
   unsigned int                     mNativeSize;                  // Native size of the font (in pt).
   unsigned int                     mImageWidth,                  // Width of the image in pixels.
                                    mImageHeight;                 // Height of the image in pixels.
   Ogre::Real                       mLineHeight;
   
 };

 class SubImage
 {
  
  friend class AnimatingSubImage;
  friend class OverlayCanvas;
  friend class MovableCanvas;
  
  public:
   
   SubImage(SharedImage*, const Region&);
   
  ~SubImage();
   
   void setRegion(const Region&);
   
   void clearAnimation();
   
   void setAnimationFrameCount(Ogre::uint nbFrames);

   void setAnimationDeltaX(Ogre::Real deltaX);

   void setAnimationDeltaY(Ogre::Real deltaY);
   
   void setAnimationDeltaWidth(Ogre::Real deltaWidth);
   
   void setAnimationDeltaHeight(Ogre::Real deltaHeight);
   
   void setAnimationLoops(bool doesLoop);
   
   void setAnimationFramesPerSecond(Ogre::Real frames_per_second);
   
   bool isAnimated() const;
   
   Ogre::uint getAnimationNbFrames() const;
   
   Ogre::Real getAnimationDeltaX() const;
   
   Ogre::Real getAnimationDeltaY() const;
   
   Ogre::Real getAnimationDeltaWidth() const;
   
   Ogre::Real getAnimationDeltaHeight() const;
   
   bool getAnimationLoops() const;
   
   Ogre::Real getAnimationFramesPerSecond() const;
   
   Ogre::uint getReferences() const;
   
  private:
   
   void _increaseReference();
   
   void _decreaseReference();
   
   SharedImage*  mImage;
   Region        mRegion;                                      // Region of image (in relative units).
   Ogre::Real    mDeltaX, mDeltaY, mDeltaWidth, mDeltaHeight;  // Delta animation offset (in relative units).
   Ogre::uint    mNbFrames;                                    // Number of frames the subimage has (at least 1 or more).
   bool          mDoesLoop;                                    // Is the animation played once or continously?
   Ogre::Real    mFramesPerSecond;                             // How many frames per second does it play at?
   Ogre::uint    mReferences;                                  // How many Canvases reference this?
   
 };
 
 struct AnimatingRectangle
 {
   SubImage*    mSubImage;
   Region       mFrameRegion;
   Region       mCanvasRegion;
   Ogre::Real   mTime;
   Ogre::uint   mFrame;
 };
 
}

#endif
 