#include "Gorilla.h"

#include "OGRE/Ogre.h"

const Ogre::uchar Gorilla::Font8x8::CompressedFont[] = {
   0xff, 0xef, 0xd7, 0xd7, 0xef, 0xff, 0xe7, 0xef, 0xdf, 0xf7, 0xef, 0xef,
   0xff, 0xff, 0xff, 0x7f, 0xc7, 0xef, 0xc7, 0xc7, 0xcf, 0x83, 0xc7, 0x83,
   0xc7, 0xc7, 0xff, 0xff, 0xdf, 0xff, 0xfb, 0xc7, 0xc7, 0xc7, 0xc3, 0xc7,
   0xc3, 0x83, 0x83, 0xc7, 0xbb, 0xc7, 0x8f, 0xbb, 0xf7, 0x7d, 0xbb, 0xc7,
   0xc7, 0xc7, 0xc3, 0xc7, 0x83, 0xbb, 0xbb, 0x7d, 0xbb, 0xbb, 0x83, 0xc7,
   0xfd, 0xc7, 0xef, 0xff, 0xf7, 0xff, 0xf7, 0xff, 0xbf, 0xff, 0xcf, 0xff,
   0xfb, 0xff, 0xff, 0xfb, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xef, 0xf3, 0xff, 0xff,
   0xff, 0xef, 0xd7, 0xd7, 0x87, 0xb3, 0xeb, 0xef, 0xef, 0xef, 0x6d, 0xef,
   0xff, 0xff, 0xff, 0xbf, 0xbb, 0xe7, 0xbb, 0xbb, 0xd7, 0xfb, 0xbb, 0xbf,
   0xbb, 0xbb, 0xcf, 0xcf, 0xef, 0xff, 0xf7, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb,
   0xbb, 0xfb, 0xfb, 0xbb, 0xbb, 0xef, 0xdf, 0xbb, 0xf7, 0x39, 0xb3, 0xbb,
   0xb7, 0xbb, 0xbb, 0xbb, 0xef, 0xbb, 0xbb, 0x7d, 0xbb, 0xbb, 0xbf, 0xf7,
   0xfb, 0xdf, 0xd7, 0xff, 0xef, 0xc7, 0xf7, 0xff, 0xbf, 0xff, 0xb7, 0xff,
   0xfb, 0xef, 0xef, 0xfb, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xef, 0xef, 0xff, 0xff,
   0xff, 0xef, 0xff, 0x01, 0xeb, 0xd3, 0xf7, 0xff, 0xf7, 0xdf, 0xab, 0xef,
   0xff, 0xff, 0xff, 0xdf, 0xbb, 0xef, 0xbf, 0xbf, 0xdb, 0xfb, 0xfb, 0xdf,
   0xbb, 0xbb, 0xcf, 0xcf, 0xf7, 0x01, 0xef, 0xbf, 0x8b, 0xbb, 0xbb, 0xfb,
   0xbb, 0xfb, 0xfb, 0xfb, 0xbb, 0xef, 0xdf, 0xdb, 0xf7, 0x55, 0xab, 0xbb,
   0xb7, 0xbb, 0xbb, 0xfb, 0xef, 0xbb, 0xbb, 0x7d, 0xd7, 0xd7, 0xdf, 0xf7,
   0xf7, 0xdf, 0xbb, 0xff, 0xff, 0xbf, 0xc7, 0xc7, 0x8f, 0xc7, 0xf7, 0x47,
   0xcb, 0xff, 0xff, 0xdb, 0xef, 0x92, 0xcb, 0xc7, 0xcb, 0xa7, 0xcb, 0xc7,
   0xc7, 0xdb, 0xbb, 0x7d, 0xbb, 0xb7, 0xc3, 0xf7, 0xef, 0xef, 0xf3, 0xff,
   0xff, 0xef, 0xff, 0xd7, 0xc7, 0xef, 0xeb, 0xff, 0xf7, 0xdf, 0xc7, 0x01,
   0xff, 0x01, 0xff, 0xef, 0xab, 0xef, 0xdf, 0xcf, 0x83, 0xc3, 0xc3, 0xef,
   0xc7, 0x87, 0xff, 0xff, 0xfb, 0xff, 0xdf, 0xdf, 0xab, 0x83, 0xc3, 0xfb,
   0xbb, 0xc3, 0x83, 0x8b, 0x83, 0xef, 0xdf, 0xe3, 0xf7, 0x6d, 0xab, 0xbb,
   0xc7, 0xbb, 0xc3, 0xc7, 0xef, 0xbb, 0xd7, 0xab, 0xef, 0xef, 0xef, 0xf7,
   0xef, 0xdf, 0xff, 0xff, 0xff, 0x87, 0xb7, 0xfb, 0xb7, 0xbb, 0xe3, 0xbb,
   0xb3, 0xef, 0xef, 0xeb, 0xef, 0x6d, 0xb7, 0xbb, 0xb7, 0xdb, 0xf3, 0xfb,
   0xef, 0xdb, 0xbb, 0x7d, 0xd7, 0xb7, 0xdf, 0xfb, 0xef, 0xdf, 0x6d, 0xff,
   0xff, 0xef, 0xff, 0x01, 0xaf, 0x97, 0x9d, 0xff, 0xf7, 0xdf, 0xab, 0xef,
   0xcf, 0xff, 0xff, 0xf7, 0xbb, 0xef, 0xef, 0xbf, 0xdf, 0xbf, 0xbb, 0xf7,
   0xbb, 0xbf, 0xcf, 0xcf, 0xf7, 0x01, 0xef, 0xef, 0x8b, 0xbb, 0xbb, 0xfb,
   0xbb, 0xfb, 0xfb, 0xbb, 0xbb, 0xef, 0xdb, 0xdb, 0xf7, 0x7d, 0x9b, 0xbb,
   0xf7, 0xbb, 0xeb, 0xbf, 0xef, 0xbb, 0xd7, 0xab, 0xd7, 0xef, 0xf7, 0xf7,
   0xdf, 0xdf, 0xff, 0xff, 0xff, 0xbb, 0xb7, 0xfb, 0xb7, 0x83, 0xf7, 0xbb,
   0xbb, 0xef, 0xef, 0xf3, 0xef, 0x6d, 0xb7, 0xbb, 0xb7, 0xdb, 0xfb, 0xe7,
   0xef, 0xdb, 0xbb, 0x6d, 0xef, 0xb7, 0xef, 0xf7, 0xef, 0xef, 0x9f, 0xff,
   0xff, 0xff, 0xff, 0xd7, 0xc3, 0x9b, 0xdd, 0xff, 0xef, 0xef, 0x6d, 0xef,
   0xcf, 0xff, 0xcf, 0xfb, 0xbb, 0xef, 0xf7, 0xbb, 0xdf, 0xbb, 0xbb, 0xf7,
   0xbb, 0xbb, 0xcf, 0xcf, 0xef, 0xff, 0xf7, 0xff, 0xfb, 0xbb, 0xbb, 0xbb,
   0xbb, 0xfb, 0xfb, 0xbb, 0xbb, 0xef, 0xdb, 0xbb, 0xf7, 0x7d, 0xbb, 0xbb,
   0xf7, 0xbb, 0xdb, 0xbb, 0xef, 0xbb, 0xef, 0xd7, 0xbb, 0xef, 0xfb, 0xf7,
   0xbf, 0xdf, 0xff, 0xff, 0xff, 0xbb, 0xb7, 0xfb, 0xb7, 0xfb, 0xf7, 0x87,
   0xbb, 0xef, 0xef, 0xeb, 0xef, 0x7d, 0xb7, 0xbb, 0xc7, 0xc7, 0xfb, 0xdf,
   0xef, 0xdb, 0xd7, 0x55, 0xd7, 0x8f, 0xf7, 0xf7, 0xef, 0xef, 0xff, 0xff,
   0xff, 0xef, 0xff, 0xd7, 0xef, 0xff, 0xa3, 0xff, 0xdf, 0xf7, 0xef, 0xef,
   0xdf, 0xff, 0xcf, 0xfd, 0xc7, 0xc7, 0x83, 0xc7, 0x8f, 0xc7, 0xc7, 0xf7,
   0xc7, 0xc7, 0xff, 0xdf, 0xdf, 0xff, 0xfb, 0xef, 0xc7, 0xbb, 0xc3, 0xc7,
   0xc3, 0x83, 0xfb, 0xc7, 0xbb, 0xc7, 0xe7, 0xbb, 0x87, 0x7d, 0xbb, 0xc7,
   0xf7, 0xc7, 0xbb, 0xc7, 0xef, 0xc7, 0xef, 0xd7, 0xbb, 0xef, 0x83, 0xc7,
   0x7f, 0xc7, 0xff, 0xff, 0xff, 0x47, 0xcb, 0xc7, 0x4f, 0xc7, 0xf7, 0xbf,
   0xbb, 0xef, 0xef, 0xdb, 0xef, 0x7d, 0xb7, 0xc7, 0xf7, 0xdf, 0xfb, 0xe3,
   0xef, 0xa7, 0xef, 0xbb, 0xbb, 0xbf, 0xc3, 0xcf, 0xef, 0xf3, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0x9f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc7,
   0xff, 0xff, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xdf, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xc7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

Ogre::uint Gorilla::Screen::sNextID = 0;
const Gorilla::Colour Gorilla::Colour::Transparent = Gorilla::Colour(0,0,0,0);

namespace Gorilla
{

enum
{
 UV_TOPLEFT,
 UV_TOPRIGHT,
 UV_BOTTOMLEFT,
 UV_BOTTOMRIGHT
};

Screen::Screen(Ogre::Viewport* vp, Ogre::uint defaultTextureSize)
: mViewport(vp), mID(sNextID++), m8x8Font(new Font8x8())
{
 
 // Initial setup.
 mSceneMgr = mViewport->getCamera()->getSceneManager();
 mInverseViewportSize.x = 1.0f / mViewport->getActualWidth();
 mInverseViewportSize.y = 1.0f / mViewport->getActualHeight();
 
 // Create Texture.
 
 mTexture = Ogre::TextureManager::getSingletonPtr()->createManual(
  "GorillaTexture" + Ogre::StringConverter::toString(mID),
  Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
  Ogre::TEX_TYPE_2D,
  defaultTextureSize, defaultTextureSize,
  0,
  (Ogre::PixelFormat) (int) Bitmap::OGRE_TEXTURE_FORMAT,
  Ogre::TU_DEFAULT);
 
 mInverseTextureSize.x = 1.0f / float(defaultTextureSize);
 mInverseTextureSize.y = 1.0f / float(defaultTextureSize);
 
 _clearTexture();
 
 // Create Material
 mMaterial = Ogre::MaterialManager::getSingletonPtr()->create("GorillaMaterial" + Ogre::StringConverter::toString(mID), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
 Ogre::Pass* pass = mMaterial->getTechnique(0)->getPass(0);
 Ogre::TextureUnitState* tus = pass->createTextureUnitState(mTexture->getName());
 tus->setTextureFiltering(Ogre::FO_NONE, Ogre::FO_NONE, Ogre::FO_NONE);
 pass->setCullingMode(Ogre::CULL_NONE);
 pass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
	pass->setDepthCheckEnabled(false);
	pass->setDepthWriteEnabled(false);
	pass->setLightingEnabled(false);

 // Create Node
 mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

 // Create Object
 mManualObject = mSceneMgr->createManualObject();
 mNode->attachObject(mManualObject);
 
 mManualObject->begin(mMaterial->getName());
 
 Ogre::AxisAlignedBox aabInf;
 aabInf.setInfinite();
 
 mManualObject->setBoundingBox(aabInf);
 mManualObject->setUseIdentityProjection(true);
 mManualObject->setUseIdentityView(true);
 
 mManualObject->position(0,0,0);
 mManualObject->textureCoord(0,0);
 mManualObject->colour(1,1,1,1);
 
 mManualObject->position(1,0,0);
 mManualObject->textureCoord(1,0);
 mManualObject->colour(1,1,1,1);
 
 mManualObject->position(1,1,0);
 mManualObject->textureCoord(1,1);
 mManualObject->colour(1,1,1,1);
 
 mManualObject->end();
 
}

Screen::~Screen()
{
#if OGRE_DEBUG_MODE == 1
 Ogre::Image img;
 mTexture->convertToImage(img);
 img.save("gorilla_debug.png");
#endif
}

// Re-draw everything, Re-pack everything, Re-Calculate Vertices, Re-Calculate UV, Update ManualObject.
void Screen::refresh()
{
 
 bool doCopy = false;
 bool doManualObject = false;
 
 for (std::vector<Block*>::iterator it = mBlocks.begin(); it != mBlocks.end(); it++)
 {
  
  if ((*it)->mMoveRequired)
  {
   doManualObject = true;
  }
  
  if ((*it)->mResizeRequired)
  {
   doManualObject = true;
   doCopy = true;
  }
  
  if ((*it)->mRedrawRequired)
  {
   doCopy = true;
  }
  
 }
 
 if (doCopy)
  _refreshTexture();
 
 if (doManualObject)
  _refreshVertices();
 
}

Font8x8* Screen::get8x8Font()
{
 return m8x8Font;
}


// Refresh (copy) texture incase a style has been CHANGED, or Block has changed its image data.
void Screen::_refreshTexture()
{
 
 Ogre::HardwarePixelBufferSharedPtr pixelBuffer = mTexture->getBuffer();
 pixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL);
 
 const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();
 
 Pixel splatPos(1,0);
 Ogre::uchar* pDest = static_cast<Ogre::uchar*>(pixelBox.data);

 for (std::vector<Block*>::iterator it = mBlocks.begin(); it != mBlocks.end(); it++)
 {
  Block* block = (*it);
  if (block->m1pxOptimised == true)
   continue;
  
  if (block->mRedrawRequired == false)
   continue;
  
  block->mBitmap->splat(pDest, pixelBox.getWidth(), pixelBox.getHeight(), splatPos);
  
  Ogre::Real top, left, bottom, right;
  top = splatPos.top;
  bottom = top + block->mBitmap->getDimensions().height;
  left = splatPos.left;
  right = left + block->mBitmap->getDimensions().width;
  
  top *= mInverseTextureSize.y;
  bottom *= mInverseTextureSize.y;
  left *= mInverseTextureSize.x;
  right *= mInverseTextureSize.x;
  
  block->mScreenUV[UV_TOPLEFT] = Ogre::Vector2(left, top);
  block->mScreenUV[UV_TOPRIGHT] = Ogre::Vector2(right, top);
  block->mScreenUV[UV_BOTTOMLEFT] = Ogre::Vector2(left, bottom);
  block->mScreenUV[UV_BOTTOMRIGHT] = Ogre::Vector2(right, bottom);
  
 }
 
 pixelBuffer->unlock();
}

// Refresh vertices incase a Block has MOVED, or Block Z-ORDER has changed.
void Screen::_refreshVertices()
{
 
 mManualObject->beginUpdate(0);
 
 Ogre::Real top, left, bottom, right, depth;
 
 for (std::vector<Block*>::iterator it = mBlocks.begin(); it != mBlocks.end(); it++)
 {
   Block* block = (*it);
   block->mMoveRequired = false;
   block->mResizeRequired = false;
   
   depth = 0 - (float(block->mScreenZOrder) * 0.01f);
   top = block->mScreenPosition.top;
   bottom = top + block->mScreenSize.height;
   left = block->mScreenPosition.left;
   right = left + block->mScreenSize.width;
   
   top *= mInverseViewportSize.y;
   bottom *= mInverseViewportSize.y;
   left *= mInverseViewportSize.x;
   right *= mInverseViewportSize.x;
   
   top = 1 - (top * 2);
   bottom = 1 - (bottom * 2);
   left = (left * 2) - 1;
   right = (right * 2) - 1;
   
   mManualObject->position(left, bottom, depth);
   mManualObject->textureCoord(block->mScreenUV[UV_BOTTOMLEFT]);
   mManualObject->colour(block->mScreenTextureColour);
   
   mManualObject->position(right, bottom, depth);
   mManualObject->textureCoord(block->mScreenUV[UV_BOTTOMRIGHT]);
   mManualObject->colour(block->mScreenTextureColour);

   mManualObject->position(left, top, depth);
   mManualObject->textureCoord(block->mScreenUV[UV_TOPLEFT]);
   mManualObject->colour(block->mScreenTextureColour);


   mManualObject->position(right, bottom, depth);
   mManualObject->textureCoord(block->mScreenUV[UV_BOTTOMRIGHT]);
   mManualObject->colour(block->mScreenTextureColour);

   mManualObject->position(right, top, depth);
   mManualObject->textureCoord(block->mScreenUV[UV_TOPRIGHT]);
   mManualObject->colour(block->mScreenTextureColour);

   mManualObject->position(left, top, depth);
   mManualObject->textureCoord(block->mScreenUV[UV_TOPLEFT]);
   mManualObject->colour(block->mScreenTextureColour);

 }
 
 mManualObject->end();
}

void Screen::_clearTexture()
{
 
 Ogre::HardwarePixelBufferSharedPtr pixelBuffer = mTexture->getBuffer();
 pixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL);
 
 const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();
 
 memset(pixelBox.data, 255, pixelBox.getWidth() * pixelBox.getHeight() * 4);
 
 pixelBuffer->unlock();
 
}

Bitmap::Bitmap(const Size& size)
: mDimensions(size), mLength(0), mData(0)
{
 setDimensions(size);
}

Bitmap::~Bitmap()
{
 if (mData)
  free(mData);
}

Bitmap::Handle  Bitmap::handle()
{
 return mData;
}

Bitmap::Handle  Bitmap::handle(const Pixel& position)
{
 return mData + (stride() * position.top) + (position.left * 4);
}

Bitmap::Handle  Bitmap::handle(Ogre::uint x, Ogre::uint y)
{
 return mData + (stride() * y) + (x * 4);
}

void Bitmap::clip(Pixel& position, Size& size)
{
 Ogre::uint top = position.top;
 Ogre::uint bottom = position.top + size.height;
 Ogre::uint left = position.left;
 Ogre::uint right = position.left + size.width;
 
}

Ogre::uint    Bitmap::stride()
{
 return mDimensions.width * 4;
}

void  Bitmap::setDimensions(const Size& size)
{
 
 if (mData)
  free(mData);
 
 mDimensions = size;
 mLength = mDimensions.square() * 4;
 mData = (Ogre::uchar*) malloc(mLength);
 
}

Size  Bitmap::getDimensions() const
{
 return mDimensions;
}

Ogre::uint  Bitmap::getLength() const
{
 return mLength;
}

void Bitmap::clear()
{
 memset(mData, 0, mLength);
}

void Bitmap::save(const Ogre::String& filename)
{
 Ogre::Image im;
 im.loadDynamicImage(mData, mDimensions.width, mDimensions.height, (Ogre::PixelFormat) (int) Bitmap::OGRE_TEXTURE_FORMAT);
 im.save(filename);
}

Font8x8::Font8x8()
{
 
 mData = new Bitmap(Size(768,8));
 
 Bitmap::Handle it = mData->handle();
 char c = 0, v = 0;

 for (Ogre::uint i=0;i < 768;i++)
 {
  c = CompressedFont[i];
  for (Ogre::uint j=0;j < 8;j++)
  {
   v = c & 1;
   c >>= 1;
   *it++ = !v; // B
   *it++ = !v; // G
   *it++ = !v; // R
   *it++ = !v; // A
  }
 }
 
 //mData->save("8x8font.png");
}

Font8x8::~Font8x8()
{
 delete mData;
}

Block::Block(const Pixel& position, const Size& size, const Ogre::uint& zorder, Screen* screen, bool optimised)
: mScreenPosition(position), mScreenSize(size), mScreenZOrder(zorder), mScreen(screen), m1pxOptimised(optimised),
  mMoveRequired(true), mRedrawRequired(false), mResizeRequired(false), mScreenTextureColour(1,1,1,1), mBitmap(0)
{
 mScreenUV[0] = Ogre::Vector2(0,0);
 mScreenUV[1] = Ogre::Vector2(0,0);
 mScreenUV[2] = Ogre::Vector2(0,0);
 mScreenUV[3] = Ogre::Vector2(0,0);

 if (!m1pxOptimised)
 {
  mBitmap = new Bitmap(size);
  mResizeRequired = true;
 }
}

Block::~Block()
{
 if (mBitmap)
  delete mBitmap;
}

void Block::_redraw()
{
 // virtual function.
}

void Block::_setTextureSize(const Size& size)
{
 if (m1pxOptimised)
 {
  if (mBitmap)
   delete mBitmap;
  return;
 }

 if (mBitmap == 0)
 {
  mBitmap = new Bitmap(size);
  return;
 }
 
 if (mBitmap->getDimensions() == size)
  return;
 
 mBitmap->setDimensions(size);
 
}

Rectangle* Screen::createRectangle(const Pixel& position, const Size& size, const Colour& background, const Ogre::uint& zorder)
{
 Rectangle* rect = new Rectangle(position, zorder, size, background, this);
 mBlocks.push_back(rect);
 rect->_redraw();
 refresh();
 return rect;
}

MonoText* Screen::createMonoText(const Pixel& position, const Ogre::uint nbCols, const Ogre::uint nbRows, const Ogre::String& initialText, const Colour& foreground, const Colour& background, const Ogre::uint& zorder)
{
 Size size(nbCols*8, nbRows*8);
 MonoText* text = new MonoText(position, size, nbCols, nbRows, initialText, foreground, background, zorder, this);
 mBlocks.push_back(text);
 text->_redraw();
 refresh();
 return text;
}

Rectangle::Rectangle(const Pixel& position, const Ogre::uint& zorder, const Size& size, const Colour& background, Screen* screen)
: Block(position, size, zorder, screen, true), mBackground(background)
{
}

Rectangle::~Rectangle()
{
}

void  Rectangle::_redraw()
{
 mScreenTextureColour = mBackground.asColourvalue();
 mMoveRequired = true;
}

MonoText::MonoText(const Pixel& position, const Size& size, const Ogre::uint nbCols, const Ogre::uint nbRows, const Ogre::String& initialText, const Colour& foreground, const Colour& background, const Ogre::uint& zorder, Screen* screen)
: Block(position, size, zorder, screen, false), mNbCols(nbCols), mNbRows(nbRows), mText(initialText), mForeground(foreground), mBackground(background)
{
 mFont = mScreen->get8x8Font();
 mMoveRequired = true;
}

MonoText::~MonoText()
{
}

void MonoText::setText(const Ogre::String& new_text, bool refreshScreen)
{
 mText = new_text;
 _redraw();
 if (refreshScreen)
  mScreen->_refreshTexture();
}

void MonoText::_redraw()
{
 
 Pixel cursor; Pixel writePos;
 mBitmap->clear();
 
 for (Ogre::uint i=0;i < mText.size();i++)
 {
  
  if (cursor.left > mNbCols)
  {
   cursor.left = 0;
   cursor.top++;
  }
  
  if (cursor.top > mNbRows)
   break;

  Ogre::uchar c = mText[i];

  if (c==' ')
  {
   cursor.left++;
   continue;
  }
  
  if (c=='\n')
  {
   cursor.left = 0;
   cursor.top++;
   continue;
  }
  
  writePos = cursor;
  writePos.left *= 8;
  writePos.top *= 8;
  mFont->splat(mBitmap, writePos , c);
  cursor.left++;
 }
 
 
 mRedrawRequired = true;
 
 
 // Debug
 //mBitmap->save("monotext.png");
}


} // namespace Gorilla

