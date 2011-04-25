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

#ifndef GORILLA_H
#define GORILLA_H

#include "Shared.h"

namespace Gorilla
{
 
 /* class. Silverback
    description.
      Main singleton class for Gorilla, that is reponsible for;
       - Parsing gorilla-font and gorilla-image files.
       - Creating, Storing and Destructing Canvases, Fonts, Images and SubImages.
    notes.
         As with any Gorilla class, any class created by Gorilla will be destroyed in the
         Silverback's constructor.
 */
 class Silverback : public Ogre::Singleton<Silverback>, public Ogre::GeneralAllocatedObject, public Ogre::FrameListener
 {
   
   public:
    
    typedef Ogre::map<Ogre::String, SharedImage*>::type  SharedImages;
    typedef Ogre::map<Ogre::String, Font*>::type         Fonts;
    typedef Ogre::map<Ogre::String, SubImage*>::type     SubImages;
    typedef Ogre::vector<OverlayCanvas*>::type           OverlayCanvases;
    typedef Ogre::vector<MovableCanvas*>::type           MovableCanvases;
    
    /* constructor. Silverback
       description.
        Constructor to create an instance of Silverback.
       note.
        This is a Singleton class, so there should be only one of these at a time.
       usage (Creation. After creation of Ogre Root, and main classes).
         
         > mSilverback = new Gorilla::Silverback();
    */
    Silverback();
    
    
    /* destructor. Silverback
       description.
         Destructor to delete an instance of Silverback.
       usage (Before destruction of Ogre Root, and main classes).
         
         > delete mSilverback
    */
   ~Silverback();
    
    /* function. loadFont
       description.
         Load a '.gorilla-font' file through the Ogre ResourceSystem and create a Font out of it.
       args.
         filename -- The filename of the .gorilla-font file
         resourceGroup -- Resource group the filename is in (defaults is; DEFAULT_RESOURCE_GROUP_NAME)
       return.
         Created font pointer, or NULL if the gorilla-font file could not be found or has incorrect syntax.
       note.
         The name of the font is given by the 'name' property in the gorilla-font file, and not the filename.
    */
    Font*  loadFont(const Ogre::String& filename, const Ogre::String& resourceGroup = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    
    /* function. _createFont
       description.
        Manually create a Font, which the glyphs and font information can be created within Code.
       args.
        name -- The name of the font.
       return.
         The created Font.
    */
    Font*  _createFont(const Ogre::String& name);
    
    /* function. _destroyFont
       description.
         Manually delete a Font.
       args.
         name -- The name of the font to delete.
       return.
         If the font was deleted or not.
       note.
         Fonts use a reference system, the Font will be deleted if the font is no longer used by any of the canvases.
    */
    bool  _destroyFont(const Ogre::String& name);
    
    /* function. getFont
       desc.
          Get a font by it's name
       args.
          name -- The name of the font
       return.
          The font, or NULL if the font does not exist.
       note.
          Font names are case-sensitive.
    */
    Font*  getFont(const Ogre::String& name) const;
    
    /* function. hasFont
       desc.
          Does a font exist?
       args.
          name -- The name of the font
       return.
          If the font exists in Gorilla or not.
       note.
          Font names are case-sensitive.
    */
    bool  hasFont(const Ogre::String& name) const;
    
    /* function. loadSubImages
       description.
         Load a '.gorilla-image' file through the Ogre ResourceSystem and create one or many Sub-images out of it.
       args.
         filename -- The filename of the .gorilla-image file
         resourceGroup -- Resource group the filename is in (defaults is; DEFAULT_RESOURCE_GROUP_NAME)
    */
    void  loadSubImages(const Ogre::String& filename, const Ogre::String& resourceGroup = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    
    SharedImage*  _createSharedImage(const Ogre::String& image_file_name);
    
    /* function. _createSubImage
       description.
         Create a subimage manually, if the SubImage already exists then that SubImage is returned.
       args.
         name -- Name of the subimage
         image -- The image to work from.
       return.
         The created subimage.
    */
    SubImage*  _createSubImage(const Ogre::String& name, SharedImage* image);
    
    /* function. _destroySubImage
       description.
         Destroy a subimage manually.
       args.
         name -- Name of the subimage
       return.
         If the subimage was destroyed or not.
       note.
         SubImages use referencing counting. The SubImage will only be destroyed if not used.
    */
    bool  _destroySubImage(const Ogre::String& name);
    
    /* function. getSubImage
       description.
         Get SubImage by name.
       args.
         name -- Name of the subimage
       note.
          SubImages names are case-sensitive.
       return.
          The subimage.
       note.
          SubImage names are case-sensitive.
    */
    SubImage*  getSubImage(const Ogre::String& name) const;
    
    /* function. hasSubImage
       description.
         Does a SubImage exist?
       args.
         name -- Name of the subimage
       return.
         If the SubImage exists or not.
       note.
          SubImage names are case-sensitive.
    */
    bool  hasSubImage(const Ogre::String& name) const;
    
    /* function. createOverlayCanvas
       desc.
         Create an Overlay canvas
       args.
         id -- RenderQueue ID (default is RENDER_QUEUE_OVERLAY)
       return.
         The overlay canvas.
    */
    OverlayCanvas*  createOverlayCanvas(Ogre::RenderQueueGroupID id = Ogre::RENDER_QUEUE_OVERLAY);
    
    /* function. createOverlayCanvas
       desc.
         Create an Movable Canvas that can be attached to a SceneNode.
       args.
         movable_object_name -- Name of the MovableCanvas (default is BLANK string, which automatically generates one)
       return.
         The movable canvas.
       note.
         As with all Ogre MovableObjects, the MovableCanvas must be attached to a SceneNode.
    */
    MovableCanvas*  createMovableCanvas(const Ogre::String& movable_object_name = Ogre::StringUtil::BLANK);
    
    /* function. destroyCanvas
       desc.
         Destroy a Canvas (Movable or Overlay).
       args.
         The canvas to destroy.
       note.
         If the Canvas is a MovableCanvas, and is attached. Then this function will detattach it for you
         automatically.
    */
    void  destroyCanvas(Canvas*);
    
    /* function. destroyAllOverlayCanvases
       desc.
         Destroy all Canvases that are OverlayCanvases.
    */
    void  destroyAllOverlayCanvases();
    
    /* function. destroyAllMovableCanvases
       desc.
         Destroy all Canvases that are MovableCanvases.
    */
    void  destroyAllMovableCanvases();
    
    /* function. getOverlayCanvases
       desc.
         Get an iterator to all of the OverlayCanvases.
    */
    Ogre::ConstVectorIterator<OverlayCanvases>  getOverlayCanvases() const;
    
    /* function. getMovableCanvases
       desc.
         Get an iterator to all of the MovableCanvases.
    */
    Ogre::ConstVectorIterator<MovableCanvases>  getMovableCanvases() const;
    
   protected:
    
    void            _createShaders();
    void            _createOverlayCanvasBaseMaterial();
    void            _createMovableCanvasBaseMaterial();
    void            _createSolidWhiteImage();
    
    SharedImages     mSharedImages;
    Fonts            mFonts;
    SubImages        mSubImages;
    SubImage*        mSolidWhiteSubImage;
    OverlayCanvases  mOverlayCanvases;
    MovableCanvases  mMovableCanvases;
    
 };
 
}


#endif
