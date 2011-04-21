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

#include "Shared.h"

namespace Gorilla
{
 
 class GorillaVertexBuffer
 {
  public:
   
   public:
   
   inline GorillaVertexBuffer() : mBuffer(0), mUsed(0), mCapacity(0)
   { // no code.
   }
  
   inline ~GorillaVertexBuffer()
   {
    if (mBuffer && mCapacity)
     OGRE_FREE(mBuffer, Ogre::MEMCATEGORY_GEOMETRY);
   }
   
   inline size_t size() const
   {
    return mUsed;
   }
   
   inline size_t capacity() const
   {
    return mCapacity;
   }
   
   inline float& operator[](size_t index)
   {
    return *(mBuffer + index);
   }

   inline const float& operator[](size_t index) const
   {
    return *(mBuffer + index);
   }

   inline float& at(size_t index)
   {
    return *(mBuffer + index);
   }

   inline const float& at(size_t index) const
   {
    return *(mBuffer + index);
   }
   
   inline void remove_all()
   {
    mUsed = 0;
   }
   
   inline void resizeNoCopy(size_t new_capacity)
   {
    
    float* new_buffer = (float*) OGRE_MALLOC(sizeof(float) * new_capacity, Ogre::MEMCATEGORY_GEOMETRY);
    
    if (mBuffer)
    {
     OGRE_FREE(mBuffer, Ogre::MEMCATEGORY_GEOMETRY);
    }
    
    mCapacity = new_capacity;
    mBuffer = new_buffer;
    mUsed = 0;
   }
   
   inline void resizeCopy(size_t new_capacity)
   {
    
    float* new_buffer = (float*) OGRE_MALLOC(sizeof(float) * new_capacity, Ogre::MEMCATEGORY_GEOMETRY);
    
    if (mUsed != 0)
    {
     if (mUsed < new_capacity)       // Copy all
      memcpy(new_buffer, mBuffer, mUsed);
     else if (mUsed >= new_capacity) // Copy some
      memcpy(new_buffer, mBuffer, new_capacity);
    }
    
    if (mBuffer)
    {
     OGRE_FREE(mBuffer, Ogre::MEMCATEGORY_GEOMETRY);
    }
    
    mCapacity = new_capacity;
    mBuffer = new_buffer;
   }
   
   inline void push_back(const float& value)
   {
    if (mUsed == mCapacity)
     resizeCopy(mUsed == 0 ? 1 : mUsed * 2);
    *(mBuffer + mUsed) = value;
    mUsed++;
   }
   
   inline void pop_back()
   {
    if (mUsed != 0)
     mUsed--;
   }
   
   inline void erase(size_t index)
   {
    *(mBuffer + index) = *(mBuffer + mUsed - 1);
    mUsed--;
   }
   
   inline  float* first()
   {
    return mBuffer;
   }
   
   inline float* last()
   {
    return mBuffer + mUsed;
   }
   
  protected:
   
   float*     mBuffer;
   size_t    mUsed, mCapacity;
   
 };
 
}
