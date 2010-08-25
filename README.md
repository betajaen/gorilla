                                                              
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

* Filled (Solid, Gradient or Sprite-based) Rectangles (Rectangle)
* Outlined Rectangles (Box)
* Lines with n-thickness (Line)
* Sprites
* Simple and Formatted Text (Captions and Text)

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

