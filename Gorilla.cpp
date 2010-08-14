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

#include "Gorilla.h"

#include "OGRE/Ogre.h"

#pragma warning ( disable : 4244 )

namespace Gorilla
{

TextureAtlas::TextureAtlas(const Ogre::String& gorillaFilename, const Ogre::String& group)
{
 
 reset();
 
 Ogre::ConfigFile f;
 f.loadFromResourceSystem(gorillaFilename, group, " ", true);
 Ogre::ConfigFile::SectionIterator seci = f.getSectionIterator();
 
 Ogre::String secName;
  while (seci.hasMoreElements())
  {
   
   secName = seci.peekNextKey();
   Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
   
   Ogre::StringUtil::toLowerCase(secName);
   if (secName == "texture")
    _processGorillaTexture(settings);
   else if (secName == "font")
    _processGorillaFontGlyphs(settings);
   else if (secName == "fontkerning")
    _processGorillaFontKerning(settings);
   else if (secName == "sprites")
    _processGorillaSprites(settings);
   
 }
 
 _processTextureCoords();
 
 // Create/Get material based on texturename.
 std::string matName = "GorillaMaterialFor" + mTexture->getName();
 mMaterial == Ogre::MaterialManager::getSingletonPtr()->getByName(matName);
 
 if (mMaterial.isNull() == false)
  return;
 
 mMaterial = Ogre::MaterialManager::getSingletonPtr()->create(matName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
 Ogre::Pass* pass = mMaterial->getTechnique(0)->getPass(0);
 Ogre::TextureUnitState* tus = pass->createTextureUnitState(mTexture->getName());
 tus->setTextureFiltering(Ogre::FO_NONE, Ogre::FO_NONE, Ogre::FO_NONE);
 pass->setCullingMode(Ogre::CULL_NONE);
 pass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
 pass->setDepthCheckEnabled(false);
 pass->setDepthWriteEnabled(false);
 pass->setLightingEnabled(false);
 
 
}

TextureAtlas::~TextureAtlas()
{
}

Ogre::String TextureAtlas::getMaterialName() const
{
 return mMaterial->getName();
}

void TextureAtlas::reset()
{
 mMaterial.setNull();
 mTexture.setNull();
 for (Ogre::uint i=0;i < 255;i++)
 {
  
  mGlyphs[i].bottom = 0;
  mGlyphs[i].top = 0;
  mGlyphs[i].left = 0;
  mGlyphs[i].right = 0;
  
  mGlyphs[i].advance = 0;
  mGlyphs[i].height = 0;
  mGlyphs[i].imageX = 0;
  mGlyphs[i].imageY = 0;
  mGlyphs[i].width = 0;
  
  for (Ogre::uint j=0;j<255;j++)
   mGlyphs[i].kerning[j] = 0;
  
 }
 
 for (Ogre::uint i=0;i < 255;i++)
 {
  mSprites[i].height = 0;
  mSprites[i].width = 0;
  mSprites[i].imageX = 0;
  mSprites[i].imageY = 0;
  mSprites[i].bottom = 0;
  mSprites[i].top = 0;
  mSprites[i].left = 0;
  mSprites[i].right = 0;
 }
 
 mRectangleCoords.x = 0;
 mRectangleCoords.y = 0;
 
 mSpaceLength = 0;
 mLineHeight = 0;
 
}

void TextureAtlas::_processGorillaTexture(Ogre::ConfigFile::SettingsMultiMap* settings)
{
 
 Ogre::String name, data;
 Ogre::ConfigFile::SettingsMultiMap::iterator i;
 for (i = settings->begin(); i != settings->end(); ++i)
 {
  name = i->first;
  data = i->second;
  Ogre::StringUtil::toLowerCase(name);
  
  if (name == "file")
  {
   mTexture = Ogre::TextureManager::getSingletonPtr()->getByName(data);
   if (mTexture.isNull())
    mTexture = Ogre::TextureManager::getSingletonPtr()->load(data, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
   
   mInverseTextureSize.x = 1.0f / mTexture->getWidth();
   mInverseTextureSize.y = 1.0f / mTexture->getHeight();
   continue;
  }
  else if (name == "rectangle")
  {
   mRectangleCoords = Ogre::StringConverter::parseVector2(data);
   mRectangleCoords.x *= mInverseTextureSize.x;
   mRectangleCoords.y *= mInverseTextureSize.y;
  }
  
 }
 
}

void TextureAtlas::_processGorillaFontGlyphs(Ogre::ConfigFile::SettingsMultiMap* settings)
{
 
 Ogre::String name, data;
 Ogre::ConfigFile::SettingsMultiMap::iterator i;
 unsigned char glyph_id;
 Ogre::StringVector str_values;
 for (i = settings->begin(); i != settings->end(); ++i)
 {
  name = i->first;
  data = i->second;
  Ogre::StringUtil::toLowerCase(name);
    
  if (name == "lineheight")
  {
   mLineHeight = Ogre::StringConverter::parseInt(data);
   continue;
  }
  else if (name == "spacelength")
  {
   mSpaceLength = Ogre::StringConverter::parseInt(data);
   continue;
  }
  
  if (name.substr(0,6) != "glyph_")
  {
   continue;
  }

  name = name.substr(6); // chop of glyph_
  glyph_id = Ogre::StringConverter::parseUnsignedInt(name);
  
  size_t comment = data.find_first_of('#');
  if (comment != std::string::npos)
   data = data.substr(0, comment);
  
  str_values = Ogre::StringUtil::split(data, " ", 5);
  
  if (str_values.size() < 4)
  {
   std::cout << "[Gorilla] Glyph #" << name << " does not have enough properties\n";
   continue;
  }
  
  mGlyphs[glyph_id].imageX = Ogre::StringConverter::parseUnsignedInt(str_values[0]);
  mGlyphs[glyph_id].imageY = Ogre::StringConverter::parseUnsignedInt(str_values[1]);
  mGlyphs[glyph_id].width = Ogre::StringConverter::parseUnsignedInt(str_values[2]);
  mGlyphs[glyph_id].height = Ogre::StringConverter::parseUnsignedInt(str_values[3]);
  
  if (str_values.size() == 5)
   mGlyphs[glyph_id].advance = Ogre::StringConverter::parseInt(str_values[4]);
  
 }
 
}

void TextureAtlas::_processGorillaFontKerning(Ogre::ConfigFile::SettingsMultiMap* settings)
{
 
 Ogre::String left_name, data;
 Ogre::ConfigFile::SettingsMultiMap::iterator i;
 unsigned char left_glyph_id;
 unsigned char right_glyph_id;
 int  kerning;
 Ogre::StringVector str_values;
 
 for (i = settings->begin(); i != settings->end(); ++i)
 {
  left_name = i->first;
  data = i->second;
  Ogre::StringUtil::toLowerCase(left_name);
  
  if (left_name.substr(0,6) != "glyph_")
   continue;
  
  size_t comment = data.find_first_of('#');
  if (comment != std::string::npos)
   data = data.substr(0, comment);
  
  left_name = left_name.substr(6); // chop of glyph_
  left_glyph_id = Ogre::StringConverter::parseUnsignedInt(left_name);
  
  str_values = Ogre::StringUtil::split(data, " ", 2);
  
  if (str_values.size() != 2)
  {
   std::cout << "[Gorilla] Kerning Glyph #" << left_name << " does not have enough properties\n";
   continue;
  }
  
  right_glyph_id = Ogre::StringConverter::parseUnsignedInt(str_values[0]);
  kerning = Ogre::StringConverter::parseInt(str_values[1]);
  
  mGlyphs[right_glyph_id].kerning[left_glyph_id] = kerning;
  
 }
}

void TextureAtlas::_processGorillaSprites(Ogre::ConfigFile::SettingsMultiMap* settings)
{

 Ogre::String name, data;
 Ogre::ConfigFile::SettingsMultiMap::iterator i;
 unsigned char sprite_id;
 Ogre::StringVector str_values;
 for (i = settings->begin(); i != settings->end(); ++i)
 {
  name = i->first;
  data = i->second;
  Ogre::StringUtil::toLowerCase(name);
  
  if (name.substr(0,7) != "sprite_")
  {
   continue;
  }
  
  size_t comment = data.find_first_of('#');
  if (comment != std::string::npos)
   data = data.substr(0, comment);
  
  name = name.substr(7); // chop of sprite_
  sprite_id = Ogre::StringConverter::parseUnsignedInt(name);
  
  str_values = Ogre::StringUtil::split(data, " ", 4);
  
  if (str_values.size() != 4)
  {
   std::cout << "[Gorilla] Sprite #" << name << " does not have enough properties\n"  << data << "\n";
   continue;
  }
  
  mSprites[sprite_id].imageX = Ogre::StringConverter::parseUnsignedInt(str_values[0]);
  mSprites[sprite_id].imageY = Ogre::StringConverter::parseUnsignedInt(str_values[1]);
  mSprites[sprite_id].width = Ogre::StringConverter::parseUnsignedInt(str_values[2]);
  mSprites[sprite_id].height = Ogre::StringConverter::parseUnsignedInt(str_values[3]);
  
 }
 
}

void TextureAtlas::_processTextureCoords()
{
 
 for (Ogre::uint i=0;i < 255;i++)
 {
  mSprites[i].left = Ogre::Real(mSprites[i].imageX) * mInverseTextureSize.x;
  mSprites[i].top = Ogre::Real(mSprites[i].imageY) * mInverseTextureSize.y;
  mSprites[i].right =  Ogre::Real(mSprites[i].imageX + mSprites[i].width) * mInverseTextureSize.x;
  mSprites[i].bottom =  Ogre::Real(mSprites[i].imageY + mSprites[i].height) * mInverseTextureSize.y;
  
  mGlyphs[i].left = Ogre::Real(mGlyphs[i].imageX) * mInverseTextureSize.x;
  mGlyphs[i].top = Ogre::Real(mGlyphs[i].imageY) * mInverseTextureSize.y;
  mGlyphs[i].right =  Ogre::Real(mGlyphs[i].imageX + mGlyphs[i].width) * mInverseTextureSize.x;
  mGlyphs[i].bottom =  Ogre::Real(mGlyphs[i].imageY + mGlyphs[i].height) * mInverseTextureSize.y;
 }
 
}

void TextureAtlas::save(const Ogre::String& filename)
{
 std::ofstream f(filename.c_str());
 
 f << "[Texture]\n";
 if (mTexture.isNull() == false)
 {
  f << "file " << mTexture->getName() << "\n";
 }
 f << "rectangle " << unsigned int(mRectangleCoords.x / mInverseTextureSize.x) << " " << unsigned int(mRectangleCoords.x / mInverseTextureSize.y) << "\n";
 
 f << "\n[Font]\n";
 f << "lineheight " << mLineHeight << "\n";
 f << "spacelength " << mSpaceLength << "\n";
 for (Ogre::uint i=0;i < 255;i++)
 {
  if (mGlyphs[i].width !=0 && mGlyphs[i].height != 0)
  {
   f << "glyph_" << i << " " << mGlyphs[i].imageX << " " << mGlyphs[i].imageY << " " << mGlyphs[i].width << " " << mGlyphs[i].height << " " << mGlyphs[i].advance << "\n";
  }
 }
 
 f << "\n[FontKerning]\n";
 for (Ogre::uint i=0;i < 255;i++)
 {
  for (Ogre::uint j=0;j < 255;j++)
  {
   if (mGlyphs[i].kerning[j] != 0)
   {
    f << "glyph_" << j << " " << i << " " << mGlyphs[i].kerning[j] << "\n";
   }
  }
 }
 
 f << "\n[Sprites]\n";
 for (Ogre::uint i=0;i < 255;i++)
 {
  if (mSprites[i].width != 0 && mSprites[i].height != 0)
  {
   f << "sprite_" << i << " " << mSprites[i].imageX << " " << mSprites[i].imageY << " " << mSprites[i].width << " " << mSprites[i].height << "\n";
  }
 }
 

}



Screen::Screen(Ogre::Viewport* vp, TextureAtlas* atlas)
: mViewport(vp),
  mAtlas(atlas),
  mUpdatePeriod(1.0f/60.0f),
  mTimer(0),
  mRedrawAllRequired(false),
  mRefreshRequired(false),
  mNextRectangleID(0),
  mNextSpriteID(0),
  mNextTextID(0),
  mNbRedraws(0),
  mNbRefreshes(0)
{
 
 refreshMarkupColours();
 
 mViewportWidth = mViewport->getActualWidth();
 mViewportHeight = mViewport->getActualHeight();
 mInverseViewportDimensions.x = 1.0f / Ogre::Real(mViewportWidth);
 mInverseViewportDimensions.y = 1.0f / Ogre::Real(mViewportHeight);
 
 Ogre::Root::getSingletonPtr()->addFrameListener(this);
 _createNodeAndObject(mViewport->getCamera()->getSceneManager(), mAtlas->getMaterialName());
}

Screen::~Screen()
{
#ifdef _DEBUG
 std::cout << "[Gorilla." << this << "] Nb. Redraws = " << mNbRedraws << "\n" <<
              "[Gorilla." << this << "] Nb. Refreshes = " << mNbRefreshes << "\n";
#endif
 
 Ogre::Root::getSingletonPtr()->removeFrameListener(this);
}

void Screen::_createNodeAndObject(Ogre::SceneManager* scenemanager, const Ogre::String& matName)
{
 // Create Node
 mNode = scenemanager->getRootSceneNode()->createChildSceneNode();

 // Create Object
 mObject = scenemanager->createManualObject();
 mNode->attachObject(mObject);

 mObject->setDynamic(true);
 
 mObject->begin(matName);

 Ogre::AxisAlignedBox aabInf;
 aabInf.setInfinite();

 mObject->setBoundingBox(aabInf);
 mObject->setUseIdentityProjection(true);
 mObject->setUseIdentityView(true);

 mObject->position(0,0,0);
 mObject->textureCoord(0,0);
 mObject->colour(1,1,1,1);

 mObject->position(0,0,1);
 mObject->textureCoord(0,0);
 mObject->colour(1,1,1,1);

 mObject->position(0,0,-1);
 mObject->textureCoord(0,0);
 mObject->colour(1,1,1,1);

 mObject->end();
}

bool Screen::frameStarted(const Ogre::FrameEvent& evt)
{
 
 mTimer += evt.timeSinceLastFrame;
 
 if (mTimer >= mUpdatePeriod)
 {
  
  
  // TOOD: Check to see if the viewport size has changed? Yes? full _redraw and refresh.
  if (mViewport->getActualWidth() != mViewportWidth || mViewport->getActualHeight() != mViewportHeight)
  {
   mRedrawAllRequired = true;
   mRefreshRequired = true;
   mViewportWidth = mViewport->getActualWidth();
   mViewportHeight = mViewport->getActualHeight();
   mInverseViewportDimensions.x = 1.0f / Ogre::Real(mViewportWidth);
   mInverseViewportDimensions.y = 1.0f / Ogre::Real(mViewportHeight);
  }
  
  mTimer = 0;
  
  if (mRedrawAllRequired)
  {
   mNbRedraws++;
   _redrawRectangles();
   _redrawSprites();
   _redrawText();
  }
  
  if (mRefreshRequired)
  {
   _refresh();
  }
  
  mRedrawAllRequired = mRefreshRequired = false;
  
 }
 
 return true;
 
}

void Screen::_refresh()
{
 
 mNbRefreshes++;
 
 mObject->beginUpdate(0);
 
 if (mRectangleData.mVertices.size() == 0) // and the rest are zero.
 {
  mObject->position(0,0,0);
  mObject->textureCoord(0,0);
  mObject->colour(0,0,0,0);
  mObject->position(0,0,0);
  mObject->textureCoord(0,0);
  mObject->colour(0,0,0,0);
  mObject->position(0,0,0);
  mObject->textureCoord(0,0);
  mObject->colour(0,0,0,0);
  mObject->end();
  return;
 }
 
 // Copy rectangles.
 DoubleVec2 rectangleUV = mAtlas->getRectangleCoords();
 for (size_t i=0;i < mRectangleData.mVertices.size(); i++)
 {
  mObject->position(mRectangleData.mVertices[i]);
  mObject->textureCoord(rectangleUV.x, rectangleUV.y);
  mObject->colour(mRectangleData.mColours[i]);
 }
 
 // Copy Sprites
 for (size_t i=0;i < mSpriteData.mVertices.size(); i++)
 {
  mObject->position(mSpriteData.mVertices[i]);
  mObject->textureCoord(mSpriteData.mUV[i]);
  mObject->colour(Ogre::ColourValue::White);
 }
 
 // Copy Text
 for (ConstTextListIterator it=mText.begin();it != mText.end();it++)
 {
  for (size_t i=0;i < (*it).second.data.mVertices.size();i++)
  {
   mObject->position(  (*it).second.data.mVertices[i]  );
   mObject->textureCoord(  (*it).second.data.mUV[i]  );
   mObject->colour(  (*it).second.data.mColours[i]  );
  }
 }
 
 mObject->end();
 
}

void    Screen::refreshMarkupColours()
{
 mMarkupColour[0] = Colour(254, 254, 254).asColourvalue();
 mMarkupColour[1] = Colour(13,  13,  13).asColourvalue();
 mMarkupColour[2] = Colour(204, 204, 204).asColourvalue();
 mMarkupColour[3] = Colour(254, 220, 129).asColourvalue();
 mMarkupColour[4] = Colour(254, 138, 129).asColourvalue();
 mMarkupColour[5] = Colour(123, 236, 110).asColourvalue();
 mMarkupColour[6] = Colour(44,  192, 171).asColourvalue();
 mMarkupColour[7] = Colour(199, 93,  142).asColourvalue();
}

void    Screen::setMarkupColour(Ogre::uint index, const Colour& colour)
{
 setMarkupColour(index, colour.asColourvalue());
}

void    Screen::setMarkupColour(Ogre::uint index, const Ogre::ColourValue& colour)
{
 
 if (index > 8)
  return;
 
 mMarkupColour[index] = colour;
}

Colour  Screen::getMarkupColour(Ogre::uint index)
{
 if (index > 8)
  return Colour();
 
 return mMarkupColour[index];
}

Ogre::uint  Screen::addRectangle(const Pixel& position, const Size& size, int zorder, const Colour& colour)
{
 return addRectangle(position.left, position.top, size.width, size.height, zorder, colour);
}

Ogre::uint  Screen::addRectangle(int left, int top, Ogre::uint width, Ogre::uint height, int zorder, const Colour& colour)
{
 
 Screen::Rectangle rect;
 rect.width = width;
 rect.height = height;
 rect.left = left;
 rect.top = top;
 rect.background = colour.asColourvalue();
 rect.depth = zorder;
 _drawRectangle(&rect);
 
 Ogre::uint id = mNextRectangleID++;
 mRectangles[id] = rect;
 mRefreshRequired = true;
 
 return id;
}

void  Screen::removeRectangle(Ogre::uint id)
{
 
 RectangleListIterator it = mRectangles.find(id);
 
 if (it != mRectangles.end())
 {
  mRectangles.erase(it);
  _redrawRectangles();
  mRefreshRequired = true;
 }
 
}

void  Screen::removeAllRectangles()
{
 mRectangles.clear();
 mRefreshRequired = true;
}
 
Ogre::uint Screen::getNbRectangles() const
{
 return mRectangles.size();
}

void  Screen::setRectanglePosition(Ogre::uint id, const Pixel& position)
{
 setRectanglePosition(id, position.left, position.top);
}

void  Screen::setRectanglePosition(Ogre::uint id, int left, int top)
{
 
 RectangleListIterator it = mRectangles.find(id);
 
 if (it != mRectangles.end())
 {
  (*it).second.left = left;
  (*it).second.top = top;
  _redrawRectangleVerticesOnly( &(*it).second );
  mRefreshRequired = true;
 }

}

void  Screen::setRectangleSize(Ogre::uint id, const Size& size)
{
 setRectangleSize(id, size.width, size.height);
}

void  Screen::setRectangleSize(Ogre::uint id, Ogre::uint width, Ogre::uint height)
{
 RectangleListIterator it = mRectangles.find(id);
 
 if (it != mRectangles.end())
 {
  (*it).second.width = width;
  (*it).second.height = height;
  _redrawRectangleVerticesOnly( &(*it).second );
  mRefreshRequired = true;
 }

}

void  Screen::setRectangleZOrder(Ogre::uint id, int zorder)
{

 RectangleListIterator it = mRectangles.find(id);
 
 if (it != mRectangles.end())
 {
  (*it).second.depth = zorder;
  _redrawRectangleVerticesOnly( &(*it).second );
  mRefreshRequired = true;
 }

}

void  Screen::setRectangleColour(Ogre::uint id, const Colour& colour)
{
 setRectangleColour(id, colour.asColourvalue());
}

void  Screen::setRectangleColour(Ogre::uint id, const Ogre::ColourValue& colour)
{
 RectangleListIterator it = mRectangles.find(id);
 
 if (it != mRectangles.end())
 {
  (*it).second.background = colour;
  _redrawRectangleColourOnly( &(*it).second );
  mRefreshRequired = true;
 }

}

void  Screen::updateRectangle(Ogre::uint id, const Pixel& position, const Size& size, int zorder, const Colour& colour)
{
 updateRectangle(id, position.left, position.top, size.width, size.height, zorder, colour);
}

void  Screen::updateRectangle(Ogre::uint id, int left, int top, Ogre::uint width, Ogre::uint height, int zorder, const Colour& colour)
{
 RectangleListIterator it = mRectangles.find(id);
 
 if (it != mRectangles.end())
 {
  (*it).second.left = left;
  (*it).second.top = top;
  (*it).second.width = width;
  (*it).second.height = height;
  (*it).second.depth = zorder;
  (*it).second.background = colour.asColourvalue();
  _redrawRectangleVerticesOnly( &(*it).second );
  _redrawRectangleColourOnly( &(*it).second );
  mRefreshRequired = true;
 }
 
}


Pixel Screen::getRectanglePosition(Ogre::uint id) const
{
 ConstRectangleListIterator it = mRectangles.find(id);
 
 if (it != mRectangles.end())
  return Pixel((*it).second.left, (*it).second.top);
 return Pixel(0,0);
}

Size Screen::getRectangleSize(Ogre::uint id) const
{
 ConstRectangleListIterator it = mRectangles.find(id);
 
 if (it != mRectangles.end())
  return Size((*it).second.width, (*it).second.height);
 return Size(0,0);
}

int Screen::getRectangleZOrder(Ogre::uint id) const
{
 ConstRectangleListIterator it = mRectangles.find(id);
 
 if (it != mRectangles.end())
  return (*it).second.depth;
 return 0;
}

Colour Screen::getRectangleColour(Ogre::uint id) const
{
 ConstRectangleListIterator it = mRectangles.find(id);
 
 if (it != mRectangles.end())
  return Colour((*it).second.background);
 return Colour();
}

void Screen::_redrawRectangleVerticesOnly(Rectangle* rect)
{
 
 Ogre::Real left, top, right, bottom, depth;

 left = _vpx(rect->left);
 top = _vpy(rect->top);
 right = _vpx(rect->left + rect->width);
 bottom = _vpy(rect->top + rect->height);
 depth = _vpz(rect->depth);
 
 size_t ver_index = rect->vertexStart;
 
 mRectangleData.mVertices[ver_index++] = Ogre::Vector3(left, bottom, depth);
 mRectangleData.mVertices[ver_index++] = Ogre::Vector3(right, bottom, depth);
 mRectangleData.mVertices[ver_index++] = Ogre::Vector3(left, top, depth);
 mRectangleData.mVertices[ver_index++] = Ogre::Vector3(right, bottom, depth);
 mRectangleData.mVertices[ver_index++] = Ogre::Vector3(right, top, depth);
 mRectangleData.mVertices[ver_index++] = Ogre::Vector3(left, top, depth);
 
}

void  Screen::_redrawRectangleColourOnly(Rectangle* rect)
{
 size_t col_index = rect->vertexStart;
 
 mRectangleData.mColours[col_index++] = rect->background;
 mRectangleData.mColours[col_index++] = rect->background;
 mRectangleData.mColours[col_index++] = rect->background;
 
 mRectangleData.mColours[col_index++] = rect->background;
 mRectangleData.mColours[col_index++] = rect->background;
 mRectangleData.mColours[col_index++] = rect->background;

}

void  Screen::_drawRectangle(Screen::Rectangle* rect)
{
  
 rect->vertexStart = mRectangleData.mVertices.size();
 
 Ogre::Real left, top, right, bottom, depth;

 left = _vpx(rect->left);
 top = _vpy(rect->top);
 right = _vpx(rect->left + rect->width);
 bottom = _vpy(rect->top + rect->height);
 depth = _vpz(rect->depth);
 

 mRectangleData.mVertices.push_back(Ogre::Vector3(left, bottom, depth));
 mRectangleData.mVertices.push_back(Ogre::Vector3(right, bottom, depth));
 mRectangleData.mVertices.push_back(Ogre::Vector3(left, top, depth));
 mRectangleData.mVertices.push_back(Ogre::Vector3(right, bottom, depth));
 mRectangleData.mVertices.push_back(Ogre::Vector3(right, top, depth));
 mRectangleData.mVertices.push_back(Ogre::Vector3(left, top, depth));
 
 mRectangleData.mColours.push_back(rect->background);
 mRectangleData.mColours.push_back(rect->background);
 mRectangleData.mColours.push_back(rect->background);
 
 mRectangleData.mColours.push_back(rect->background);
 mRectangleData.mColours.push_back(rect->background);
 mRectangleData.mColours.push_back(rect->background);
 
  
}

void Screen::_redrawRectangles()
{
 
 if (mRectangles.size() == 0)
 {
  mRectangleData.mVertices.clear();
  mRectangleData.mColours.clear();
 }
 
 size_t maxVertexSize = mRectangles.size() * 6;
 
 mRectangleData.mVertices.reserve(maxVertexSize);
 mRectangleData.mColours.reserve(maxVertexSize);
 
 Ogre::Real left, top, right, bottom, depth;
 size_t ver_index = 0, col_index = 0;

 for (RectangleListIterator rect = mRectangles.begin(); rect != mRectangles.end(); rect++)
 {
  (*rect).second.vertexStart = ver_index;
  
  left = _vpx((*rect).second.left);
  top = _vpy((*rect).second.top);
  right = _vpx((*rect).second.left + (*rect).second.width);
  bottom = _vpy((*rect).second.top + (*rect).second.height);
  depth = _vpz((*rect).second.depth);
  
  mRectangleData.mVertices[ver_index++] = Ogre::Vector3(left, bottom, depth);
  mRectangleData.mVertices[ver_index++] = Ogre::Vector3(right, bottom, depth);
  mRectangleData.mVertices[ver_index++] = Ogre::Vector3(left, top, depth);
  mRectangleData.mVertices[ver_index++] = Ogre::Vector3(right, bottom, depth);
  mRectangleData.mVertices[ver_index++] = Ogre::Vector3(right, top, depth);
  mRectangleData.mVertices[ver_index++] = Ogre::Vector3(left, top, depth);
  
  mRectangleData.mColours[col_index++] = (*rect).second.background;
  mRectangleData.mColours[col_index++] = (*rect).second.background;
  mRectangleData.mColours[col_index++] = (*rect).second.background;
  
  mRectangleData.mColours[col_index++] = (*rect).second.background;
  mRectangleData.mColours[col_index++] = (*rect).second.background;
  mRectangleData.mColours[col_index++] = (*rect).second.background;
  
 }
 
}

Ogre::uint  Screen::addSprite(const Pixel& position, Ogre::uint sprite_id, int zorder)
{
 return addSprite(position.left, position.top, sprite_id, zorder);
}

Ogre::uint  Screen::addSprite(int left, int top, Ogre::uint sprite_id, int zorder)
{
 
 Sprite sprite;
 sprite.left = left;
 sprite.top = top;
 sprite.depth = zorder;
 sprite.sprite = mAtlas->getSprite(sprite_id);
 sprite.vertexStart = 0;
 
 _drawSprite(&sprite);
 
 Ogre::uint id = mNextSpriteID++;
 mSprites[id] = sprite;
 mRefreshRequired = true;
 
 return id;
}

void  Screen::removeSprite(Ogre::uint id)
{
 SpriteListIterator it = mSprites.find(id);
 
 if (it != mSprites.end())
 {
  mSprites.erase(it);
  _redrawSprites();
  mRefreshRequired = true;
 }
}

void  Screen::removeAllSprites()
{
 mSprites.clear();
 mRefreshRequired = true;
}

Ogre::uint Screen::getNbSprites() const
{
 return mSprites.size();
}

void  Screen::setSpritePosition(Ogre::uint id, const Pixel& position)
{
 setSpritePosition(id, position.left, position.top);
}

void  Screen::setSpritePosition(Ogre::uint id, int left, int top)
{

 SpriteListIterator it = mSprites.find(id);
 
 if (it != mSprites.end())
 {
  (*it).second.left = left;
  (*it).second.top = top;
  _redrawSpriteVerticesOnly( &(*it).second );
  mRefreshRequired = true;
 }

}

void  Screen::setSpriteZOrder(Ogre::uint id, int zorder)
{
 SpriteListIterator it = mSprites.find(id);
 
 if (it != mSprites.end())
 {
  (*it).second.depth = zorder;
  _redrawSpriteVerticesOnly( &(*it).second );
  mRefreshRequired = true;
 }
}

void  Screen::updateSprite(Ogre::uint id, const Pixel& position, int zorder)
{
 updateSprite(id, position.left, position.top, zorder);
}

void  Screen::updateSprite(Ogre::uint id, int left, int top, int zorder)
{
 SpriteListIterator it = mSprites.find(id);
 
 if (it != mSprites.end())
 {
  (*it).second.left = left;
  (*it).second.top = top;
  (*it).second.depth = zorder;
  _redrawSpriteVerticesOnly( &(*it).second );
  mRefreshRequired = true;
 }
}

void  Screen::updateSprite(Ogre::uint id, Ogre::uint sprite_id)
{
 SpriteListIterator it = mSprites.find(id);
 
 if (it != mSprites.end())
 {
  
  SpriteTexture* new_sprite = mAtlas->getSprite(sprite_id);
  
  if (new_sprite->width == (*it).second.sprite->width && new_sprite->height == (*it).second.sprite->height)
  {
   (*it).second.sprite = new_sprite;
   _redrawSpriteUVOnly( &(*it).second );
  }
  else
  {
   (*it).second.sprite = new_sprite;
   _redrawSpriteVerticesOnly( &(*it).second );
   _redrawSpriteUVOnly( &(*it).second );
  }
  
  mRefreshRequired = true;
 }
}

int   Screen::getSpriteZOrder(Ogre::uint id) const
{
 ConstSpriteListIterator it = mSprites.find(id);
 
 if (it != mSprites.end())
  return (*it).second.depth;
 return 0;
}

Pixel  Screen::getSpritePosition(Ogre::uint id) const
{

 ConstSpriteListIterator it = mSprites.find(id);
 
 if (it != mSprites.end())
  return Pixel((*it).second.left, (*it).second.top);
 return Pixel(0,0);
}

Size  Screen::estimateSpriteSize(Ogre::uint sprite_id) const
{
 if (sprite_id > 255)
  return Size(0,0);
 SpriteTexture* sprite = mAtlas->getSprite(sprite_id);
 return Size(sprite->width, sprite->height);
}

void  Screen::_redrawSprites()
{

 if (mSprites.size() == 0)
 {
  mSpriteData.mVertices.clear();
  mSpriteData.mUV.clear();
 }
 
 size_t maxVertexSize = mSprites.size() * 6;
 
 mSpriteData.mVertices.reserve(maxVertexSize);
 mSpriteData.mUV.reserve(maxVertexSize);
 
 Ogre::Real left, top, right, bottom, depth;
 size_t ver_index = 0, uv_index = 0;

 for (SpriteListIterator sprite = mSprites.begin(); sprite != mSprites.end(); sprite++)
 {
  (*sprite).second.vertexStart = ver_index;
  
  left = _vpx((*sprite).second.left);
  top = _vpy((*sprite).second.top);
  right = _vpx((*sprite).second.left + (*sprite).second.sprite->width);
  bottom = _vpy((*sprite).second.top + (*sprite).second.sprite->height);
  depth = _vpz((*sprite).second.depth);
  
  mSpriteData.mVertices[ver_index++] = Ogre::Vector3(left, bottom, depth);
  mSpriteData.mVertices[ver_index++] = Ogre::Vector3(right, bottom, depth);
  mSpriteData.mVertices[ver_index++] = Ogre::Vector3(left, top, depth);
  mSpriteData.mVertices[ver_index++] = Ogre::Vector3(right, bottom, depth);
  mSpriteData.mVertices[ver_index++] = Ogre::Vector3(right, top, depth);
  mSpriteData.mVertices[ver_index++] = Ogre::Vector3(left, top, depth);
  
  mSpriteData.mUV[uv_index++] = Ogre::Vector2((*sprite).second.sprite->left, (*sprite).second.sprite->bottom);
  mSpriteData.mUV[uv_index++] = Ogre::Vector2((*sprite).second.sprite->right, (*sprite).second.sprite->bottom);
  mSpriteData.mUV[uv_index++] = Ogre::Vector2((*sprite).second.sprite->left, (*sprite).second.sprite->top);
  
  mSpriteData.mUV[uv_index++] = Ogre::Vector2((*sprite).second.sprite->right, (*sprite).second.sprite->bottom);
  mSpriteData.mUV[uv_index++] = Ogre::Vector2((*sprite).second.sprite->right, (*sprite).second.sprite->top);
  mSpriteData.mUV[uv_index++] = Ogre::Vector2((*sprite).second.sprite->left, (*sprite).second.sprite->top);
 }

}

void  Screen::_drawSprite(Sprite* sprite)
{

 sprite->vertexStart = mSpriteData.mVertices.size();
 
 
 Ogre::Real left, top, right, bottom, depth;

 left = _vpx(sprite->left);
 top = _vpy(sprite->top);
 right = _vpx(sprite->left + sprite->sprite->width);
 bottom = _vpy(sprite->top + sprite->sprite->height);
 depth = _vpz(sprite->depth);
 
 mSpriteData.mVertices.push_back(Ogre::Vector3(left, bottom, depth));
 mSpriteData.mVertices.push_back(Ogre::Vector3(right, bottom, depth));
 mSpriteData.mVertices.push_back(Ogre::Vector3(left, top, depth));
 mSpriteData.mVertices.push_back(Ogre::Vector3(right, bottom, depth));
 mSpriteData.mVertices.push_back(Ogre::Vector3(right, top, depth));
 mSpriteData.mVertices.push_back(Ogre::Vector3(left, top, depth));
 
 mSpriteData.mUV.push_back(Ogre::Vector2(sprite->sprite->left, sprite->sprite->bottom));
 mSpriteData.mUV.push_back(Ogre::Vector2(sprite->sprite->right, sprite->sprite->bottom));
 mSpriteData.mUV.push_back(Ogre::Vector2(sprite->sprite->left, sprite->sprite->top));
 mSpriteData.mUV.push_back(Ogre::Vector2(sprite->sprite->right, sprite->sprite->bottom));
 mSpriteData.mUV.push_back(Ogre::Vector2(sprite->sprite->right, sprite->sprite->top));
 mSpriteData.mUV.push_back(Ogre::Vector2(sprite->sprite->left, sprite->sprite->top));
 
}

void  Screen::_redrawSpriteVerticesOnly(Sprite* sprite)
{

 Ogre::Real left, top, right, bottom, depth;

 left = _vpx(sprite->left);
 top = _vpy(sprite->top);
 right = _vpx(sprite->left + sprite->sprite->width);
 bottom = _vpy(sprite->top + sprite->sprite->height);
 depth = _vpz(sprite->depth);
 
 size_t ver_index = sprite->vertexStart;
 
 mSpriteData.mVertices[ver_index++] = Ogre::Vector3(left, bottom, depth);
 mSpriteData.mVertices[ver_index++] = Ogre::Vector3(right, bottom, depth);
 mSpriteData.mVertices[ver_index++] = Ogre::Vector3(left, top, depth);
 mSpriteData.mVertices[ver_index++] = Ogre::Vector3(right, bottom, depth);
 mSpriteData.mVertices[ver_index++] = Ogre::Vector3(right, top, depth);
 mSpriteData.mVertices[ver_index++] = Ogre::Vector3(left, top, depth);
}

void Screen::_redrawSpriteUVOnly(Sprite* sprite)
{
 
 size_t uv_index = sprite->vertexStart;
 
 mSpriteData.mUV[uv_index++] = Ogre::Vector2(sprite->sprite->left, sprite->sprite->bottom);
 mSpriteData.mUV[uv_index++] = Ogre::Vector2(sprite->sprite->right, sprite->sprite->bottom);
 mSpriteData.mUV[uv_index++] = Ogre::Vector2(sprite->sprite->left, sprite->sprite->top);
 mSpriteData.mUV[uv_index++] = Ogre::Vector2(sprite->sprite->right, sprite->sprite->bottom);
 mSpriteData.mUV[uv_index++] = Ogre::Vector2(sprite->sprite->right, sprite->sprite->top);
 mSpriteData.mUV[uv_index++] = Ogre::Vector2(sprite->sprite->left, sprite->sprite->top);
 
}

Ogre::uint  Screen::addText(const Pixel& position, const Ogre::String& caption,  int zorder, const Colour& colour)
{
 return addText(position.left, position.top, caption, zorder, colour.asColourvalue());
}

Ogre::uint  Screen::addText(int left, int top, const Ogre::String& caption,  int zorder, const Ogre::ColourValue& colour)
{
 Text text;
 text.left = left;
 text.top = top;
 text.text = caption;
 text.colour = colour;
 text.depth = zorder;

 _drawText(&text);
 
 Ogre::uint id = mNextTextID++;
 mText[id] = text;
 mRefreshRequired = true;
 
 return id;

}

void  Screen::setTextPosition(Ogre::uint id, const Pixel& position)
{
 setTextPosition(id, position.left, position.top);
}

void  Screen::setTextPosition(Ogre::uint id, int left, int top)
{
 
 TextListIterator it = mText.find(id);
 
 if (it != mText.end())
 {
  (*it).second.left = left;
  (*it).second.top = top;
  _drawText( &(*it).second );
  mRefreshRequired = true;
 }
 
}

void  Screen::setTextCaption(Ogre::uint id, const Ogre::String& text)
{
 
 TextListIterator it = mText.find(id);
 
 if (it != mText.end())
 {
  (*it).second.text = text;
  _drawText( &(*it).second );
  mRefreshRequired = true;
 }
 
}

void  Screen::setTextZOrder(Ogre::uint id, int zorder)
{
 TextListIterator it = mText.find(id);
 
 if (it != mText.end())
 {
  (*it).second.depth = zorder;
  _drawText( &(*it).second );
  mRefreshRequired = true;
 }
}

void  Screen::setTextColour(Ogre::uint id, const Ogre::ColourValue& colour)
{
 TextListIterator it = mText.find(id);
 
 if (it != mText.end())
 {
  (*it).second.colour = colour;
  _drawText( &(*it).second );
  mRefreshRequired = true;
 }
}

void  Screen::setTextColour(Ogre::uint id, const Colour& colour)
{
 TextListIterator it = mText.find(id);
 
 if (it != mText.end())
 {
  (*it).second.colour = colour.asColourvalue();
  _drawText( &(*it).second );
  mRefreshRequired = true;
 }
}

void  Screen::updateText(Ogre::uint id, const Pixel& position, const Ogre::String& caption, int zorder, const Colour& colour)
{
 updateText(id, position.left, position.top, caption, zorder, colour);
}

void  Screen::updateText(Ogre::uint id, int left, int top, const Ogre::String& caption, int zorder, const Colour& colour)
{
 TextListIterator it = mText.find(id);
 
 if (it != mText.end())
 {
  (*it).second.left = left;
  (*it).second.top = top;
  (*it).second.text = caption;
  (*it).second.depth = zorder;
  (*it).second.colour = colour.asColourvalue();

  _drawText( &(*it).second );
  mRefreshRequired = true;
 }
}

Pixel  Screen::getTextPosition(Ogre::uint id) const
{
 ConstTextListIterator it = mText.find(id);
 
 if (it != mText.end())
  return Pixel((*it).second.left, (*it).second.top);
 return Pixel(0,0);
}

int  Screen::getTextZOrder(Ogre::uint id) const
{
 ConstTextListIterator it = mText.find(id);
 
 if (it != mText.end())
  return (*it).second.depth;
 return 0;
}


Colour  Screen::getTextColour(Ogre::uint id) const
{
 ConstTextListIterator it = mText.find(id);
 
 if (it != mText.end())
  return Colour((*it).second.colour);
 return Colour();
}

Size  Screen::estimateTextSize(const Ogre::String& text) const
{
 return Size(0,0);
}

Ogre::uint countChars(const Ogre::String& text, Ogre::uchar match)
{
 Ogre::uint count = 0;
 for (size_t i=0;i < text.length();i++)
 {
  if (text[i] == match)
   count++;
 }
 return count;
}

void Screen::_redrawText()
{
 for (TextListIterator it = mText.begin(); it != mText.end(); it++)
 {
  _drawText(&(*it).second);
 }
}

void  Screen::_drawText(Text* text)
{
 
 size_t estSize = (text->text.size() - countChars(text->text, '\n')) * 6;
 
 text->data.mVertices.clear();
 text->data.mUV.clear();
 text->data.mColours.clear();
 
 text->data.mVertices.reserve(estSize);
 text->data.mUV.reserve(estSize);
 text->data.mColours.reserve(estSize);
 
 Ogre::ColourValue workingColour = text->colour;
 Glyph* texture = 0;
 char c = 0;
 
 Pixel cursor(text->left, text->top);
 
 Ogre::Real left, top, right, bottom, depth;
  
 depth = _vpz(text->depth);
 char lastChar = 0;
 
 for (Ogre::uint i=0;i < text->text.size();i++)
 {
  
  Ogre::uchar c = text->text[i];
  
  if (c == ' ')
  {
   cursor.left += mAtlas->getSpaceLength();
   continue;
  }
  
  if (c == '\n')
  {
   cursor.left = text->left;
   cursor.top += mAtlas->getLineHeight();
   continue;
  }
  
  texture = mAtlas->getGlyph(c);
  
  
  left = _vpx(cursor.left + texture->kerning[lastChar]);
  top = _vpy(cursor.top);
  right = _vpx(cursor.left + texture->kerning[lastChar] + texture->width);
  bottom = _vpy(cursor.top + texture->kerning[lastChar] + texture->height);
  
  text->data.mVertices.push_back(  Ogre::Vector3(left, bottom, depth)  );
  text->data.mVertices.push_back(  Ogre::Vector3(right, bottom, depth)  );
  text->data.mVertices.push_back(  Ogre::Vector3(left, top, depth)  );
  text->data.mVertices.push_back(  Ogre::Vector3(right, bottom, depth)  );
  text->data.mVertices.push_back(  Ogre::Vector3(right, top, depth)  );
  text->data.mVertices.push_back(  Ogre::Vector3(left, top, depth)  );
  
  text->data.mUV.push_back(Ogre::Vector2(texture->left, texture->bottom));
  text->data.mUV.push_back(Ogre::Vector2(texture->right, texture->bottom));
  text->data.mUV.push_back(Ogre::Vector2(texture->left, texture->top));
  text->data.mUV.push_back(Ogre::Vector2(texture->right, texture->bottom));
  text->data.mUV.push_back(Ogre::Vector2(texture->right, texture->top));
  text->data.mUV.push_back(Ogre::Vector2(texture->left, texture->top));
  
  text->data.mColours.push_back(workingColour);
  text->data.mColours.push_back(workingColour);
  text->data.mColours.push_back(workingColour);
  
  text->data.mColours.push_back(workingColour);
  text->data.mColours.push_back(workingColour);
  text->data.mColours.push_back(workingColour);
  
  cursor.left += texture->advance;
  lastChar = c;
 }
 
}


}
