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
   
  protected:
   
   /** function. _transformVertices
       desc.
           Transform pixel vertices, into user coordinates; i.e. screen or world
           space. Vertices are formatted as x,y,z,tex_x,tex_y,col_r,col_g,col_b,col_a
       args.
           const float* -- The source vertices.
           float*       -- The destination vertices. Memory size is at least the same size as source vertices.
   */
   virtual void _transformVertices(const float* source, float* dest) = 0;
   
   struct Layer
   {
     Layer();
    ~Layer();
     Ogre::map<size_t, GorillaVertexBuffer*>  mVertexBuffers;
     Ogre::map<size_t, bool>                  mVertexBufferDirty;
     bool                                     mAllVertexBuffersDirty;
   };
   
   struct CompiledVertexBuffer
   {
     CompiledVertexBuffer();
    ~CompiledVertexBuffer();
     Ogre::HardwareVertexBufferSharedPtr  mVertexBuffer;
     size_t                               mVertexBufferSize;
     Ogre::RenderOperation*               mRenderOpPtr;
     GorillaFile*                         mGorillaFile;
   };
   
   struct Animation
   {
    GorillaFile::Sprite*  mSprite;
    unsigned int          mFrame;
    bool                  mIsAnimating;
    unsigned int          mVertexIDs[6];  // Vertices in the *compiled* buffer so they can be 
                                          // changed each frame without recompiling the vertex buffer each time.
                                          // This could be tricky to get working properly.
   };
   
   // Current layer being worked on, or NULL for none.
   Layer*  mCurrentLayer;
   
   // Master copy of all vertices, organised by layers, then organized by TextureAtlases.
   Ogre::map<size_t, Layer*>  mLayers;
   
   // Final copy of all the vertices organised by GorillaFiles (materials).
   Ogre::map<size_t, CompiledVertexBuffer*>  mVertexBuffers;
   
   // If *a* vertex buffer is dirty.
   bool  mVertexBufferDirty;
   
 };
 
}


#endif
