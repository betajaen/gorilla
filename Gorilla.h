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

namespace Gorilla
{

struct Glyph;
struct SpriteTexture;
class TextureAtlas;
class Screen;
class Text;

struct DoubleVec2
{
 DoubleVec2() {}
 DoubleVec2(const Ogre::Vector2& v) : x(v.x), y(v.y) {}
 double x,y;
};

struct Pixel
{
 inline Pixel() : top(0), left(0) {}
 inline Pixel(Ogre::uint Left, Ogre::uint Top) : top(Top), left(Left) {}
 inline void set(Ogre::uint Left, Ogre::uint Top) {top=Top;left=Left;}
 inline bool operator==(const Pixel& other) const
 {
   return ( left == other.left && top == other.top );
 }
 inline bool operator!=(const Pixel& other) const
 {
   return ( left != other.left && top != other.top );
 }

 Ogre::uint top, left;
};

struct Size
{
 inline Size() : width(1), height(1) {}
 inline Size(Ogre::uint Width, Ogre::uint Height) : width(Width), height(Height) {}
 inline void set(Ogre::uint Width, Ogre::uint Height) {width=Width;height=Height;}
 inline Ogre::uint square() { return width * height; }
 inline bool operator==(const Size& other) const
 {
   return ( width == other.width && height == other.height );
 }
 inline bool operator!=(const Size& other) const
 {
   return ( width != other.width && height != other.height );
 }
 Ogre::uint width, height;
};

struct Colour
{
 
 enum WebColour
 {
  AliceBlue=0xf0f8ff, Gainsboro=0xdcdcdc, MistyRose=0xffe4e1,
  AntiqueWhite=0xfaebd7, GhostWhite=0xf8f8ff, Moccasin=0xffe4b5,
  Aqua=0x00ffff, Gold=0xffd700, NavajoWhite=0xffdead,
  Aquamarine=0x7fffd4, Goldenrod=0xdaa520, Navy=0x000080,
  Azure=0xf0ffff, Gray=0x808080, OldLace=0xfdf5e6,
  Beige=0xf5f5dc, Green=0x008000, Olive=0x808000,
  Bisque=0xffe4c4, GreenYellow=0xadff2f, OliveDrab=0x6b8e23,
  Black=0x000000, Grey=0x808080, Orange=0xffa500,
  BlanchedAlmond=0xffebcd, Honeydew=0xf0fff0, OrangeRed=0xff4500,
  Blue=0x0000ff, HotPink=0xff69b4, Orchid=0xda70d6,
  BlueViolet=0x8a2be2, IndianRed=0xcd5c5c, PaleGoldenrod=0xeee8aa,
  Brown=0xa52a2a, Indigo=0x4b0082, PaleGreen=0x98fb98,
  Burlywood=0xdeb887, Ivory=0xfffff0, PaleTurquoise=0xafeeee,
  CadetBlue=0x5f9ea0, Khaki=0xf0e68c, PaleVioletRed=0xdb7093,
  Chartreuse=0x7fff00, Lavender=0xe6e6fa, PapayaWhip=0xffefd5,
  Chocolate=0xd2691e, LavenderBlush=0xfff0f5, PeachPuff=0xffdab9,
  Coral=0xff7f50, LawnGreen=0x7cfc00, Peru=0xcd853f,
  CornflowerBlue=0x6495ed, LemonChiffon=0xfffacd, Pink=0xffc0cb,
  Cornsilk=0xfff8dc, LightBlue=0xadd8e6, Plum=0xdda0dd,
  Crimson=0xdc143c, LightCoral=0xf08080, PowderBlue=0xb0e0e6,
  Cyan=0x00ffff, LightCyan=0xe0ffff, Purple=0x800080,
  DarkBlue=0x00008b, LightGoldenrodyellow=0xfafad2, Red=0xff0000,
  DarkCyan=0x008b8b, LightGray=0xd3d3d3, RosyBrown=0xbc8f8f,
  DarkGoldenrod=0xb8860b, LightGreen=0x90ee90, RoyalBlue=0x4169e1,
  DarkGray=0xa9a9a9, LightGrey=0xd3d3d3, SaddleBrown=0x8b4513,
  DarkGreen=0x006400, LightPink=0xffb6c1, Salmon=0xfa8072,
  DarkGrey=0xa9a9a9, LightSalmon=0xffa07a, SandyBrown=0xf4a460,
  DarkKhaki=0xbdb76b, LightSeagreen=0x20b2aa, SeaGreen=0x2e8b57,
  DarkMagenta=0x8b008b, LightSkyblue=0x87cefa, SeaShell=0xfff5ee,
  DarkOlivegreen=0x556b2f, LightSlategray=0x778899, Sienna=0xa0522d,
  DarkOrange=0xff8c00, LightSlategrey=0x778899, Silver=0xc0c0c0,
  DarkOrchid=0x9932cc, LightSteelblue=0xb0c4de, SkyBlue=0x87ceeb,
  DarkRed=0x8b0000, LightYellow=0xffffe0, SlateBlue=0x6a5acd,
  DarkSalmon=0xe9967a, Lime=0x00ff00, SlateGray=0x708090,
  DarkSeagreen=0x8fbc8f, LimeGreen=0x32cd32, SlateGrey=0x708090,
  DarkSlateblue=0x483d8b, Linen=0xfaf0e6, Snow=0xfffafa,
  DarkSlategray=0x2f4f4f, Magenta=0xff00ff, SpringGreen=0x00ff7f,
  DarkSlategrey=0x2f4f4f, Maroon=0x800000, SteelBlue=0x4682b4,
  DarkTurquoise=0x00ced1, MediumAquamarine=0x66cdaa, Tan=0xd2b48c,
  DarkViolet=0x9400d3, MediumBlue=0x0000cd, Teal=0x008080,
  DeepPink=0xff1493, MediumOrchid=0xba55d3, Thistle=0xd8bfd8,
  DeepSkyblue=0x00bfff, MediumPurple=0x9370db, Tomato=0xff6347,
  DimGray=0x696969, MediumSeaGreen=0x3cb371, Turquoise=0x40e0d0,
  DimGrey=0x696969, MediumSlateBlue=0x7b68ee, Violet=0xee82ee,
  DodgerBlue=0x1e90ff, MediumSpringGreen=0x00fa9a, Wheat=0xf5deb3,
  FireBrick=0xb22222, MediumTurquoise=0x48d1cc, White=0xffffff,
  FloralWhite=0xfffaf0, MediumBioletRed=0xc71585, WhiteSmoke=0xf5f5f5,
  ForestGreen=0x228b22, MidnightBlue=0x191970, Yellow=0xffff00,
  Fuchsia=0xff00ff, MintCream=0xf5fffa, YellowGreen=0x9acd32
 }; // enum WebColours

 Colour() : r(0), g(0), b(0), a(255) {}
 
 Colour(const Ogre::ColourValue& ogre_colour)
 {
  r = Ogre::uint8(ogre_colour.r * 255.f);
  g = Ogre::uint8(ogre_colour.g * 255.f);
  b = Ogre::uint8(ogre_colour.b * 255.f);
  a = Ogre::uint8(ogre_colour.a * 255.f);
 }
 
 Colour(Colour::WebColour web_colour, Ogre::uint8 alpha = 255)
 {
  b = web_colour & 0xFF;
  g = (web_colour >> 8) & 0xFF;
  r = (web_colour >> 16) & 0xFF;
  a = alpha;
 }
 
 Colour(Ogre::uint8 red, Ogre::uint8 green, Ogre::uint8 blue, Ogre::uint8 alpha = 255)
 : r(red), g(green), b(blue), a(alpha) {}
 
 Ogre::ColourValue asColourvalue() const
 {
  return Ogre::ColourValue(float(r) / 255.0f,float(g) / 255.0f,float(b) / 255.0f,float(a) / 255.0f);
 }
 Ogre::uint8 r, g, b, a;
 
 static Colour Random(Ogre::Real alpha = 1.0)
 {
  return Colour(
   Ogre::ColourValue(
    Ogre::Math::RangeRandom(0,1),
    Ogre::Math::RangeRandom(0,1),
    Ogre::Math::RangeRandom(0,1),
    alpha));
 }
 
 static const Colour Transparent;
};

struct Glyph
{
 // Texture
 Ogre::uint imageX, imageY, width, height; int advance;
 // UV
 Ogre::Real left, top, right, bottom;
 // Font kerning. (Subtract from advance when after character)
 int kerning[255];
};

struct SpriteTexture
{
 Ogre::Real left, top, right, bottom;
 Ogre::uint imageX, imageY, width, height;
};

struct BakedData
{
 Ogre::vector<Ogre::Vector3>::type mVertices;
 Ogre::vector<Ogre::Vector2>::type mUV;
 Ogre::vector<Ogre::ColourValue>::type mColours;
};

class TextureAtlas
{
  
 public:
  
  TextureAtlas(const Ogre::String& gorillaFilename, const Ogre::String& resourceGroup = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  
 ~TextureAtlas();
  
  Ogre::String  getMaterialName() const;
  
  void save(const Ogre::String& filename);
  
  inline Glyph*  getGlyph(unsigned char glyph_id)
  {
   return &mGlyphs[glyph_id];
  }
  
  inline SpriteTexture*  getSprite(unsigned char sprite_id)
  {
   return &mSprites[sprite_id];
  }
  
  inline const DoubleVec2  getRectangleCoords() const
  {
   return mRectangleCoords;
  }
  
  inline const Ogre::uint  getSpaceLength() const
  {
   return mSpaceLength;
  }
  
  inline const Ogre::uint  getLineHeight() const
  {
   return mLineHeight;
  }
  
  void reset();
  
 protected:
  
  void _processTextureCoords();
  void _processGorillaTexture(Ogre::ConfigFile::SettingsMultiMap*);
  void _processGorillaFontGlyphs(Ogre::ConfigFile::SettingsMultiMap*);
  void _processGorillaFontKerning(Ogre::ConfigFile::SettingsMultiMap*);
  void _processGorillaSprites(Ogre::ConfigFile::SettingsMultiMap*);
  
  Ogre::MaterialPtr  mMaterial;
  Ogre::TexturePtr   mTexture;
  Glyph              mGlyphs[255];
  SpriteTexture      mSprites[255];
  DoubleVec2         mRectangleCoords;
  int                mSpaceLength;
  int                mLineHeight;
  Ogre::Vector2      mInverseTextureSize;
};

class Screen : public Ogre::FrameListener
{
  
 public:
  
  friend class Text;
  
  struct Rectangle
  {
   int                 left, top, depth;
   Ogre::uint          width, height;
   Ogre::ColourValue   background;
   size_t              vertexStart;
  };
  
  struct Sprite
  {
   SpriteTexture*      sprite;
   int                 left, top, depth;
   size_t              vertexStart;
  };
  
  struct Text
  {
   int                 left, top, depth;
   Ogre::ColourValue   colour;
   Ogre::String        text;
   BakedData           data;
  };

  typedef Ogre::map<Ogre::uint, Rectangle>::type::iterator       RectangleListIterator;
  typedef Ogre::map<Ogre::uint, Rectangle>::type::const_iterator ConstRectangleListIterator;
  
  typedef Ogre::map<Ogre::uint, Sprite>::type::iterator          SpriteListIterator;
  typedef Ogre::map<Ogre::uint, Sprite>::type::const_iterator    ConstSpriteListIterator;
  
  typedef Ogre::map<Ogre::uint, Text>::type::iterator            TextListIterator;
  typedef Ogre::map<Ogre::uint, Text>::type::const_iterator      ConstTextListIterator;
  
  Screen(Ogre::Viewport*, TextureAtlas*);
 ~Screen();
  
  Ogre::uint  addRectangle(const Pixel& position, const Size& size, int zorder = 0, const Colour& colour = Colour(Colour::White));
  Ogre::uint  addRectangle(int left, int top, Ogre::uint width, Ogre::uint height, int zorder = 0, const Colour& colour = Colour(Colour::White));
  void        removeRectangle(Ogre::uint);
  void        removeAllRectangles();
  Ogre::uint  getNbRectangles() const;
  void        setRectanglePosition(Ogre::uint id, const Pixel& position);
  void        setRectanglePosition(Ogre::uint id, int left, int top);
  void        setRectangleSize(Ogre::uint id, const Size& size);
  void        setRectangleSize(Ogre::uint id, Ogre::uint width, Ogre::uint height);
  void        setRectangleZOrder(Ogre::uint id, int zorder);
  void        setRectangleColour(Ogre::uint id, const Ogre::ColourValue& colour);
  void        setRectangleColour(Ogre::uint id, const Colour& colour);
  void        updateRectangle(Ogre::uint id, const Pixel& position, const Size& size, int zorder, const Colour& colour);
  void        updateRectangle(Ogre::uint id, int left, int top, Ogre::uint width, Ogre::uint height, int zorder, const Colour& colour);
  Pixel       getRectanglePosition(Ogre::uint id) const;
  Size        getRectangleSize(Ogre::uint id) const;
  int         getRectangleZOrder(Ogre::uint id) const;
  Colour      getRectangleColour(Ogre::uint id) const;
  void        _redrawRectangles();

  Ogre::uint  addSprite(const Pixel& position, Ogre::uint sprite_id, int zorder = 0);
  Ogre::uint  addSprite(int left, int top, Ogre::uint sprite_id, int zorder = 0);
  void        removeSprite(Ogre::uint);
  void        removeAllSprites();
  Ogre::uint  getNbSprites() const;
  void        setSpritePosition(Ogre::uint id, const Pixel& position);
  void        setSpritePosition(Ogre::uint id, int left, int top);
  void        setSpriteZOrder(Ogre::uint id, int zorder);
  int         getSpriteZOrder(Ogre::uint id) const;
  Pixel       getSpritePosition(Ogre::uint id) const;
  void        updateSprite(Ogre::uint id, const Pixel& position, int zorder);
  void        updateSprite(Ogre::uint id, int left, int top, int zorder);
  void        updateSprite(Ogre::uint id, Ogre::uint sprite_id);
  Size        estimateSpriteSize(Ogre::uint id) const;
  
  void        _redrawSprites();
  
  Ogre::uint   addText(const Pixel&, const Ogre::String& caption, int zorder = 0, const Colour& colour = Colour(Colour::White));
  Ogre::uint   addText(int left, int top, const Ogre::String& caption, int zorder = 0, const Ogre::ColourValue& colour = Ogre::ColourValue::White);
  void         setTextPosition(Ogre::uint id, const Pixel& position);
  void         setTextPosition(Ogre::uint id, int left, int top);
  void         setTextCaption(Ogre::uint id, const Ogre::String& text);
  void         setTextZOrder(Ogre::uint id, int zorder);
  void         setTextColour(Ogre::uint id, const Ogre::ColourValue& colour);
  void         setTextColour(Ogre::uint id, const Colour& colour);
  void         updateText(Ogre::uint id, const Pixel& position, const Ogre::String&, int zorder, const Colour& colour);
  void         updateText(Ogre::uint id, int left, int top, const Ogre::String&, int zorder, const Colour& colour);
  Pixel        getTextPosition(Ogre::uint id) const;
  Size         estimateTextSize(const Ogre::String& text) const;
  int          getTextZOrder(Ogre::uint id) const;
  Colour       getTextColour(Ogre::uint id) const;
  
  void         refreshMarkupColours();
  void         setMarkupColour(Ogre::uint colour_palette_index, const Colour&);
  void         setMarkupColour(Ogre::uint colour_palette_index, const Ogre::ColourValue&);
  Colour       getMarkupColour(Ogre::uint colour_palette_index);
  
  void        _redrawText();
  
  bool        frameStarted(const Ogre::FrameEvent& evt);
  void        _refresh();
  void        _createNodeAndObject(Ogre::SceneManager*, const Ogre::String& matName);
  
 protected:

  void        _drawRectangle(Rectangle*);
  void        _redrawRectangleVerticesOnly(Rectangle*);
  void        _redrawRectangleColourOnly(Rectangle*);
  

  void        _drawSprite(Sprite*);
  void        _redrawSpriteVerticesOnly(Sprite*);
  void        _redrawSpriteUVOnly(Sprite*);


  void        _drawText(Text*);


  inline  Ogre::Real  _vpx(Ogre::Real x)
  {
   
   if (x < 0) // Becomes right.
   {
    x = mViewportWidth + x;
   }
   
   x /= mViewportWidth;
   
   return (x * 2) - 1;
  }
  
  inline  Ogre::Real  _vpy(Ogre::Real y)
  {
   
   if (y < 0) // Becomes bottom.
   {
    y = mViewportHeight + y;
   }
   
   y /= mViewportHeight;
   return 1 - (y * 2);
  }
  
  inline  Ogre::Real  _vpz(int z)
  {
   return Ogre::Real(z) * 0.001f;
  }
  
  
  Ogre::map<Ogre::uint, Rectangle>::type  mRectangles;
  Ogre::map<Ogre::uint, Sprite>::type     mSprites;
  Ogre::map<Ogre::uint, Text>::type       mText;
  Ogre::ColourValue                       mMarkupColour[8];
  TextureAtlas*                           mAtlas;
  Ogre::SceneNode*                        mNode;
  Ogre::ManualObject*                     mObject;
  Ogre::Viewport*                         mViewport;
  Ogre::uint                              mViewportWidth;
  Ogre::uint                              mViewportHeight;
  Ogre::Vector2                           mInverseViewportDimensions;
  bool                                    mRedrawAllRequired;
  bool                                    mRefreshRequired;
  BakedData                               mRectangleData;
  BakedData                               mSpriteData;
  Ogre::Real                              mUpdatePeriod, mTimer;
  Ogre::uint                              mNextRectangleID;
  Ogre::uint                              mNextSpriteID;
  Ogre::uint                              mNextTextID;
  Ogre::uint                              mNbRedraws;
  Ogre::uint                              mNbRefreshes;
  
};


};

#endif
