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

#include "OGRE/Ogre.h"

namespace Gorilla
{
  
 class Silverback;
 class TextureAtlas;
 class Screen;
 class Renderable;
 class Canvas;
 class SpriteLayer;
 class Text;
 class Glyph;
 class Sprite;
  
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
 
 struct Quad
 {
  Ogre::Real         left, top, right, bottom;          
  Ogre::ColourValue  colour[4];                         // left/top, right/top, bottom/right, bottom/left
  Ogre::Real         uvLeft, uvTop, uvRight, uvBottom;  
  size_t             source;
 };
 
 struct Vertex
 {
  Ogre::Vector3      position;
  Ogre::ColourValue  colour;
  Ogre::Vector2      uv;
 };
 
 struct Kerning
 {
  Kerning(Ogre::uint c, Ogre::Real k) : character(c), kerning(k) {}
  Ogre::uint  character;
  Ogre::Real  kerning;
 };
 
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
   
   inline void erase(size_t index)
   {
    index = at(size()-1);
    pop_back();
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
   
   Ogre::Real  uvTop, uvLeft, uvRight, uvBottom, spriteWidth, spriteHeight;
   
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
      return 0;
     return mGlyphs[character];
    }
    
    inline Sprite*  getSprite(const Ogre::String& name) const
    {
     std::map<Ogre::String, Sprite*>::const_iterator it = mSprites.find(name);
     if (it == mSprites.end())
      return 0;
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
    
    inline Ogre::Real getRectangleCoordsX() const
    {
     return mRectangleCoords.x;
    }
    
    inline Ogre::Real getRectangleCoordsY() const
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
    
    inline Ogre::Real getGlyphMonoWidth() const
    {
     return mGlyphMonoWidth;
    }
    
    void  refreshMarkupColours();
    
    void  setMarkupColour(Ogre::uint colour_palette_index, const Ogre::ColourValue&);
    
    Ogre::ColourValue  getMarkupColour(Ogre::uint colour_palette_index);
    
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
    Ogre::uint                       mGlyphRangeBegin, mGlyphRangeEnd;
    Ogre::Real                       mGlyphScale;
    Ogre::Real                       mGlyphSpaceLength;
    Ogre::Real                       mGlyphLineHeight;
    Ogre::Real                       mGlyphBaseline;
    Ogre::Real                       mGlyphAllGlyphsKerning;
    Ogre::Real                       mGlyphMonoWidth;
    Ogre::Vector2                    mRectangleCoords;
    Ogre::Vector2                    mInverseTextureSize;
    Ogre::ColourValue                mMarkupColour[10];
  };
  

 
 class Screen : public Ogre::RenderQueueListener
 { 
  public:
   
   Screen(Ogre::Viewport*, TextureAtlas*);
   
  ~Screen();
   
   Canvas*       createCanvas(int layer = 0);
   
   SpriteLayer*  createSpriteLayer(int layer = 0);
   
   Text*         createText(Ogre::Real left, Ogre::Real top, const Ogre::String& initialText, int layer = 0);
   
   inline void      vpX(Ogre::Real& x)
   {
     if (x < 0) // Becomes right.
      x = mViewportWidth + x;
     
     vpW(x);
   }
   
   inline void      vpY(Ogre::Real& y)
   {
     if (y < 0) // Becomes bottom.
      y = mViewportHeight + y;
     
     vpH(y);
   }
   
   inline void      vpW(Ogre::Real& x)
   {
     x *= mInvViewportWidth;
     x = (x * 2) - 1;
   }
   
   inline void      vpH(Ogre::Real& y)
   {
     y *= mInvViewportHeight;
     y = 1 - (y * 2);
   }
   
   inline Ogre::Real  getViewportWidth() const
   { 
    return mViewportWidth;
   }
   
   inline Ogre::Real  getViewportHeight() const
   { 
    return mViewportHeight;
   }

   void  layerRedrawRequested(Ogre::uint layer);

   void  _registerListener();
   void  _unregisterListener();

   void  _createBuffer();
   void  _destroyBuffer();
   void  _checkBuffer(size_t requestedSize);

   void  _calculateLayers();
   void  _redrawAll();
   void  _redrawLayer(Ogre::uint id);
   void  _copyToVertexBuffer();
   
   void  _prepareRenderSystem();
   void  _render();
   
   void  renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& repeatThisInvocation);
   
   inline TextureAtlas*     getAtlas() const { return mAtlas; }
   
  protected:
   
   Ogre::RenderSystem*      mRenderSystem;
   Ogre::SceneManager*      mSceneMgr;
   Ogre::Viewport*          mViewport;
   Ogre::Real               mViewportWidth,
                            mViewportHeight,
                            mInvViewportWidth,
                            mInvViewportHeight;

   TextureAtlas*            mAtlas;
   std::vector<Renderable*> mRenderables;            // Master copy of all Renderables.
   
   // Go through the renderables, get depth of each then add to Depths.
   // Then lock the buffer, then each renderable in depth order, tell them to render
   // to a shared vertex.
   // unlock buffer.
   buffer<Renderable*>      mLayers[16];             // Pointers to Renderables in QuadBuffer sorted by Depth.
   buffer<Vertex>           mVertexLayerBuffer[16];  // Compiled vertex layer for each depth.
   bool                     mRedrawLayerNeeded[16];  // If a layer needs a full redraw.
   bool                     mLayerRedrawNeeded;
   bool                     mRedrawAll;              // Every layer needs a redraw.
   
   Ogre::HardwareVertexBufferSharedPtr    mVertexBuffer;
   size_t                                 mMaxVertexCount, mKnownVertexCount;
   Ogre::RenderOperation                  mRenderOp;
 };
 

 class Renderable
 {
   
  public:
   
   Renderable(Screen*, Ogre::uint layer);
   
  ~Renderable();
   
   bool             isVisible() const
   {
    return mIsVisible;
   }
   
   void             setVisible(bool is_visible)
   {
    mIsVisible = is_visible;
    _redrawNeeded();
   }
   
   void             show()
   {
    mIsVisible = true;
    _redrawNeeded();
   }
   
   void             hide()
   {
    mIsVisible = false;
    _redrawNeeded();
   }
   
   Ogre::uint       getLayer() const
   {
    return  mLayer;
   }
   
   inline void      vpX(Ogre::Real& x)
   {
    return mScreen->vpX(x);
   }
   
   inline void      vpY(Ogre::Real& y)
   {
    return mScreen->vpY(y);
   }
   
   inline void      vpW(Ogre::Real& w)
   {
    return mScreen->vpW(w);
   }
   
   inline void      vpH(Ogre::Real& h)
   {
    return mScreen->vpH(h);
   }
   
   inline void      requestLayerRedraw()
   {
    mScreen->layerRedrawRequested(mLayer);
   }
   
   void             render(buffer<Vertex>&);
   
   virtual void     redraw() {}
   
   inline Ogre::Vector2  getMin() const
   {
    return mMin;
   }
   
   inline Ogre::Vector2  getMax() const
   {
    return mMax;
   }
   
  protected:
   
   inline void      pushQuad(const Quad& quad, const Ogre::Radian& angle);
   
   inline void      pushLine(const Ogre::Vector2& a, const Ogre::Vector2& b, Ogre::Real thickness, const Ogre::ColourValue& colour);
   
   inline void      pushSprite(const Ogre::Vector2& position, const Ogre::Vector2& scale, Sprite*, const Ogre::ColourValue& tint);
   
   inline void      pushGlyph(Glyph*, Ogre::Real left, Ogre::Real top, const Ogre::ColourValue& colour);
   
   inline void      _redrawNeeded()
   {
    mRedrawNeeded = true;
    requestLayerRedraw();
   }
   
   inline void      reset()
   {
    mMin = Ogre::Vector2(mScreen->getViewportWidth(), mScreen->getViewportHeight());
    mMax = Ogre::Vector2::ZERO;
    
    mVertices.remove_all();
   }

   buffer<Vertex>   mVertices;
   
   Screen*          mScreen;
   
   bool             mRedrawNeeded;
   
   Ogre::Vector2    mMin, mMax;
   
   // Static temporary variables.
   static Ogre::Real  tLeft, tRight, tTop, tBottom, t1, t2, tA,tB,tC,tD;
   static Vertex      tVertex;
   
  private:
    
   Ogre::uint       mLayer;
   
   bool             mIsVisible;
   
 };
 
 class Canvas : public Renderable
 {
  
  friend class Screen;
  
  public:
   
   size_t  addRectangle(Ogre::Real left, Ogre::Real top, Ogre::uint width, Ogre::uint height, const Ogre::ColourValue& colour = Ogre::ColourValue::White);
   void    removeRectangle(size_t id);
   void    setRectangleColour(size_t id, const Ogre::ColourValue& colour);
   void    setRectangleColour(size_t id, const Ogre::ColourValue& topLeft, const Ogre::ColourValue& topRight, const Ogre::ColourValue& bottomRight,const Ogre::ColourValue& bottomLeft);
   void    setRectanglePosition(size_t id, Ogre::Real left, Ogre::Real top);
   void    setRectangleSize(size_t id, Ogre::uint width, Ogre::uint height);
   void    setRectangleBackground(size_t id, const Ogre::String& sprite_name, bool resetColour = true);
   void    setRectangleAngle(size_t id, const Ogre::Degree&);
   void    setRectangleAngle(size_t id, const Ogre::Radian&);
   void    setRectangleMinMax(size_t id, const Ogre::Vector2& min, const Ogre::Vector2& max);
   void    clearRectangleBackground(size_t id);
   
   size_t  addLine(int x1, int y1, int x2, int y2, int thickness, const Ogre::ColourValue& colour = Ogre::ColourValue::White);
   void    removeLine(size_t);
   void    setLineColour(size_t, const Ogre::ColourValue&);
   void    setLineCoords(size_t, int x1, int y1, int x2, int y2);
   void    setLineOrigin(size_t, int x1, int y1);
   void    setLineEnd(size_t, int x2, int y2);
   void    setLineThickness(size_t, int thickness);
   
  protected:
   
   struct Rectangle
   {
    Quad         quad;
    size_t       id;
    Ogre::Radian angle;
   };
   
   struct Line
   {
    Ogre::Vector2     a,b;
    Ogre::ColourValue colour;
    Ogre::Real        thickness;
    size_t            id;
   };
   
   Canvas(Ogre::uint layer, Screen*);
   
  ~Canvas();
   
   void  redraw();
   
   std::map<size_t, Rectangle> mRectangles;
   std::map<size_t, Line>      mLines;
   size_t                      mNextRectangleID, mNextLineID;
   
 };
 
 class SpriteLayer : public Renderable
 {
    
   friend class Screen;
    
   public:
    
    size_t         addSprite(Ogre::Real left, Ogre::Real top, const Ogre::String& name);
    void           removeSprite(size_t id);
    void           setSpritePosition(size_t id, Ogre::Real left, Ogre::Real top);
    void           setSprite(size_t id, const Ogre::String& name);
    void           setSpriteScale(size_t id, Ogre::Real scaleX, Ogre::Real scaleY);
    void           getSpritePosition(size_t, int& left, int& top);
    Ogre::Vector2  getSpriteScale(size_t);
    Sprite*        getSprite(size_t);

    
   protected:
    
    struct Bob // A homage to http://en.wikipedia.org/wiki/Blitter_object
    {
     Sprite*            sprite;
     Ogre::Vector2      position, scale;
     Ogre::ColourValue  tint;
     size_t             id;
    };
    
    SpriteLayer(Ogre::uint layer, Screen*);
    
   ~SpriteLayer();
    
    void  redraw();
    
    std::map<size_t, Bob> mBobs;
    size_t                mNextBobID;
   
 };
 
 class Text : public Renderable
 {
    
   friend class Screen;
    
   public:
    
    void   setPosition(const Ogre::Vector2& position)
    {
     mLeft = position.x;
     mTop = position.y;
     _redrawNeeded();
    }
    
    void   setText(const Ogre::String& text)
    {
     mText = text;
     _redrawNeeded();
    }

    void   move(Ogre::Real left, Ogre::Real top)
    {
     mLeft = left;
     mTop = top;
     _redrawNeeded();
    }

    void   setTop(Ogre::Real top)
    {
     mTop = top;
     _redrawNeeded();
    }
     
    void   setLeft(Ogre::Real left)
    {
     mLeft = left;
     _redrawNeeded();
    }

    void   setFormatted(bool val)
    {
     mDoFormatting = true;
     _redrawNeeded();
    }

    void   setMonospaced(bool val)
    {
     mDoMonospace = val;
     _redrawNeeded();
    }

    void   setColour(const Ogre::ColourValue& colour)
    {
     mColour = colour;
     _redrawNeeded();
    }
    
    Ogre::String getText() const
    {
     return mText;
    }
    
    bool   isFormatted() const
    {
     return mDoFormatting;
    }
    
    bool   isMonospace() const
    {
     return mDoMonospace;
    }
    
    Ogre::ColourValue  getColour() const
    {
     return mColour;
    }
    
    Ogre::Real   getTop() const
    {
     return mTop;
    }
    
    Ogre::Real   getLeft() const
    {
     return mLeft;
    }
    
   protected:
    
    Text(Ogre::Real left, Ogre::Real top, const Ogre::String& initialText, Ogre::uint layer, Screen*);
    
   ~Text();
    
    void  redraw();
    
    void  _doMarkup(size_t& index);
    
    Ogre::Real         mLeft, mTop;
    Ogre::String       mText;
    bool               mDoFormatting;
    bool               mDoMonospace;
    Ogre::ColourValue  mColour;
    
    static bool               tMonospace;
    static Glyph*             tGlyph;
    static Ogre::ColourValue  tWorkingColour;
    
 }; // Class Text
 
 
} // namespace Gorilla


#endif
