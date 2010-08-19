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

#include "OGRE/OgrePrerequisites.h"
#include "OGRE/OgreCommon.h"
#include "OGRE/OgreVector2.h"
#include "OGRE/OgreVector3.h"
#include "OGRE/OgreColourValue.h"
#include "OGRE/OgreMaterial.h"
#include "OGRE/OgreTexture.h"
#include "OGRE/OgreFrameListener.h"
#include "OGRE/OgreResourceGroupManager.h"
#include "OGRE/OgreConfigFile.h"
#include "OGRE/OgreHardwareBufferManager.h"
#include "OGRE/OgreHardwareVertexBuffer.h"
#include "OGRE/OgreSimpleRenderable.h"
#include "OGRE/OgreRenderQueueListener.h"

namespace Gorilla
{
  
  class Screen;
  
  inline Ogre::ColourValue rgb(Ogre::uchar r, Ogre::uchar g, Ogre::uchar b, Ogre::uchar a = 255)
  {
   return Ogre::ColourValue(float(r) / 255.0f,float(g) / 255.0f,float(b) / 255.0f, float(a) / 255.0f);
  }
  
  inline Ogre::ColourValue random_colour(Ogre::Real alpha = 1.0)
  {
   return Ogre::ColourValue(
     Ogre::Math::RangeRandom(0,1),
     Ogre::Math::RangeRandom(0,1),
     Ogre::Math::RangeRandom(0,1),
     alpha); 
  }
  
  // Slightly rewritten and stolen from NxOgre's Buffer class.
  template<typename T> class buffer
  {

   public:

    static const size_t npos = -1;

    inline buffer() : mFirst(0), mLast(0), mEnd(0)
    { // constructor.
    }

    inline buffer(const buffer& other) : mFirst(0), mLast(0), mEnd(0)
    { // copy constructor.
     resize(other.size());
     std::copy(other.mFirst, other.mLast, mFirst);
    }

    inline ~buffer()
    {
     if (mFirst)
      free(mFirst);
    }

    inline size_t size() const
    {
     return size_t(mLast - mFirst);
    }

    inline size_t capacity() const
    {
     return size_t(mEnd - mFirst);
    }

    inline T& operator[](size_t index)
    {
     return *(mFirst + index);
    }

    inline const T& operator[](size_t index) const
    {
     return *(mFirst + index);
    }

    inline T& at(size_t index)
    {
     return *(mFirst + index);
    }

    inline const T& at(size_t index) const
    {
     return *(mFirst + index);
    }

    inline size_t index(const T& value)
    {
     T* it = mFirst;
     while(it != mLast)
     {
      if (*it == value)
       return it - mFirst;
     }
     return npos;
    }

    inline void resize(size_t new_size)
    {
     
     T* new_first = (T*) malloc(sizeof(T) * new_size);

     if (size() != 0)
     {
      if (size() < new_size)
       std::copy(mFirst, mLast, new_first);
      else if (size() >= new_size)
       std::copy(mFirst, mFirst + new_size, new_first);
     }
     
     free(mFirst);
     mEnd = new_first + new_size;
     mLast = new_first + size();
     mFirst = new_first;
     
    }
     
    inline void remove_all()
    {
     mLast = mFirst;
    }

    inline void push_back(const T& value)
    {
     if (mLast == mEnd)
      resize(size() == 0 ? 1 : size() * 2);
     *mLast = value;
     mLast++;
    }

    inline void pop_back()
    {
     if (mLast != mFirst)
      mLast--;
    }

    inline void fill(const T& value)
    {
     std::fill(mFirst, mLast, value);
    }

    inline T* first()
    {
     return mFirst;
    }

    inline T* last()
    {
     return mLast;
    }
    
   protected:
    
     T *mFirst, *mLast, *mEnd;
    
  };
  
  struct Kerning
  {
   Kerning(Ogre::uint c, Ogre::Real k) : character(c), kerning(k) {}
   Ogre::uint  character;
   Ogre::Real  kerning;
  };

  class Glyph
  {
    
   public:
    
    Glyph() : uvTop(0), uvBottom(0), uvWidth(0), uvHeight(0), uvLeft(0), uvRight(0), glyphWidth(0), glyphHeight(0), glyphAdvance(0) {}
    
   ~Glyph() {}
    
    Ogre::Real          uvTop, uvBottom, uvWidth, uvHeight, uvLeft, uvRight,
                        glyphWidth, glyphHeight, glyphAdvance;
    buffer<Kerning>     kerning;
    
    // Get kerning value of a character to the right of another.
    // Ab  -- get the kerning value of b, pass on A.
    inline const Ogre::Real getKerning(char left_of) const
    { 
     if (kerning.size() == 0)
      return 0;
     for (size_t i=0;i < kerning.size();i++)
     {
      if (kerning[i].character == left_of)
       return kerning[i].kerning;
     }
     return 0;
    }
    
  };
  
  class Sprite
  {
    
   public:

    Sprite() {}
    
   ~Sprite() {}
    
    Ogre::Real          uvTop, uvLeft, uvRight, uvBottom, spriteWidth, spriteHeight;
    
  };
  
  class TextureAtlas
  {
    
    friend class Silverback;
    
    public:
     
     inline Ogre::TexturePtr  getTexture() const
     {
      return mTexture; 
     }
     
     inline Ogre::MaterialPtr  getMaterial() const
     {
      return mMaterial; 
     }
     
     inline Ogre::String  getMaterialName() const
     {
      return mMaterial->getName(); 
     }
     
     inline Glyph*  getGlyph(Ogre::uint character) const
     {
      character = character - mGlyphRangeBegin;
      if (character > mGlyphRangeEnd)
       return mNullGlyph;
      return mGlyphs[character];
     }
     
     inline Sprite*  getSprite(const Ogre::String& name) const
     {
      std::map<Ogre::String, Sprite*>::const_iterator it = mSprites.find(name);
      if (it == mSprites.end())
       return mNullSprite;
      return (*it).second;
     }
     
     inline Ogre::Real getGlyphKerning() const
     {
      return mGlyphAllGlyphsKerning;
     }
     
     inline Ogre::Real getGlyphSpaceLength() const
     {
      return mGlyphSpaceLength;
     }
     
     inline Ogre::Real getGlyphBaseline() const
     {
      return mGlyphBaseline;
     }
     
     inline Ogre::Real getGlyphLineHeight() const
     {
      return mGlyphLineHeight;
     }
     
     inline Ogre::Real getRectangleCoordX() const
     {
      return mRectangleCoords.x;
     }
     
     inline Ogre::Real getRectangleCoordY() const
     {
      return mRectangleCoords.y;
     }
     
     inline Ogre::Pass* getPass() const
     {
      return mPass;
     }
    
     inline Ogre::uint getGlyphRangeBegin() const
     {
      return mGlyphRangeBegin;
     }
    
     inline Ogre::uint getGlyphRangeEnd() const
     {
      return mGlyphRangeEnd;
     }

    protected:
     
     TextureAtlas(const Ogre::String& gorillaFile, const Ogre::String& group);
    ~TextureAtlas();
     
     void  _reset();
     void  _load(const Ogre::String& gorillaFile, const Ogre::String& groupName);
     void  _loadTexture(Ogre::ConfigFile::SettingsMultiMap*);
     void  _loadGlyphs(Ogre::ConfigFile::SettingsMultiMap*);
     void  _loadKerning(Ogre::ConfigFile::SettingsMultiMap*);
     void  _loadSprites(Ogre::ConfigFile::SettingsMultiMap*);
     void  _save(const Ogre::String& gorillaFile);
     void  _createMaterial();
     void  _calculateCoordinates();
    
     Ogre::TexturePtr                 mTexture;
     Ogre::MaterialPtr                mMaterial;
     Ogre::TextureUnitState*          mTextureUnit;
     Ogre::Pass*                      mPass;
     std::vector<Glyph*>              mGlyphs;
     std::map<Ogre::String, Sprite*>  mSprites;
     Glyph*                           mNullGlyph;
     Sprite*                          mNullSprite;
     Ogre::uint                       mGlyphRangeBegin, mGlyphRangeEnd;
     Ogre::Real                       mGlyphScale;
     Ogre::Real                       mGlyphSpaceLength;
     Ogre::Real                       mGlyphLineHeight;
     Ogre::Real                       mGlyphBaseline;
     Ogre::Real                       mGlyphAllGlyphsKerning;
     Ogre::Vector2                    mRectangleCoords;
     Ogre::Vector2                    mInverseTextureSize;
   };

  struct Vertex
  {
   Ogre::Vector3      position;
   Ogre::ColourValue  colour;
   Ogre::Vector2      uv;
  };
  
  typedef buffer<Vertex> VertexList;

  class Silverback : public Ogre::Singleton<Silverback>
  {
    
   public:
    
    Silverback();
    
   ~Silverback();
    
    void  loadAtlas(const Ogre::String& name, const Ogre::String& group = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    Screen*  createScreen(Ogre::Viewport*, const Ogre::String& atlas);
    void  destroyScreen(Screen*);
    
   protected:
    
    std::map<Ogre::String, TextureAtlas*>  mAtlases;
    std::vector<Screen*>                   mScreens;
    
  };
  
  
  
  class Screen : public Ogre::RenderQueueListener
  {
   
   friend class Silverback;
   
   public:
    
    struct Rectangle
    { 
      Rectangle() {}
     ~Rectangle() {}
      Ogre::Real         left, top, width, height, depth;
      Ogre::ColourValue  colour;
      size_t             vertexStart;
    };
    
    struct Sprite
    {
      Sprite() {}
     ~Sprite() {}
      Ogre::Real         left, top, depth;
      ::Gorilla::Sprite* sprite;
      size_t             vertexStart;
    };
    
    struct Text
    {
      Text() {}
     ~Text() {}
      Ogre::Real         left, top, depth, knownWidth, knownHeight;
      Ogre::String       text;
      Ogre::ColourValue  colour;
      VertexList         vertices;
      bool               redrawRequested;
    };
    inline      TextureAtlas* getAtlas() const { return mAtlas; }
    Ogre::uint  getViewportWidth() const { return mViewportWidth; }
    Ogre::uint  getViewportHeight() const { return mViewportHeight; }

    size_t      createRectangle(int left, int top, int width, int height, const Ogre::ColourValue& background = Ogre::ColourValue::White, int depth = 0);
    void        removeRectangle(size_t);
    void        removeAllRectangles();
    Ogre::uint  getNbRectangles() const;
    void        setRectanglePosition(Ogre::uint id, int left, int top);
    void        setRectangleSize(Ogre::uint id, Ogre::uint width, Ogre::uint height);
    void        setRectangleZOrder(Ogre::uint id, int zorder);
    void        setRectangleColour(Ogre::uint id, const Ogre::ColourValue& colour);
    void        updateRectangle(Ogre::uint id, int left, int top, Ogre::uint width, Ogre::uint height, int zorder, const Ogre::ColourValue& colour);
    void        getRectanglePosition(size_t id, int& left, int& top) const;
    void        getRectangleSize(Ogre::uint id) const;
    int         getRectangleZOrder(Ogre::uint id) const;
    Ogre::ColourValue getRectangleColour(Ogre::uint id) const;

    size_t createSprite(int left, int top, const Ogre::String& sprite_name, int depth = 0);
    
    size_t createText(int left, int top, const Ogre::String& caption, const Ogre::ColourValue& background = Ogre::ColourValue::White, int depth = 0);
    
    
    void   setTextCaption(size_t text_id, const Ogre::String& caption);
    void   getTextSize(size_t text_id, int& width, int& height);
    
    void   refreshMarkupColours();
    void   setMarkupColour(Ogre::uint colour_palette_index, const Ogre::ColourValue&);
    Ogre::ColourValue  getMarkupColour(Ogre::uint colour_palette_index);

    void  _createVertexBuffer();
    void  _destroyVertexBuffer();
    void  _prepareVertexBuffer(size_t neededVertices);
    void  _prepareRenderSystem();
    void  _startListening();
    void  _stopListening();
    void  refresh();
    void  renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& repeatThisInvocation);
    
    void  _redrawAllRectanges();
    void  _drawRectangle(Screen::Rectangle* rect);
    void  _updateRectangle(Screen::Rectangle* rect);
    
    void  _redrawAllSprites();
    void  _drawSprite(Screen::Sprite* sprite);
    void  _updateSprite(Screen::Sprite* sprite);
    
    void  _redrawAllText();
    void  _drawText(Screen::Text* text);
    bool  _domarkup(Screen::Text* text, size_t& index, Ogre::ColourValue& colour, Ogre::Real& cursorLeft, Ogre::Real& cursorTop, Ogre::Real& nextLineHeight);

    
    inline Ogre::Real _vpx(Ogre::Real x)
    {
     
     if (x < 0) // Becomes right.
     {
      x = mViewportWidth + x;
     }
     
     x *= mInvViewportWidth;
     
     return (x * 2) - 1;
    }
    
    inline Ogre::Real _vpy(Ogre::Real y)
    {
     
     if (y < 0) // Becomes bottom.
     {
      y = mViewportHeight + y;
     }
     
     y *= mInvViewportHeight;
     return 1 - (y * 2);
    }
    
    inline Ogre::Real _vpz(int z)
    {
     return -0.025f + (Ogre::Real(z) * 0.001f);
    }
    
    static inline void pushQuadFixedUV(Vertex& vertex, VertexList&, const Ogre::Real& left, const Ogre::Real& top, const Ogre::Real& bottom, const Ogre::Real& right, const Ogre::Real& depth, const Ogre::ColourValue& colour, const Ogre::Real& fixedU, const Ogre::Real& fixedV);
    
    static inline void setQuadFixedUV(Vertex& vertex, size_t begin, VertexList&, const Ogre::Real& left, const Ogre::Real& top, const Ogre::Real& bottom, const Ogre::Real& right, const Ogre::Real& depth, const Ogre::ColourValue& colour, const Ogre::Real& fixedU, const Ogre::Real& fixedV);
    
    static inline void pushQuad(Vertex& vertex, VertexList&, const Ogre::Real& left, const Ogre::Real& top, const Ogre::Real& bottom, const Ogre::Real& right, const Ogre::Real& depth, const Ogre::ColourValue& colour, const Ogre::Real& uvLeft, const Ogre::Real& uvTop, const Ogre::Real& uvRight, const Ogre::Real& uvBottom);
    
    static inline void setQuad(Vertex& vertex, size_t begin, VertexList&, const Ogre::Real& left, const Ogre::Real& top, const Ogre::Real& bottom, const Ogre::Real& right, const Ogre::Real& depth, const Ogre::ColourValue& colour, const Ogre::Real& uvLeft, const Ogre::Real& uvTop, const Ogre::Real& uvRight, const Ogre::Real& uvBottom);
    
   protected:
    
    Screen(Ogre::Viewport*, Silverback*, TextureAtlas*);
   ~Screen();
    
    Ogre::SceneManager*                    mSceneMgr;
    Ogre::RenderSystem*                    mRenderSystem;
    Ogre::Viewport*                        mViewport;
    Ogre::Real                             mViewportWidth,
                                           mViewportHeight,
                                           mInvViewportWidth,
                                           mInvViewportHeight;
    Silverback*                            mSilverback;
    TextureAtlas*                          mAtlas;
    Ogre::HardwareVertexBufferSharedPtr    mVertexBuffer;
    size_t                                 mMaxVertexCount;
    Ogre::RenderOperation                  mRenderOp;
    std::map<size_t, Screen::Rectangle*>   mRectangles;
    std::map<size_t, Screen::Sprite*>      mSprites;
    std::map<size_t, Screen::Text*>        mText;
    size_t                                 mNextRectangleID, mNextSpriteID, mNextTextID;
    VertexList                             mRectangleData, mSpriteData;
    bool                                   mRefreshRequired;
    size_t                                 mAllTextSize;
    Ogre::ColourValue                      mMarkupColour[10];
    
  };
  
};


#endif