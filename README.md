# CDDA Map Editor

Converts .bmp images into pre-formatted data for CDDA JSON.

# How To Use
Maps require 3 bitmaps: a tile map, a monster map, and an item map named as:

  Map.bmp
  
  Map_m.bmp
  
  Map_i.bmp
  
  

# Rules.txt defines the output behavior.

  Tiles are defined as:
  
  `R G B=SYMBOL;` ex: `80 250 250=5;`
  
  Monster groups are defined as:
  
  `R G B=GROUPNAME DENSITY;` ex: `255 0 0=mon_zombie 50;`
  
  Item groups are defined as: 
  
  `R G B=GROUPNAME CHANCE REPEAT_LOW REPEAT_HIGH;` ex: `0 0 255=mechanics 80 1 3;`
  
# Execution

Place a all three 24-bit bitmaps into the application folder and run the application.

The map JSON will be output in it's entirety, along with a separate file `Map_extras.json` containing useful entries for defining overmap specials.

*Note: There is no sanity checking for image size, so make sure your image dimensions are some multiple of 24
