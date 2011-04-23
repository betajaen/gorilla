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

#ifndef GORILLA_H
#define GORILLA_H

#include "Shared.h"

namespace Gorilla
{
 
 class Silverback : public Ogre::Singleton<Silverback>, public Ogre::GeneralAllocatedObject, public Ogre::FrameListener
 {
   
   public:
    
    Silverback();
    
   ~Silverback();
    
    Font*           loadFont(const Ogre::String& filename, const Ogre::String& resourceGroup = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    
    Font*           _createFont(const Ogre::String& name);
    
    bool            _destroyFont(const Ogre::String& name);
    
    Font*           getFont(const Ogre::String& name) const;
    
    bool            hasFont(const Ogre::String& name) const;
    
    void            loadSprites(const Ogre::String& filename, const Ogre::String& resourceGroup = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    
    Sprite*         _createSprite(const Ogre::String& name);
    
    void            _destroySprite(const Ogre::String& name);
    
    Sprite*         getSprite(const Ogre::String& name) const;
    
    bool            hasSprite(const Ogre::String& name) const;
    
    OverlayCanvas*  createOverlayCanvas(Ogre::RenderQueueGroupID id = Ogre::RENDER_QUEUE_OVERLAY);
    
    MovableCanvas*  createMovableCanvas(const Ogre::String& movable_object_name = Ogre::StringUtil::BLANK);
    
    
    
    void            destroyCanvas(Canvas*);
        
    void            destroyAllOverlayCanvases();
    
    void            destroyAllMovableCanvases();
    
    Ogre::ConstVectorIterator<OverlayCanvas*>  getOverlayCanvases() const;
    
    Ogre::ConstVectorIterator<MovableCanvas*>  getMovableCanvases() const;

   protected:
    
    void            _createShaders();
    void            _createOverlayCanvasBaseMaterial();
    void            _createMovableCanvasBaseMaterial();
    void            _createSolidWhiteSprite();
    
    Ogre::map<Ogre::String, SharedImage*>::type mImages;
    Ogre::map<Ogre::String, Font*>::type        mFonts;
    Ogre::map<Ogre::String, Sprite*>::type      mSprites;
    Sprite*                                     mSolidWhiteSprite;
    std::vector<OverlayCanvas*>                 mOverlayCanvases;
    std::vector<MovableCanvas*>                 mMovableCanvases;
    
 };
 
}


#endif
