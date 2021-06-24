# CDDA Map Editor

Converts .bmp images into pre-formatted data for CDDA JSON.

# How To Use
Colors on the bitmap are converted to symbols and printed to the output file.

Colors are defined in "rules.txt" as "R G B=S" ex: "100 255 120=+"

Place a 24-bit bitmap into the application folder and run the application.
It will output entries for "om_terrain" and "rows" for the mapgen json, and "overmaps" for the overmap special json.

*Note: There is no sanity checking for image size, so make sure your image dimensions are some multiple of 24

*Also Note: Overmaps are set to z-level 9 (line 250) , so change that if you don't want your specials to spawn in space
