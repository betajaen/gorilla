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

template<>   Gorilla::Silverback*   Ogre::Singleton<Gorilla::Silverback>::ms_Singleton = 0;

Gorilla::Vertex Gorilla::Renderable::tVertex = Gorilla::Vertex();
Ogre::Real Gorilla::Renderable::tTop = 0;
Ogre::Real Gorilla::Renderable::tBottom = 0;
Ogre::Real Gorilla::Renderable::tLeft = 0;
Ogre::Real Gorilla::Renderable::tRight = 0;
Ogre::Real Gorilla::Renderable::t1 = 0;
Ogre::Real Gorilla::Renderable::t2 = 0;
Ogre::Real Gorilla::Renderable::tA = 0;
Ogre::Real Gorilla::Renderable::tB = 0;
Ogre::Real Gorilla::Renderable::tC = 0;
Ogre::Real Gorilla::Renderable::tD = 0;
bool Gorilla::Text::tMonospace = false;
Gorilla::Glyph* Gorilla::Text::tGlyph = 0;
Ogre::ColourValue Gorilla::Text::tWorkingColour = Ogre::ColourValue();

#define GET_SPRITE_OR_DIE(NAME) \
  Sprite* sprite = mScreen->getAtlas()->getSprite(NAME); \
  if (sprite == 0) return;

#define GET_RECTANGLE_OR_DIE(ID) \
 std::map<size_t, Rectangle>::iterator it = mRectangles.find(ID);\
 if (it == mRectangles.end())return;\
 Rectangle* rect = &(*it).second;

#define GET_LINE_OR_DIE(ID) \
 std::map<size_t, Line>::iterator it = mLines.find(ID);\
 if (it == mLines.end())return;\
 Line* line = &(*it).second;

#define GET_LINE_OR_DIE_TRYING(ID, TRYING) \
 std::map<size_t, Line>::const_iterator it = mLines.find(ID);\
 if (it == mLines.end())return TRYING;\
 const Line* line = &(*it).second;

#define GET_BOB_OR_DIE(ID) \
 std::map<size_t, Bob>::iterator it = mBobs.find(ID);\
 if (it == mBobs.end())return;\
 Bob* bob = &(*it).second;

#define GET_BOB_OR_DIE_TRYING(ID, TRYING) \
 std::map<size_t, Bob>::iterator it = mBobs.find(ID);\
 if (it == mBobs.end())return TRYING;\
 Bob* bob = &(*it).second;

#define GET_CAPTION_OR_DIE(ID) \
 std::map<size_t, Caption>::iterator it = mCaptions.find(ID);\
 if (it == mCaptions.end())return;\
 Caption* caption = &(*it).second;

#define GET_BOX_OR_DIE(ID) \
 std::map<size_t, Box>::iterator it = mBoxes.find(ID);\
 if (it == mBoxes.end())return;\
 Box* box = &(*it).second;

#define GET_CAPTION_OR_DIE_TRYING(ID, TRYING) \
 std::map<size_t, Caption>::const_iterator it = mCaptions.find(ID);\
 if (it == mCaptions.end())return TRYING;\
 const Caption* caption = &(*it).second;

#define PUSH_VERTEX_NOUV(X, Y)                                  \
 tVertex.position.x = X; tVertex.position.y = Y;                \
 tVertex.uv.x = quad.uvLeft; tVertex.uv.y = quad.uvTop;         \
 tVertex.colour = quad.colour[0];                               \
 mVertices.push_back(tVertex);                                   
 

namespace Gorilla
{

static const Ogre::Radian NoRotation = Ogre::Radian();

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
  // canvas x y
  // >  canvas 100 105
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
  
  glyph->uvLeft    = Ogre::StringConverter::parseUnsignedInt(  str_values[0]  );
  glyph->uvTop     = Ogre::StringConverter::parseUnsignedInt(  str_values[1]  );
  glyph->uvWidth   = Ogre::StringConverter::parseUnsignedInt(  str_values[2]  );
  glyph->uvHeight  = Ogre::StringConverter::parseUnsignedInt(  str_values[3]  );
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
 
}

void  TextureAtlas::_calculateCoordinates()
{
 
 for(std::vector<Glyph*>::iterator it = mGlyphs.begin(); it != mGlyphs.end(); it++)
 {
  
  (*it)->uvLeft        *= mInverseTextureSize.x;
  (*it)->uvTop         *= mInverseTextureSize.y;
  (*it)->uvRight       *= mInverseTextureSize.x;
  (*it)->uvBottom      *= mInverseTextureSize.y;
  (*it)->glyphAdvance  *= mGlyphScale;
  (*it)->glyphWidth     = (*it)->uvWidth * 0.999; // Hack
  (*it)->glyphHeight    = (*it)->uvHeight * 0.999; // Hack
  
  for (Ogre::uint i=0;i < (*it)->kerning.size();i++)
   (*it)->kerning[i].kerning *= mGlyphScale;
  
 }
 
 for (std::map<Ogre::String, Sprite*>::iterator it = mSprites.begin(); it != mSprites.end(); it++)
 {
  
  (*it).second->uvRight    = (*it).second->uvLeft + (*it).second->spriteWidth;
  (*it).second->uvBottom   = (*it).second->uvTop  + (*it).second->spriteHeight;
  
  (*it).second->uvLeft    *= mInverseTextureSize.x;
  (*it).second->uvTop     *= mInverseTextureSize.y;
  (*it).second->uvRight   *= mInverseTextureSize.x * 0.999;  // Hack
  (*it).second->uvBottom  *= mInverseTextureSize.y * 0.999;  // Hack
  
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
 
 // OGRE_DELETE Screens.
 for (std::vector<Screen*>::iterator it = mScreens.begin(); it != mScreens.end(); it++)
  OGRE_DELETE (*it);
 
 mScreens.clear();
 
 // OGRE_DELETE Atlases.
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
 
 for (std::vector<Renderable*>::iterator it = mRenderables.begin(); it != mRenderables.end();it++)
  OGRE_DELETE (*it);
 mRenderables.clear();
 
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
 for (std::vector<Renderable*>::iterator it = mRenderables.begin(); it != mRenderables.end();it++)
 {
  mLayers[ (*it)->getLayer() ].push_back( (*it) );
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
  mLayers[id][i]->render(mVertexLayerBuffer[id]);
 }
 
}

void  Screen::layerRedrawRequested(Ogre::uint layer)
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

void  Screen::renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& repeatThisInvocation)
{
 if (queueGroupId == Ogre::RENDER_QUEUE_OVERLAY)
 {
  
  // TODO: Check Viewport size here, if changed then call _redraw all
  
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

Canvas*  Screen::createCanvas(int layer)
{
 Canvas* canvas = OGRE_NEW Canvas(layer, this);
 mRenderables.push_back(canvas);
 mLayers[layer].push_back(canvas);
 mRedrawLayerNeeded[layer] = true;
 mLayerRedrawNeeded = true;
 return canvas;
}

SpriteLayer*  Screen::createSpriteLayer(int layer)
{
 SpriteLayer* sprite_layer = OGRE_NEW SpriteLayer(layer, this);
 mRenderables.push_back(sprite_layer);
 mLayers[layer].push_back(sprite_layer);
 mRedrawLayerNeeded[layer] = true;
 mLayerRedrawNeeded = true;
 return sprite_layer;
}

Text*  Screen::createText(Ogre::Real left, Ogre::Real top, const Ogre::String& initialText, int layer)
{
 Text* text = OGRE_NEW Text(left, top, initialText, layer, this);
 mRenderables.push_back(text);
 mLayers[layer].push_back(text);
 mRedrawLayerNeeded[layer] = true;
 mLayerRedrawNeeded = true;
 return text;
}

void   Screen::destroy(Renderable* renderable)
{
 
 if (renderable == 0)
  return;
 
 mRenderables.erase(std::find(mRenderables.begin(), mRenderables.end(), renderable));
 OGRE_DELETE renderable;
 
}

Renderable::Renderable(Screen* screen, Ogre::uint layer)
: mScreen(screen), mLayer(layer), mRedrawNeeded(false), mIsVisible(true)
{
}

Renderable::~Renderable()
{
 // virtual function.
}

void Renderable::render(buffer<Vertex>& buffer)
{
 
 if (mIsVisible == false)
  return;
 
 if (mRedrawNeeded)
 {
  redraw();
  mRedrawNeeded = false;
 }
 
 for (size_t i=0;i < mVertices.size(); i++)
  buffer.push_back(mVertices[i]);
}

void Renderable::pushQuad(const Quad& quad, const Ogre::Radian& angle)
{
 
 tLeft = quad.left;   tTop = quad.top;
 tRight = quad.right; tBottom = quad.bottom;
  
 tVertex.position.z = 0;
 
 // Left/Bottom
 tVertex.position.x = tLeft; tVertex.position.y = tBottom;
 tVertex.uv.x = quad.uvLeft; tVertex.uv.y = quad.uvBottom;
 tVertex.colour = quad.colour[3];
 mVertices.push_back(tVertex);
 
 // Right/Top
 tVertex.position.x = tRight; tVertex.position.y = tTop;
 tVertex.uv.x = quad.uvRight; tVertex.uv.y = quad.uvTop;
 tVertex.colour = quad.colour[1];
 mVertices.push_back(tVertex);
 
 // Left/Top
 tVertex.position.x = tLeft; tVertex.position.y = tTop;
 tVertex.uv.x = quad.uvLeft; tVertex.uv.y = quad.uvTop;
 tVertex.colour = quad.colour[0];
 mVertices.push_back(tVertex);

 // Left/Bottom
 tVertex.position.x = tLeft; tVertex.position.y = tBottom;
 tVertex.uv.x = quad.uvLeft; tVertex.uv.y = quad.uvBottom;
 tVertex.colour = quad.colour[3];
 mVertices.push_back(tVertex);

 // Right/Bottom
 tVertex.position.x = tRight; tVertex.position.y = tBottom;
 tVertex.uv.x = quad.uvRight; tVertex.uv.y = quad.uvBottom;
 tVertex.colour = quad.colour[2];
 mVertices.push_back(tVertex);

 // Right/Top
 tVertex.position.x = tRight; tVertex.position.y = tTop;
 tVertex.uv.x = quad.uvRight; tVertex.uv.y = quad.uvTop;
 tVertex.colour = quad.colour[1];
 mVertices.push_back(tVertex);
 
 for (size_t i = mVertices.size()-6;i < mVertices.size();i++)
 {
  
  if (angle.valueRadians() != 0)
  {

    tA = mVertices[i].position.x;
    tB = mVertices[i].position.y;
    t1 = tA - quad.left;
    t2 = tB - quad.top;
    mVertices[i].position.x = Ogre::Math::Cos(angle) * t1 - Ogre::Math::Sin(angle) * t2 + quad.left;
    mVertices[i].position.y = Ogre::Math::Sin(angle) * t1 + Ogre::Math::Cos(angle) * t2 + quad.top;
  }

  vpW(mVertices[i].position.x);
  vpH(mVertices[i].position.y);

  if      (mVertices[i].position.x < mMin.x)   mMin.x = mVertices[i].position.x;
  else if (mVertices[i].position.x > mMax.x)   mMax.x = mVertices[i].position.x;
  
  if      (mVertices[i].position.y < mMin.y)   mMin.y = mVertices[i].position.y;
  else if (mVertices[i].position.y > mMax.y)   mMax.y = mVertices[i].position.y;
  
 }
 
}

void  Renderable::pushLine(const Ogre::Vector2& a, const Ogre::Vector2& b, Ogre::Real thickness, const Ogre::ColourValue& colour)
{
 Ogre::Vector2 perp = (b-a).perpendicular().normalisedCopy();

 Ogre::Real halfThickness = thickness * 0.5f;
 
 Ogre::Vector2 aLeft =  a - perp * halfThickness;
 Ogre::Vector2 aRight = a + perp * halfThickness;
 Ogre::Vector2 bLeft =  b - perp * halfThickness;
 Ogre::Vector2 bRight = b + perp * halfThickness;
 
 tVertex.position.z = 0;
 tVertex.uv.x = mScreen->getAtlas()->getRectangleCoordsX(); tVertex.uv.y = mScreen->getAtlas()->getRectangleCoordsY();
 tVertex.colour = colour;
 
 // Left/Bottom
 tVertex.position.x = bLeft.x;  tVertex.position.y = bLeft.y;
 mVertices.push_back(tVertex);
 
 // Right/Top
 tVertex.position.x = aRight.x;  tVertex.position.y = aRight.y;
 mVertices.push_back(tVertex);
 
 // Left/Top
 tVertex.position.x = aLeft.x;  tVertex.position.y = aLeft.y;
 mVertices.push_back(tVertex);
 
 // Left/Bottom
 tVertex.position.x = bLeft.x;  tVertex.position.y = bLeft.y;
 mVertices.push_back(tVertex);
 
 // Right/Bottom
 tVertex.position.x = bRight.x;  tVertex.position.y = bRight.y;
 mVertices.push_back(tVertex);
 
 // Right/Top
 tVertex.position.x = aRight.x;  tVertex.position.y = aRight.y;
 mVertices.push_back(tVertex);
 
 for (size_t i = mVertices.size()-6;i < mVertices.size();i++)
 { 

  if      (mVertices[i].position.x < mMin.x)   mMin.x = mVertices[i].position.x;
  else if (mVertices[i].position.x > mMax.x)   mMax.x = mVertices[i].position.x;
  
  if      (mVertices[i].position.y < mMin.y)   mMin.y = mVertices[i].position.y;
  else if (mVertices[i].position.y > mMax.y)   mMax.y = mVertices[i].position.y;
  
  vpW(mVertices[i].position.x);
  vpH(mVertices[i].position.y);
  
 }
  
  
 
}

void  Renderable::pushSprite(const Ogre::Vector2& position, const Ogre::Vector2& scale, Sprite* sprite, const Ogre::ColourValue& tint)
{
 
 tLeft = position.x;
 tTop = position.y;
 tRight = tLeft + (sprite->spriteWidth * scale.x);
 tBottom = tTop + (sprite->spriteHeight * scale.y);
 
 tVertex.position.z = 0;
 tVertex.colour = tint;

 // Left/Bottom
 tVertex.position.x = tLeft; tVertex.position.y = tBottom;
 tVertex.uv.x = sprite->uvLeft; tVertex.uv.y =sprite->uvBottom;

 mVertices.push_back(tVertex);
 
 // Right/Top
 tVertex.position.x = tRight; tVertex.position.y = tTop;
 tVertex.uv.x = sprite->uvRight; tVertex.uv.y = sprite->uvTop;
 mVertices.push_back(tVertex);
 
 // Left/Top
 tVertex.position.x = tLeft; tVertex.position.y = tTop;
 tVertex.uv.x = sprite->uvLeft; tVertex.uv.y = sprite->uvTop;
 mVertices.push_back(tVertex);

 // Left/Bottom
 tVertex.position.x = tLeft; tVertex.position.y = tBottom;
 tVertex.uv.x = sprite->uvLeft; tVertex.uv.y = sprite->uvBottom;
 mVertices.push_back(tVertex);

 // Right/Bottom
 tVertex.position.x = tRight; tVertex.position.y = tBottom;
 tVertex.uv.x = sprite->uvRight; tVertex.uv.y = sprite->uvBottom;
 mVertices.push_back(tVertex);

 // Right/Top
 tVertex.position.x = tRight; tVertex.position.y = tTop;
 tVertex.uv.x = sprite->uvRight; tVertex.uv.y = sprite->uvTop;
 mVertices.push_back(tVertex);
 
 for (size_t i = mVertices.size()-6;i < mVertices.size();i++)
 { 

  if      (mVertices[i].position.x < mMin.x)   mMin.x = mVertices[i].position.x;
  else if (mVertices[i].position.x > mMax.x)   mMax.x = mVertices[i].position.x;
  
  if      (mVertices[i].position.y < mMin.y)   mMin.y = mVertices[i].position.y;
  else if (mVertices[i].position.y > mMax.y)   mMax.y = mVertices[i].position.y;
  
  vpW(mVertices[i].position.x);
  vpH(mVertices[i].position.y);
 }
  
}

void  Renderable::pushGlyph(Glyph* glyph, Ogre::Real left, Ogre::Real top, const Ogre::ColourValue& colour)
{
 tLeft = left;   tTop = top;
 tRight = left + glyph->glyphWidth; tBottom = top + glyph->glyphHeight;
 
 tVertex.position.z = 0;
 tVertex.colour = colour;
 
 // Left/Bottom
 tVertex.position.x = tLeft; tVertex.position.y = tBottom;
 tVertex.uv.x = glyph->uvLeft; tVertex.uv.y = glyph->uvBottom;
 mVertices.push_back(tVertex);
 
 // Right/Top
 tVertex.position.x = tRight; tVertex.position.y = tTop;
 tVertex.uv.x = glyph->uvRight; tVertex.uv.y = glyph->uvTop;
 mVertices.push_back(tVertex);
 
 // Left/Top
 tVertex.position.x = tLeft; tVertex.position.y = tTop;
 tVertex.uv.x = glyph->uvLeft; tVertex.uv.y = glyph->uvTop;
 mVertices.push_back(tVertex);

 // Left/Bottom
 tVertex.position.x = tLeft; tVertex.position.y = tBottom;
 tVertex.uv.x = glyph->uvLeft; tVertex.uv.y = glyph->uvBottom;
 mVertices.push_back(tVertex);

 // Right/Bottom
 tVertex.position.x = tRight; tVertex.position.y = tBottom;
 tVertex.uv.x = glyph->uvRight; tVertex.uv.y = glyph->uvBottom;
 mVertices.push_back(tVertex);

 // Right/Top
 tVertex.position.x = tRight; tVertex.position.y = tTop;
 tVertex.uv.x = glyph->uvRight; tVertex.uv.y = glyph->uvTop;
 mVertices.push_back(tVertex);
 
 for (size_t i = mVertices.size()-6;i < mVertices.size();i++)
 { 

  if      (mVertices[i].position.x < mMin.x)   mMin.x = mVertices[i].position.x;
  else if (mVertices[i].position.x > mMax.x)   mMax.x = mVertices[i].position.x;
  
  if      (mVertices[i].position.y < mMin.y)   mMin.y = mVertices[i].position.y;
  else if (mVertices[i].position.y > mMax.y)   mMax.y = mVertices[i].position.y;
  
  vpW(mVertices[i].position.x);
  vpH(mVertices[i].position.y);
  
 }
  
 
}

void  Renderable::pushBox(const Quad& quad)
{
 
 tLeft = quad.left;   tTop = quad.top;
 tRight = quad.right; tBottom = quad.bottom;
  
 tVertex.position.z = 0;
 
 // A - B
 PUSH_VERTEX_NOUV(tLeft, tTop);
 PUSH_VERTEX_NOUV(tRight, tTop-1);
 PUSH_VERTEX_NOUV(tLeft, tTop-1);
 
 PUSH_VERTEX_NOUV(tLeft, tTop);
 PUSH_VERTEX_NOUV(tRight, tTop);
 PUSH_VERTEX_NOUV(tRight, tTop-1);

 //       B
 //       |
 //       C

 PUSH_VERTEX_NOUV(tRight, tTop-1);
 PUSH_VERTEX_NOUV(tRight+1, tTop-1);
 PUSH_VERTEX_NOUV(tRight, tBottom);
 
 PUSH_VERTEX_NOUV(tRight, tBottom);
 PUSH_VERTEX_NOUV(tRight+1, tBottom);
 PUSH_VERTEX_NOUV(tRight+1, tTop-1);
 
 //  D - C

 PUSH_VERTEX_NOUV(tLeft, tBottom+1);
 PUSH_VERTEX_NOUV(tRight+1, tBottom);
 PUSH_VERTEX_NOUV(tLeft, tBottom);
 
 PUSH_VERTEX_NOUV(tLeft, tBottom+1);
 PUSH_VERTEX_NOUV(tRight+1, tBottom+1);
 PUSH_VERTEX_NOUV(tRight+1, tBottom);
 
 //  A
 //  |
 //  D
 
 PUSH_VERTEX_NOUV(tLeft-1, tBottom+1);
 PUSH_VERTEX_NOUV(tLeft, tTop-1);
 PUSH_VERTEX_NOUV(tLeft-1, tTop-1);
 
 PUSH_VERTEX_NOUV(tLeft-1, tBottom+1);
 PUSH_VERTEX_NOUV(tLeft, tBottom+1);
 PUSH_VERTEX_NOUV(tLeft, tTop-1);
 
 
 for (size_t i = mVertices.size()-(4*6);i < mVertices.size();i++)
 {
  
  vpW(mVertices[i].position.x);
  vpH(mVertices[i].position.y);

  if      (mVertices[i].position.x < mMin.x)   mMin.x = mVertices[i].position.x;
  else if (mVertices[i].position.x > mMax.x)   mMax.x = mVertices[i].position.x;
  
  if      (mVertices[i].position.y < mMin.y)   mMin.y = mVertices[i].position.y;
  else if (mVertices[i].position.y > mMax.y)   mMax.y = mVertices[i].position.y;
  
 }
}

Canvas::Canvas(Ogre::uint layer, Screen* screen)
: Renderable(screen, layer), mNextRectangleID(0), mNextLineID(0), mNextCaptionID(0), mNextBoxID(0)
{
}

Canvas::~Canvas()
{
 // std::maps should take take of destructing Rectangle/Lines since they aren't pointers.
}

void  Canvas::redraw()
{
 mVertices.remove_all();
 for (std::map<size_t, Rectangle>::const_iterator it = mRectangles.begin(); it != mRectangles.end(); it++)
  pushQuad((*it).second.quad, (*it).second.angle);
 for (std::map<size_t, Box>::iterator it = mBoxes.begin(); it != mBoxes.end(); it++)
  pushBox((*it).second.quad);
 for (std::map<size_t, Line>::const_iterator it = mLines.begin(); it != mLines.end(); it++)
  pushLine((*it).second.a, (*it).second.b,(*it).second.thickness, (*it).second.colour);
 for (std::map<size_t, Caption>::iterator it = mCaptions.begin(); it != mCaptions.end(); it++)
  _drawCaption(&(*it).second);
}


size_t Canvas::addRectangle(Ogre::Real left, Ogre::Real top, Ogre::Real width, Ogre::Real height, const Ogre::ColourValue& colour)
{
 Rectangle rect;
 rect.id = mNextRectangleID++;
 rect.quad.left = left;
 rect.quad.top = top;
 rect.quad.right = left + width;
 rect.quad.bottom = top + height;
 rect.quad.colour[0] = 
 rect.quad.colour[1] = 
 rect.quad.colour[2] = 
 rect.quad.colour[3] = colour;
 rect.quad.uvLeft = rect.quad.uvRight = mScreen->getAtlas()->getRectangleCoordsX();
 rect.quad.uvTop = rect.quad.uvBottom = mScreen->getAtlas()->getRectangleCoordsY();
 mRectangles[rect.id] = rect;
 
 mRedrawNeeded = true;
 requestLayerRedraw();
 return rect.id;
}

void Canvas::removeRectangle(size_t id)
{
 
 std::map<size_t, Rectangle>::iterator it = mRectangles.find(id);
 
 if (it == mRectangles.end())
  return;
 
 mRectangles.erase(it);
 mRedrawNeeded = true;
 requestLayerRedraw();
 
}

void Canvas::setRectangleColour(size_t id, const Ogre::ColourValue& colour)
{
 GET_RECTANGLE_OR_DIE(id)
 rect->quad.colour[0] = 
 rect->quad.colour[1] = 
 rect->quad.colour[2] =
 rect->quad.colour[3] = colour;
 _redrawNeeded();
}

void Canvas::setRectangleColour(size_t id, const Ogre::ColourValue& topLeft, const Ogre::ColourValue& topRight,const Ogre::ColourValue& bottomRight,const Ogre::ColourValue& bottomLeft)
{
 
 GET_RECTANGLE_OR_DIE(id);
 
 rect->quad.colour[0] = topLeft;
 rect->quad.colour[1] = topRight;
 rect->quad.colour[2] = bottomRight;
 rect->quad.colour[3] = bottomLeft;
 
 _redrawNeeded();
 
}

void  Canvas::setRectanglePosition(size_t id, Ogre::Real left, Ogre::Real top)
{
 GET_RECTANGLE_OR_DIE(id);
 tA = rect->quad.right - rect->quad.left;
 tB = rect->quad.bottom - rect->quad.top;
 rect->quad.left = left;
 rect->quad.top = top;
 rect->quad.right = left + tA;
 rect->quad.bottom = top + tB;
 _redrawNeeded();
}

void  Canvas::setRectangleSize(size_t id, Ogre::Real width, Ogre::Real height)
{
 GET_RECTANGLE_OR_DIE(id);
 rect->quad.right = rect->quad.left + width;
 rect->quad.bottom = rect->quad.top + height;
 _redrawNeeded(); 
}


void  Canvas::setRectangleBackground(size_t id, const Ogre::String& sprite_name, bool resetColour)
{
 
 GET_RECTANGLE_OR_DIE(id);
 
 GET_SPRITE_OR_DIE(sprite_name);

 rect->quad.uvLeft = sprite->uvLeft;
 rect->quad.uvTop = sprite->uvTop;
 rect->quad.uvBottom = sprite->uvBottom;
 rect->quad.uvRight = sprite->uvRight;

 if (resetColour)
 {
  rect->quad.colour[0] = 
  rect->quad.colour[1] = 
  rect->quad.colour[2] = 
  rect->quad.colour[3] = Ogre::ColourValue::White;
 }

 _redrawNeeded(); 
}

void  Canvas::clearRectangleBackground(size_t id)
{

 GET_RECTANGLE_OR_DIE(id);
 
 rect->quad.uvLeft = rect->quad.uvRight = mScreen->getAtlas()->getRectangleCoordsX();
 rect->quad.uvTop = rect->quad.uvBottom = mScreen->getAtlas()->getRectangleCoordsY();
 
 _redrawNeeded(); 
}

void  Canvas::setRectangleAngle(size_t id, const Ogre::Degree& angle)
{
 setRectangleAngle(id, Ogre::Radian(angle.valueRadians()));
}

void  Canvas::setRectangleAngle(size_t id, const Ogre::Radian& angle)
{
 GET_RECTANGLE_OR_DIE(id);
 rect->angle = angle;
 _redrawNeeded();  
}

void  Canvas::setRectangleMinMax(size_t id, const Ogre::Vector2& min, const Ogre::Vector2& max)
{
 GET_RECTANGLE_OR_DIE(id);
 rect->quad.left = min.x;
 rect->quad.top = min.y;
 rect->quad.right = max.x;
 rect->quad.bottom = max.y;
 _redrawNeeded();
}

size_t Canvas::addLine(Ogre::Real x1, Ogre::Real y1, Ogre::Real x2, Ogre::Real y2, Ogre::Real thickness, const Ogre::ColourValue& colour)
{
 Line line;
 line.id = mNextLineID++;
 line.a.x = x1;
 line.a.y = y1;
 line.b.x = x2;
 line.b.y = y2;
 line.thickness = thickness;
 line.colour = colour;
 
 mLines[line.id] = line;
 
 mRedrawNeeded = true;
 requestLayerRedraw();
 return line.id;
}

void  Canvas::removeLine(size_t id)
{
 
 std::map<size_t, Line>::iterator it = mLines.find(id);
 if (it == mLines.end())
  return;
 
 mLines.erase(it);
 _redrawNeeded();
}

void  Canvas::setLineColour(size_t id, const Ogre::ColourValue& colour)
{
 GET_LINE_OR_DIE(id)
 line->colour = colour;
 _redrawNeeded();
}

void  Canvas::setLineCoords(size_t id, Ogre::Real x1, Ogre::Real y1, Ogre::Real x2, Ogre::Real y2)
{
 GET_LINE_OR_DIE(id)
 line->a.x = x1;
 line->a.y = y1;
 line->b.x = x2;
 line->b.y = y2;
 _redrawNeeded();
}

void  Canvas::setLineCoords(size_t id, const Ogre::Vector4& coords)
{
 GET_LINE_OR_DIE(id)
 line->a.x = coords.x;
 line->a.y = coords.y;
 line->b.x = coords.z;
 line->b.y = coords.w;
 _redrawNeeded();
}

void  Canvas::setLineOrigin(size_t id, Ogre::Real x1, Ogre::Real y1)
{
 GET_LINE_OR_DIE(id)
 line->a.x = x1;
 line->a.y = y1;
 _redrawNeeded();
}

void  Canvas::setLineEnd(size_t id, Ogre::Real x2, Ogre::Real y2)
{
 GET_LINE_OR_DIE(id)
 line->b.x = x2;
 line->b.y = y2;
 _redrawNeeded();
}

void  Canvas::setLineThickness(size_t id, Ogre::Real thickness)
{
 GET_LINE_OR_DIE(id)
 line->thickness = thickness;
 _redrawNeeded();
}

Ogre::ColourValue Canvas::getLineColour(size_t id) const
{
 GET_LINE_OR_DIE_TRYING(id, Ogre::ColourValue::White);
 return line->colour;
}

Ogre::Vector4  Canvas::getLineCoords(size_t id) const
{
 GET_LINE_OR_DIE_TRYING(id, Ogre::Vector4(0,0,0,0) );
 return Ogre::Vector4(line->a.x, line->a.y, line->b.x, line->b.y);
}

Ogre::Real  Canvas::getLineThickness(size_t id) const
{
 GET_LINE_OR_DIE_TRYING(id, 0.0f);
 return line->thickness;
}



size_t  Canvas::addBox(Ogre::Real left, Ogre::Real top, Ogre::Real width, Ogre::Real height, const Ogre::ColourValue& colour)
{
 Box box;
 box.id = mNextBoxID++;
 box.quad.left = left;
 box.quad.top = top;
 box.quad.right = left + width;
 box.quad.bottom = top + height;
 box.quad.colour[0] = 
 box.quad.colour[1] = 
 box.quad.colour[2] = 
 box.quad.colour[3] = colour;
 box.quad.uvLeft = box.quad.uvRight = mScreen->getAtlas()->getRectangleCoordsX();
 box.quad.uvTop = box.quad.uvBottom = mScreen->getAtlas()->getRectangleCoordsY();
 mBoxes[box.id] = box;
 
 _redrawNeeded();
 return box.id;
}

void  Canvas::removeBox(size_t id)
{
 
 std::map<size_t, Box>::iterator it = mBoxes.find(id);
 
 if (it == mBoxes.end())
  return;
 
 mBoxes.erase(it);
 _redrawNeeded();
 
}

void  Canvas::setBoxPosition(size_t id, Ogre::Real left, Ogre::Real top)
{
 
 GET_BOX_OR_DIE(id)
 Ogre::Real width = box->quad.right - box->quad.left;
 Ogre::Real height = box->quad.bottom - box->quad.top;
 box->quad.left = left;
 box->quad.top = top;
 box->quad.right = left + width;
 box->quad.bottom = top + height;
 
 _redrawNeeded();
}

void  Canvas::setBoxSize(size_t id, Ogre::Real width, Ogre::Real height)
{
 
 GET_BOX_OR_DIE(id)
 box->quad.right = box->quad.left + width;
 box->quad.bottom = box->quad.top + height;
 
 _redrawNeeded();
}

void  Canvas::setBoxColour(size_t id, const Ogre::ColourValue& colour)
{
 
 GET_BOX_OR_DIE(id)
 box->quad.colour[0] = colour;
 
 _redrawNeeded();
}

size_t Canvas::addCaption(Ogre::Real left, Ogre::Real top, const Ogre::String& text, const Ogre::ColourValue& colour)
{
 Caption caption;
 caption.id = mNextRectangleID++;
 caption.left = left;
 caption.top = top;
 caption.text = text;
 caption.width = 0;
 caption.height = 0;
 caption.horizontalClip = -1;
 caption.colour = colour;
 mCaptions[caption.id] = caption;
 
 _redrawNeeded();
 return caption.id;
}

void   Canvas::removeCaption(size_t id)
{
 
 std::map<size_t, Caption>::iterator it = mCaptions.find(id);
 
 if (it == mCaptions.end())
  return;
 
 mCaptions.erase(it);
 _redrawNeeded();
}

void   Canvas::setCaptionPosition(size_t id, Ogre::Real left, Ogre::Real top)
{
 GET_CAPTION_OR_DIE(id)
 caption->left = left;
 caption->top = top;
 _redrawNeeded();
}

void   Canvas::setCaptionText(size_t id, const Ogre::String& text)
{
 GET_CAPTION_OR_DIE(id)
 caption->text = text;
 _redrawNeeded();
}

void   Canvas::setCaptionHorizontalClip(size_t id, Ogre::Real clip)
{
 GET_CAPTION_OR_DIE(id)
 caption->horizontalClip = clip;
 _redrawNeeded();
}

void  Canvas::setCaptionColour(size_t id, const Ogre::ColourValue& colour)
{
 GET_CAPTION_OR_DIE(id)
 caption->colour = colour;
 _redrawNeeded();
}

Ogre::Vector2 Canvas::getCaptionSize(size_t id)
{
 std::map<size_t, Caption>::iterator it = mCaptions.find(id);
 if (it == mCaptions.end())return Ogre::Vector2::ZERO;
 Caption* caption = &(*it).second;

 if (caption->width == 0 && caption->text.length() != 0)
  _calculateCaptionSize(caption);
 
 return Ogre::Vector2(caption->width, caption->height);
}

Ogre::Vector2 Canvas::getCaptionPosition(size_t id) const
{
 GET_CAPTION_OR_DIE_TRYING(id, Ogre::Vector2::ZERO);
 return Ogre::Vector2(caption->left, caption->top);
}

Ogre::String  Canvas::getCaptionString(size_t id) const
{
 GET_CAPTION_OR_DIE_TRYING(id, Ogre::StringUtil::BLANK);
 return caption->text;
}

Ogre::ColourValue Canvas::getCaptionColour(size_t id) const
{
 GET_CAPTION_OR_DIE_TRYING(id, Ogre::ColourValue::White);
 return caption->colour;
}

void Canvas::_drawCaption(Caption* caption)
{
 tA = caption->left;
 tB = 0;
 Glyph* glyph = 0;
 unsigned char thisChar = 0, lastChar = 0;
 caption->width = 0;
 caption->height = mScreen->getAtlas()->getGlyphLineHeight();

 for (size_t i = 0; i < caption->text.size();i++)
 {
  
  
  thisChar = caption->text[i];
  
  if (thisChar == ' ')
  {
   tA += mScreen->getAtlas()->getGlyphSpaceLength();
   lastChar = thisChar;
   continue;
  }
  
  glyph = mScreen->getAtlas()->getGlyph(thisChar);
  if (glyph == 0)
   continue;
  
  tB = glyph->getKerning(lastChar);
  if (tB == 0)
   tB = mScreen->getAtlas()->getGlyphKerning();
  
  if (caption->horizontalClip >= 0)
  {
   if (tA + glyph->glyphWidth >= caption->horizontalClip)
    break;
  }

  pushGlyph(glyph, tA + tB, caption->top, caption->colour);
  tA += glyph->glyphAdvance + tB;
  
  caption->width = tA - caption->left;
  
  lastChar = thisChar;
 }
 
}

void  Canvas::_calculateCaptionSize(Caption* caption)
{
 tA = 0;
 tB = 0;
 caption->width = 0;
 caption->height = mScreen->getAtlas()->getGlyphLineHeight();
 unsigned char thisChar = 0, lastChar = 0;
 Glyph* glyph = 0;
 
 for (size_t i = 0; i < caption->text.size();i++)
 {
  
  thisChar = caption->text[i];
  
  if (thisChar == ' ')
  {
   tA += mScreen->getAtlas()->getGlyphSpaceLength();
   lastChar = thisChar;
   continue;
  }
  glyph = mScreen->getAtlas()->getGlyph(thisChar);
  if (glyph == 0)
   continue;

  tB = glyph->getKerning(lastChar);
  if (tB == 0)
   tB = mScreen->getAtlas()->getGlyphKerning();
  
  if (caption->horizontalClip >= 0)
  {
   if (tA + glyph->glyphWidth >= caption->horizontalClip)
    break;
  }
  
  tA += glyph->glyphAdvance + tB;
  
  caption->width = tA;
  
  lastChar = thisChar;
 }
 
}

SpriteLayer::SpriteLayer(Ogre::uint layer, Screen* screen)
: Renderable(screen, layer), mNextBobID(0)
{
}

SpriteLayer::~SpriteLayer()
{
 // Nothing to OGRE_DELETE!
}

void SpriteLayer::redraw()
{
 
 reset();
 
 for (std::map<size_t, Bob>::const_iterator it = mBobs.begin(); it != mBobs.end(); it++)
  pushSprite((*it).second.position, (*it).second.scale, (*it).second.sprite, (*it).second.tint);
}

size_t SpriteLayer::addSprite(Ogre::Real left, Ogre::Real top, const Ogre::String& name)
{
 Sprite* sprite = mScreen->getAtlas()->getSprite(name);
 if (sprite == 0)
  return -1;
 
 Bob bob;
 bob.sprite = sprite;
 bob.position.x = left;
 bob.position.y = top;
 bob.scale.x = 1;
 bob.scale.y = 1;
 bob.id = mNextBobID++;
 
 mBobs[bob.id] = bob;
 
 mRedrawNeeded = true;
 requestLayerRedraw();
 return bob.id;
}

void SpriteLayer::removeSprite(size_t id)
{
 
 std::map<size_t, Bob>::iterator it = mBobs.find(id);
 
 if (it == mBobs.end())
  return;
 
 mBobs.erase(it);
 mRedrawNeeded = true;
 requestLayerRedraw();
}

void SpriteLayer::setSpritePosition(size_t id, Ogre::Real left, Ogre::Real top)
{
 GET_BOB_OR_DIE(id)
 bob->position.x = left;
 bob->position.y = top;
 _redrawNeeded();
}

void SpriteLayer::setSprite(size_t id, const Ogre::String& name)
{
 GET_BOB_OR_DIE(id);
 if (Sprite* sprite = mScreen->getAtlas()->getSprite(name))
 {
  bob->sprite = sprite;
  _redrawNeeded();
 }
}

void SpriteLayer::setSpriteScale(size_t id, Ogre::Real scaleX, Ogre::Real scaleY)
{
 GET_BOB_OR_DIE(id)
 
 bob->scale.x = scaleX;
 bob->scale.y = scaleY;
 
 _redrawNeeded();
}

Ogre::Vector2 SpriteLayer::getSpritePosition(size_t id)
{
 GET_BOB_OR_DIE_TRYING(id, Ogre::Vector2::ZERO)
 return Ogre::Vector2(bob->position.x, bob->position.y);
}

Ogre::Vector2  SpriteLayer::getSpriteScale(size_t id)
{
 GET_BOB_OR_DIE_TRYING(id, Ogre::Vector2::ZERO)
 return bob->scale;
}

Sprite* SpriteLayer::getSprite(size_t id)
{
 GET_BOB_OR_DIE_TRYING(id, 0);
 return bob->sprite;
}

Text::Text(Ogre::Real left, Ogre::Real top, const Ogre::String& initialText, Ogre::uint layer, Screen* screen)
: Renderable(screen, layer)
{
 mLeft = left;
 mTop = top;
 mText = initialText;
 mDoFormatting = true;
 mDoMonospace = false;
 mColour = Ogre::ColourValue::White;
 _redrawNeeded();
}

Text::~Text()
{
 // Nothing to OGRE_DELETE!
}

void  Text::redraw()
{
 
 reset();
 
 if (mText.length() == 0)
  return;
 
 t1 = mLeft;
 t2 = mTop;
 
 tMonospace = mDoMonospace;
 tWorkingColour = mColour;
 unsigned char thisChar = 0, lastChar = 0;
 tB = mScreen->getAtlas()->getGlyphLineHeight();
 
 for (size_t index=0;index < mText.size(); index++)
 {
  
  thisChar = mText[index];
  
  // Filter out other non-wanted characters.

  
  // TODO: Call markup function here, if does markup. Then check to see if returns false, then break.
  if (mDoFormatting)
  {
   
   _doMarkup(index);
   
   if (index > mText.size() - 1)
    break;
   
   thisChar = mText[index];
  }
  
  // Space.
  if (thisChar == ' ')
  {
   if (tMonospace)
    t1 += mScreen->getAtlas()->getGlyphMonoWidth();
   else
    t1 += mScreen->getAtlas()->getGlyphSpaceLength();
   lastChar = thisChar; 
   continue;
  }
  
  // Newline.
  if (thisChar == '\n')
  {
   t1 = mLeft;
   t2 += tB;
   lastChar = thisChar;
   tB = mScreen->getAtlas()->getGlyphLineHeight();
   continue;
  }
  
  if (thisChar < mScreen->getAtlas()->getGlyphRangeBegin() || thisChar > mScreen->getAtlas()->getGlyphRangeEnd())
   continue;
  
  tGlyph = mScreen->getAtlas()->getGlyph(thisChar);
  if (tGlyph == 0)
   continue;
  
  // Calculate Kerning.
  if (tMonospace)
  {
   tA = 0;
  }
  else
  {
   tA = tGlyph->getKerning(lastChar);
   if (tA == 0)
    tA = mScreen->getAtlas()->getGlyphKerning();
  }
    
  pushGlyph(tGlyph, t1 + tA, t2, tWorkingColour);
  
  if (tMonospace)
   t1 += mScreen->getAtlas()->getGlyphMonoWidth();
  else
   t1 += tGlyph->glyphAdvance + tA;
  

  lastChar = thisChar;
  
 }
 
}

void   Text::_doMarkup(size_t& index)
{
 
 unsigned char thisChar = mText[index];
 unsigned char nextChar = 0;
 
 if (thisChar < mScreen->getAtlas()->getGlyphRangeBegin() || thisChar > mScreen->getAtlas()->getGlyphRangeEnd())
  return;
 
 if (index <= mText.size() -1)
  nextChar = mText[index+1];
 
 if (thisChar != '%')
  return;
 
 // Escape code for '%'
 if (nextChar == '%')
 {
  index++;
  return;
 }
 
 // Reset Colour
 //  %RColour Reset!
 if (nextChar == 'r' || nextChar == 'R')
 {
  tWorkingColour = mColour;
  index+=2;
  return;
 }
 
 // Palette Colours.
 //  %0Using Colour 0
 if (nextChar >= '0' && nextChar <= '9')
 {
  tWorkingColour = mScreen->getAtlas()->getMarkupColour(nextChar - 48);
  index+=2;
  return;
 }
 
 // Monospace.
 //  %MMonospaced Text%M
 if (nextChar == 'M' || nextChar == 'm')
 {
  tMonospace = !tMonospace;
  index+=2;
  return;
 }
 
 // Sprites.
 //  %:this_is_the_name_of_my_sprite%
 if (nextChar == ':')
 {
  size_t j = index + 2;
  bool hasSprite = false;
  for (;j < mText.size();j++)
  {
   
   if (mText[j] == '%')
   {
    hasSprite = true;
    break;
   }
   
  }

  if (!hasSprite)
   return;
  
  Ogre::String sprite_name = mText.substr(index + 2, j - index - 2);
  Sprite* sprite = mScreen->getAtlas()->getSprite(sprite_name);
  if (sprite == 0)
   return;

  if (sprite->spriteHeight > tB)
   tB = sprite->spriteHeight;
  
  pushSprite(Ogre::Vector2(t1, t2), Ogre::Vector2(1,1), sprite, Ogre::ColourValue::White);
  
  t1 += sprite->spriteWidth + (0 - mScreen->getAtlas()->getGlyphKerning());
  index += sprite_name.length() + 3; // %:%
  
  return;
  
  
 }
 
}



} // namespace Gorilla

#undef GET_SPRITE_OR_DIE
#undef GET_RECTANGLE_OR_DIE
#undef GET_LINE_OR_DIE
#undef GET_LINE_OR_DIE_TRYING
#undef GET_BOB_OR_DIE
#undef GET_BOB_OR_DIE_TRYING
#undef GET_CAPTION_OR_DIE
#undef GET_BOX_OR_DIE
#undef GET_CAPTION_OR_DIE_TRYING
#undef PUSH_VERTEX_NOUV
