![HalfMapper](https://github.com/gzalo/HalfMapper/blob/master/extras/logo.png)

This project is a renderer designed specifically to explore the world of Half-Life.
It allows for realtime rendering of the Black Mesa Research Facility.

**The main aim is not to make it playable**, just to be able to navigable (noclip). Models, decals, and other objects won't be rendered.

## Download
The latest binary for Windows can be downloaded here: http://gzalo.com/halfmapper_en/

The configuration file, config.xml, lets you set a few settings (FOV, resolution). Another config file can be created in order to load maps from GoldSrc games, for instance check halflife.xml. In order to load another config file, drag and drop it to the executable (or pass it in the commandline as an argument).

**It needs a Half Life installation**
If using the WON version, PAK files will have to be extracted. The map folder and files halflife.wad and liquids.wad are needed for the program to run. WON is untested, so please report any issues.
For other platforms it can be compiled after installing the required libraries and using the alternate makefile. It can be compiled under Windows with MinGW.

Isometric is supported! (samples here: http://imgur.com/a/jPVgD and http://imgur.com/a/40J7N)
![Isometric support](http://i.imgur.com/ghh8OeT.jpg)
![Lambda Core](http://i.imgur.com/y5xzn7Q.png)
Counter-Strike 1.6 maps are supported as well!
![Counter-Strike 1.6](http://i.imgur.com/Imyw50V.png)

Video showing the program http://www.youtube.com/watch?v=Hl2HbV3UbMs

**List of overlaps found: https://github.com/gzalo/HalfMapper/blob/master/overlaps.md **

## TODO list (ordered from highest to lowest priority)
- Try to add other Black Mesa maps to expand the universe: Opposing Force, Blue Shift and Decay might be possible. Other GoldSrc games maps will probably load without problems, such as Counter-Strike.

- Fixing areas that overlap (map loading sections) that cause z-fighting and some extra walls

- Improve code, clean up some stuff and use newer OpenGL3.

## Used libraries
- SDL for window and event management

- OpenGL and GLU for rendering

- GLEW for easy extension access

- TinyXML2 for configuration files
