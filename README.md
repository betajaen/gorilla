                                                              
                                          88  88  88              
                                          ""  88  88              
                                              88  88              
     ,adPPYb,d8   ,adPPYba,   8b,dPPYba,  88  88  88  ,adPPYYba,  
    a8"    `Y88  a8"     "8a  88P'   "Y8  88  88  88  ""     `Y8  
    8b       88  8b       d8  88          88  88  88  ,adPPPPP88  
    "8a,   ,d88  "8a,   ,a8"  88          88  88  88  88,    ,88  
     `"YbbdP"Y8   `"YbbdP"'   88          88  88  88  `"8bbdP"Y8  
     aa,    ,88                                                   
      "Y8bbdP"                                                    

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

What is Gorilla?
================

Gorilla is a C++ based HUD and simple 2D overlay system for the [Ogre3D] graphics
engine, it comes in two files Gorilla.cpp and Gorilla.h

[Ogre3D]: http://www.ogre3d.org

Gorilla is intended to substitute the Ogre overlay system, and to provide
a good framework and basis for any Ogre programmer to build their HUD or GUI
system upon.

Gorilla is designed for speed; It uses Texture Atlases and tries to render
everything in one batch. 

Gorilla can support

* Filled (Solid, Gradient or Sprite-based) Rectangles (Canvas Rectangles)
* Outlined Rectangles (Canvas Boxes)
* Lines with n-thickness (Canvas Lines)
* Sprites (SpriteLayer Sprites)
* Plain and formatted text (Canvas Captions and Text)

Beastie only uses Ogre as a dependency, and is developed with Ogre 1.7 in mind;  
but should work with any 1.x version.


Using Gorilla
=============

All of Gorilla's classes are contained in the `Gorilla` namespace. All classes and member functions use the `camelCase` notation, with the exception that class names the first letter is always captialised.

To include Gorilla in your project, simply copy the two Gorilla files `Gorilla.cpp` and `Gorilla.h` into your project. Making sure you correctly include and link to Ogre. To increase compliation speed you may want to `#include` Gorilla in your static headers file.

Main classes
============

The main class is called "Silverback" named after the Head Gorilla. Silverback is responsible for the `Screen` and `TextureAtlas` classes. 

Silverback is a singleton so should be created roughly after Root is created and destroyed just before. Screens are tied to the same level as SceneManagers, so any the lifetime of a Screen is the same as a SceneManager. A TextureAtlas will retain the same lifetime as the Silverback.

To create the Silverback, just create an instance

    mSilverback = new Gorilla::Silverback();


TextureAtlas
============

*Please note*: There is a seperate file explaining the Gorilla file format.

The TextureAtlas file represents a `.gorilla` file which contains all the needed information that describes the portions of a single texture. Such as Glyph and Sprite information, text kerning, line heights and so on. It isn't typically used by the end-user.

To load in a `.gorilla` file simply, load it in.

    mSilverback->loadAtlas("dejavu")
  
This will reference the "dejavu.gorilla" file in the General ResourceGroup. The dejavu.gorilla file and accompanying image file may then be stored in a sub-directory or compressed file, assuming Ogre can access it.

Screen
======

A screen is a container class for all 2D operations upon the RenderWindow, it is comparible in function to the SceneManager. Each Screen may use a seperate TextureAtlas or share one together, but each Screen rendered seperately as one batch. So five screens is one batch. Typically it is normal just to have one screen, or a second one for debug output.

Screens need a Viewport to render too, and will get the SceneManager from the Viewport's Camera to listen for drawing updates. All Screen drawings are done after the RENDER_QUEUE_OVERLAY renderqueue.

    Gorilla::Screen* screen = mSilverback->createScreen(mViewport, "dejavu");


Canvas, Text and SpriteLayers
=============================

There are three main types of operations that are drawn onto the screen;

* Canvas
* Text
* SpriteLayer

Like Screen's you can have many Canvases, Text or SpriteLayers. Collectively these are known as Renderables. As all of these belong to a single Screen, they are all rendered in the same single batch.

To help with z-ordering, renderables are assigned to a layer from 0-15. Renderables on layer 0 are rendered first and on 15 rendered last. This means that anything on layer 15 will be "on top" of layer 14, 13 on 12, 12 on 10 and so on.

**Please note:** everything in Gorilla unless specified is measured in *pixels* and not screen coordinates.

Handles
-------

In the Canvas and SpriteLayer classes, portions of these are handled by size_t types known as "handles", or if you wish an internal id for a line, box, sprite, etc.

To change a line colour for example; you need to pass on the new colour, and the handle of the line you wish to change, to the Canvas, setLineColour function. 

Canvas
------

The Canvas class is the bulk of Gorilla, it is used to draw Rectangles, Boxes, Lines and non-formatted single lined text known as Captions. Typically for the shape drawing operations a single white pixel is reserved on the Texture which the shapes use the UV coordinates for. Captions (like its big brother Text) use the Glyphs from the TextureAtlas.

    screen->createCanvas(3)   // where 3 is the layer number, 0 is default.

Text
----

Text is a renderable which it's sole purpose is to draw a large block of text that may cover multiple lines, be in different colours and even have sprites inserted within the text itself.

Text is reserved for "big" things; such as messages, command prompts, rather than little things than the health of the player (which would probably be done in a Canvas using a Caption).

Text uses a markup system to change colours (these are set/fetched from the TextureAtlas palette), to inserting sprites into the text.

A sample of the markup is as follows:

    Wow look at this %4scary%R picture of an Ogre! %:ogre_head%

Which means draw the text, but for "scary" render it in colour 4, and after "Ogre!" put the sprite "ogre_head" there.


SpriteLayer
-----------

A SpriteLayer is to draw 2D quads on the screen known as sprites. You may use sprites for a number of things from displaying the inventory of the player, to a full mini 2D shoot 'em up.


Classes
=======

Silverback
----------

### void loadAtlas(const Ogre::String& name, const Ogre::String& group)

Create a TextureAtlas from a ".gorilla" file. Name is the name of
the TextureAtlas, as well as the first part of the filename of the gorilla file; i.e. name.gorilla, the gorilla file can be loaded from a different resource group if you give that name as the second argument, otherwise it will assume to be "General".

### Screen*  createScreen(Ogre::Viewport*, const Ogre::String& atlas)

Create a Screen using a Viewport and a name of a previously loaded TextureAtlas. Both must exist. The screen will register itself as a RenderQueueListener to the SceneManager that has the Camera which is tied to the Viewport.

Each screen is considered a new batch. To reduce your batch count in Gorilla, reduce the number of screens you use.

### void  destroyScreen(Screen*);

Destroy an existing screen (and contents).

Screen
------

### Canvas*  createCanvas(int layer)

Create a new Canvas to draw too. If layer is omitted then "0" is assumed to be the layer, otherwise layer id must be between 0 and 15.

As with screens there can be many Canvases per screen.

### SpriteLayer*  createSpriteLayer(int layer)

Create a new SpriteLayer for Sprites.  If layer is omitted then "0" is assumed to be the layer, otherwise layer id must be between 0 and 15.

As with screens there can be many SpriteLayers per screen.

### Text*  createText(Ogre::Real left, Ogre::Real top, const Ogre::String& initialText, int layer)

Create a formatted text on the screen, with the coordinates given as left and top. initialText can be blank if desired.

If layer is omitted then "0" is assumed to be the layer, otherwise layer id must be between 0 and 15.

#### Markup codes.

    %x              Change Text Colour (x = 0..9), See TextureAtlas::getPaletteColour
    %R              Reset Text Colour to normal
    %M              Switch to monospace mode of writing (ignore kerning and fixed glyph width).
    %:spritename%   Insert a sprite named "spritename" here. Line height for this line only will
                    be changed to the height of the sprite.
    %%              Insert a percentage sign %.

### void  destroy(Renderable*)
   
Destroy (and destroy its contents) of a Canvas, SpriteLayer or Text.


Renderable (inherited by Canvas, SpriteLayer and Text)
------------------------------------------------------

### bool isVisible()

Is the Renderable visible?

### void setVisible(bool is_visible)

Set the Renderable to invisible or visible. If the Renderable is marked invisible, then it will not be drawn. The triangle count may go down, but the batch count will not.

### void show()

Equilvant to setVisible(true)

### void hide()

Equilvant to setVisible(false)

### Ogre::uint getLayer()

Get the layer id from 0 to 15.

### Ogre::Vector2 getMin()

Get the nearest vertex drawn to the top-left of the screen.

These units are in screen coordinates, and can only be given to functions that accept those units.

### Ogre::Vector2 getMax()

Get the nearest vertex drawn to the bottom-right of the screen.

These units are in screen coordinates, and can only be given to functions that accept those units.


Canvas (inherits Renderable)
----------------------------

### size_t addRectangle(Ogre::Real left, Ogre::Real top, Ogre::Real width, Ogre::Real height, const Ogre::ColourValue& colour)

Creates a filled rectangle (of colour) at left,top with the size of width, height. A handle is returned that represents that rectangle for future use.

If colour is omitted then White is assumed to be the colour of the rectangle

left/top/width/height is measured in pixels.

### void removeRectangle(size_t id)

Remove a rectangle with given handle

###  void setRectangleColour(size_t id, const Ogre::ColourValue& colour)

Set the rectangle's colour to something else.

###  void setRectangleColour(size_t id, const Ogre::ColourValue& topLeft, const Ogre::ColourValue& topRight, const Ogre::ColourValue& bottomRight,const Ogre::ColourValue& bottomLeft)

Set each the colour of each corner of the rectangle individually, if colours are different then a gradient between those colours will be drawn.

### void setRectanglePosition(size_t id, Ogre::Real left, Ogre::Real top)

Set the rectangle's position to somewhere else

left/top is measured in pixels.

### void setRectangleSize(size_t id, Ogre::Real width, Ogre::Real height)

width/height is measured in pixels.

Set the rectangle's size

### void setRectangleBackground(size_t id, const Ogre::String& sprite_name, bool resetColour)

Instead of using a solid colour, use a sprite instead. If the rectangle is a different size to the sprite, then the sprite will be stretched.

If resetColour is true then the Colour is set to white again, otherwise the existing colour may tint/recolour the sprite. By default this argument is true.

### void clearRectangleBackground(size_t id)

Use a solid colour again.

### void setRectangleAngle(size_t id, const Ogre::Degree&)

Change the angle of the rectangle in degrees.

Note: The point of rotation is the top-left corner of the rectangle.

### void setRectangleAngle(size_t id, const Ogre::Radian&)

Change the angle of the rectangle in radians

Note: The point of rotation is the top-left corner of the rectangle.

### void setRectangleMinMax(size_t id, const Ogre::Vector2& min, const Ogre::Vector2& max)

Set the rectangle coordinates using min/max coordinates. Where min is the coordinate of the top-left corner of the rectangle and max is the coordinate of the bottom-right corner of the rectangle..

min/max is measured in pixels

### size_t  addLine(Ogre::Real x1, Ogre::Real y1, Ogre::Real x2, Ogre::Real y2, Ogre::Real thickness, const Ogre::ColourValue& colour);
   
Add a line at x1,y1 and draw it to x2,y2, with a given thickness. If the colour is omitted then the line colour is assumed to be white.

### void removeLine(size_t)

Remove a line by the given handle

### void setLineColour(size_t, const Ogre::ColourValue&)

Change the line colour to something else.

### void setLineCoords(size_t, Ogre::Real x1, Ogre::Real y1, Ogre::Real x2, Ogre::Real y2)

Change the line coordinates to something else.

### void setLineCoords(size_t, const Ogre::Vector4& coords)

Change the line coordinates to something else; using a Vector4.

    x --> x1
    y --> y1
    z --> x2
    w --> y2

### void setLineOrigin(size_t, Ogre::Real x1, Ogre::Real y1)

Change the line origin (x1,y1) to something else.

### void setLineEnd(size_t, Ogre::Real x2, Ogre::Real y2)

Change the line end (x2,y2) to something else

### void setLineThickness(size_t, Ogre::Real thickness)

Set the line thickness (in pixels)

### Ogre::ColourValue getLineColour(size_t id) const

Get the colour of a line

### Ogre::Vector4 getLineCoords(size_t id) const

Get the coordinates of a line as a Vector4

    x --> x1
    y --> y1
    z --> x2
    w --> y2

### Ogre::Real getLineThickness(size_t id) const

Get the line thickenss (in pixels)

### size_t addCaption(Ogre::Real left, Ogre::Real top, const Ogre::String&, const Ogre::ColourValue&)

Add a piece of text at left/top. If the colour is omitted then the line colour is assumed to be white.

Note: Captions ignore markup and all special characters including newlines `\n`.

### void removeCaption(size_t);

Remove a caption

### void setCaptionPosition(size_t, Ogre::Real left, Ogre::Real top)

Set a caption's position

### void setCaptionText(size_t, const Ogre::String& text)

Change the text of a caption

### void setCaptionHorizontalClip(size_t, Ogre::Real clip)

When a caption is drawn across the screen, it should stop drawing after it's reached "clip".

clip is measured in pixels, where the text should stop being drawn.

### void    setCaptionColour(size_t, const Ogre::ColourValue&)

Set the colour of the caption

### Ogre::Vector2 getCaptionPosition(size_t) const

Get the caption's position

### Ogre::String  getCaptionString(size_t) const

Get the text being drawn by the caption

### Ogre::ColourValue getCaptionColour(size_t) const

Get the colour of the caption

### Ogre::Vector2 getCaptionSize(size_t)

Get the complete size of the caption, measured in pixels.

### size_t  addBox(Ogre::Real left, Ogre::Real top, Ogre::Real width, Ogre::Real height, const Ogre::ColourValue&)

Add a box (outlined rectangle) at left/top with the size of width/height. If the colour is omitted then the line colour is assumed to be white.

### void  removeBox(size_t);

Remove a box

### void  setBoxPosition(size_t, Ogre::Real left, Ogre::Real top)

Move a box to a new position

### void  setBoxSize(size_t, Ogre::Real width, Ogre::Real height)

Set a box's new size

### void  setBoxColour(size_t, const Ogre::ColourValue&)

Set a box's colour

### Ogre::Vector2  getBoxPosition(size_t) const

Get a box's position

### Ogre::Vector2  getBoxSize(size_t) const

Get a box's size

### Ogre::ColourValue  getBoxColour(size_t) const
 
Get a box's colour

Text (inherit's Renderable)
---------------------------

### void  setPosition(const Ogre::Vector2& position)

Set the position of the Text

### void  setText(const Ogre::String& text)

Set the text

### void  setTop(Ogre::Real top)

Set the Y position of the text

### void  setLeft(Ogre::Real left)

Set the X position of the text

### void  setFormatted(bool val)

Should the text use markup?

### void  setMonospaced(bool val)

Should the text ignore glyph width and kerning, and use a fixed width instead.

### void  setColour(const Ogre::ColourValue& colour)

Set the base colour of the text

### Ogre::String  getText() const

Get the text

### bool  isFormatted() const

Is markup being used?

### bool   isMonospace() const

Is the text monospaced?

### Ogre::ColourValue  getColour() const

Get the base colour of the text

### Ogre::Real  getTop() const

Get the Y position

### Ogre::Real  getLeft() const

Get the X position


SpriteLayer
-----------

### size_t  addSprite(Ogre::Real left, Ogre::Real top, const Ogre::String& name);

Add a sprite at left/top. Set the sprite image to name which is fetched from the TextureAtlas.

### void  removeSprite(size_t id);

Remove a sprite.

### void  setSpritePosition(size_t id, Ogre::Real left, Ogre::Real top)

Set the position of a sprite

### void  setSprite(size_t id, const Ogre::String& name)

Change the image of a sprite to another one.

### void  setSpriteScale(size_t id, Ogre::Real scaleX, Ogre::Real scaleY)

Set the scale of the sprite

### Ogre::Vector2  getSpritePosition(size_t)

Get a Sprite's position

### Ogre::Vector2  getSpriteScale(size_t)

Get a sprite's scale

