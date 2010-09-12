Gorilla Schema
==============

This a schema file for the Gorilla `.gorilla` file format.

Layout
------

A Gorilla file is a simple text file, each element is organise into a key pair, seperated by a single space. Key/Pairs are organised into sections.

    [SectionName]
    key value1 value2 value3
    key1 value1 value2

Sections
--------

A Gorilla is organised into several sections:

* Texture -- Referenced texture and canvas shape UV coords
* Font -- Glyphs and Glyph information
* FontKerning -- Font kerning
* Sprite -- Sprite information

Section: Texture
----------------

### file <texturename>  REQUIRED

Image file to use.

### whitepixel <x> <y>   REQUIRED

Where a white (RGB 255 255 255) pixel is in the image, for best results the surrounding pixels should be white too.

Section: Font.x
---------------

Where "x" is the font index.

### lineheight <h>      REQUIRED
`lineheight 22`

Amount of vertical position to move downwards on a new line.

    <h>     Height in pixels

### spacelength <w>     REQUIRED
`spacelength 5`

Amount of space to move across when a space is needed.

    <w>     Width in pixels

### baseline <h>        REQUIRED
`baseline 18`
Distance from the top where the character "sit's" upon. See http://en.wikipedia.org/wiki/Baseline_%28typography%29

    <h>     Height in pixels

**NOTE: There is a current bug in Gorilla or Ogre, that makes the texture corrupted at 100%. For this reason, I recommend you put the scale at 0.999**

### letterspacing <k>         REQUIRED
`letterspacing -1`

Amount of horizontal space to move forwards/backwards when a letter is drawn. 

This value is used when no kerning information is found for a character

    <r>     Kerning value (in pixels)

### monowidth <w>       REQUIRED
`monowidth 15`

Amount of horiziontal space to move fowards when a letter is drawn in monospace mode.

    <w>     Value (in pixels)

### range <lower> <upper>
`range 33 126`

Available glyphs to use, lower is the ascii code for the lowest character available and upper is the highest character available. Typically for a English character set this would be 33 126

    <lower>     Lowest character
    <upper>     Highest character


### glyph_<id> <x> <y> <w> <h> (advance)        REQUIRED (advance is optional)
`glyph_65 388 0 14 23 13`

Glyph image in the texture atlas. All glyph images need to have the same height, but width is variable.

    <x>         Left position of where the glyph starts
    <y>         Top position of where the glyph starts
    <w>         Width of the glyph
    <h>         Height of the glyph
    (advance)   Move across this distance when draw (OPTIONAL)


### kerning_<right_character> <left_character> <kerning>
`kerning_74 45 1`

Kerning value for when a right character is drawn after a left character.

    <right_character>       ASCII code for the right character
    <left_character>        ASCII code for the left character
    <kerning>               Kerning value.

Section: Sprite
---------------

### <name> <x> <y> <w> <h>
`ogrehead 0 464 48 48`

A single sprite.

    <name>      Name of the sprite
    <x>         Left of the top/left position of the sprite
    <y>         Top of the top/left position of the sprite
    <w>         Width of the sprite
    <h>         height of the sprite


