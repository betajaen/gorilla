// gorilla.h, version: 08-08-2010

/*
   
   
   Gorilla
   
   
   
   
Software Licence
----------------
                                                                                  
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


namespace Gorilla
{
  
  struct Pixel;
  struct Size;
  struct Colour;
  class Bitmap;
  class Screen;
  class Block;
  class Rectangle;
  class MonoText;
  
  struct Pixel
  {
   inline Pixel() : top(0), left(0) {}
   inline Pixel(Ogre::uint Left, Ogre::uint Top) : top(Top), left(Left) {}
   inline void set(Ogre::uint Left, Ogre::uint Top) {top=Top;left=Left;}
   inline bool operator==(const Pixel& other) const
   {
     return ( left ==  other.left && top == other.top );
   }
   inline bool operator!=(const Pixel& other) const
   {
     return ( left !=  other.left && top != other.top );
   }

   Ogre::uint  top, left;
  };
  
  struct Size
  {
   inline Size() : width(1), height(1) {}
   inline Size(Ogre::uint Width, Ogre::uint Height) : width(Width), height(Height) {}
   inline void set(Ogre::uint Width, Ogre::uint Height) {width=Width;height=Height;}
   inline Ogre::uint square() { return width * height; }
   inline bool operator==(const Size& other) const
   {
     return ( width ==  other.width && height == other.height );
   }
   inline bool operator!=(const Size& other) const
   {
     return ( width !=  other.width && height != other.height );
   }
   Ogre::uint  width, height;
  };
  
  struct Colour
  {
   
   enum WebColour
   {
    AliceBlue=0xf0f8ff,       Gainsboro=0xdcdcdc,             MistyRose=0xffe4e1,
    AntiqueWhite=0xfaebd7,    GhostWhite=0xf8f8ff,            Moccasin=0xffe4b5,
    Aqua=0x00ffff,            Gold=0xffd700,                  NavajoWhite=0xffdead,
    Aquamarine=0x7fffd4,      Goldenrod=0xdaa520,             Navy=0x000080,
    Azure=0xf0ffff,           Gray=0x808080,                  OldLace=0xfdf5e6,
    Beige=0xf5f5dc,           Green=0x008000,                 Olive=0x808000,
    Bisque=0xffe4c4,          GreenYellow=0xadff2f,           OliveDrab=0x6b8e23,
    Black=0x000000,           Grey=0x808080,                  Orange=0xffa500,
    BlanchedAlmond=0xffebcd,  Honeydew=0xf0fff0,              OrangeRed=0xff4500,
    Blue=0x0000ff,            HotPink=0xff69b4,               Orchid=0xda70d6,
    BlueViolet=0x8a2be2,      IndianRed=0xcd5c5c,             PaleGoldenrod=0xeee8aa,
    Brown=0xa52a2a,           Indigo=0x4b0082,                PaleGreen=0x98fb98,
    Burlywood=0xdeb887,       Ivory=0xfffff0,                 PaleTurquoise=0xafeeee,
    CadetBlue=0x5f9ea0,       Khaki=0xf0e68c,                 PaleVioletRed=0xdb7093,
    Chartreuse=0x7fff00,      Lavender=0xe6e6fa,              PapayaWhip=0xffefd5,
    Chocolate=0xd2691e,       LavenderBlush=0xfff0f5,         PeachPuff=0xffdab9,
    Coral=0xff7f50,           LawnGreen=0x7cfc00,             Peru=0xcd853f,
    CornflowerBlue=0x6495ed,  LemonChiffon=0xfffacd,          Pink=0xffc0cb,
    Cornsilk=0xfff8dc,        LightBlue=0xadd8e6,             Plum=0xdda0dd,
    Crimson=0xdc143c,         LightCoral=0xf08080,            PowderBlue=0xb0e0e6,
    Cyan=0x00ffff,            LightCyan=0xe0ffff,             Purple=0x800080,
    DarkBlue=0x00008b,        LightGoldenrodyellow=0xfafad2,  Red=0xff0000,
    DarkCyan=0x008b8b,        LightGray=0xd3d3d3,             RosyBrown=0xbc8f8f,
    DarkGoldenrod=0xb8860b,   LightGreen=0x90ee90,            RoyalBlue=0x4169e1,
    DarkGray=0xa9a9a9,        LightGrey=0xd3d3d3,             SaddleBrown=0x8b4513,
    DarkGreen=0x006400,       LightPink=0xffb6c1,             Salmon=0xfa8072,
    DarkGrey=0xa9a9a9,        LightSalmon=0xffa07a,           SandyBrown=0xf4a460,
    DarkKhaki=0xbdb76b,       LightSeagreen=0x20b2aa,         SeaGreen=0x2e8b57,
    DarkMagenta=0x8b008b,     LightSkyblue=0x87cefa,          SeaShell=0xfff5ee,
    DarkOlivegreen=0x556b2f,  LightSlategray=0x778899,        Sienna=0xa0522d,
    DarkOrange=0xff8c00,      LightSlategrey=0x778899,        Silver=0xc0c0c0,
    DarkOrchid=0x9932cc,      LightSteelblue=0xb0c4de,        SkyBlue=0x87ceeb,
    DarkRed=0x8b0000,         LightYellow=0xffffe0,           SlateBlue=0x6a5acd,
    DarkSalmon=0xe9967a,      Lime=0x00ff00,                  SlateGray=0x708090,
    DarkSeagreen=0x8fbc8f,    LimeGreen=0x32cd32,             SlateGrey=0x708090,
    DarkSlateblue=0x483d8b,   Linen=0xfaf0e6,                 Snow=0xfffafa,
    DarkSlategray=0x2f4f4f,   Magenta=0xff00ff,               SpringGreen=0x00ff7f,
    DarkSlategrey=0x2f4f4f,   Maroon=0x800000,                SteelBlue=0x4682b4,
    DarkTurquoise=0x00ced1,   MediumAquamarine=0x66cdaa,      Tan=0xd2b48c,
    DarkViolet=0x9400d3,      MediumBlue=0x0000cd,            Teal=0x008080,
    DeepPink=0xff1493,        MediumOrchid=0xba55d3,          Thistle=0xd8bfd8,
    DeepSkyblue=0x00bfff,     MediumPurple=0x9370db,          Tomato=0xff6347,
    DimGray=0x696969,         MediumSeaGreen=0x3cb371,        Turquoise=0x40e0d0,
    DimGrey=0x696969,         MediumSlateBlue=0x7b68ee,       Violet=0xee82ee,
    DodgerBlue=0x1e90ff,      MediumSpringGreen=0x00fa9a,     Wheat=0xf5deb3,
    FireBrick=0xb22222,       MediumTurquoise=0x48d1cc,       White=0xffffff,
    FloralWhite=0xfffaf0,     MediumBioletRed=0xc71585,       WhiteSmoke=0xf5f5f5,
    ForestGreen=0x228b22,     MidnightBlue=0x191970,          Yellow=0xffff00,
    Fuchsia=0xff00ff,         MintCream=0xf5fffa,             YellowGreen=0x9acd32
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
    b =  web_colour & 0xFF;
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
   
   static const Colour Transparent;
  };
  
  class Bitmap
  {
   public:
    
    typedef Ogre::uchar* Handle;
    
    Bitmap(const Size& size);
   ~Bitmap();
   
   enum
   {
    OGRE_TEXTURE_FORMAT = Ogre::PF_BYTE_BGRA
   };
   
   void          save(const Ogre::String& filename);
   
   void          splat(Ogre::uchar* pDest, Ogre::uint width, Ogre::uint height, const Pixel& position)
   {
    Ogre::uchar* source_handle = mData;
    Ogre::uint   source_stride = stride();
    Ogre::uint   target_stride = width * 4;
    Ogre::uchar* target_handle = pDest + (target_stride * position.top) + (position.left * 4);

    for (Ogre::uint y=0;y < mDimensions.height;y++)
    {
     for (Ogre::uint x=0;x < mDimensions.width;x++)
     {
      *target_handle++ = *source_handle++;
      *target_handle++ = *source_handle++;
      *target_handle++ = *source_handle++;
      *target_handle++ = *source_handle++;
     }
     target_handle += (target_stride - source_stride);
    }
   }
   
   void          clear();
   Handle        handle();
   Handle        handle(const Pixel&);
   Handle        handle(Ogre::uint x, Ogre::uint y);
   void          clip(Pixel&, Size&);
   Ogre::uint    stride();
   void          setDimensions(const Size&);
   Size          getDimensions() const;
   Ogre::uint    getLength() const;

   protected:
    
    Ogre::uchar*   mData;
    Size           mDimensions;
    Ogre::uint     mLength;
    
  };
  
  class Font8x8
  {
    
   public:
    
    Font8x8();
    
   ~Font8x8();
    
    inline void splat(Bitmap* bitmap, const Pixel& position, Ogre::uchar value, const Colour& colour = Colour(Colour::White))
    {
     if (value < 32 || value > 128)
      return;
     
     Ogre::uint source_stride = mData->stride();
     Bitmap::Handle source_handle = mData->handle((value-32)*8, 0);
     
     Ogre::uint target_stride = bitmap->stride();
     Bitmap::Handle target_handle = bitmap->handle(position);
     Ogre::uint y = 0, x = 0;
     
     for (y=0;y<8;y++)
     {
      for (x=0;x<8;x++)
      {
       *target_handle++ = (*source_handle++) * colour.b;
       *target_handle++ = (*source_handle++) * colour.g;
       *target_handle++ = (*source_handle++) * colour.r;
       *target_handle++ = (*source_handle++) * colour.a;
      }
      target_handle += (target_stride - (8*4));
      source_handle += (source_stride - (8*4));
     }
     
    }
    
   protected:
    
    Bitmap*          mData;
    
    static const Ogre::uchar CompressedFont[780];
    
  };
  
  class Screen
  {
    
   public:
    
    Screen(Ogre::Viewport*, Ogre::uint defaultTextureSize = 512);
    
   ~Screen();
    
    // Re-draw everything, Re-pack everything, Re-Calculate Vertices, Re-Calculate UV, Update ManualObject.
    void refresh();
    
    // Refresh texture incase a style has been CHANGED, or Block has changed its image data.
    void _refreshTexture();
    
    // Refresh vertices incase a Block has MOVED, or Block Z-ORDER has changed.
    void _refreshVertices();
    
    //
    Rectangle*  createRectangle(const Pixel& position, const Size& size, const Colour& = Colour(Colour::White), const Ogre::uint& zorder = 0);
    
    //
    MonoText*  createMonoText(const Pixel& position, const Ogre::uint nbCols, const Ogre::uint nbRows, const Ogre::String& initialText = "", const Colour& foreground = Colour(Colour::White), const Colour& background = Colour::Transparent, const Ogre::uint& zorder = 0);
    
    Font8x8*   get8x8Font();

   protected:
    
    void _clearTexture();
    
    std::vector<Block*>                mBlocks;
    Ogre::uint                         mID;
    Ogre::Viewport*                    mViewport;
    Ogre::SceneManager*                mSceneMgr;
    Ogre::SceneNode*                   mNode;
    Ogre::ManualObject*                mManualObject;
    Ogre::MaterialPtr                  mMaterial;
    Ogre::TexturePtr                   mTexture;
    Ogre::Vector2                      mInverseViewportSize;
    Ogre::Vector2                      mInverseTextureSize;
    Ogre::uint                         mDebugNbTexturePackings;
    Ogre::uint                         mDebugNbTextureCopies;
    Ogre::uint                         mDebugNbManualObjectUpdates;
    Font8x8*                           m8x8Font;
    static Ogre::uint                  sNextID;
  };
  
  class Block
  {
   
   friend class Screen;
   
   public:
    
    Block(const Pixel&, const Size&, const Ogre::uint& zorder, Screen*, bool optimised);
    
   ~Block();
    
    void setPosition();
    
    Pixel getPosition();
    
    Ogre::Vector2 getScreenPosition();
    
    void setZOrder();
    
    Ogre::uint getZOrder();
    
   protected:
    
    virtual void       _redraw();
    void               _setTextureSize(const Size&);
    
    bool               mRedrawRequired;
    bool               mResizeRequired;
    bool               mMoveRequired;
    bool               m1pxOptimised;   // Uses no textures, uses ColourValue as colour.
    
    Bitmap*            mBitmap;
    Pixel              mScreenPosition;
    Size               mScreenSize;
    Pixel              mScreenTexturePosition;
    Size               mScreenTextureSize;
    Ogre::uint         mScreenZOrder;
    Ogre::Vector2      mScreenUV[4];
    Ogre::ColourValue  mScreenTextureColour; // Used when optimised.
    Screen*            mScreen;
  };
  
  class Rectangle : public Block
  {
   
   friend class Screen;
   
   public:
    
    Rectangle(const Pixel& position, const Ogre::uint& zorder, const Size& size, const Colour& background, Screen*);
    
   ~Rectangle();
    
   protected:
    
    void  _redraw();
    
    Colour   mBackground;
    
  };
  
  class MonoText : public Block
  {
   
   friend class Screen;
   
   public:
    
    MonoText(const Pixel& position,  const Size& size, const Ogre::uint nbCols, const Ogre::uint nbRows, const Ogre::String& initialText, const Colour& foreground, const Colour& background, const Ogre::uint& zorder, Screen*);
    
   ~MonoText();
    
   protected:
    
    void _redraw();
    
    Colour       mForeground;
    Colour       mBackground;
    Font8x8*     mFont;
    Ogre::String mText;
    Ogre::uint   mNbCols, mNbRows;
    
  };
  
  
  
};

#endif
