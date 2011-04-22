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

#ifndef GORILLA_FILE_H
#define GORILLA_FILE_H

#include "Shared.h"

namespace Gorilla
{
 
 class Font
 {
   
  public:
   
   typedef unsigned int GlyphID;
   
   Font(const Ogre::String& image, unsigned int native_size);
   
  ~Font();
   
   void setNativeSize(unsigned int size);
   
   void setLineHeight(Ogre::Real line_height_in_pixels);
   
   void addGlyph(GlyphID, Ogre::uint x, Ogre::uint y, Ogre::uint w, Ogre::uint h, Ogre::Real advance = 0);
   
   void addKerning(GlyphID left, GlyphID right, Ogre::Real kerning);
   
   void getGlyphCoordinates(const Ogre::Real& font_size_in_pt, const Ogre::Real& canvas_dpi, Ogre::Real& x, Ogre::Real& y, Ogre::Real& xx, Ogre::Real& yy) const;
   
   void getGlyphKerning(const GlyphID& left, const GlyphID& right, Ogre::Real& kerning) const;
   
   void getGlyphAdvance(const GlyphID& glyph, Ogre::Real& advance);
   
  protected:
   
   struct Glyph
   {
     Glyph();
    ~Glyph();
     Ogre::Real                     mX, mY, mXX, mYY;             // Relative positions (in 0..1 coordinates).
     Ogre::Real                     mAdvance;                     // Glyph advance - in pixels.
     std::map<GlyphID, Ogre::Real>  mKerning;                     // Kerning value when to the left of another character.
   };
   
   Ogre::MaterialPtr                mMaterial;                    // Material to use.
   std::map<GlyphID, Glyph*>        mGlyphs;                      // All of the glyphs.
   unsigned int                     mNativeSize;                  // Native size of the font (in pt).
   unsigned int                     mImageWidth,                  // Width of the image in pixels.
                                    mImageHeight;                 // Height of the image in pixels.
   Ogre::Real                       mLineHeight;
   
 };

 class Sprite
 {
  
  friend class AnimatingSprite;
  
  public:
   
   Sprite(const Ogre::String& image, unsigned int native_size);
   
  ~Sprite();
   
   void clearAnimation();
   
   void setAnimation(unsigned int nbFrames, Ogre::Real deltaFrameX, Ogre::Real deltaFrameY);
   
   void addTiming(unsigned int frame, Ogre::Real time = 0);
   
   void doesLoop(bool val);
   
  protected:
   
   Ogre::Real               mX, mY, mXX, mYY, mFrameX, mFrameY;
   bool                     mIsAnimated;
   bool                     mLoops;
   std::vector<Ogre::Real>  mTiming;
   
 };
 
 class AnimatingSprite
 {
   
  public:
   
   AnimatingSprite(Sprite*, unsigned int frame = 0);
   
  ~AnimatingSprite();
   
   void update(const Ogre::Real& time);
   
   inline unsigned int getFrame() const
   {
    return mCurrentFrame;
   }
   
   inline void getRegion(Ogre::Real& x, Ogre::Real& y, Ogre::Real& xx, Ogre::Real& yy) const
   {
    x = mX;
    y = mY;
    xx = mXX;
    yy = mYY;
   }

   inline bool isAnimating() const
   {
    return mIsAnimating;
   }
   
   inline void pause()
   {
    mIsAnimating = false;
   }
   
   inline void play()
   {
    mIsAnimating = true;
   }
   
  protected:
   
   Sprite*                  mSprite;
   Ogre::Real               mX, mY, mXX, mYY;
   unsigned int             mCurrentFrame;
   bool                     mIsAnimating;
   Ogre::Real               mTimeLeft;
   
 };
 
}

#endif
 