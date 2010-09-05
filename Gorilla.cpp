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

#pragma warning ( disable : 4244 )

template<> Gorilla::Silverback* Ogre::Singleton<Gorilla::Silverback>::ms_Singleton = 0;

#define PUSH_VERTEX(VERTICES, VERTEX, X, Y, UV, COLOUR)   \
  VERTEX.position.x = X;                                           \
  VERTEX.position.y = Y;                                           \
  VERTEX.position.z = 0;                                           \
  VERTEX.uv.x = UV.x;                                              \
  VERTEX.uv.y = UV.y;                                              \
  VERTEX.colour = COLOUR;                                          \
  VERTICES.push_back(VERTEX);                                       

#define PUSH_TRIANGLE(VERTICES, VERTEX, A, B, C, UV, COLOUR)       \
  PUSH_VERTEX(VERTICES, VERTEX, A.x, A.y, UV, COLOUR) \
  PUSH_VERTEX(VERTICES, VERTEX, B.x, B.y, UV, COLOUR) \
  PUSH_VERTEX(VERTICES, VERTEX, C.x, C.y, UV, COLOUR) 

#define PUSH_QUAD(VERTICES, VERTEX, POSITIONS, COLOURS, UV)   \
  PUSH_VERTEX(VERTICES, VERTEX, POSITIONS[3].x, POSITIONS[3].y, UV[3], COLOURS[3]) \
  PUSH_VERTEX(VERTICES, VERTEX, POSITIONS[1].x, POSITIONS[1].y, UV[1], COLOURS[1]) \
  PUSH_VERTEX(VERTICES, VERTEX, POSITIONS[0].x, POSITIONS[0].y, UV[0], COLOURS[0]) \
     \
  PUSH_VERTEX(VERTICES, VERTEX, POSITIONS[3].x, POSITIONS[3].y, UV[3], COLOURS[3]) \
  PUSH_VERTEX(VERTICES, VERTEX, POSITIONS[2].x, POSITIONS[2].y, UV[2], COLOURS[2]) \
  PUSH_VERTEX(VERTICES, VERTEX, POSITIONS[1].x, POSITIONS[1].y, UV[1], COLOURS[1])  

#define PUSH_QUAD2(VERTICES, VERTEX, POSITIONS, COLOUR, UV)   \
  PUSH_VERTEX(VERTICES, VERTEX, POSITIONS[3].x, POSITIONS[3].y, UV[3], COLOUR) \
  PUSH_VERTEX(VERTICES, VERTEX, POSITIONS[1].x, POSITIONS[1].y, UV[1], COLOUR) \
  PUSH_VERTEX(VERTICES, VERTEX, POSITIONS[0].x, POSITIONS[0].y, UV[0], COLOUR) \
     \
  PUSH_VERTEX(VERTICES, VERTEX, POSITIONS[3].x, POSITIONS[3].y, UV[3], COLOUR) \
  PUSH_VERTEX(VERTICES, VERTEX, POSITIONS[2].x, POSITIONS[2].y, UV[2], COLOUR) \
  PUSH_VERTEX(VERTICES, VERTEX, POSITIONS[1].x, POSITIONS[1].y, UV[1], COLOUR)  


namespace Gorilla
{
 
 Ogre::ColourValue rgb(Ogre::uchar r, Ogre::uchar g, Ogre::uchar b, Ogre::uchar a )
 {
  static const Ogre::Real inv255 = Ogre::Real(0.00392156863);
  return Ogre::ColourValue(Ogre::Real(r) * inv255, Ogre::Real(g) * inv255, Ogre::Real(b) * inv255, Ogre::Real(a) * inv255);
 }

 Ogre::ColourValue webcolour(Colours::Colour web_colour, Ogre::Real alpha)
 {
  static const Ogre::Real inv255 = Ogre::Real(0.00392156863);
  Ogre::ColourValue ret;
  ret.b = Ogre::Real(web_colour & 0xFF) * inv255;
  ret.g = Ogre::Real((web_colour >> 8) & 0xFF) * inv255;
  ret.r = Ogre::Real((web_colour >> 16) & 0xFF) * inv255;
  ret.a = alpha;
  return ret;
 }
 
 TextureAtlas::TextureAtlas(const Ogre::String& gorillaFile, const Ogre::String& groupName)
 {
  
  _reset();
  _load(gorillaFile, groupName);
  _calculateCoordinates();
  _createMaterial();
  
 }

 TextureAtlas::~TextureAtlas()
 {
 }

 void  TextureAtlas::_reset()
 {
  refreshMarkupColours();
 }

 void  TextureAtlas::_load(const Ogre::String& gorillaFile, const Ogre::String& groupName)
 {
  
  Ogre::ConfigFile f;
  f.loadFromResourceSystem(gorillaFile, groupName, " ", true);
  Ogre::ConfigFile::SectionIterator seci = f.getSectionIterator();
  
  Ogre::String secName;
  while (seci.hasMoreElements())
  {
   
   secName = seci.peekNextKey();
   Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
   
   Ogre::StringUtil::toLowerCase(secName);
   
   if (secName == "texture")
    _loadTexture(settings);
   else if (secName == "font" || secName == "glyphs")
    _loadGlyphs(settings);
   else if (secName == "fontkerning" || secName == "glyphskerning" || secName == "kerning")
    _loadKerning(settings);
   else if (secName == "sprites")
    _loadSprites(settings);
   
  }
  
 }

 void  TextureAtlas::_loadTexture(Ogre::ConfigFile::SettingsMultiMap* settings)
 {
  
  
  Ogre::String name, data;
  Ogre::ConfigFile::SettingsMultiMap::iterator i;
  for (i = settings->begin(); i != settings->end(); ++i)
  {
   name = i->first;
   data = i->second;

   size_t comment = data.find_first_of('#');
   if (comment != std::string::npos)
    data = data.substr(0, comment);

   Ogre::StringUtil::toLowerCase(name);
   
   // file texturename ~groupname
   // >  file myatlas.png
   // >  file myatlas.png ~loadinggroup
   if (name == "file")
   {
    Ogre::String textureName = data;
    Ogre::String groupName = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;
    size_t groupSplit = data.find_first_of('~');
    if (groupSplit != std::string::npos)
    {
     textureName = data.substr(0, groupSplit);
     groupName = data.substr(groupSplit+1);
     Ogre::StringUtil::trim(textureName);
     Ogre::StringUtil::trim(groupName);
    }
    
    mTexture = Ogre::TextureManager::getSingletonPtr()->getByName(data, groupName);
    
    if (mTexture.isNull())
     mTexture = Ogre::TextureManager::getSingletonPtr()->load(textureName, groupName);
    
    mInverseTextureSize.x = 1.0f / mTexture->getWidth();
    mInverseTextureSize.y = 1.0f / mTexture->getHeight();
    
    continue;
   }
   // layer x y
   // >  layer 100 105
   else if (name == "rectangle")
   {
    mRectangleCoords = Ogre::StringConverter::parseVector2(data);
    mRectangleCoords.x *= mInverseTextureSize.x;
    mRectangleCoords.y *= mInverseTextureSize.y;
   }
   
  }
  
  
 }

 void  TextureAtlas::_loadGlyphs(Ogre::ConfigFile::SettingsMultiMap* settings)
 {
  
  Ogre::String name, data;
  Ogre::ConfigFile::SettingsMultiMap::iterator i;
  
  Ogre::StringVector str_values;
  for (i = settings->begin(); i != settings->end(); ++i) 
  {
  
   name = i->first;
   data = i->second;

   size_t comment = data.find_first_of('#');
   if (comment != std::string::npos)
    data = data.substr(0, comment);

   Ogre::StringUtil::toLowerCase(name);
   
   // lineheight lineheight
   // >  lineheight 15
   if (name == "lineheight")
   {
    mGlyphLineHeight = Ogre::StringConverter::parseReal(data);
    continue;
   }
   // spacelength spacelength
   // >  spacelength 3
   else if (name == "spacelength")
   {
    mGlyphSpaceLength = Ogre::StringConverter::parseReal(data);
    continue;
   }
   // spacelength spacelength
   // >  spacelength 3
   else if (name == "baseline")
   {
    mGlyphBaseline = Ogre::StringConverter::parseReal(data);
    continue;
   }
   // monowidth width
   // >  monowidth 3
   else if (name == "monowidth")
   {
    mGlyphMonoWidth = Ogre::StringConverter::parseReal(data);
    continue;
   }
   // scale glyph_scale
   // >  scale 0.75
   else if (name == "scale")
   {
    mGlyphScale = Ogre::StringConverter::parseReal(data);
    continue;
   }
   // range lower upper
   // >  range 33 126
   else if (name == "range")
   {
    Ogre::Vector2 t = Ogre::StringConverter::parseVector2(data);
    mGlyphRangeBegin = t.x;
    mGlyphRangeEnd = t.y;
   }
   // kerning kerning
   // >  kerning -1
   else if (name == "kerning")
   {
    mGlyphAllGlyphsKerning = Ogre::StringConverter::parseReal(data);
   }
  }
  

  for (Ogre::uint index = mGlyphRangeBegin; index < mGlyphRangeEnd; index++)
  {
   
   Glyph* glyph = OGRE_NEW Glyph();
   mGlyphs.push_back(glyph);
   
   std::stringstream s;
   s << "glyph_" << index;
   
   i = settings->find(s.str());
   if (i == settings->end())
    continue;
   
   str_values = Ogre::StringUtil::split((*i).second, " ", 5);
   
   if (str_values.size() < 4)
   {
    std::cout << "[Gorilla] Glyph #" << (*i).second << " does not have enough properties.\n";
    continue;
   }
   
   glyph->uvLeft    = Ogre::StringConverter::parseReal(  str_values[0]  );
   glyph->uvTop     = Ogre::StringConverter::parseReal(  str_values[1]  );
   glyph->uvWidth   = Ogre::StringConverter::parseReal(  str_values[2]  );
   glyph->uvHeight  = Ogre::StringConverter::parseReal(  str_values[3]  );
   glyph->uvRight   = glyph->uvLeft + glyph->uvWidth;
   glyph->uvBottom  = glyph->uvTop + glyph->uvHeight;
   
   if (str_values.size() == 5)
     glyph->glyphAdvance = Ogre::StringConverter::parseInt(  str_values[4]  );
   else
    glyph->glyphAdvance = glyph->uvWidth;
   
   
  }
  
  
 }

 void  TextureAtlas::_loadKerning(Ogre::ConfigFile::SettingsMultiMap* settings)
 {
  
  Ogre::String left_name, data;
  Ogre::ConfigFile::SettingsMultiMap::iterator i;
  Ogre::uint left_glyph_id;
  Ogre::uint right_glyph_id;
  int kerning;
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
   
   
   mGlyphs[right_glyph_id - mGlyphRangeBegin]->kerning.push_back(Kerning(left_glyph_id, kerning));
   
  }
  
 }

 void  TextureAtlas::_loadSprites(Ogre::ConfigFile::SettingsMultiMap* settings)
 {
  
  Ogre::String sprite_name, data;
  Ogre::ConfigFile::SettingsMultiMap::iterator i;
  
  Ogre::StringVector str_values;
  for (i = settings->begin(); i != settings->end(); ++i)
  {
  
   sprite_name = i->first;
   data = i->second;
   
   size_t comment = data.find_first_of('#');
   if (comment != std::string::npos)
    data = data.substr(0, comment);
   
   str_values = Ogre::StringUtil::split(data, " ", 4);
   
   if (str_values.size() != 4)
   {
    std::cout << "[Gorilla] Sprite #" << sprite_name << " does not have enough properties\n" << data << "\n";
    continue;
   }
   
   Sprite* sprite = OGRE_NEW Sprite();
   
   sprite->uvLeft = Ogre::StringConverter::parseUnsignedInt(str_values[0]);
   sprite->uvTop = Ogre::StringConverter::parseUnsignedInt(str_values[1]);
   sprite->spriteWidth = Ogre::StringConverter::parseUnsignedInt(str_values[2]);
   sprite->spriteHeight = Ogre::StringConverter::parseUnsignedInt(str_values[3]);
   
   mSprites[sprite_name] = sprite;
   
  }
  
 }

 void  TextureAtlas::_save(const Ogre::String& gorillaFile)
 {
 }

 void  TextureAtlas::_createMaterial()
 {
  
  std::string matName = "GorillaMaterialFor" + mTexture->getName();
  mMaterial == Ogre::MaterialManager::getSingletonPtr()->getByName(matName);
  
  if (mMaterial.isNull() == false)
   return;

  mMaterial = Ogre::MaterialManager::getSingletonPtr()->create(matName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  mPass = mMaterial->getTechnique(0)->getPass(0);
  
  mPass->setCullingMode(Ogre::CULL_NONE);
  mPass->setDepthCheckEnabled(false);
  mPass->setDepthWriteEnabled(false);
  mPass->setLightingEnabled(false);
  mPass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);

  mTextureUnit = mPass->createTextureUnitState(mTexture->getName());
  mTextureUnit->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
  mTextureUnit->setTextureFiltering(Ogre::FO_NONE, Ogre::FO_NONE, Ogre::FO_NONE);
  //mTextureUnit->setTextureAnisotropy(2);
  //mTextureUnit->setNumMipmaps(0);
  //mTextureUnit->setTextureAnisotropy(0);
 }

 void  TextureAtlas::_calculateCoordinates()
 {
  
  for(std::vector<Glyph*>::iterator it = mGlyphs.begin(); it != mGlyphs.end(); it++)
  {
   (*it)->uvLeft        *= mInverseTextureSize.x;
   (*it)->uvTop         *= mInverseTextureSize.y;
   (*it)->uvRight       *= mInverseTextureSize.x;
   (*it)->uvBottom      *= mInverseTextureSize.y;

   (*it)->texCoords[TopLeft].x = (*it)->uvLeft;
   (*it)->texCoords[TopLeft].y = (*it)->uvTop;
   (*it)->texCoords[TopRight].x = (*it)->uvRight;
   (*it)->texCoords[TopRight].y = (*it)->uvTop;
   (*it)->texCoords[BottomRight].x = (*it)->uvRight;
   (*it)->texCoords[BottomRight].y = (*it)->uvBottom;
   (*it)->texCoords[BottomLeft].x = (*it)->uvLeft;
   (*it)->texCoords[BottomLeft].y = (*it)->uvBottom;

   (*it)->glyphAdvance  *= mGlyphScale;

   (*it)->glyphWidth     = (*it)->uvWidth;
   (*it)->glyphHeight    = (*it)->uvHeight;
   
   for (Ogre::uint i=0;i < (*it)->kerning.size();i++)
    (*it)->kerning[i].kerning *= mGlyphScale;
  }
  
  for (std::map<Ogre::String, Sprite*>::iterator it = mSprites.begin(); it != mSprites.end(); it++)
  {
   (*it).second->uvRight    = (*it).second->uvLeft + (*it).second->spriteWidth;
   (*it).second->uvBottom   = (*it).second->uvTop  + (*it).second->spriteHeight;
   
   (*it).second->uvLeft    *= mInverseTextureSize.x;
   (*it).second->uvTop     *= mInverseTextureSize.y;
   (*it).second->uvRight   *= mInverseTextureSize.x;
   (*it).second->uvBottom  *= mInverseTextureSize.y;

   (*it).second->texCoords[TopLeft].x = (*it).second->uvLeft;
   (*it).second->texCoords[TopLeft].y = (*it).second->uvTop;
   (*it).second->texCoords[TopRight].x = (*it).second->uvRight;
   (*it).second->texCoords[TopRight].y = (*it).second->uvTop;
   (*it).second->texCoords[BottomRight].x = (*it).second->uvRight;
   (*it).second->texCoords[BottomRight].y = (*it).second->uvBottom;
   (*it).second->texCoords[BottomLeft].x = (*it).second->uvLeft;
   (*it).second->texCoords[BottomLeft].y = (*it).second->uvBottom;

  }
  
 }

 void   TextureAtlas::refreshMarkupColours()
 {
  mMarkupColour[0] = rgb(255, 255, 255);
  mMarkupColour[1] = rgb(0, 0, 0);
  mMarkupColour[2] = rgb(204, 204, 204);
  mMarkupColour[3] = rgb(254, 220, 129);
  mMarkupColour[4] = rgb(254, 138, 129);
  mMarkupColour[5] = rgb(123, 236, 110);
  mMarkupColour[6] = rgb(44,  192, 171);
  mMarkupColour[7] = rgb(199, 93,  142);
  mMarkupColour[8] = rgb(254, 254, 254);
  mMarkupColour[9] = rgb(13,  13,  13);
 }

 void   TextureAtlas::setMarkupColour(Ogre::uint index, const Ogre::ColourValue& colour)
 { 
  if (index > 9)
   return;
  
  mMarkupColour[index] = colour;
 }

 Ogre::ColourValue  TextureAtlas::getMarkupColour(Ogre::uint index)
 {
  if (index > 9)
   return Ogre::ColourValue::White;
  
  return mMarkupColour[index];
 }



 Silverback::Silverback()
 {
 }

 Silverback::~Silverback()
 {
  
  // Delete Screens.
  for (std::vector<Screen*>::iterator it = mScreens.begin(); it != mScreens.end(); it++)
   OGRE_DELETE (*it);
  
  mScreens.clear();
  
  // Delete Atlases.
  for (std::map<Ogre::String, TextureAtlas*>::iterator it = mAtlases.begin(); it != mAtlases.end(); it++)
   OGRE_DELETE (*it).second;
  mAtlases.clear();
  
 }

 void Silverback::loadAtlas(const Ogre::String &name, const Ogre::String &group)
 {
  TextureAtlas* atlas = OGRE_NEW TextureAtlas(name + ".gorilla", group);
  mAtlases[name] = atlas;
 }

 Screen* Silverback::createScreen(Ogre::Viewport* viewport, const Ogre::String& atlas_name)
 {
  TextureAtlas* atlas = (*mAtlases.find(atlas_name)).second;
  Screen* screen = OGRE_NEW Screen(viewport, atlas);
  mScreens.push_back(screen);
  return screen;
 }














 Screen::Screen(Ogre::Viewport* viewport, TextureAtlas* atlas)
 : mViewport(viewport), mAtlas(atlas), mLayerRedrawNeeded(false)
 {
  for (size_t i=0;i < 16;i++)
   mRedrawLayerNeeded[i] = false;

  mSceneMgr = mViewport->getCamera()->getSceneManager();
  
  mRenderSystem = Ogre::Root::getSingletonPtr()->getRenderSystem();
  
  mViewportWidth = mViewport->getActualWidth();
  mViewportHeight = mViewport->getActualHeight();
  mInvViewportWidth = 1.0f / mViewportWidth;
  mInvViewportHeight = 1.0f / mViewportHeight;

  _createBuffer();
  _registerListener();
 }

 Screen::~Screen()
 {
  _unregisterListener();
  
  for (std::vector<Layer*>::iterator it = mAllLayers.begin(); it != mAllLayers.end();it++)
   OGRE_DELETE (*it);
  mAllLayers.clear();
  
  _destroyBuffer();
 }

 void  Screen::_registerListener()
 {
  mSceneMgr->addRenderQueueListener(this);
 }

 void  Screen::_unregisterListener()
 {
  mSceneMgr->removeRenderQueueListener(this);
 }

 void  Screen::_createBuffer()
 {

  mMaxVertexCount = 32 * 6;
  mRenderOp.vertexData = OGRE_NEW Ogre::VertexData;
  mRenderOp.vertexData->vertexStart = 0;

  Ogre::VertexDeclaration* vDecl = mRenderOp.vertexData->vertexDeclaration;

  size_t offset = 0;
  
  vDecl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
  offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

  vDecl->addElement(0, offset, Ogre::VET_FLOAT4, Ogre::VES_DIFFUSE);
  offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT4);
  
  vDecl->addElement(0, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);

  
  mVertexBuffer = Ogre::HardwareBufferManager::getSingletonPtr()->createVertexBuffer(
   vDecl->getVertexSize(0),
   mMaxVertexCount,
   Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE,
   false
  );
  
  mRenderOp.vertexData->vertexBufferBinding->setBinding(0, mVertexBuffer);
  mRenderOp.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;
  mRenderOp.useIndexes = false;
 }

 void  Screen::_destroyBuffer()
 {
  OGRE_DELETE mRenderOp.vertexData;
  mRenderOp.vertexData = 0;
  mVertexBuffer.setNull();
  mMaxVertexCount = 0;
 }

 void  Screen::_checkBuffer(size_t neededVertices)
 {

  if (mMaxVertexCount == 0)
   _createBuffer();
  
  if (neededVertices > mMaxVertexCount)
  {
   
   size_t newVertexBufferSize = 1;
   
   while (newVertexBufferSize < neededVertices)
    newVertexBufferSize <<= 1;
   
   mVertexBuffer = Ogre::HardwareBufferManager::getSingletonPtr()->createVertexBuffer(
    mRenderOp.vertexData->vertexDeclaration->getVertexSize(0),
    newVertexBufferSize,
    Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE,
    false
   );
   mMaxVertexCount = newVertexBufferSize;
   mRenderOp.vertexData->vertexStart = 0;
   mRenderOp.vertexData->vertexBufferBinding->setBinding(0, mVertexBuffer);
  }
  
 }

 void  Screen::_calculateLayers()
 {
  
  // Clear all layers.
  for (Ogre::uint i=0;i < 16;i++)
  {
   mLayers[i].remove_all();
   mVertexLayerBuffer[i].remove_all();
   mRedrawLayerNeeded[i] = false;
  }
  
  //
  for (std::vector<Layer*>::iterator it = mAllLayers.begin(); it != mAllLayers.end();it++)
  {
   mLayers[ (*it)->getIndex() ].push_back( (*it) );
  }
  
  mRedrawAll = true;
 }

 void  Screen::_redrawAll()
 {
  _calculateLayers();
  for (Ogre::uint i=0;i < 16;i++)
   _redrawLayer(i); 
 }

 void  Screen::_redrawLayer(Ogre::uint id)
 {
  mRedrawLayerNeeded[id] = false;
  mVertexLayerBuffer[id].remove_all();
  
  for (size_t i=0;i < mLayers[id].size();i++)
  {
   if (mLayers[id][i]->mVisible)
    mLayers[id][i]->_render(mVertexLayerBuffer[id]);
  }
  
 }

 void  Screen::_layerRedrawRequested(Ogre::uint layer)
 {
  mLayerRedrawNeeded = true;
  mRedrawLayerNeeded[layer] = true;
 }

 void  Screen::_copyToVertexBuffer()
 {
  
  mKnownVertexCount = 0;
  for (size_t i=0;i < 16;i++)
   mKnownVertexCount += mVertexLayerBuffer[i].size();
  
  _checkBuffer(mKnownVertexCount);
  
  Vertex* writeIterator = (Vertex*) mVertexBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
  
  for (size_t layer=0;layer < 16;layer++)
  {
   for (size_t vertex=0;vertex < mVertexLayerBuffer[layer].size();vertex++)
   {
    *writeIterator++ = mVertexLayerBuffer[layer][vertex];
   }
  }
  
  mVertexBuffer->unlock();
  mRenderOp.vertexData->vertexCount = mKnownVertexCount;
 }

 void  Screen::_forceViewportChange()
 {
  mViewportWidth = mViewport->getActualWidth();
  mViewportHeight = mViewport->getActualHeight();
  mInvViewportWidth = 1.0f / mViewportWidth;
  mInvViewportHeight = 1.0f / mViewportHeight;

  _redrawAll();
 }

 void  Screen::renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& repeatThisInvocation)
 {
  if (queueGroupId == Ogre::RENDER_QUEUE_OVERLAY)
  {
   
   if (mViewport->getActualWidth() != mViewportWidth || mViewport->getActualHeight() != mViewportHeight)
   {
    
    mViewportWidth = mViewport->getActualWidth();
    mViewportHeight = mViewport->getActualHeight();
    mInvViewportWidth = 1.0f / mViewportWidth;
    mInvViewportHeight = 1.0f / mViewportHeight;
    
    _redrawAll();
   }

   // Check to see if redraw needed here.
   if ( mLayerRedrawNeeded == true)
   {
    for (size_t i=0;i < 16;i++)
     if (mRedrawLayerNeeded[i])
      _redrawLayer(i);
    mLayerRedrawNeeded = false;
    _copyToVertexBuffer();
   }
   
   if (mRenderOp.vertexData->vertexCount)
   {
    _prepareRenderSystem();
    _render();
   }
  }
 }

 void  Screen::_prepareRenderSystem()
 {

  mRenderSystem->_setWorldMatrix( Ogre::Matrix4::IDENTITY );
  mRenderSystem->_setProjectionMatrix( Ogre::Matrix4::IDENTITY );
  mRenderSystem->_setViewMatrix( Ogre::Matrix4::IDENTITY );
  
  mSceneMgr->_setPass(mAtlas->getPass());
 }

 void  Screen::_render()
 {
  mRenderSystem->_render(mRenderOp);
 }
 
 Layer*  Screen::createLayer(Ogre::uint index)
 {
  Layer* layer = OGRE_NEW Layer(index, this);
  mAllLayers.push_back(layer);
  mLayers[index].push_back(layer);
  mRedrawLayerNeeded[index] = true;
  mLayerRedrawNeeded = true;
  return layer;
 }
  
 void   Screen::destroy(Layer* layer)
 {
  
  if (layer == 0)
   return;
  
  mAllLayers.erase(std::find(mAllLayers.begin(), mAllLayers.end(), layer));
  // TOOD: Erase layer from mLayers[x] as well.
  OGRE_DELETE layer;
  
 }
 
 
 
 Layer::Layer(Ogre::uint index, Screen* screen)
 : mIndex(index), mScreen(screen), mVisible(true)
 {
 }
 
 Layer::~Layer()
 {
  destroyAllRectangles();
  destroyAllPolygons();
  destroyAllLineLists();
  destroyAllQuadLists();
  destroyAllCaptions();
  destroyAllMarkupTexts();
 }
 
 void Layer::_markDirty()
 {
  mScreen->_layerRedrawRequested(mIndex);
 }
 
 Rectangle* Layer::createRectangle(Ogre::Real left, Ogre::Real top, Ogre::Real width, Ogre::Real height)
 {
  Rectangle* rectangle = OGRE_NEW Rectangle(left, top, width, height, this);
  mRectangles.push_back(rectangle);
  return rectangle;
 }
 
 void Layer::destroyRectangle(Rectangle* rectangle)
 {
  if (rectangle == 0)
   return;
  
  mRectangles.erase(std::find(mRectangles.begin(), mRectangles.end(), rectangle));
  OGRE_DELETE rectangle;
  _markDirty();
 }
 
 void Layer::destroyAllRectangles()
 {
  
  for (Rectangles::iterator it = mRectangles.begin(); it != mRectangles.end(); it++)
  {
   OGRE_DELETE (*it);
  }
  
  mRectangles.clear();
 }

 Polygon* Layer::createPolygon(Ogre::Real left, Ogre::Real top, Ogre::Real radius, Ogre::uint sides)
 {
  if (sides < 3)
   sides = 3;
  
  Polygon* polygon = OGRE_NEW Polygon(left, top, radius, sides, this);
  mPolygons.push_back(polygon);
  return polygon;
 }

 void Layer::destroyPolygon(Polygon* polygon)
 {
  if (polygon == 0)
   return;
  
  mPolygons.erase(std::find(mPolygons.begin(), mPolygons.end(), polygon));
  OGRE_DELETE polygon;
  _markDirty();
 }

 void Layer::destroyAllPolygons()
 {
  
  for (Polygons::iterator it = mPolygons.begin(); it != mPolygons.end(); it++)
  {
   OGRE_DELETE (*it);
  }
  
  mPolygons.clear();
 }

 LineList* Layer::createLineList()
 {
  LineList* linelist = OGRE_NEW LineList(this);
  mLineLists.push_back(linelist);
  return linelist;
 }

 void Layer::destroyLineList(LineList* linelist)
 {
  if (linelist == 0)
   return;
  
  mLineLists.erase(std::find(mLineLists.begin(), mLineLists.end(), linelist));
  OGRE_DELETE linelist;
  _markDirty();
 }

 void Layer::destroyAllLineLists()
 {
  
  for (LineLists::iterator it = mLineLists.begin(); it != mLineLists.end(); it++)
  {
   OGRE_DELETE (*it);
  }
  
  mLineLists.clear();
 }

 QuadList* Layer::createQuadList()
 {
  QuadList* quadlist = OGRE_NEW QuadList(this);
  mQuadLists.push_back(quadlist);
  return quadlist;
 }

 void Layer::destroyQuadList(QuadList* quadlist)
 {
  if (quadlist == 0)
   return;
  
  mQuadLists.erase(std::find(mQuadLists.begin(), mQuadLists.end(), quadlist));
  OGRE_DELETE quadlist;
  _markDirty();
 }

 void Layer::destroyAllQuadLists()
 {
  
  for (QuadLists::iterator it = mQuadLists.begin(); it != mQuadLists.end(); it++)
  {
   OGRE_DELETE (*it);
  }
  
  mQuadLists.clear();
 }

 Caption* Layer::createCaption(Ogre::Real x, Ogre::Real y, const Ogre::String& text)
 {
  Caption* caption = OGRE_NEW Caption(x, y, text, this);
  mCaptions.push_back(caption);
  return caption;
 }

 void Layer::destroyCaption(Caption* caption)
 {
  if (caption == 0)
   return;
  
  mCaptions.erase(std::find(mCaptions.begin(), mCaptions.end(), caption));
  OGRE_DELETE caption;
  _markDirty();
 }

 void Layer::destroyAllCaptions()
 {
  
  for (Captions::iterator it = mCaptions.begin(); it != mCaptions.end(); it++)
  {
   OGRE_DELETE (*it);
  }
  
  mCaptions.clear();
 }

 MarkupText* Layer::createMarkupText(Ogre::Real x, Ogre::Real y, const Ogre::String& text)
 {
  MarkupText* markuptext = OGRE_NEW MarkupText(x, y, text, this);
  mMarkupTexts.push_back(markuptext);
  return markuptext;
 }

 void Layer::destroyMarkupText(MarkupText* markuptext)
 {
  if (markuptext == 0)
   return;
  
  mMarkupTexts.erase(std::find(mMarkupTexts.begin(), mMarkupTexts.end(), markuptext));
  OGRE_DELETE markuptext;
  _markDirty();
 }

 void Layer::destroyAllMarkupTexts()
 {
  
  for (MarkupTexts::iterator it = mMarkupTexts.begin(); it != mMarkupTexts.end(); it++)
  {
   OGRE_DELETE (*it);
  }
  
  mMarkupTexts.clear();
 }

 void Layer::_render(buffer<Vertex>& vertices)
 {
  
   Ogre::Real xTexel = mScreen->getRenderSystem()->getHorizontalTexelOffset(),
              yTexel = mScreen->getRenderSystem()->getVerticalTexelOffset(),
              invWidth = mScreen->getViewportWidth(),
              invHeight = mScreen->getViewportHeight();

  size_t begin = vertices.size();
  size_t i = 0;
  
  // Render/redraw rectangles
  for (Rectangles::iterator it = mRectangles.begin(); it != mRectangles.end(); it++)
  {
   
   if ((*it)->mDirty)
    (*it)->_redraw();
  
   for (i=0; i < (*it)->mVertices.size(); i++)
    vertices.push_back((*it)->mVertices[i]);
  
  }
  
  // Render/redraw polygons
  for (Polygons::iterator it = mPolygons.begin(); it != mPolygons.end(); it++)
  {
   
   if ((*it)->mDirty)
    (*it)->_redraw();
  
   for (i=0; i < (*it)->mVertices.size(); i++)
    vertices.push_back((*it)->mVertices[i]);
  
  }

  // Render/redraw line lists
  for (LineLists::iterator it = mLineLists.begin(); it != mLineLists.end(); it++)
  {
   
   if ((*it)->mDirty)
    (*it)->_redraw();
  
   for (i=0; i < (*it)->mVertices.size(); i++)
    vertices.push_back((*it)->mVertices[i]);
  
  }

  // Render/redraw quad lists
  for (QuadLists::iterator it = mQuadLists.begin(); it != mQuadLists.end(); it++)
  {
   
   if ((*it)->mDirty)
    (*it)->_redraw();
  
   for (i=0; i < (*it)->mVertices.size(); i++)
    vertices.push_back((*it)->mVertices[i]);
  
  }

  // Render/redraw caption
  for (Captions::iterator it = mCaptions.begin(); it != mCaptions.end(); it++)
  {
   
   if ((*it)->mDirty)
    (*it)->_redraw();
  
   for (i=0; i < (*it)->mVertices.size(); i++)
    vertices.push_back((*it)->mVertices[i]);
  
  }
  
  // Render/redraw caption
  for (MarkupTextIterator::iterator it = mMarkupTexts.begin(); it != mMarkupTexts.end(); it++)
  {
   
   if ((*it)->mTextDirty)
    (*it)->_calculateCharacters();
   
   if ((*it)->mDirty)
    (*it)->_redraw();
  
   for (i=0; i < (*it)->mVertices.size(); i++)
    vertices.push_back((*it)->mVertices[i]);
  
  }
  
  for (size_t i = begin; i < vertices.size(); i++)
  {
   vertices[i].position.x = ((vertices[i].position.x + xTexel) / invWidth) * 2 - 1;
   vertices[i].position.y = ((vertices[i].position.y + yTexel) / invHeight) * -2 + 1;
  }
  
 }
 
 
 Rectangle::Rectangle(Ogre::Real left, Ogre::Real top, Ogre::Real width, Ogre::Real height, Layer* layer) : mLayer(layer)
 {
  mDirty       = true;
  mLeft        = left;
  mTop         = top;
  mRight       = left + width;
  mBottom      = top + height;
  mBorderWidth = 0.0f;
  mBackgroundColour[0] = mBackgroundColour[1] =  mBackgroundColour[2] = mBackgroundColour[3] = Ogre::ColourValue::White;
  mUV[0] = mUV[1] = mUV[2] = mUV[3] = mLayer->_getSolidUV();
  
 }
 
 
 void  Rectangle::_redraw()
 {
  
  if (mDirty == false)
   return;
  
  mVertices.remove_all();
  
  Ogre::Vector2 a, b, c, d;
  a.x = mLeft;  a.y = mTop;
  b.x = mRight; b.y = mTop;
  c.x = mLeft;  c.y = mBottom;
  d.x = mRight; d.y = mBottom;
  
  // Border
  if (mBorderWidth != 0)
  {
    Ogre::Vector2 i = a, j = b, k = c, l = d;
    i.x -= mBorderWidth;    i.y -= mBorderWidth;
    j.x += mBorderWidth;    j.y -= mBorderWidth;
    k.x -= mBorderWidth;    k.y += mBorderWidth;
    l.x += mBorderWidth;    l.y += mBorderWidth;
    
    Vertex temp;
    Ogre::Vector2 uv = mLayer->_getSolidUV();
    
    // North
    PUSH_TRIANGLE(mVertices, temp, a, j, i, uv, mBorderColour[Border_North])
    PUSH_TRIANGLE(mVertices, temp, a, b, j, uv, mBorderColour[Border_North])
    
    // East
    PUSH_TRIANGLE(mVertices, temp, d, j, b, uv, mBorderColour[Border_East])
    PUSH_TRIANGLE(mVertices, temp, d, l, j, uv, mBorderColour[Border_East])
    
    // South
    PUSH_TRIANGLE(mVertices, temp, k, d, c, uv, mBorderColour[Border_South])
    PUSH_TRIANGLE(mVertices, temp, k, l, d, uv, mBorderColour[Border_South])
    
    // West
    PUSH_TRIANGLE(mVertices, temp, k, a, i, uv, mBorderColour[Border_West])
    PUSH_TRIANGLE(mVertices, temp, k, c, a, uv, mBorderColour[Border_West])
    
  }
  
  // Fill
  if (mBackgroundColour[0].a != 0)
  {
   Vertex temp;
   // Triangle A
   PUSH_VERTEX(mVertices, temp, c.x, c.y, mUV[3], mBackgroundColour[3]);    // Left/Bottom  3
   PUSH_VERTEX(mVertices, temp, b.x, b.y, mUV[1], mBackgroundColour[1]);    // Right/Top    1
   PUSH_VERTEX(mVertices, temp, a.x, a.y, mUV[0], mBackgroundColour[0]);    // Left/Top     0
   
   // Triangle B
   PUSH_VERTEX(mVertices, temp, c.x, c.y, mUV[3], mBackgroundColour[3]);    // Left/Bottom   3
   PUSH_VERTEX(mVertices, temp, d.x, d.y, mUV[2], mBackgroundColour[2]);    // Right/Bottom  2
   PUSH_VERTEX(mVertices, temp, b.x, b.y, mUV[1], mBackgroundColour[1]);    // Right/Top     1
  }
  
  mDirty = false;
  
 }
 
Polygon::Polygon(Ogre::Real left, Ogre::Real top, Ogre::Real radius, size_t sides, Layer* layer)
 : mLayer(layer), mSprite(0)
 {
  mDirty        = true;
  mLayer->_markDirty();
  mLeft           = left;
  mTop            = top;
  mRadius         = radius;
  mSides          = sides;
  mBackgroundColour         = Ogre::ColourValue::White;
  mBorderColour.a = 0;
  mBorderWidth    = 0;
 }
 
 void  Polygon::_redraw()
 {
  
  if (mDirty == false)
   return;
  
  mVertices.remove_all();
  Vertex temp;
  
  Ogre::Real theta = mAngle.valueRadians();
  Ogre::Real inc = (2 * Ogre::Math::PI) / Ogre::Real(mSides);

  Ogre::Vector2 lastVertex;
  lastVertex.x = mLeft + (mRadius * cos(theta));
  lastVertex.y = mTop + (mRadius * sin(theta));
  Ogre::Vector2 thisVertex;
  
  if (mBorderWidth != 0)
  {
   
   Ogre::Vector2 lastOuterVertex, outerVertex, thisVertex, uv;
   uv = mLayer->_getSolidUV();
    
   lastOuterVertex.x = mLeft + ((mRadius + mBorderWidth) * cos(theta));
   lastOuterVertex.y = mTop + ((mRadius + mBorderWidth) * sin(theta));
   
   
   for (size_t i=0;i < mSides;i++)
   {
    theta += inc;
    thisVertex.x = mLeft + (mRadius * Ogre::Math::Cos(theta));
    thisVertex.y = mTop + (mRadius * Ogre::Math::Sin(theta));
    outerVertex.x = mLeft + ((mRadius + mBorderWidth) * Ogre::Math::Cos(theta));
    outerVertex.y = mTop + ((mRadius + mBorderWidth) * Ogre::Math::Sin(theta));
    
    PUSH_TRIANGLE(mVertices, temp, lastVertex, outerVertex, lastOuterVertex, uv, mBorderColour);
    PUSH_TRIANGLE(mVertices, temp, lastVertex, thisVertex, outerVertex, uv, mBorderColour);
    
    lastVertex = thisVertex;
    lastOuterVertex = outerVertex;
   }
  }
  
  if (mBackgroundColour.a != 0)
  {
     if (mSprite)
     {
      Ogre::Real xRadius = mSprite->spriteWidth * 0.5f;
      Ogre::Real yRadius = mSprite->spriteHeight * 0.5f;
      
      Ogre::Vector2 centerUV, lastUV, thisUV, baseUV, texSize = mLayer->_getTextureSize();
      baseUV.x = mSprite->uvLeft * texSize.x;
      baseUV.y = mSprite->uvTop * texSize.y;
      baseUV.x += xRadius;
      baseUV.y += yRadius;
      
      centerUV = baseUV / texSize;
      lastUV = baseUV;
      lastUV.x = baseUV.x + (xRadius * Ogre::Math::Cos(theta));
      lastUV.y = baseUV.y + (yRadius * Ogre::Math::Sin(theta));
      lastUV /= texSize;
      
      for (size_t i=0;i < mSides;i++)
      {
       PUSH_VERTEX(mVertices, temp, mLeft, mTop, centerUV, mBackgroundColour);
       theta += inc;
       thisVertex.x = mLeft + (mRadius * Ogre::Math::Cos(theta));
       thisVertex.y = mTop + (mRadius * Ogre::Math::Sin(theta));
       thisUV.x = baseUV.x + (xRadius * Ogre::Math::Cos(theta));
       thisUV.y = baseUV.y + (yRadius * Ogre::Math::Sin(theta));
       thisUV /= texSize;
       PUSH_VERTEX(mVertices, temp, thisVertex.x, thisVertex.y, thisUV, mBackgroundColour);
       PUSH_VERTEX(mVertices, temp, lastVertex.x, lastVertex.y, lastUV, mBackgroundColour);
       lastVertex = thisVertex;
       lastUV = thisUV;
      }
     }
     else
     {
      Ogre::Vector2 uv = uv = mLayer->_getSolidUV();
      
      for (size_t i=0;i < mSides;i++)
      {
       PUSH_VERTEX(mVertices, temp, mLeft, mTop, uv, mBackgroundColour);
       theta += inc;
       thisVertex.x = mLeft + (mRadius * Ogre::Math::Cos(theta));
       thisVertex.y = mTop + (mRadius * Ogre::Math::Sin(theta));
       PUSH_VERTEX(mVertices, temp, thisVertex.x, thisVertex.y, uv, mBackgroundColour);
       PUSH_VERTEX(mVertices, temp, lastVertex.x, lastVertex.y, uv, mBackgroundColour);
       lastVertex = thisVertex;
      }
     }
  }
  
  mDirty = false;
  
 }
 


 LineList::LineList(Layer* layer) : mLayer(layer)
 {
  mDirty = false;
 }
 
 void  LineList::begin(Ogre::Real lineThickness, const Ogre::ColourValue& colour)
 {
  mDirty = false;
  mPositions.remove_all();
  mThickness = lineThickness;
  mColour = colour;
 }
 
 void  LineList::position(Ogre::Real x, Ogre::Real y)
 {
  mPositions.push_back(Ogre::Vector2(x,y));
 }
 
 void  LineList::position(const Ogre::Vector2& position)
 {
  mPositions.push_back(position);
 }
 
 void  LineList::end(bool closed)
 {
  mIsClosed = closed;
  mDirty = true;
 }
 
 void  LineList::_redraw()
 {
  if (mDirty == false)
   return;
  
  mVertices.remove_all();
  
  if (mPositions.size() < 2)
   return;
  
  Vertex temp;
  
  Ogre::Real halfThickness = mThickness * 0.5f;
  
  Ogre::Vector2 perp, lastLeft, lastRight, thisLeft, thisRight, uv = mLayer->_getSolidUV();
  
  size_t i = 1;
  
  for (;i < mPositions.size();i++)
  {
   
   perp      = (mPositions[i] - mPositions[i-1]).perpendicular().normalisedCopy();
   lastLeft  = mPositions[i-1] - perp * halfThickness;
   lastRight = mPositions[i-1] + perp * halfThickness;
   thisLeft  = mPositions[i] - perp * halfThickness;
   thisRight = mPositions[i] + perp * halfThickness;
   
   // Triangle A
   PUSH_VERTEX(mVertices, temp, lastRight.x, lastRight.y, uv, mColour);       // Left/Bottom
   PUSH_VERTEX(mVertices, temp, thisLeft.x, thisLeft.y, uv, mColour);         // Right/Top
   PUSH_VERTEX(mVertices, temp, lastLeft.x, lastLeft.y, uv, mColour);          // Left/Top
   // Triangle B
   PUSH_VERTEX(mVertices, temp, lastRight.x, lastRight.y, uv, mColour);       // Left/Bottom
   PUSH_VERTEX(mVertices, temp, thisRight.x, thisRight.y, uv, mColour);      // Right/Bottom
   PUSH_VERTEX(mVertices, temp, thisLeft.x, thisLeft.y, uv, mColour);         // Right/Top
   
  }
  
  if (mIsClosed)
  {
   i = mPositions.size() - 1;
   perp      = (mPositions[0] - mPositions[i]).perpendicular().normalisedCopy();
   lastLeft  = mPositions[i] - perp * halfThickness;
   lastRight = mPositions[i] + perp * halfThickness;
   thisLeft  = mPositions[0] - perp * halfThickness;
   thisRight = mPositions[0] + perp * halfThickness;
   
   // Triangle A
   PUSH_VERTEX(mVertices, temp, lastRight.x, lastRight.y, uv, mColour);       // Left/Bottom
   PUSH_VERTEX(mVertices, temp, thisLeft.x, thisLeft.y, uv, mColour);         // Right/Top
   PUSH_VERTEX(mVertices, temp, lastLeft.x, lastLeft.y, uv, mColour);          // Left/Top
   // Triangle B
   PUSH_VERTEX(mVertices, temp, lastRight.x, lastRight.y, uv, mColour);       // Left/Bottom
   PUSH_VERTEX(mVertices, temp, thisRight.x, thisRight.y, uv, mColour);      // Right/Bottom
   PUSH_VERTEX(mVertices, temp, thisLeft.x, thisLeft.y, uv, mColour);         // Right/Top
   
  }
  
 }

 QuadList::QuadList(Layer* layer)
 : mLayer(layer)
 {
  mWhiteUV = mLayer->_getSolidUV();
 }
 
 void  QuadList::begin()
 {
  mQuads.remove_all();
  mDirty = false;
 }

 void  QuadList::rectangle(Ogre::Real x, Ogre::Real y, Ogre::Real w, Ogre::Real h, const Ogre::ColourValue colour)
 {
  Quad q;
  q.mPosition[TopLeft].x = x; q.mPosition[TopLeft].y = y;
  q.mPosition[TopRight].x = x + w; q.mPosition[TopRight].y = y;
  q.mPosition[BottomRight].x = x + w; q.mPosition[BottomRight].y = y + h;
  q.mPosition[BottomLeft].x = x; q.mPosition[BottomLeft].y = y + h;


  q.mColour[0] = q.mColour[1] = q.mColour[2] = q.mColour[3] = colour;
  q.mUV[0] = q.mUV[1] = q.mUV[2] = q.mUV[3] = mWhiteUV;
  mQuads.push_back(q);
 }
 
 void  QuadList::gradient(Ogre::Real x, Ogre::Real y, Ogre::Real w, Ogre::Real h, Gradient gradient, const Ogre::ColourValue& colourA, const Ogre::ColourValue& colourB)
 {
  Quad q;
  if (gradient == Gradient_NorthSouth)
  {
   q.mColour[0] = q.mColour[1] = colourA;
   q.mColour[2] = q.mColour[3] = colourB;
  }
  else if (gradient == Gradient_WestEast)
  {
   q.mColour[0] = q.mColour[3] = colourA;
   q.mColour[1] = q.mColour[2] = colourB;
  }
  else if (gradient == Gradient_Diagonal)
  {
   Ogre::ColourValue avg;
   avg.r = (colourA.r + colourB.r) * 0.5f;
   avg.g = (colourA.g + colourB.g) * 0.5f;
   avg.b = (colourA.b + colourB.b) * 0.5f;
   avg.a = (colourA.a + colourB.a) * 0.5f;
   q.mColour[0] = colourA;
   q.mColour[1] = avg = q.mColour[3] = avg;
   q.mColour[2] = colourB;
  }
  
  q.mPosition[TopLeft].x = x; q.mPosition[TopLeft].y = y;
  q.mPosition[TopRight].x = x + w; q.mPosition[TopRight].y = y;
  q.mPosition[BottomRight].x = x + w; q.mPosition[BottomRight].y = y + h;
  q.mPosition[BottomLeft].x = x; q.mPosition[BottomLeft].y = y + h;
  
  q.mUV[0] = q.mUV[1] = q.mUV[2] = q.mUV[3] = mWhiteUV;
  mQuads.push_back(q);
 }
 
 void  QuadList::sprite(Ogre::Real x, Ogre::Real y, Ogre::Real w, Ogre::Real h, Sprite* sprite)
 {
  Quad q;
  q.mPosition[TopLeft].x     = q.mPosition[BottomLeft].x    = x;
  q.mPosition[TopLeft].y     = q.mPosition[TopRight].y      = y;
  q.mPosition[TopRight].x    = q.mPosition[BottomRight].x   = x + w;
  q.mPosition[BottomRight].y = q.mPosition[BottomLeft].y    = y + h;
  q.mColour[0] = q.mColour[1] = q.mColour[2] = q.mColour[3] = Ogre::ColourValue::White;
  
  q.mUV[TopLeft].x = sprite->uvLeft;
  q.mUV[TopLeft].y = sprite->uvTop;
  
  q.mUV[TopRight].x = sprite->uvRight;
  q.mUV[TopRight].y = sprite->uvTop;
  
  q.mUV[BottomLeft].x = sprite->uvLeft;
  q.mUV[BottomLeft].y = sprite->uvBottom;
  
  q.mUV[BottomRight].x = sprite->uvRight;
  q.mUV[BottomRight].y = sprite->uvBottom;
  mQuads.push_back(q);
  
 }

 void  QuadList::border(Ogre::Real x, Ogre::Real y, Ogre::Real w, Ogre::Real h, Ogre::Real thickness, const Ogre::ColourValue& colour)
 {
  border(x,y,w,h,thickness,colour,colour,colour,colour);
 }
 
void  QuadList::border(Ogre::Real x, Ogre::Real y, Ogre::Real w, Ogre::Real h, Ogre::Real thickness, const Ogre::ColourValue& northColour, const Ogre::ColourValue& eastColour, const Ogre::ColourValue& southColour, const Ogre::ColourValue& westColour)
 {
  
  Ogre::Vector2 a(x,y), b(x+w,y), c(x,y+h), d(x+w,y+h),
                       i = a, j = b, k = c, l = d;
  i.x -= thickness;    i.y -= thickness;
  j.x += thickness;    j.y -= thickness;
  k.x -= thickness;    k.y += thickness;
  l.x += thickness;    l.y += thickness;
  
  Vertex temp;
  Ogre::Vector2 uv = mLayer->_getSolidUV();
  
  // North
  Quad q;
  q.mUV[0] = q.mUV[1] = q.mUV[2] = q.mUV[3] = mWhiteUV;

  // North
  q.mPosition[TopLeft] = i;
  q.mPosition[TopRight] = j;
  q.mPosition[BottomLeft] = a;
  q.mPosition[BottomRight] = b;
  q.mColour[0] = q.mColour[1] = q.mColour[2] = q.mColour[3] = northColour;
  mQuads.push_back(q);

  // East
  q.mPosition[TopLeft] = b;
  q.mPosition[TopRight] = j;
  q.mPosition[BottomLeft] = d;
  q.mPosition[BottomRight] = l;
  q.mColour[0] = q.mColour[1] = q.mColour[2] = q.mColour[3] = eastColour;
  mQuads.push_back(q);
  
  // South
  q.mPosition[TopLeft] = c;
  q.mPosition[TopRight] = d;
  q.mPosition[BottomLeft] = k;
  q.mPosition[BottomRight] = l;
  q.mColour[0] = q.mColour[1] = q.mColour[2] = q.mColour[3] = southColour;
  mQuads.push_back(q);
  
  // West
  q.mPosition[TopLeft] = i;
  q.mPosition[TopRight] = a;
  q.mPosition[BottomLeft] = k;
  q.mPosition[BottomRight] = c;
  q.mColour[0] = q.mColour[1] = q.mColour[2] = q.mColour[3] = westColour;
  mQuads.push_back(q);
  
 }
 
 void  QuadList::glyph(Ogre::Real x, Ogre::Real y, unsigned char character, const Ogre::ColourValue& colour)
 {
  
  Glyph* glyph = mLayer->_getGlyph(character);
  if (glyph == 0)
   return;
  
  Quad q;
  q.mPosition[TopLeft].x = x;
  q.mPosition[TopLeft].y = y;
  q.mPosition[TopRight].x = x + glyph->glyphWidth;
  q.mPosition[TopRight].y = y;
  q.mPosition[BottomRight].x = x + glyph->glyphWidth;
  q.mPosition[BottomRight].y = y + glyph->glyphHeight;
  q.mPosition[BottomLeft].x = x;
  q.mPosition[BottomLeft].y = y + glyph->glyphHeight;
  
  q.mColour[0] = q.mColour[1] = q.mColour[2] = q.mColour[3] = colour;

  q.mUV[TopLeft].x = glyph->uvLeft;
  q.mUV[TopLeft].y = glyph->uvTop;

  q.mUV[TopRight].x = glyph->uvRight;
  q.mUV[TopRight].y = glyph->uvTop;

  q.mUV[BottomRight].x = glyph->uvRight;
  q.mUV[BottomRight].y = glyph->uvBottom;

  q.mUV[BottomLeft].x = glyph->uvLeft;
  q.mUV[BottomLeft].y = glyph->uvBottom;
  
  mQuads.push_back(q);
 }
 
 void  QuadList::glyph(Ogre::Real x, Ogre::Real y, Ogre::Real w, Ogre::Real h, unsigned char character, const Ogre::ColourValue& colour)
 {
  Glyph* glyph = mLayer->_getGlyph(character);
  if (glyph == 0)
   return;
  
  Quad q;
  q.mPosition[TopLeft].x = x;
  q.mPosition[TopLeft].y = y;
  q.mPosition[TopRight].x = x + w;
  q.mPosition[TopRight].y = y;
  q.mPosition[BottomRight].x = x + w;
  q.mPosition[BottomRight].y = y + h;
  q.mPosition[BottomLeft].x = x;
  q.mPosition[BottomLeft].y = y + h;
  
  q.mColour[0] = q.mColour[1] = q.mColour[2] = q.mColour[3] = colour;

  q.mUV[TopLeft].x = glyph->uvLeft;
  q.mUV[TopLeft].y = glyph->uvTop;

  q.mUV[TopRight].x = glyph->uvRight;
  q.mUV[TopRight].y = glyph->uvTop;

  q.mUV[BottomRight].x = glyph->uvRight;
  q.mUV[BottomRight].y = glyph->uvBottom;

  q.mUV[BottomLeft].x = glyph->uvLeft;
  q.mUV[BottomLeft].y = glyph->uvBottom;
  
  mQuads.push_back(q);
 }
 
 void  QuadList::end()
 {
  mDirty = true;
  mLayer->_markDirty();
 }
 
 void  QuadList::_redraw()
 {
  if (mDirty == false)
   return;
  
  mVertices.remove_all();
  Vertex temp;
  
  for (size_t i=0;i < mQuads.size();i++)
  {
   PUSH_QUAD(mVertices, temp, mQuads[i].mPosition, mQuads[i].mColour, mQuads[i].mUV)
  }
  
  mDirty = false;
 }
 
 
 Caption::Caption(Ogre::Real left, Ogre::Real top, const Ogre::String& caption, Layer* layer)
 : mLayer(layer)
 {
  mDirty        = true;
  mLayer->_markDirty();
  mLeft           = left;
  mTop            = top;
  mWidth          = 0.0f;
  mHeight         = 0.0f;
  mText           = caption;
  mColour         = Ogre::ColourValue::White;
  mBackground.a   = 0.0f;
  mAlignment      = TextAlign_Left;
 }

 void Caption::_calculateDrawSize(Ogre::Vector2& retSize)
 {
  TextureAtlas* atlas = mLayer->_getAtlas();
  Ogre::Real cursor = 0,
             kerning;

  unsigned char thisChar = 0, lastChar = 0;
  Glyph* glyph = 0;
  retSize.x = 0;
  retSize.y = atlas->getGlyphLineHeight();
  
  for (size_t i=0;i < mText.length();i++)
  {
   thisChar = mText[i];
   
   if (thisChar == ' ')
   {
    lastChar = thisChar;
    cursor += atlas->getGlyphSpaceLength();
    continue;
   }
   
   if (thisChar < atlas->getGlyphRangeBegin() || thisChar > atlas->getGlyphRangeEnd())
   {
    lastChar = 0;
    continue;
   }
   
   glyph = atlas->getGlyph(thisChar);
   kerning = glyph->getKerning(lastChar);
   if (kerning == 0)
    kerning = atlas->getGlyphKerning();
      
   cursor  += glyph->glyphAdvance + kerning;
   lastChar = thisChar;
   
  } // for
  
  retSize.x = cursor - kerning;
 }
 
 void Caption::_redraw()
 {
  
  if (mDirty == false)
   return;
  
  mVertices.remove_all();
  
  Ogre::Vector2 uv = mLayer->_getSolidUV();
  
  if (mBackground.a > 0)
  {
   Ogre::Vector2  a, b, c, d;
   a.x = mLeft;  a.y = mTop;
   b.x = mLeft + mWidth; b.y = mTop;
   c.x = mLeft;  c.y = mTop + mHeight;
   d.x = mLeft + mWidth; d.y = c.y = mTop + mHeight;
   Vertex temp;
   PUSH_TRIANGLE(mVertices, temp, c, b, a, uv, mBackground);
   PUSH_TRIANGLE(mVertices, temp, c, d, b, uv, mBackground);
  }
  
  Ogre::Real left = 0, top = 0, right = 0, bottom = 0, cursorX = 0, cursorY = 0, kerning = 0, texelOffsetX = mLayer->_getTexelX(), texelOffsetY = mLayer->_getTexelY();
  Ogre::Vector2 knownSize;
  Glyph* glyph = 0;
  
  bool clipLeft = false, clipRight = false;
  Ogre::Real clipLeftPos = 0, clipRightPos = 0;
  
  if (mAlignment == TextAlign_Left)
  {
   cursorX = mLeft;
   
   if (mWidth)
   {
    clipRight = true;
    clipRightPos = mLeft + mWidth;
   }

  }
  else if (mAlignment == TextAlign_Centre)
  {
   _calculateDrawSize(knownSize);
   cursorX = mLeft + (mWidth * 0.5) - (knownSize.x * 0.5f);
   
   if (mWidth)
   {
    clipLeft = true;
    clipLeftPos = mLeft;
    clipRight = true;
    clipRightPos = mLeft + mWidth;
   }
   
  }
  else if (mAlignment == TextAlign_Right)
  {
   _calculateDrawSize(knownSize);
   cursorX = mLeft + mWidth - knownSize.x;
   if (mWidth)
   {
    clipLeft = true;
    clipLeftPos = mLeft;
   }
  }
  
  if (mVerticalAlign == VerticalAlign_Top)
   cursorY = mTop;
  else if (mVerticalAlign == VerticalAlign_Middle)
   cursorY = mTop + (mHeight * 0.5) - (mLayer->_getGlyphHeight() * 0.5);
  else if (mVerticalAlign == VerticalAlign_Bottom)
   cursorY = mTop +  mHeight - mLayer->_getGlyphHeight();
  
  unsigned char thisChar = 0, lastChar = 0;
  Vertex temp;
  mClippedLeftIndex = std::string::npos;
  mClippedRightIndex = std::string::npos;
  
  for (size_t i=0;i < mText.size();i++)
  {
   thisChar = mText[i];
   
   if (thisChar == ' ')
   {
    lastChar = thisChar;
    cursorX += mLayer->_getGlyphSpaceLength();
    continue;
   }
   
   if (thisChar < mLayer->_getGlyphRangeBegin() || thisChar > mLayer->_getGlyphRangeEnd())
   {
    lastChar = 0;
    continue;
   }
   
   glyph = mLayer->_getGlyph(thisChar);
   kerning = glyph->getKerning(lastChar);
   if (kerning == 0)
    kerning = mLayer->_getDefaultGlyphKerning();
   
   left = cursorX;
   top = cursorY;
   right = left + glyph->glyphWidth + texelOffsetX;
   bottom = top + glyph->glyphHeight + texelOffsetY;
   
   if (clipLeft)
   {
    if (left < clipLeftPos)
    {
     if (mClippedLeftIndex == std::string::npos)
       mClippedLeftIndex = i;
     cursorX  += glyph->glyphAdvance + kerning;
     lastChar = thisChar;
     continue;
    }
   }
   
   if (clipRight)
   {
    if (right > clipRightPos)
    {
     if (mClippedRightIndex == std::string::npos)
       mClippedRightIndex = i;
     cursorX  += glyph->glyphAdvance + kerning;
     lastChar = thisChar;
     continue;
    }
   }
   
   // Triangle A
   PUSH_VERTEX(mVertices, temp, left, bottom, glyph->texCoords[BottomLeft], mColour);  // Left/Bottom  3
   PUSH_VERTEX(mVertices, temp, right, top, glyph->texCoords[TopRight], mColour);    // Right/Top    1
   PUSH_VERTEX(mVertices, temp, left, top, glyph->texCoords[TopLeft], mColour);     // Left/Top     0
   
   // Triangle B
   PUSH_VERTEX(mVertices, temp, left, bottom, glyph->texCoords[BottomLeft], mColour);  // Left/Bottom  3
   PUSH_VERTEX(mVertices, temp, right, bottom, glyph->texCoords[BottomRight], mColour); // Right/Bottom 2
   PUSH_VERTEX(mVertices, temp, right, top, glyph->texCoords[TopRight], mColour);    // Right/Top    1
   
   
   cursorX  += glyph->glyphAdvance + kerning;
   lastChar = thisChar;
   
  } // for
  
  
  mDirty = false;
 }
 
 
 MarkupText::MarkupText(Ogre::Real left, Ogre::Real top, const Ogre::String& text, Layer* parent)
 : mLayer(parent)
 {
  mDirty          = true;
  mTextDirty      = true;
  mLayer->_markDirty();
  mLeft           = left;
  mTop            = top;
  mWidth          = 0.0f;
  mHeight         = 0.0f;
  mText           = text;
  mBackground.a   = 0.0f;
 }
 
 void MarkupText::_calculateCharacters()
 {
  if (mTextDirty == false)
   return;
  
  Ogre::Real cursorX = mLeft, cursorY = mTop, kerning = 0, texelOffsetX = mLayer->_getTexelX(), texelOffsetY = mLayer->_getTexelY(), right = 0, bottom = 0;
  unsigned int thisChar = 0, lastChar = 0;
  Glyph* glyph = 0;
  
  mCharacters.remove_all();
  
  bool markupMode = false;
  Ogre::ColourValue colour = mLayer->_getMarkupColour(0);
  bool fixedWidth = false;
  
  for(size_t i=0;i < mText.length();i++)
  {
   
   thisChar = mText[i];
   
   if (thisChar == ' ')
   {
    lastChar = thisChar;
    cursorX += mLayer->_getGlyphSpaceLength();
    continue;
   }
   
   if (thisChar == '\n')
   {
    lastChar = thisChar;
    cursorX = mLeft;
    cursorY += mLayer->_getGlyphHeight();
    continue;
   }
   
   if (thisChar < mLayer->_getGlyphRangeBegin() || thisChar > mLayer->_getGlyphRangeEnd())
   {
    lastChar = 0;
    continue;
   }
   
   if (thisChar == '%' && markupMode == false)
   {
    markupMode = true;
    continue;
   }
   
   if (markupMode == true)
   {
    if (thisChar == '%')
    {
     // Escape Character.
    }
    else
    {
     markupMode = false;
     
     if (thisChar >= '0' && thisChar <= '9')
     {
      colour = mLayer->_getMarkupColour(thisChar - 48);
     }
     else if (thisChar == 'R' || thisChar == 'r')
     {
      colour = mLayer->_getMarkupColour(0);
     }
     else if (thisChar == 'M' || thisChar == 'm')
     {
      fixedWidth = !fixedWidth;
     }
     else if (thisChar == ':')
     {
      bool foundIt = false;
      size_t begin = i;
      while(i < mText.size())
      {
       if (mText[i] == '%')
       {
        foundIt = true;
        break;
       }
       i++;
      }
      
      if (foundIt == false)
       return;
      
      Ogre::String sprite_name = mText.substr(begin+1, i - begin - 1);
      
      Sprite* sprite = mLayer->_getSprite(sprite_name);
      if (sprite == 0)
       continue;
      
      right = cursorX + sprite->spriteWidth;
      bottom = cursorY + sprite->spriteHeight;
      
      Character c;
      c.mIndex = i;
      c.mPosition[TopLeft].x = cursorX;
      c.mPosition[TopLeft].y = cursorY;
      c.mPosition[TopRight].x = right;
      c.mPosition[TopRight].y = cursorY;
      c.mPosition[BottomLeft].x = cursorX;
      c.mPosition[BottomLeft].y = bottom;
      c.mPosition[BottomRight].x = right;
      c.mPosition[BottomRight].y = bottom;
      c.mUV[0] = sprite->texCoords[0];
      c.mUV[1] = sprite->texCoords[1];
      c.mUV[2] = sprite->texCoords[2];
      c.mUV[3] = sprite->texCoords[3];
      c.mColour = colour;
      
      mCharacters.push_back(c);
      
      cursorX  += sprite->spriteWidth;
      
     }
     

     continue;
    }
    markupMode = false;
   }
   
   glyph = mLayer->_getGlyph(thisChar);
   
   if (!fixedWidth)
   {
    kerning = glyph->getKerning(lastChar);
    if (kerning == 0)
     kerning = mLayer->_getDefaultGlyphKerning();
   }
   
   right = cursorX + glyph->glyphWidth + texelOffsetX;
   bottom = cursorY + glyph->glyphHeight + texelOffsetY;
   
   Character c;
   c.mIndex = i;
   c.mPosition[TopLeft].x = cursorX;
   c.mPosition[TopLeft].y = cursorY;
   c.mPosition[TopRight].x = right;
   c.mPosition[TopRight].y = cursorY;
   c.mPosition[BottomLeft].x = cursorX;
   c.mPosition[BottomLeft].y = bottom;
   c.mPosition[BottomRight].x = right;
   c.mPosition[BottomRight].y = bottom;
   c.mUV[0] = glyph->texCoords[0];
   c.mUV[1] = glyph->texCoords[1];
   c.mUV[2] = glyph->texCoords[2];
   c.mUV[3] = glyph->texCoords[3];
   c.mColour = colour;
   
   mCharacters.push_back(c);
   
   if (fixedWidth)
     cursorX  += mLayer->_getGlyphFixedWidth();
   else
     cursorX  += glyph->glyphAdvance + kerning;
   
   lastChar = thisChar;
  }
  
  mTextDirty = false;
 }
 
 void  MarkupText::_redraw()
 {
  
  if (mDirty == false)
   return;
  
  mVertices.remove_all();
  
  Vertex temp;
  for (size_t i=0; i < mCharacters.size();i++)
  {
   PUSH_QUAD2(mVertices, temp, mCharacters[i].mPosition, mCharacters[i].mColour, mCharacters[i].mUV);
  }
  
  mDirty = false;
 }

} // namespace Gorilla

