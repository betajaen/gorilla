                                                              
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
    - Filled Rectangles (Rectangle)
    - Outlined Rectangles (Box)
    - Lines with n-thickness (Line)
    - Sprites
    - Simple and Formatted Text (Captions and Text)

Beastie only uses Ogre as a dependency, and is developed with Ogre 1.7 in mind;  
but should work with any 1.x version.

