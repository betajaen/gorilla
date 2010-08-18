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


template<>   Gorilla::System*   Ogre::Singleton<Gorilla::System>::ms_Singleton = 0;


namespace Gorilla
{

/////////////////////////////////////////////////////////////////////////////////

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
  // rectangle x y
  // >  rectangle 100 105
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
   mGlyphLineHeight = Ogre::StringConverter::parseInt(data);
   continue;
  }
  // spacelength spacelength
  // >  spacelength 3
  else if (name == "spacelength")
  {
   mGlyphSpaceLength = Ogre::StringConverter::parseInt(data);
   continue;
  }
  // spacelength spacelength
  // >  spacelength 3
  else if (name == "baseline")
  {
   mGlyphBaseline = Ogre::StringConverter::parseInt(data);
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
   mGlyphAllGlyphsKerning = Ogre::StringConverter::parseInt(data);
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
  
  std::cout << "Kerning: " << left_glyph_id << right_glyph_id << " kerning\n";
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
 mPass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
 mPass->setDepthCheckEnabled(false);
 mPass->setDepthWriteEnabled(false);
 mPass->setLightingEnabled(false);

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
  (*it)->glyphWidth     = (*it)->uvWidth * mGlyphScale;
  (*it)->glyphHeight    = (*it)->uvHeight * mGlyphScale;
  
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
  
 }
 
}

/////////////////////////////////////////////////////////////////////////////////

System::System()
{
}

System::~System()
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

void System::loadAtlas(const Ogre::String &name, const Ogre::String &group)
{
 TextureAtlas* atlas = new TextureAtlas(name + ".gorilla", group);
 mAtlases[name] = atlas;
}

Screen* System::createScreen(Ogre::Viewport* viewport, const Ogre::String& atlas_name)
{
 TextureAtlas* atlas = (*mAtlases.find(atlas_name)).second;
 Screen* screen = new Screen(viewport, this, atlas);
 mScreens.push_back(screen);
 return screen;
}


Screen::Screen(Ogre::Viewport* viewport, System* system, TextureAtlas* atlas)
: mViewport(viewport),
  mSystem(system),
  mAtlas(atlas),
  mSceneMgr(viewport->getCamera()->getSceneManager()),
  mRenderSystem(0),
  mMaxVertexCount(0),
  mWriteVertex(0),
  mNextRectangleID(0),
  mNextSpriteID(0),
  mNextTextID(0),
  mRefreshRequired(false)
{

 mRenderSystem = Ogre::Root::getSingletonPtr()->getRenderSystem();

 mViewportWidth = mViewport->getActualWidth();
 mViewportHeight = mViewport->getActualHeight();
 mInvViewportWidth = 1.0f / mViewportWidth;
 mInvViewportHeight = 1.0f / mViewportHeight;

 _createVertexBuffer();
 _startListening();
}

Screen::~Screen()
{
 _stopListening();
 _destroyVertexBuffer();
}


size_t Screen::createRectangle(int left, int top, int width, int height, const Ogre::ColourValue& colour, int depth)
{
 Rectangle* rect = new Rectangle();
 rect->left = left;
 rect->top = top;
 rect->width = width;
 rect->height = height;
 rect->colour = colour;
 rect->depth = depth;
 
 size_t id = mNextRectangleID++;
 mRectangles[id] = rect;
 
 _drawRectangle(rect);
 mRefreshRequired = true;
 
 return id;
}

size_t Screen::createSprite(int left, int top, const Ogre::String& sprite_name, int depth)
{
 ::Gorilla::Screen::Sprite* sprite = new ::Gorilla::Screen::Sprite();
 sprite->left = left;
 sprite->top = top;
 sprite->depth = depth;
 sprite->sprite = mAtlas->getSprite(sprite_name);
 
 size_t id = mNextSpriteID++;
 mSprites[id] = sprite;
 
 _drawSprite(sprite);
 mRefreshRequired = true;
 
 return id;
}

size_t Screen::createText(int left, int top, const Ogre::String& caption, const Ogre::ColourValue& colour, int depth)
{
 ::Gorilla::Screen::Text* text = new ::Gorilla::Screen::Text();
 text->left = left;
 text->top = top;
 text->depth = depth;
 text->text = caption;
 text->colour = colour;
 
 size_t id = mNextTextID++;
 mText[id] = text;
 
 _drawText(text);
 mRefreshRequired = true;
 
 return id;
}

void Screen::_createVertexBuffer()
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

void Screen::_destroyVertexBuffer()
{
 delete mRenderOp.vertexData;
 mRenderOp.vertexData = 0;
 mVertexBuffer.setNull();
 mMaxVertexCount = 0;
}

void  Screen::_prepareVertexBuffer(size_t neededVertices)
{
 
 if (mMaxVertexCount == 0)
  _createVertexBuffer();
 
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
  std::cout << "Resized buffer to " << newVertexBufferSize << "\n";
 }
 
}

void Screen::refresh()
{
 
 mRefreshRequired = false;
 
 if (mRectangles.size() == 0 && mSprites.size() == 0)
  return;
 
 size_t vertexBufferSize = mRectangleData.size() + mSpriteData.size()+ mTextData.size();
 std::cout << "Estimated Vertex Buffer Size" << vertexBufferSize << "\n";

 _prepareVertexBuffer(vertexBufferSize);
 
 mRenderOp.vertexData->vertexCount = 0;
 float* writeIterator = (float*) mVertexBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
 
 // Copy Rectangles.
 for (size_t i = 0; i < mRectangleData.size(); i++)
 {
  *writeIterator++ = mRectangleData[i].position.x;
  *writeIterator++ = mRectangleData[i].position.y;
  *writeIterator++ = mRectangleData[i].position.z;
  *writeIterator++ = mRectangleData[i].colour.r;
  *writeIterator++ = mRectangleData[i].colour.g;
  *writeIterator++ = mRectangleData[i].colour.b;
  *writeIterator++ = mRectangleData[i].colour.a;
  *writeIterator++ = mRectangleData[i].uv.x;
  *writeIterator++ = mRectangleData[i].uv.y;
 }

 mRenderOp.vertexData->vertexCount +=  mRectangleData.size();
 
 // Copy Sprites
 for (size_t i = 0; i < mSpriteData.size(); i++)
 {
  *writeIterator++ = mSpriteData[i].position.x;
  *writeIterator++ = mSpriteData[i].position.y;
  *writeIterator++ = mSpriteData[i].position.z;
  *writeIterator++ = mSpriteData[i].colour.r;
  *writeIterator++ = mSpriteData[i].colour.g;
  *writeIterator++ = mSpriteData[i].colour.b;
  *writeIterator++ = mSpriteData[i].colour.a;
  *writeIterator++ = mSpriteData[i].uv.x;
  *writeIterator++ = mSpriteData[i].uv.y;
 }
 
 mRenderOp.vertexData->vertexCount += mSpriteData.size();
 
 // Copy Text
 for (size_t i = 0; i < mTextData.size(); i++)
 {
  *writeIterator++ = mTextData[i].position.x;
  *writeIterator++ = mTextData[i].position.y;
  *writeIterator++ = mTextData[i].position.z;
  *writeIterator++ = mTextData[i].colour.r;
  *writeIterator++ = mTextData[i].colour.g;
  *writeIterator++ = mTextData[i].colour.b;
  *writeIterator++ = mTextData[i].colour.a;
  *writeIterator++ = mTextData[i].uv.x;
  *writeIterator++ = mTextData[i].uv.y;
 }
 
 mRenderOp.vertexData->vertexCount += mTextData.size();

 std::cout << "Vertex Count = " << mRenderOp.vertexData->vertexCount << "\n";

 mVertexBuffer->unlock();
 std::cout << "++ Refreshed\n";
}

void  Screen::_prepareRenderSystem()
{
 
 mRenderSystem->_setWorldMatrix( Ogre::Matrix4::IDENTITY );
 mRenderSystem->_setProjectionMatrix( Ogre::Matrix4::IDENTITY );
 mRenderSystem->_setViewMatrix( Ogre::Matrix4::IDENTITY );
 
 mSceneMgr->_setPass(mAtlas->getPass(), true, false);
 
}

void Screen::_startListening()
{
 mSceneMgr->addRenderQueueListener(this);
}

void Screen::_stopListening()
{
 mSceneMgr->removeRenderQueueListener(this);
}

void Screen::renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& repeatThisInvocation)
{
 
 if (queueGroupId == Ogre::RENDER_QUEUE_OVERLAY)
 {
  
  if (mViewport->getActualWidth() != mViewportWidth || mViewport->getActualHeight() != mViewportHeight)
  {
   mViewportWidth = mViewport->getActualWidth();
   mViewportHeight = mViewport->getActualHeight();
   mInvViewportWidth = 1.0f / mViewportWidth;
   mInvViewportHeight = 1.0f / mViewportHeight;
   
   _redrawAllRectanges();
   _redrawAllSprites();
   
   mRefreshRequired = true;
  }
  
  
  if (mRefreshRequired)
   refresh();
  
  if (mRenderOp.vertexData->vertexCount == 0)
   return; /// Oh dear. Someone forgot to _endUpdate
  
  _prepareRenderSystem();
  mRenderSystem->_render(mRenderOp);
  
 }

}


void Screen::pushQuadFixedUV(Vertex& vertex, VertexList& list, const Ogre::Real& left, const Ogre::Real& top, const Ogre::Real& bottom, const Ogre::Real& right, const Ogre::Real& depth, const Ogre::ColourValue& colour, const Ogre::Real& fixedU, const Ogre::Real& fixedV)
{
 vertex.colour = colour;
 vertex.position.z = depth;
 vertex.uv.x = fixedU;
 vertex.uv.y = fixedV;

 // Triangle A, Vertex 1
 vertex.position.x = left; vertex.position.y = bottom;
 list.push_back(vertex);

 // Triangle A, Vertex 2
 vertex.position.x = right; vertex.position.y = top;
 list.push_back(vertex);

 // Triangle A, Vertex 3
 vertex.position.x = left; vertex.position.y = top;
 list.push_back(vertex);

 // Triangle B, Vertex 4
 vertex.position.x = left; vertex.position.y = bottom;
 list.push_back(vertex);

 // Triangle B, Vertex 5
 vertex.position.x = right; vertex.position.y = bottom;
 list.push_back(vertex);

 // Triangle B, Vertex 6
 vertex.position.x = right; vertex.position.y = top;
 list.push_back(vertex);
 
 
}

void Screen::setQuadFixedUV(Vertex& vertex, size_t begin, VertexList& list, const Ogre::Real& left, const Ogre::Real& top, const Ogre::Real& bottom, const Ogre::Real& right, const Ogre::Real& depth, const Ogre::ColourValue& colour, const Ogre::Real& fixedU, const Ogre::Real& fixedV)
{
 vertex.colour = colour;
 vertex.position.z = depth;
 vertex.uv.x = fixedU;
 vertex.uv.y = fixedV;

 // Triangle A, Vertex 1
 vertex.position.x = left; vertex.position.y = bottom;
 list[begin++] = vertex;

 // Triangle A, Vertex 2
 vertex.position.x = right; vertex.position.y = top;
 list[begin++] = vertex;

 // Triangle A, Vertex 3
 vertex.position.x = left; vertex.position.y = top;
 list[begin++] = vertex;

 // Triangle B, Vertex 4
 vertex.position.x = left; vertex.position.y = bottom;
 list[begin++] = vertex;

 // Triangle B, Vertex 5
 vertex.position.x = right; vertex.position.y = bottom;
 list[begin++] = vertex;

 // Triangle B, Vertex 6
 vertex.position.x = right; vertex.position.y = top;
 list[begin++] = vertex;
 
 
}



void Screen::pushQuad(Vertex& vertex, VertexList& list, const Ogre::Real& left, const Ogre::Real& top, const Ogre::Real& bottom, const Ogre::Real& right, const Ogre::Real& depth, const Ogre::ColourValue& colour, const Ogre::Real& uvLeft, const Ogre::Real& uvTop, const Ogre::Real& uvRight, const Ogre::Real& uvBottom)
{
 vertex.colour = colour;
 vertex.position.z = depth;

 // Triangle A, Vertex 1
 vertex.position.x = left; vertex.position.y = bottom;
 vertex.uv.x = uvLeft; vertex.uv.y = uvBottom;
 list.push_back(vertex);

 // Triangle A, Vertex 2
 vertex.position.x = right; vertex.position.y = top;
 vertex.uv.x = uvRight; vertex.uv.y = uvTop;
 list.push_back(vertex);

 // Triangle A, Vertex 3
 vertex.position.x = left; vertex.position.y = top;
 vertex.uv.x = uvLeft; vertex.uv.y = uvTop;
 list.push_back(vertex);

 // Triangle B, Vertex 4
 vertex.position.x = left; vertex.position.y = bottom;
 vertex.uv.x = uvLeft; vertex.uv.y = uvBottom;
 list.push_back(vertex);

 // Triangle B, Vertex 5
 vertex.position.x = right; vertex.position.y = bottom;
 vertex.uv.x = uvRight; vertex.uv.y = uvBottom;
 list.push_back(vertex);

 // Triangle B, Vertex 6
 vertex.position.x = right; vertex.position.y = top;
 vertex.uv.x = uvRight; vertex.uv.y = uvTop;
 list.push_back(vertex);
 
 
}

void Screen::setQuad(Vertex& vertex, size_t begin, VertexList& list, const Ogre::Real& left, const Ogre::Real& top, const Ogre::Real& bottom, const Ogre::Real& right, const Ogre::Real& depth, const Ogre::ColourValue& colour, const Ogre::Real& uvLeft, const Ogre::Real& uvTop, const Ogre::Real& uvRight, const Ogre::Real& uvBottom)
{
 vertex.colour = colour;
 vertex.position.z = depth;

 // Triangle A, Vertex 1
 vertex.position.x = left; vertex.position.y = bottom;
 vertex.uv.x = uvLeft; vertex.uv.y = uvBottom;
 list[begin++] = vertex;

 // Triangle A, Vertex 2
 vertex.position.x = right; vertex.position.y = top;
 vertex.uv.x = uvRight; vertex.uv.y = uvTop;
 list[begin++] = vertex;

 // Triangle A, Vertex 3
 vertex.position.x = left; vertex.position.y = top;
 vertex.uv.x = uvLeft; vertex.uv.y = uvTop;
 list[begin++] = vertex;

 // Triangle B, Vertex 4
 vertex.position.x = left; vertex.position.y = bottom;
 vertex.uv.x = uvLeft; vertex.uv.y = uvBottom;
 list[begin++] = vertex;

 // Triangle B, Vertex 5
 vertex.position.x = right; vertex.position.y = bottom;
 vertex.uv.x = uvRight; vertex.uv.y = uvBottom;
 list[begin++] = vertex;

 // Triangle B, Vertex 6
 vertex.position.x = right; vertex.position.y = top;
 vertex.uv.x = uvRight; vertex.uv.y = uvTop;
 list[begin++] = vertex;
 
}

void  Screen::_redrawAllRectanges()
{
 
 mRectangleData.remove_all();
 for(std::map<size_t, Rectangle*>::iterator it = mRectangles.begin(); it != mRectangles.end(); it++)
  _drawRectangle((*it).second);
 
}

void  Screen::_drawRectangle(Screen::Rectangle* rect)
{
  
 Ogre::Real left, top, right, bottom, depth;

 left = _vpx(rect->left);
 top = _vpy(rect->top);
 right = _vpx(rect->left + rect->width);
 bottom = _vpy(rect->top + rect->height);
 depth = _vpz(rect->depth);
 
 Vertex tempVertex;
 
 rect->vertexStart = mRectangleData.size();
 std::cout << left << " " << top << " " << right << " " << bottom << " " << depth << "\n";
 pushQuadFixedUV(tempVertex, mRectangleData, left, top, bottom, right, depth, rect->colour, mAtlas->getRectangleCoordX(), mAtlas->getRectangleCoordY());
 
 
}

void Screen::_updateRectangle(Screen::Rectangle* rect)
{
 
 Ogre::Real left, top, right, bottom, depth;

 left = _vpx(rect->left);
 top = _vpy(rect->top);
 right = _vpx(rect->left + rect->width);
 bottom = _vpy(rect->top + rect->height);
 depth = _vpz(rect->depth);
 
 Vertex tempVertex;
 
 std::cout << left << " " << top << " " << right << " " << bottom << " " << depth << "\n";
 setQuadFixedUV(tempVertex, rect->vertexStart, mRectangleData,  left, top, bottom, right, depth, rect->colour, mAtlas->getRectangleCoordX(), mAtlas->getRectangleCoordY());
}

void  Screen::_redrawAllSprites()
{
 
 mSpriteData.remove_all();
 
 for(std::map<size_t, Screen::Sprite*>::iterator it = mSprites.begin(); it != mSprites.end(); it++)
  _drawSprite((*it).second);
 
}

void  Screen::_drawSprite(Screen::Sprite* sprite)
{
  
 Ogre::Real left, top, right, bottom, depth;

 left = _vpx(sprite->left);
 top = _vpy(sprite->top);
 right = _vpx(sprite->left + sprite->sprite->spriteWidth);
 bottom = _vpy(sprite->top + sprite->sprite->spriteHeight);
 depth = _vpz(sprite->depth);
 
 Vertex tempVertex;
 
 sprite->vertexStart = mSpriteData.size();
 std::cout << "L:" << left << " T:" << top << ", R:" << right << ", B: " << bottom << ", D:" << depth << "Width: " << sprite->sprite->spriteWidth << ", Height:" << sprite->sprite->spriteHeight << "\n";
 pushQuad(tempVertex, mSpriteData, left, top, bottom, right, depth, Ogre::ColourValue::White, sprite->sprite->uvLeft, sprite->sprite->uvTop, sprite->sprite->uvRight, sprite->sprite->uvBottom);
 
 
}

void Screen::_updateSprite(Screen::Sprite* sprite)
{
 
 Ogre::Real left, top, right, bottom, depth;

 left = _vpx(sprite->left);
 top = _vpy(sprite->top);
 right = _vpx(sprite->left + sprite->sprite->spriteWidth);
 bottom = _vpy(sprite->top + sprite->sprite->spriteHeight);
 depth = _vpz(sprite->depth);
 
 Vertex tempVertex;
 
 std::cout << "L:" << left << " T:" << top << ", R:" << right << ", B: " << bottom << ", D:" << depth << "Width: " << sprite->sprite->spriteWidth << ", Height:" << sprite->sprite->spriteHeight << "\n";
 setQuad(tempVertex, sprite->vertexStart, mSpriteData, left, top, bottom, right, depth, Ogre::ColourValue::White, sprite->sprite->uvLeft, sprite->sprite->uvTop, sprite->sprite->uvRight, sprite->sprite->uvBottom);
}


void  Screen::_redrawAllText()
{
 
 mTextData.remove_all();
 for(std::map<size_t, Text*>::iterator it = mText.begin(); it != mText.end(); it++)
  _drawText((*it).second);
 
}

void  Screen::_drawText(Screen::Text* text)
{
 
 Ogre::Real left, top, right, bottom, depth = _vpz(text->depth), cursorLeft, cursorTop, kerning;
 
 unsigned char thisChar = 0, lastChar = 0;
 
 cursorLeft = text->left;
 cursorTop = text->top;
 
 Glyph* glyph = 0;
 Vertex tempVertex;
 
 for (Ogre::uint i=0;i < text->text.size(); i++)
 {
  
  
  thisChar = text->text[i];
  
  if (thisChar == ' ')
  {
   cursorLeft += mAtlas->getGlyphSpaceLength();
   lastChar = thisChar;
   continue;
  }
  
  if (thisChar == '\n')
  {
   cursorLeft = text->left;
   cursorTop += mAtlas->getGlyphLineHeight();
   lastChar = thisChar;
   continue;   
  }
  
  glyph = mAtlas->getGlyph(thisChar);
  kerning = glyph->getKerning(lastChar);
  
  if (kerning == 0)
   kerning = mAtlas->getGlyphKerning();
  
  left = _vpx(cursorLeft + kerning);
  top = _vpy(cursorTop);
  right = _vpx(cursorLeft + kerning + glyph->glyphWidth);
  bottom = _vpy(cursorTop + glyph->glyphHeight);
  //std::cout << "Glyph Width = " << glyph->glyphWidth << "\n";
  //std::cout << "Glyph Height = " << glyph->glyphHeight << "\n";

  cursorLeft += glyph->glyphAdvance + kerning;
  
  pushQuad(tempVertex, mTextData, left, top, bottom, right, depth, Ogre::ColourValue::White, glyph->uvLeft, glyph->uvTop, glyph->uvRight, glyph->uvBottom);
  
  lastChar = thisChar;
 }
 
}

void Screen::_updateText(Screen::Text* text)
{
 // TODO
}

/////////////////////////////////////////////////////////////////////////////////



} // namespace Gorilla