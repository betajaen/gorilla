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

namespace Gorilla
{

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
  
  Glyph* glyph = new Glyph();
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
  
  Sprite* sprite = new Sprite();
  
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







Silverback::Silverback()
{
}

Silverback::~Silverback()
{
 
 // Delete Screens.
 for (std::vector<Screen*>::iterator it = mScreens.begin(); it != mScreens.end(); it++)
  delete (*it);
 
 mScreens.clear();
 
 // Delete Atlases.
 for (std::map<Ogre::String, TextureAtlas*>::iterator it = mAtlases.begin(); it != mAtlases.end(); it++)
  delete (*it).second;
 mAtlases.clear();
 
}

void Silverback::loadAtlas(const Ogre::String &name, const Ogre::String &group)
{
 TextureAtlas* atlas = new TextureAtlas(name + ".gorilla", group);
 mAtlases[name] = atlas;
}

Screen* Silverback::createScreen(Ogre::Viewport* viewport, const Ogre::String& atlas_name)
{
 TextureAtlas* atlas = (*mAtlases.find(atlas_name)).second;
 Screen* screen = new Screen(viewport, atlas);
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
 // TODO: Delete Renderables
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
 mRenderOp.vertexData = new Ogre::VertexData;
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
 delete mRenderOp.vertexData;
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
 Canvas* canvas = new Canvas(layer, this);
 mRenderables.push_back(canvas);
 mLayers[layer].push_back(canvas);
 mRedrawLayerNeeded[layer] = true;
 mLayerRedrawNeeded = true;
 return canvas;
}


Renderable::Renderable(Screen* screen, Ogre::uint layer)
: mScreen(screen), mLayer(layer), mRedrawNeeded(false)
{
}

Renderable::~Renderable()
{
}

void Renderable::render(buffer<Vertex>& buffer)
{
 
 if (mRedrawNeeded)
 {
  redraw();
  mRedrawNeeded = false;
 }
 
 for (size_t i=0;i < mVertices.size(); i++)
  buffer.push_back(mVertices[i]);
}

void Renderable::pushQuad(const Quad& quad)
{
 
 tLeft = quad.left;   tTop = quad.top;
 tRight = quad.right; tBottom = quad.bottom;
 vpX(tLeft);   vpY(tTop);
 vpW(tRight);  vpH(tBottom);
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

}

Canvas::Canvas(Ogre::uint layer, Screen* screen)
: Renderable(screen, layer), mNextRectangleID(0)
{
}

Canvas::~Canvas()
{
}

size_t Canvas::addRectangle(int left, int top, Ogre::uint width, Ogre::uint height, const Ogre::ColourValue& colour)
{
 Rectangle rect;
 rect.id = mNextRectangleID++;
 rect.quad.left = left;
 rect.quad.top = top;
 rect.quad.right = left + width;
 rect.quad.bottom = top + height;
 rect.quad.colour[0] = rect.quad.colour[1] = rect.quad.colour[2] = rect.quad.colour[3] = colour;
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

 std::map<size_t, Rectangle>::iterator it = mRectangles.find(id);
 
 if (it == mRectangles.end())
  return;
 
 (*it).second.quad.colour[0] = colour;
 (*it).second.quad.colour[1] = colour;
 (*it).second.quad.colour[2] = colour;
 (*it).second.quad.colour[3] = colour;

 mRedrawNeeded = true;
 requestLayerRedraw();
}

void Canvas::setRectangleColour(size_t id, const Ogre::ColourValue& topLeft, const Ogre::ColourValue& topRight,const Ogre::ColourValue& bottomRight,const Ogre::ColourValue& bottomLeft)
{

 std::map<size_t, Rectangle>::iterator it = mRectangles.find(id);
 
 if (it == mRectangles.end())
  return;
 
 (*it).second.quad.colour[0] = topLeft;
 (*it).second.quad.colour[1] = topRight;
 (*it).second.quad.colour[2] = bottomRight;
 (*it).second.quad.colour[3] = bottomLeft;

 mRedrawNeeded = true;
 requestLayerRedraw();
}

void  Canvas::setRectanglePosition(size_t id, int left, int top)
{

 std::map<size_t, Rectangle>::iterator it = mRectangles.find(id);
 
 if (it == mRectangles.end())
  return;
 
 (*it).second.quad.left = left;
 (*it).second.quad.top = top;

 mRedrawNeeded = true;
 requestLayerRedraw();
}

void  Canvas::setRectangleSize(size_t id, Ogre::uint width, Ogre::uint height)
{

 std::map<size_t, Rectangle>::iterator it = mRectangles.find(id);
 
 if (it == mRectangles.end())
  return;
 
 (*it).second.quad.right = (*it).second.quad.left + width;
 (*it).second.quad.bottom = (*it).second.quad.top + height;

 mRedrawNeeded = true;
 requestLayerRedraw();
}


void  Canvas::setRectangleBackground(size_t id, const Ogre::String& sprite_name, bool resetColour)
{

 std::map<size_t, Rectangle>::iterator it = mRectangles.find(id);
 
 if (it == mRectangles.end())
  return;
 
 Sprite* sprite = mScreen->getAtlas()->getSprite(sprite_name);
 
 (*it).second.quad.uvLeft = sprite->uvLeft;
 (*it).second.quad.uvTop = sprite->uvTop;
 (*it).second.quad.uvBottom = sprite->uvBottom;
 (*it).second.quad.uvRight = sprite->uvRight;

 if (resetColour)
 {
  (*it).second.quad.colour[0] = Ogre::ColourValue::White;
  (*it).second.quad.colour[1] = Ogre::ColourValue::White;
  (*it).second.quad.colour[2] = Ogre::ColourValue::White;
  (*it).second.quad.colour[3] = Ogre::ColourValue::White;
 }

 mRedrawNeeded = true;
 requestLayerRedraw();
}

void  Canvas::clearRectangleBackground(size_t id)
{

 std::map<size_t, Rectangle>::iterator it = mRectangles.find(id);
 
 if (it == mRectangles.end())
  return;
 
 (*it).second.quad.uvLeft = (*it).second.quad.uvRight = mScreen->getAtlas()->getRectangleCoordsX();
 (*it).second.quad.uvTop = (*it).second.quad.uvBottom = mScreen->getAtlas()->getRectangleCoordsY();

 mRedrawNeeded = true;
 requestLayerRedraw();
}


void  Canvas::redraw()
{
 mVertices.remove_all();
 for (std::map<size_t, Rectangle>::const_iterator it = mRectangles.begin(); it != mRectangles.end(); it++)
  pushQuad((*it).second.quad);
}


}