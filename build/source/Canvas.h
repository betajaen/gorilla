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

#ifndef GORILLA_CANVAS_H
#define GORILLA_CANVAS_H

#include "Shared.h"

namespace Gorilla
{
 class Canvas
 {
   
  public:
   
   // Start adding shapes to a layer.
   // - If clear is true, then previous shapes of this layer are removed. (Default Behaviour)
   // - If clear is false, then the existing shapes are preserved and further shapes are drawn after those.
   // Call end() after use.
   void begin(size_t layer_index, bool clear = true);
   
   // Stop adding shapes to the current layer.
   // Should be called after a begin().
   void end();
   
   void setFill(const Ogre::ColourValue& colour);
   
   void setFill(const Ogre::String& subimage_name);
   
   void setFill(SubImage*);
   
   void setStroke(const Ogre::ColourValue& colour);
   
   void setStrokeWidth(Ogre::Real width);
   
   void setTextColour(const Ogre::String&);
   
   void placeLine(Ogre::Real from_x, Ogre::Real from_y, Ogre::Real to_x, Ogre::Real to_y);
   
   void beginPath(Ogre::Real begin_x, Ogre::Real begin_y);
   
   void addPathPoint(Ogre::Real x, Ogre::Real y);
   
   void endPath();
   
   void beginPolygon(Ogre::Real begin_x, Ogre::Real begin_y);
   
   void addPolygonPoint(Ogre::Real x, Ogre::Real y);
   
   void endPolygon();
   
   // Create a Rectangle at x,y according to sub-image width and height.
   void placeRectangle(Ogre::Real x, Ogre::Real y);
   
   // Create a Rectangle at x,y according to w and h in size.
   void placeRectangle(Ogre::Real x, Ogre::Real y, Ogre::Real w, Ogre::Real h);
   
   void placeCharacter(Ogre::Real x, Ogre::Real y, Ogre::uint glyph_id);
   
   void placeText(Ogre::Real x, Ogre::Real y, const Ogre::String& text);
   
   void placeQuad(Ogre::Real tl_x, Ogre::Real tl_y, Ogre::Real tr_x, Ogre::Real tr_y, Ogre::Real bl_x, Ogre::Real bl_y, Ogre::Real br_x, Ogre::Real br_y);
   
  protected:
   
   size_t                       mNextQuadID;
   SubImage*                    mCurrentSubImage;
   Font*                        mCurrentFont;
   Ogre::ColourValue            mCurrentFillColour, mCurrentTextColour;
   Ogre::Real                   mBuildBeginX, mBuildBeginY;
   Ogre::vector<Ogre::Vector2>  mBuildPoints;
   
 };
 
}


#endif
